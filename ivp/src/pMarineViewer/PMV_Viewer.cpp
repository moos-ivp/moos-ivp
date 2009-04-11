/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PMV_Viewer.cpp                                       */
/*    DATE: Nov 11th 2004                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include "PMV_Viewer.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

PMV_Viewer::PMV_Viewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
{
  m_var_index = -1;
  m_centric_view = false;
  m_centric_view_sticky = true;
}

//-------------------------------------------------------------
// Procedure: draw()

void PMV_Viewer::draw()
{
  MarineViewer::draw();

  drawPolygons();
  drawGrids();
  drawSegLists();
  drawCircles();
  MarineViewer::drawPoints();

  if(m_vehiset.isViewable("vehicles")) {
    vector<string> svector = m_vehiset.getVehiNames();
    int vsize = svector.size();
    for(int i=0; i<vsize; i++) {
      string vehiname = svector[i];
      bool   isactive = (vehiname == m_vehiset.getActiveVehicle());
      string vehibody = m_vehiset.getStringInfo(vehiname, "body");
      
      // Perhaps draw the history points for each vehicle.
      if(m_vehiset.isViewable("trails")) {
	CPList point_list = m_vehiset.getVehiHist(vehiname);
	int trails_length = (int)(m_vehiset.getDoubleInfo("trails_length"));
	drawPoints(point_list, trails_length);
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
  default:
    return(Fl_Gl_Window::handle(event));
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
    center_needs_adjusting = true;
    m_centric_view = true;
    handled = true;
  }
  else if(param == "view_marker") {
    handled = m_vmarkers.addVMarker(value, m_geodesy);
  }
  else if((param == "ais_report") || (param == "ais_report_local")){
    handled = m_vehiset.setParam(param, value);
    if(handled && m_centric_view && m_centric_view_sticky) {
      center_needs_adjusting = true;
    }
  }
  else if((param == "scope") || (param == "scope_variable"))
    handled = addScopeVariable(value);
  else
    handled = m_vehiset.setParam(param, value);
  
  if(center_needs_adjusting)
    setWeightedCenterView();

  return(handled);
}


//-------------------------------------------------------------
// Procedure: setParam

bool PMV_Viewer::setParam(string param, double value)
{
  // Intercept and disable the centric mode if user pans
  if((param == "pan_x") || (param == "pan_y")) {
    m_centric_view = false;
  }

  bool handled = MarineViewer::setParam(param, value);

  if(!handled) {
    handled = m_vehiset.setParam(param, value);
  }

  return(handled);
}

//-------------------------------------------------------------
// Procedure: drawVehicle

void PMV_Viewer::drawVehicle(string vname, bool active, string vehibody)
{
  ObjectPose opose = m_vehiset.getObjectPose(vname);
  if(!opose.isValid())
    return;

  // If there has been no explicit mapping of color to the given vehicle
  // name then the "inactive_vehicle_color" will be returned below.
  vector<double> vehi_color;
  if(active)
    vehi_color = m_vehiset.getColor("active_vehicle_color");
  else
    vehi_color = m_vehiset.getColor(vname);

  vector<double> vname_color = m_vehiset.getColor("vehicle_name_color");
  
  bool vname_draw = m_vehiset.isViewable("vehicle_names");
  
  double shape_scale  = m_vehiset.getDoubleInfo("vehicle_shape_scale");
  double shape_length = m_vehiset.getDoubleInfo("vlength") * shape_scale;
  
  drawCommonVehicle(vname, opose, vehi_color, vname_color, vehibody, 
		    shape_length, vname_draw, 1);
}

//-------------------------------------------------------------
// Procedure: drawPoints

void PMV_Viewer::drawPoints(CPList &cps, int trail_length)
{
  if(!m_vehiset.isViewable("trails"))
    return;

  vector<double> xvect;
  vector<double> yvect;

  int trails_gap = (int)(m_vehiset.getDoubleInfo("trails_gap"));
  if(trails_gap <= 0)
    trails_gap = 1;


  list<ColoredPoint>::reverse_iterator p;
  int i=0;
  for(p=cps.rbegin(); (p!=cps.rend() && (i<trail_length)); p++) {
    if((i % trails_gap) == 0) {
      if(p->isValid()) {
	xvect.push_back(p->m_x);
	yvect.push_back(p->m_y);
      }
    }
    i++;
  }

  vector<double> cvect = m_vehiset.getColor("trails_color");
  double point_size = m_vehiset.getDoubleInfo("trails_point_size");
  bool connections  = m_vehiset.isViewable("trails_connect");

  drawPointList(xvect, yvect, point_size, cvect, connections);
}

//-------------------------------------------------------------
// Procedure: handleLeftMouse

void PMV_Viewer::handleLeftMouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 1.0);
  double sy = snapToStep(my, 1.0);

  m_left_click =  "x=" + doubleToString(sx,1) + ",";
  m_left_click += "y=" + doubleToString(sy,1);

  //cout << "Left Mouse click at [" << m_left_click << "] meters." << endl;
}

//-------------------------------------------------------------
// Procedure: handleRightMouse

void PMV_Viewer::handleRightMouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 1.0);
  double sy = snapToStep(my, 1.0);
  
  m_right_click =  "x=" + doubleToString(sx,1) + ",";
  m_right_click += "y=" + doubleToString(sy,1);

  //cout << "Right Mouse click at [" << m_right_click << "] meters." << endl;
}

//-------------------------------------------------------------
// Procedure: setWeightedCenterView()

void PMV_Viewer::setWeightedCenterView()
{
  bool centric_view = m_centric_view;

  if(!centric_view)
    return;

  double avg_pos_x, avg_pos_y;
  bool ok = m_vehiset.getWeightedCenter(avg_pos_x, avg_pos_y);
  if(!ok)
    return;

  // First determine how much we're off in terms of meters
  double delta_x = avg_pos_x - m_back_img.get_x_at_img_ctr();
  double delta_y = avg_pos_y - m_back_img.get_y_at_img_ctr();
  
  // Next determine how much in terms of pixels
  double pix_per_mtr = m_back_img.get_pix_per_mtr();

  double x_pixels = pix_per_mtr * delta_x;
  double y_pixels = pix_per_mtr * delta_y;
  
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
  if(m_var_index == -1)
    m_var_index = 0;
  return(true);    
}


//-------------------------------------------------------------
// Procedure: updateScopeVariable
//      Note: 

bool PMV_Viewer::updateScopeVariable(string varname, string value, 
				     string vtime, string vsource)
{
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
  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++) {
    if(m_var_names[i] == varname) {
      m_var_index = i;
      return;
    }
  }
}


// ----------------------------------------------------------
// Procedure: getStringInfo

string PMV_Viewer::getStringInfo(const string& info_type, int precision)
{
  string result = "error";

  if(info_type == "scope_var") {
    if(m_var_index != -1)
      return(m_var_names[m_var_index]);
    else
      return("n/a");
  }
  else if(info_type == "scope_val") {
    if(m_var_index != -1)
      return(m_var_vals[m_var_index]);
    else
      return("To add Scope Variables: SCOPE=VARNAME in the MOOS config block");
  }
  else if(info_type == "scope_time") {
    if(m_var_index != -1)
      return(m_var_time[m_var_index]);
    else
      return("n/a");
  }
  else if(info_type == "scope_source") {
    if(m_var_index != -1)
      return(m_var_source[m_var_index]);
    else
      return("n/a");
  }
  else if(info_type == "left_click_info")
    result = m_left_click;
  else if(info_type == "right_click_info")
    result = m_right_click;
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
  
  //cout << "GSI type:" << info_type << " result:[" << result << "]" << endl;
  return(result);
}
  

