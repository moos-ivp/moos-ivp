/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PMV_Viewer.cpp                                       */
/*    DATE: Nov 11th 2004                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "PMV_Viewer.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "ColorParse.h"
#include "BearingLine.h"

// As of Release 15.4 this is now set in CMake, defaulting to be defined
// #define USE_UTM 

#ifdef _WIN32
#   include <float.h>
#endif

template <typename T>
bool
my_isnan(const T x)
{
#if __cplusplus >= 201103L
  using std::isnan;
#endif
#ifdef _WIN32
  return _isnan(x);
#else
  return isnan(x);
#endif
}

using namespace std;

PMV_Viewer::PMV_Viewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
{
  m_scoping        = false;
  m_var_index      = 0;
  m_var_index_prev = 0;
  m_curr_time      = 0;
  m_draw_count     = 0;

  m_centric_view   = "";
  m_centric_view_sticky = true;
  m_reference_point     = "datum";
  m_reference_bearing   = "relative";
  m_mouse_x   = 0;
  m_mouse_y   = 0;
  m_mouse_lat = 0;
  m_mouse_lon = 0;
  m_lclick_ix = 0;
  m_rclick_ix = 0;
  m_bclick_ix = 0;
 
  string str = "x=$(XPOS),y=$(YPOS),lat=$(LAT),lon=$(LON),";
  str += "vname=$(VNAME),counter=$(IX)";
  VarDataPair lft_pair("MVIEWER_LCLICK", str); 
  VarDataPair rgt_pair("MVIEWER_RCLICK", str);
  lft_pair.set_key("any_left");
  rgt_pair.set_key("any_right");
  lft_pair.set_ptype("left");
  rgt_pair.set_ptype("right");
  m_var_data_pairs_all.push_back(lft_pair);
  m_var_data_pairs_all.push_back(rgt_pair);
}

//-------------------------------------------------------------
// Procedure: draw()

void PMV_Viewer::draw()
{
  m_elapsed = (m_curr_time - m_last_draw_time);

#if 0
  double elapsed = (m_curr_time - m_last_draw_time);
  if((m_draw_count > 20) && (elapsed < 0.033))
    return;
#endif

  MarineViewer::draw();
  m_draw_count++;

  m_last_draw_time = m_curr_time;

  if(m_geo_settings.viewable("hash_viewable"))
    calculateDrawHash();

  vector<string> vnames = m_geoshapes_map.getVehiNames();
  for(unsigned int i=0; i<vnames.size(); i++) {
    vector<XYPolygon> polys   = m_geoshapes_map.getPolygons(vnames[i]);
    vector<XYWedge>   wedges  = m_geoshapes_map.getWedges(vnames[i]);
    vector<XYGrid>    grids   = m_geoshapes_map.getGrids(vnames[i]);
    vector<XYConvexGrid> cgrids = m_geoshapes_map.getConvexGrids(vnames[i]);
    vector<XYSegList> segls   = m_geoshapes_map.getSegLists(vnames[i]);
    vector<XYSeglr>   seglrs  = m_geoshapes_map.getSeglrs(vnames[i]);
    vector<XYVector>  vectors = m_geoshapes_map.getVectors(vnames[i]);
    vector<XYRangePulse> rng_pulses = m_geoshapes_map.getRangePulses(vnames[i]);
    vector<XYCommsPulse> cms_pulses = m_geoshapes_map.getCommsPulses(vnames[i]);
    const map<string, XYPoint>&  points  = m_geoshapes_map.getPoints(vnames[i]);
    const map<string, XYCircle>& circles = m_geoshapes_map.getCircles(vnames[i]);
    const map<string, XYMarker>& markers = m_geoshapes_map.getMarkers(vnames[i]);

    drawPolygons(polys);
    drawGrids(grids);
    drawConvexGrids(cgrids);
    drawSegLists(segls);
    drawSeglrs(seglrs);
    drawCircles(circles, m_curr_time);
    drawPoints(points);
    drawVectors(vectors);
    drawWedges(wedges);
    drawRangePulses(rng_pulses, m_curr_time);
    drawCommsPulses(cms_pulses, m_curr_time);
    drawMarkers(markers);
  }

  drawOpArea(m_op_area);
  drawDatum(m_op_area);
  drawDropPoints();

  // Draw Mouse position
  if(Fl::event_state(FL_SHIFT)) {
    string str = "(" + doubleToString(m_mouse_x,2) + ", " +
      doubleToString(m_mouse_y,2) + ")";
    ColorPack cpack("yellow");
    drawText(m_mouse_x, m_mouse_y, str, cpack, 12);
  }
  else if(Fl::event_state(FL_CTRL)) {
    string str = "(" + doubleToString(m_mouse_lat,6) + ", " +
      doubleToString(m_mouse_lon,6) + ")";    
    ColorPack cpack("yellow");
    drawText(m_mouse_x, m_mouse_y, str, cpack, 12);
  }
  // End Draw Mouse position

  if(m_vehi_settings.isViewableVehicles()) {
    vector<string> svector = m_vehiset.getVehiNames();
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++) {
      string vehiname = svector[i];
      bool   isactive = (vehiname == m_vehiset.getActiveVehicle());
      string vehibody = m_vehiset.getStringInfo(vehiname, "body");
      
      // Perhaps draw the history points for each vehicle.
      if(m_vehi_settings.isViewableTrails()) {
	CPList point_list = m_vehiset.getVehiHist(vehiname);
	unsigned int trails_length = m_vehi_settings.getTrailsLength();
	drawTrailPoints(point_list, trails_length);
      }
      // Next draw the vehicle shapes. If the vehicle index is the 
      // one "active", draw it in a different color.
      drawVehicle(vehiname, isactive, vehibody);
    }
  }

  glFlush();
}

//-------------------------------------------------------------
// Procedure: handle

int PMV_Viewer::handle(int event)
{
  int vx, vy;
  switch(event) {
  case FL_PUSH:
    vx = Fl::event_x();
    vy = h() - Fl::event_y();
    if(Fl_Window::handle(event) != 1) {
      if(Fl::event_button() == FL_LEFT_MOUSE)
	handleLeftMouse(vx, vy);
      if(Fl::event_button() == FL_RIGHT_MOUSE)
	handleRightMouse(vx, vy);
    }
    return(1);
    break;
  case FL_ENTER:
    return(1);
    break;
  case FL_MOVE:
    vx = Fl::event_x();
    vy = h() - Fl::event_y();
    handleMoveMouse(vx, vy);
    return(1);
    break;
  default:
    return(MarineViewer::handle(event));
  }
}

//-------------------------------------------------------------
// Procedure: setParam

bool PMV_Viewer::setParam(string param, string value)
{
  if(MarineViewer::setParam(param, value))
    return(true);

  param = tolower(stripBlankEnds(param));
  value = stripBlankEnds(value);
  
  bool handled = false;
  bool center_needs_adjusting = false;
  
  if(param == "center_view") {
    if((value=="average") || (value=="active") || (value=="reference")) {
      center_needs_adjusting = true;
      m_centric_view = value;
      handled = true;
    }
  }
  else if(param == "op_vertex")
    handled = m_op_area.addVertex(value, m_geodesy);

  else if((param == "filter_out_tag") || (param == "trail_reset")) {
    handled = true;
    m_geoshapes_map.clear(value);
    m_vehiset.clear(value);
    VarDataPair new_pair("HELM_MAP_CLEAR", 0); 
    m_var_data_pairs_non_mouse.push_back(new_pair);
  }
  else if(param == "reference_tag") {
    handled = true;
    if(value == "bearing-absolute")
      m_reference_bearing = "absolute";
    else if(value == "bearing-relative")
      m_reference_bearing = "relative";
    else if(value == "datum")
      m_reference_point = "datum";
    else if(!strContainsWhite(value)) {
      m_vehiset.setParam("center_vehicle_name", value);
      m_reference_point = value;
    }
    else
      handled = false;
  }
  else if(param == "vcolor") {
    string vname  = biteStringX(value, '=');
    string vcolor = value;
    if((vname != "") && isColor(vcolor)) {
      m_map_vcolor[vname] = vcolor;
      handled = true;
    }
  }
  else if(param == "new_report_variable") {
    handled = m_vehiset.setParam(param, value);
  }
  else if((param == "lclick_ix_start") && isNumber(value)) {
    m_lclick_ix = atoi(value.c_str());
    handled = true;
  }
  else if((param == "rclick_ix_start") && isNumber(value)) {
    m_rclick_ix = atoi(value.c_str());
    handled = true;
  }
  else if((param == "view_marker") || (param == "marker")) {
    handled = m_geoshapes_map.addGeoShape(toupper(param), value, "shoreside");
  }
  else {
    handled = handled || m_vehi_settings.setParam(param, value);
    handled = handled || m_vehiset.setParam(param, value);
    handled = handled || m_op_area.setParam(param, value);
  }

  if(center_needs_adjusting)
    setWeightedCenterView();

  return(handled);
}


//-------------------------------------------------------------
// Procedure: handleNodeReport()

bool PMV_Viewer::handleNodeReport(string report_str, string& whynot)
{
  bool handled = m_vehiset.handleNodeReport(report_str, whynot);
  if(handled && (m_centric_view != "") && m_centric_view_sticky) 
    setWeightedCenterView();

  return(handled);
}



//-------------------------------------------------------------
// Procedure: addGeoShape()

bool PMV_Viewer::addGeoShape(string param, string value, string community,
			     double timestamp)
{
  return(m_geoshapes_map.addGeoShape(param, value, community, timestamp));
}


//-------------------------------------------------------------
// Procedure: setParam

bool PMV_Viewer::setParam(string param, double value)
{
  // Intercept and disable the centric mode if user pans
  if((param == "pan_x") || (param == "pan_y")) {
    m_centric_view = "";
  }
  else if(param == "curr_time") {
    m_curr_time = value;
    m_vehiset.setParam(param, value);
    return(true);
  }
  else if(param == "time_warp") {
    m_time_warp = value;
    return(true);
  }

  bool handled = MarineViewer::setParam(param, value);

  handled = handled || m_vehi_settings.setParam(param, value);
  handled = handled || m_vehiset.setParam(param, value);

  return(handled);
}

//-------------------------------------------------------------
// Procedure: getStaleVehicles

vector<string> PMV_Viewer::getStaleVehicles(double thresh)
{
  vector<string> rvector;

  vector<string> vnames = m_vehiset.getVehiNames();
  for(unsigned int i=0; i<vnames.size(); i++) {
    string vname = vnames[i];

    NodeRecord record = m_vehiset.getNodeRecord(vname);
    
    double age_report = m_vehiset.getDoubleInfo(vname, "age_ais");

    if(age_report > thresh)
      rvector.push_back(vname);
  }
  return(rvector);
}


//-------------------------------------------------------------
// Procedure: drawVehicle

void PMV_Viewer::drawVehicle(string vname, bool active, string vehibody)
{
  NodeRecord record = m_vehiset.getNodeRecord(vname);
  if(!record.valid())  // FIXME more rigorous test
    return;

  double age_report = m_vehiset.getDoubleInfo(vname, "age_ais");

  BearingLine bng_line = m_vehiset.getBearingLine(vname);

  // If there has been no explicit mapping of color to the given vehicle
  // name then the "inactive_vehicle_color" will be returned below.
  ColorPack vehi_color;
  if(active)
    vehi_color = m_vehi_settings.getColorActiveVehicle();
  else
    vehi_color = m_vehi_settings.getColorInactiveVehicle();

  // Possibly get the color from the node record
  if(record.getColor() != "")
    vehi_color.setColor(record.getColor());

  // Vehicle color can be overruled in pMarineViewer with the vcolor param
  if(m_map_vcolor.count(vname) != 0) 
    vehi_color.setColor(m_map_vcolor[vname]);
  
  ColorPack vname_color = m_vehi_settings.getColorVehicleName();  
  string vnames_mode = m_vehi_settings.getVehiclesNameMode();
  
  double shape_scale  = m_vehi_settings.getVehiclesShapeScale();

  //  double shape_length = m_vehiset.getDoubleInfo(vname, "vlength") * shape_scale;
  record.setLength(record.getLength() * shape_scale);

  string vname_aug = vname;
  bool  vname_draw = true;
  if(vnames_mode == "off")
    vname_draw = false;
  else if(vnames_mode == "names+mode") {
    string helm_mode = m_vehiset.getStringInfo(vname, "helm_mode");
    string helm_amode = m_vehiset.getStringInfo(vname, "helm_allstop_mode");
    if((helm_mode != "none") && (helm_mode != "unknown-mode"))
      vname_aug += " (" + helm_mode + ")";
    if(helm_amode != "clear") 
      vname_aug += " (" + helm_amode + ")";
  }
  else if(vnames_mode == "names+shortmode") {
    string helm_mode  = m_vehiset.getStringInfo(vname, "helm_mode");
    string helm_amode = m_vehiset.getStringInfo(vname, "helm_allstop_mode");
    if((helm_mode != "none") && (helm_mode != "unknown-mode")) {
      helm_mode = modeShorten(helm_mode);
      vname_aug += " (" + helm_mode + ")";
    }
    if((helm_amode != "clear") && (helm_amode != "n/a"))
      vname_aug += " (" + helm_amode + ")";
  }
  else if(vnames_mode == "names+auxmode") {
    string helm_mode_aux = m_vehiset.getStringInfo(vname, "helm_auxmode");
    if(helm_mode_aux != "")
      vname_aug += " (" + helm_mode_aux + ")";
    else
      vname_aug += " (no auxmode info)";
  }
  else if(vnames_mode == "names+depth") {
    string str_depth = doubleToStringX(record.getDepth(), 1);
    vname_aug += " (depth=" + str_depth + ")";
  }

  // If the NODE_REPORT is old, disregard the vname_mode and instead 
  // indicate the staleness
  double stale_report_thresh = m_vehi_settings.getStaleReportThresh();
  if(age_report > stale_report_thresh) {
    string age_str = doubleToString(age_report,0);
    vname_aug = vname + "(Stale Report: " + age_str + ")";
  } 

  record.setName(vname_aug);

  drawCommonVehicle(record, bng_line, vehi_color, vname_color, vname_draw, 1);
}

//-------------------------------------------------------------
// Procedure: drawTrailPoints

void PMV_Viewer::drawTrailPoints(CPList &cps, unsigned int trail_length)
{
  if(!m_vehi_settings.isViewableTrails())
    return;

  XYSegList segl;

  list<ColoredPoint>::reverse_iterator p;
  unsigned int i=0;
  for(p=cps.rbegin(); (p!=cps.rend() && (i<trail_length)); p++) {
    if(p->isValid())
      segl.add_vertex(p->m_x, p->m_y);
    i++;
  }

  ColorPack   cpack = m_vehi_settings.getColorTrails();
  double    pt_size = m_vehi_settings.getTrailsPointSize();
  bool    connected = m_vehi_settings.isViewableTrailsConnect();

  segl.set_label("trails");
  segl.set_color("vertex", cpack.str());
  segl.set_color("label", "invisible");
  segl.set_vertex_size(pt_size);
  if(connected)
    segl.set_color("edge", "white");
  else
    segl.set_color("edge", "invisible");

  drawSegList(segl);
}

//-------------------------------------------------------------
// Procedure: handleMoveMouse
//      Note: The MOOSGeodesy is a superclass variable, initialized
//            in the superclass. The m_geodesy_initialized variable
//            is also a superclass variable.

void PMV_Viewer::handleMoveMouse(int vx, int vy)
{
  if(!m_geodesy_initialized)
    return;

  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);

  double new_lat, new_lon;

  bool ok = false;
#ifdef USE_UTM
  ok = m_geodesy.UTM2LatLong(mx, my, new_lat, new_lon);
#else
  ok = m_geodesy.LocalGrid2LatLong(mx, my, new_lat, new_lon);
#endif
  
  if(!ok || my_isnan(new_lat) || my_isnan(new_lon))
    return;
  
  m_mouse_x = snapToStep(mx, 0.01);
  m_mouse_y = snapToStep(my, 0.01);
  m_mouse_lon = new_lon;
  m_mouse_lat = new_lat;
}

//-------------------------------------------------------------
// Procedure: handleLeftMouse                       
//      Note: The MOOSGeodesy is a superclass variable, initialized
//            in the superclass. The m_geodesy_initialized variable
//            is also a superclass variable.

void PMV_Viewer::handleLeftMouse(int vx, int vy)
{
  if(!m_geodesy_initialized)
    return;

  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 1.0);
  double sy = snapToStep(my, 1.0);

  string vname_closest = m_vehiset.getClosestVehicle(sx, sy);
  string up_vname_closest = toupper(vname_closest);
  
  double dlat, dlon;

  bool ok = false;
#ifdef USE_UTM
  ok = m_geodesy.UTM2LatLong(sx, sy, dlat, dlon);
#else
  ok = m_geodesy.LocalGrid2LatLong(sx, sy, dlat, dlon);
#endif

  if(!ok || my_isnan(dlat) || my_isnan(dlon))
    return;

  string slat = doubleToString(dlat, 8);
  string slon = doubleToString(dlon, 8);

  // If the mouse is clicked while holding down either the SHIFT or
  // CONTROL keys, this is interpreted as a request for a drop-point.
  if((Fl::event_state(FL_SHIFT)) || (Fl::event_state(FL_ALT))) {
    XYPoint dpt(mx, my);
    string latlon, localg, native;
    localg = "(" + intToString(mx) + ", " + intToString(my) + ")";
    latlon = "("  + slat + ", " + slon + ")";
    if(Fl::event_state(FL_SHIFT))
      native = localg;
    else 
      native = latlon;
    dpt.set_label(native);
    dpt.set_vertex_size(3);
    m_drop_points.addPoint(dpt, latlon, localg, native);
  }
  else if(Fl::event_state(FL_CTRL))
    return(handleRightMouse(vx, vy));
  // Otherwise (no SHIFT/CONTROL key), the left click will be 
  // interpreted as a "mouse-poke". 
  else {
    // The aim is to build a vector of VarDataPairs from the "raw" set
    // residing in m_var_data_pairs_all, by replacing all $(KEY) 
    // occurances with the values found under the mouse location. 
    m_var_data_pairs_lft.clear();
    unsigned int i, vsize = m_var_data_pairs_all.size();
    for(i=0; i<vsize; i++) {
      string ikey = m_var_data_pairs_all[i].get_key();
      if((ikey == "any_left") || (ikey == m_left_mouse_key)) {
	VarDataPair pair = m_var_data_pairs_all[i];
	if(pair.get_ptype() == "left") {
	  string var = pair.get_var();

	  // In most cases pattern replacement is done on the right side of
	  // vardata pair, but in these cases it can be done on the left side,
	  // affecting the MOOS variable name involved in the post.
	  if(strContains(var, "$(VNAME_CLOSEST)")) 
	    var = findReplace(var, "$(VNAME_CLOSEST)", vname_closest);
	  if(strContains(var, "$[VNAME_CLOSEST]")) 
	    var = findReplace(var, "$[VNAME_CLOSEST]", vname_closest);
	  
	  if(strContains(var, "$(UP_VNAME_CLOSEST)")) 
	    var = findReplace(var, "$(UP_VNAME_CLOSEST)", up_vname_closest);
	  if(strContains(var, "$[UP_VNAME_CLOSEST]")) 
	    var = findReplace(var, "$[UP_VNAME_CLOSEST]", up_vname_closest);
	  pair.set_var(var);


	  if(pair.is_string()) {
	    string str = m_var_data_pairs_all[i].get_sdata();
	    if(strContains(str, "$(XPOS)")) 
	      str = findReplace(str, "$(XPOS)", doubleToString(sx,1));
	    if(strContains(str, "$[XPOS]")) 
	      str = findReplace(str, "$[XPOS]", doubleToString(sx,1));
	    if(strContains(str, "$(X)")) 
	      str = findReplace(str, "$(X)", doubleToString(sx,0));
	    
	    if(strContains(str, "$(X:1)")) 
	      str = findReplace(str, "$(X:1)", doubleToString(sx,1));
	    if(strContains(str, "$(X:2)")) 
	      str = findReplace(str, "$(X:2)", doubleToString(sx,2));
	    
	    if(strContains(str, "$(YPOS)")) 
	      str = findReplace(str, "$(YPOS)", doubleToString(sy,1));
	    if(strContains(str, "$[YPOS]")) 
	      str = findReplace(str, "$[YPOS]", doubleToString(sy,1));

	    if(strContains(str, "$(IX)"))
	      str = findReplace(str, "$(IX)", intToString(m_lclick_ix));
	    if(strContains(str, "$[IX]"))
	      str = findReplace(str, "$[IX]", intToString(m_lclick_ix));
	    
	    if(strContains(str, "$(BIX)"))
	      str = findReplace(str, "$(BIX)", intToString(m_bclick_ix));
	    if(strContains(str, "$[BIX]"))
	      str = findReplace(str, "$[BIX]", intToString(m_bclick_ix));
	    
	    if(strContains(str, "$(Y)")) 
	      str = findReplace(str, "$(Y)", doubleToString(sy,0));
	    if(strContains(str, "$[Y]")) 
	      str = findReplace(str, "$[Y]", doubleToString(sy,0));

	    if(strContains(str, "$(Y:1)")) 
	      str = findReplace(str, "$(Y:1)", doubleToString(sy,1));
	    if(strContains(str, "$(Y:2)")) 
	      str = findReplace(str, "$(Y:2)", doubleToString(sy,2));
	    
	    if(strContains(str, "$(LAT)")) 
	      str = findReplace(str, "$(LAT)", doubleToString(dlat,8));
	    if(strContains(str, "$[LAT]")) 
	      str = findReplace(str, "$[LAT]", doubleToString(dlat,8));

	    if(strContains(str, "$(LON)")) 
	      str = findReplace(str, "$(LON)", doubleToString(dlon,8));
	    if(strContains(str, "$[LON]")) 
	      str = findReplace(str, "$[LON]", doubleToString(dlon,8));

	    if(strContains(str, "$(VNAME_CLOSEST)")) 
	      str = findReplace(str, "$(VNAME_CLOSEST)", vname_closest);
	    if(strContains(str, "$[VNAME_CLOSEST]")) 
	      str = findReplace(str, "$[VNAME_CLOSEST]", vname_closest);

	    if(strContains(str, "$(UP_VNAME_CLOSEST)")) 
	      str = findReplace(str, "$(UP_VNAME_CLOSEST)", up_vname_closest);
	    if(strContains(str, "$[UP_VNAME_CLOSEST]")) 
	      str = findReplace(str, "$[UP_VNAME_CLOSEST]", up_vname_closest);
	    
	    if(strContains(str, "$(VNAME)")) {
	      string vname = getStringInfo("active_vehicle_name");
	      str = findReplace(str, "$(VNAME)", vname);
	    }
	    if(strContains(str, "$[VNAME]")) {
	      string vname = getStringInfo("active_vehicle_name");
	      str = findReplace(str, "$[VNAME]", vname);
	    }
	    pair.set_sdata(str);
	  }
	  m_var_data_pairs_lft.push_back(pair);
	}
      }
    }
    m_lclick_ix++;
    m_bclick_ix++;
  }
}

//-------------------------------------------------------------
// Procedure: handleRightMouse
//      Note: The MOOSGeodesy is a superclass variable, initialized
//            in the superclass. The m_geodesy_initialized variable
//            is also a superclass variable.

void PMV_Viewer::handleRightMouse(int vx, int vy)
{
  if(!m_geodesy_initialized)
    return;

  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 1.0);
  double sy = snapToStep(my, 1.0);
  
  double dlat, dlon;

  bool ok = false;
#ifdef USE_UTM
  ok = m_geodesy.UTM2LatLong(sx, sy, dlat, dlon);
#else
  ok = m_geodesy.LocalGrid2LatLong(sx, sy, dlat, dlon);
#endif

  if(!ok || my_isnan(dlat) || my_isnan(dlon))
    return;

  // The aim is to build a vector of VarDataPairs from the "raw" set
  // residing in m_var_data_pairs_all, by replacing all $(KEY) 
  // occurances with the values found under the mouse location. 
  m_var_data_pairs_rgt.clear();
  unsigned int i, vsize = m_var_data_pairs_all.size();
  for(i=0; i<vsize; i++) {
    string ikey = m_var_data_pairs_all[i].get_key();
    if((ikey == "any_right") || (ikey == m_right_mouse_key)) {
      VarDataPair pair = m_var_data_pairs_all[i];
      if(pair.get_ptype() == "right") {
	if(pair.is_string()) {
	  string str = m_var_data_pairs_all[i].get_sdata();
	  if(strContains(str, "$(XPOS)")) 
	    str = findReplace(str, "$(XPOS)", doubleToString(sx,1));
	  if(strContains(str, "$[XPOS]")) 
	    str = findReplace(str, "$[XPOS]", doubleToString(sx,1));
	  if(strContains(str, "$(YPOS)")) 
	    str = findReplace(str, "$(YPOS)", doubleToString(sy,1));
	  if(strContains(str, "$[YPOS]")) 
	    str = findReplace(str, "$[YPOS]", doubleToString(sy,1));
	  if(strContains(str, "$(LAT)")) 
	    str = findReplace(str, "$(LAT)", doubleToString(dlat,8));
	  if(strContains(str, "$[LAT]")) 
	    str = findReplace(str, "$[LAT]", doubleToString(dlat,8));
	  if(strContains(str, "$(LON)")) 
	    str = findReplace(str, "$(LON)", doubleToString(dlon,8));
	  if(strContains(str, "$[LON]")) 
	    str = findReplace(str, "$[LON]", doubleToString(dlon,8));
	  if(strContains(str, "$(IX)")) 
	    str = findReplace(str, "$(IX)", intToString(m_rclick_ix));
	  if(strContains(str, "$[IX]")) 
	    str = findReplace(str, "$[IX]", intToString(m_rclick_ix));
	  if(strContains(str, "$(BIX)")) 
	    str = findReplace(str, "$(BIX)", intToString(m_bclick_ix));
	  if(strContains(str, "$[BIX]")) 
	    str = findReplace(str, "$[BIX]", intToString(m_bclick_ix));
	  pair.set_sdata(str);
	}
	m_var_data_pairs_rgt.push_back(pair);
      }
    }
  }
  m_rclick_ix++;
  m_bclick_ix++;
}

//-------------------------------------------------------------
// Procedure: setWeightedCenterView()

void PMV_Viewer::setWeightedCenterView()
{
  if(m_centric_view == "")
    return;

  double pos_x, pos_y;
  bool ok1 = false;
  bool ok2 = false;
  if(m_centric_view == "average") {
    ok1 = m_vehiset.getWeightedCenter(pos_x, pos_y);
    ok2 = true;
  }
  else if(m_centric_view == "active") {
    ok1 = m_vehiset.getDoubleInfo("active", "xpos", pos_x);
    ok2 = m_vehiset.getDoubleInfo("active", "ypos", pos_y);
  }
  else if(m_centric_view == "reference") {
    ok1 = m_vehiset.getDoubleInfo("center_vehicle", "xpos", pos_x);
    ok2 = m_vehiset.getDoubleInfo("center_vehicle", "ypos", pos_y);
  }

  if(!ok1 || !ok2)
    return;

  // First determine how much we're off in terms of meters
  double delta_x = pos_x - m_back_img.get_x_at_img_ctr();
  double delta_y = pos_y - m_back_img.get_y_at_img_ctr();
  
  // Next determine how much in terms of pixels
  double pix_per_mtr_x = m_back_img.get_pix_per_mtr_x();
  double pix_per_mtr_y = m_back_img.get_pix_per_mtr_y();

  double x_pixels = pix_per_mtr_x * delta_x;
  double y_pixels = pix_per_mtr_y * delta_y;
  
  m_vshift_x = -x_pixels;
  m_vshift_y = -y_pixels;
}


//-------------------------------------------------------------
// Procedure: addScopeVariable
//      Note: 

bool PMV_Viewer::addScopeVariable(string varname)
{
  varname = stripBlankEnds(varname);
  if(strContainsWhite(varname))
    return(false);
  
  bool scoping_already = false;
  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++) 
    if(m_var_names[i] == varname)
      scoping_already = true;

  if(scoping_already)
    return(false);

  m_var_names.push_back(varname);
  m_var_vals.push_back("");
  m_var_source.push_back("");
  m_var_time.push_back("");

  m_scoping = true;
  return(true);    
}

//-------------------------------------------------------------
// Procedure: updateScopeVariable
//      Note: 

bool PMV_Viewer::updateScopeVariable(string varname, string value, 
				     string vtime, string vsource)
{
  if(!m_scoping)
    return(false);

  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++) {
    if(m_var_names[i] == varname) {
      m_var_vals[i] = value;
      m_var_source[i] = vsource;
      m_var_time[i] = vtime;
      return(true);
    }
  }
  return(false);
}

//-------------------------------------------------------------
// Procedure: setActiveScope
//      Note: 

void PMV_Viewer::setActiveScope(string varname)
{
  if(m_var_names.size() <= 1)
    return;
  
  if(varname == "_previous_scope_var_") {
    unsigned int tmp = m_var_index;
    m_var_index = m_var_index_prev;
    m_var_index_prev = tmp;
    return;
  }

  unsigned int i, vsize = m_var_names.size();
  if(varname == "_cycle_scope_var_") {
    m_var_index_prev = m_var_index;
    m_var_index++;
    if(m_var_index >= vsize) {
      m_var_index_prev = vsize-1;
      m_var_index = 0;
    }
    return;
  }

  for(i=0; i<vsize; i++) {
    if(m_var_names[i] == varname) {
      m_var_index_prev = m_var_index;
      m_var_index = i;
      return;
    }
  }
}

//-------------------------------------------------------------
// Procedure: isScopeVariable
//      Note: 

bool PMV_Viewer::isScopeVariable(string varname) const
{
  return(vectorContains(m_var_names, varname));
}

//-------------------------------------------------------------
// Procedure: addMousePoke
//      Note: 

void PMV_Viewer::addMousePoke(string side, string key, string vardata_pair)
{
  string var  = biteStringX(vardata_pair, '=');
  string data = vardata_pair;
  VarDataPair new_pair(var, data, "auto");
  new_pair.set_ptype(side);
  new_pair.set_key(key);
  
  m_var_data_pairs_all.push_back(new_pair);
}

// ----------------------------------------------------------
// Procedure: getStringInfo

string PMV_Viewer::getStringInfo(const string& info_type, int precision)
{
  string result = "error";

  if(info_type == "scope_var") {
    if(m_scoping)
      return(m_var_names[m_var_index]);
    else
      return("n/a");
  }
  else if(info_type == "scope_val") {
    if(m_scoping)
      return(m_var_vals[m_var_index]);
    else
      return("To add Scope Variables: SCOPE=VARNAME in the MOOS config block");
  }
  else if(info_type == "scope_time") {
    if(m_scoping)
      return(m_var_time[m_var_index]);
    else
      return("n/a");
  }
  else if(info_type == "scope_source") {
    if(m_scoping)
      return(m_var_source[m_var_index]);
    else
      return("n/a");
  }
  else if(info_type == "range") {
    double xpos, ypos;
    bool   dhandled1 = m_vehiset.getDoubleInfo("active", "xpos", xpos);
    bool   dhandled2 = m_vehiset.getDoubleInfo("active", "ypos", ypos);
    if(dhandled1 && dhandled2) {
      double x_center = 0;
      double y_center = 0;
      if(m_reference_point != "datum") {
	double cxpos, cypos;
	dhandled1 = m_vehiset.getDoubleInfo("center_vehicle", "xpos", cxpos);
	dhandled2 = m_vehiset.getDoubleInfo("center_vehicle", "ypos", cypos);
	if(dhandled1 && dhandled2) {
	  x_center = cxpos;
	  y_center = cypos;
	}
      }
      double range = hypot((xpos-x_center), (ypos-y_center));
      result = doubleToString(range, precision);
    }
  }
  else if(info_type == "bearing") {
    double xpos, ypos;
    bool   dhandled1 = m_vehiset.getDoubleInfo("active", "xpos", xpos);
    bool   dhandled2 = m_vehiset.getDoubleInfo("active", "ypos", ypos);
    if(dhandled1 && dhandled2) {
      double x_center = 0;
      double y_center = 0;
      double h_heading = 0;
      if(m_reference_point != "datum") {
	double cxpos, cypos, heading;
	bool ok1 = m_vehiset.getDoubleInfo("center_vehicle", "xpos", cxpos);
	bool ok2 = m_vehiset.getDoubleInfo("center_vehicle", "ypos", cypos);
	bool ok3 = m_vehiset.getDoubleInfo("center_vehicle", "heading", heading);
	if(ok1 && ok2 && ok3) {
	  x_center = cxpos;
	  y_center = cypos;
	  h_heading = heading;
	}
      }
      double bearing = 0;
      if((m_reference_bearing == "absolute") || (m_reference_point == "datum"))
	bearing = relAng(x_center, y_center, xpos, ypos);
      else if(m_reference_bearing == "relative") {
	bearing = relAng(x_center, y_center, xpos, ypos);
	bearing = angle360(bearing - h_heading);
      }
      result = doubleToString(bearing, precision);
    }
  }
  else {
    string sresult;
    bool   shandled = m_vehiset.getStringInfo("active", info_type, sresult);
    if(shandled) {
      result = sresult;
    }
    else {
      double dresult;
      bool   dhandled = m_vehiset.getDoubleInfo("active", info_type, dresult);
      if(dhandled)
	result = doubleToString(dresult, precision);
    }
  }
  
  return(result);
}
  
// ----------------------------------------------------------
// Procedure: getLeftMousePairs
// Procedure: getRightMousePairs
// Procedure: getNonMousePairs

vector<VarDataPair> PMV_Viewer::getLeftMousePairs(bool clear)
{
  vector<VarDataPair> rvector = m_var_data_pairs_lft;
  if(clear)
    m_var_data_pairs_lft.clear();
  return(rvector);
}

vector<VarDataPair> PMV_Viewer::getRightMousePairs(bool clear)
{
  vector<VarDataPair> rvector = m_var_data_pairs_rgt;
  if(clear)
    m_var_data_pairs_rgt.clear();
  return(rvector);
}

vector<VarDataPair> PMV_Viewer::getNonMousePairs(bool clear)
{
  vector<VarDataPair> rvector = m_var_data_pairs_non_mouse;
  if(clear)
    m_var_data_pairs_non_mouse.clear();
  return(rvector);
}

//-------------------------------------------------------------
// Procedure: shapeCount()

unsigned int PMV_Viewer::shapeCount(const string& gtype, 
				    const string& vname) const
{
  return(m_geoshapes_map.size(gtype, vname));
}

//-------------------------------------------------------------
// Procedure: clearGeoShapes()

void PMV_Viewer::clearGeoShapes(string vname, string shape, string stype) 
{
  m_geoshapes_map.clear(vname, shape, stype);
}

//-------------------------------------------------------------
// Procedure: calculateDrawHash()

void PMV_Viewer::calculateDrawHash()
{
  double xl = m_geoshapes_map.getXMin();
  if(m_vehiset.getXMin() < xl)
    xl = m_vehiset.getXMin();

  double xh = m_geoshapes_map.getXMax();
  if(m_vehiset.getXMax() > xh)
    xh = m_vehiset.getXMax();

  double yl = m_geoshapes_map.getYMin();
  if(m_vehiset.getYMin() < yl)
    yl = m_vehiset.getYMin();

  double yh = m_geoshapes_map.getYMax();
  if(m_vehiset.getYMax() > yh)
    yh = m_vehiset.getYMax();

  double buffer = 1000;

  drawFastHash(xl-buffer, xh+buffer, yl-buffer, yh+buffer);
}










