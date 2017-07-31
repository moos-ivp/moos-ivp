/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NavPlotViewer.cpp                                    */
/*    DATE: May 31st, 2005                                       */
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

#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include "NavPlotViewer.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "GeomUtils.h"
#include "IO_GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

NavPlotViewer::NavPlotViewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
{
  m_hash_shade   = 0.35;

  m_trails       = "window"; // "none, to-present, window, all"
  m_curr_time    = 0;
  m_step_by_secs = true;

  // Bounding box of all vehicle positions and min/max time globally
  m_min_xpos     = 0;
  m_min_ypos     = 0;
  m_max_xpos     = 0;
  m_max_ypos     = 0;

  m_shape_scale  = 2;
  m_draw_geo     = true;
  m_streaming    = false;
  m_center_refresh = false;

  m_alt_nav_prefix = "NAV_GT_";

  m_geo_settings.setParam("hash_viewable", "true");
}

//-------------------------------------------------------------
// Procedure: setParam

bool NavPlotViewer::setParam(string param, string value)
{
  bool handled = false;

  // Intercept this parameter - before handled by MarineViewer
  if(param == "trails_viewable") {
    handled = true;
    value = tolower(value);
    if(value == "true")
      m_trails = "all";
    else if(value == "false")
      m_trails = "none";
    else if(value == "toggle") {
      if(m_trails == "none")
	m_trails = "to-present";
      else if(m_trails == "to-present")
	m_trails = "window";
      else if(m_trails == "window")
	m_trails = "all";
      else if(m_trails == "all")
	m_trails = "none";
    }
    else
      handled = false;
  }
      
  else if(param == "center_view") {
    if(value == "average") {
      m_center_refresh = !m_center_refresh;
      setCenterView("ctr_of_vehicles");
    }
    else if(value == "objects")
      setCenterView("ctr_of_vehicles");
    else if(value == "none")
      m_center_refresh = false;
    handled = true;
  }
  
  else if(param == "vehicle_shape_scale") {
    if(value == "reset")
      m_shape_scale = 1;
    else if(value == "bigger")
      m_shape_scale *= 1.25;
    else if(value == "smaller")
      m_shape_scale *= 0.8;
    handled = true;
  }

  if(!handled)
    handled = MarineViewer::setParam(param, value);
  
  return(handled);
}

//-------------------------------------------------------------
// Procedure: setParam

bool NavPlotViewer::setParam(string param, double value)
{
  if(MarineViewer::setParam(param, value))
    return(true);
  
  return(true);
}

//-------------------------------------------------------------
// Procedure: setMinimalMem()

void NavPlotViewer::setMinimalMem()
{
  m_draw_geo = false;
}

//-------------------------------------------------------------
// Procedure: setDataBroker()

void NavPlotViewer::setDataBroker(ALogDataBroker dbroker)
{
  m_dbroker = dbroker;

  m_vnames.clear();
  m_vtypes.clear();
  m_vlengths.clear();
  for(unsigned int aix=0; aix<m_dbroker.sizeALogs(); aix++) {
    string vehicle_name = m_dbroker.getVNameFromAix(aix);
    string vehicle_type = m_dbroker.getVTypeFromAix(aix);
    string vehicle_color = m_dbroker.getVColorFromAix(aix);
    double vehicle_length = m_dbroker.getVLengthFromAix(aix);
    m_vnames.push_back(vehicle_name);
    m_vtypes.push_back(vehicle_type);
    m_vcolors.push_back(vehicle_color);
    m_vlengths.push_back(vehicle_length);
  }
}

//-------------------------------------------------------------
// Procedure: initPlots()

void NavPlotViewer::initPlots()
{
  unsigned int alogs = m_dbroker.sizeALogs();
  for(unsigned int aix=0; aix<alogs; aix++) {
    string vname = m_dbroker.getVNameFromAix(aix);
    unsigned int mix;
    
    // Get the "normal" nav positions
    mix = m_dbroker.getMixFromVNameVarName(vname, "NAV_X");
    LogPlot logplot_navx = m_dbroker.getLogPlot(mix);
    addLogPlotNAVX(logplot_navx);

    mix = m_dbroker.getMixFromVNameVarName(vname, "NAV_Y");
    LogPlot logplot_navy = m_dbroker.getLogPlot(mix);
    addLogPlotNAVY(logplot_navy);

    mix = m_dbroker.getMixFromVNameVarName(vname, "NAV_HEADING");
    LogPlot logplot_hdg = m_dbroker.getLogPlot(mix);
    addLogPlotHDG(logplot_hdg);

    // Get the alternate nav positions. If they dont exist, its ok
    // the logplots will just be empty.
    LogPlot logplot_gt_navx, logplot_gt_navy, logplot_gt_hdg;
    
    mix = m_dbroker.getMixFromVNameVarName(vname, m_alt_nav_prefix+"X");
    if(mix < m_dbroker.sizeMix())
      logplot_gt_navx = m_dbroker.getLogPlot(mix);
    addLogPlotNAVX_GT(logplot_gt_navx);

    mix = m_dbroker.getMixFromVNameVarName(vname, m_alt_nav_prefix+"Y");
    if(mix < m_dbroker.sizeMix())
      logplot_gt_navy = m_dbroker.getLogPlot(mix);
    addLogPlotNAVY_GT(logplot_gt_navy);

    mix = m_dbroker.getMixFromVNameVarName(vname, m_alt_nav_prefix+"HEADING");
    if(mix < m_dbroker.sizeMix())
      logplot_gt_hdg = m_dbroker.getLogPlot(mix);
    addLogPlotHDG_GT(logplot_gt_hdg);
  }
  
  // If opening in "quick" mode, will not read/draw geometry objs
  if(m_draw_geo) {
    for(unsigned int aix=0; aix<alogs; aix++) {
      VPlugPlot vplot = m_dbroker.getVPlugPlot(aix);
      addVPlugPlot(vplot);
    }
  }
}

//-------------------------------------------------------------
// Procedure: addLogPlotNAVX

void NavPlotViewer::addLogPlotNAVX(const LogPlot& lp)
{
  // First see if the new logplot expands the x or time bounds
  double lp_min_xpos = lp.getMinVal();
  double lp_max_xpos = lp.getMaxVal();
  bool   virgin = ((m_navx_plot.size()==0) || (m_navx_gt_plot.size()==0));
  if(virgin || (lp_min_xpos < m_min_xpos))
    m_min_xpos = lp_min_xpos;
  if(virgin || (lp_max_xpos > m_max_xpos))
    m_max_xpos = lp_max_xpos;

  // Then add the new logplot 
  m_navx_plot.push_back(lp);
}

//-------------------------------------------------------------
// Procedure: addLogPlotNAVX_GT

void NavPlotViewer::addLogPlotNAVX_GT(const LogPlot& lp)
{
  // First see if the new logplot expands the x or time bounds
  double lp_min_xpos = lp.getMinVal();
  double lp_max_xpos = lp.getMaxVal();
  bool   virgin = ((m_navx_plot.size()==0) || (m_navx_gt_plot.size()==0));
  if(virgin || (lp_min_xpos < m_min_xpos))
    m_min_xpos = lp_min_xpos;
  if(virgin || (lp_max_xpos > m_max_xpos))
    m_max_xpos = lp_max_xpos;

  // Then add the new logplot 
  m_navx_gt_plot.push_back(lp);
}

//-------------------------------------------------------------
// Procedure: addLogPlotNAVY

void NavPlotViewer::addLogPlotNAVY(const LogPlot& lp)
{
  // First see if the new logplot expands the Y or Time bounds
  double lp_min_ypos = lp.getMinVal();
  double lp_max_ypos = lp.getMaxVal();
  bool   virgin = ((m_navy_plot.size()==0) || (m_navy_gt_plot.size()==0));
  if(virgin || (lp_min_ypos < m_min_ypos)) 
    m_min_ypos = lp_min_ypos;
  if(virgin || (lp_max_ypos > m_max_ypos)) 
    m_max_ypos = lp_max_ypos;

  // Then add the new logplot 
  m_navy_plot.push_back(lp);
}

//-------------------------------------------------------------
// Procedure: addLogPlotNAVY_GT

void NavPlotViewer::addLogPlotNAVY_GT(const LogPlot& lp)
{
  // First see if the new logplot expands the Y or Time bounds
  double lp_min_ypos = lp.getMinVal();
  double lp_max_ypos = lp.getMaxVal();
  bool   virgin = ((m_navy_plot.size()==0) || (m_navy_gt_plot.size()==0));
  if(virgin || (lp_min_ypos < m_min_ypos)) 
    m_min_ypos = lp_min_ypos;
  if(virgin || (lp_max_ypos > m_max_ypos)) 
    m_max_ypos = lp_max_ypos;

  // Then add the new logplot 
  m_navy_gt_plot.push_back(lp);
}

//-------------------------------------------------------------
// Procedure: addLogPlotHDG

void NavPlotViewer::addLogPlotHDG(const LogPlot& lp)
{
  m_hdg_plot.push_back(lp);
}

//-------------------------------------------------------------
// Procedure: addLogPlotHDG_GT

void NavPlotViewer::addLogPlotHDG_GT(const LogPlot& lp)
{
  m_hdg_gt_plot.push_back(lp);
}

//-------------------------------------------------------------
// Procedure: addLogPlotStartTime

void NavPlotViewer::addLogPlotStartTime(double start_time)
{
  m_start_time.push_back(start_time);
}

//-------------------------------------------------------------
// Procedure: addVPlugPlot

void NavPlotViewer::addVPlugPlot(const VPlugPlot& gp)
{
  m_vplug_plot.push_back(gp);
}


//-------------------------------------------------------------
// Procedure: draw()

void NavPlotViewer::draw()
{
  MarineViewer::draw();
  drawTrails();
  drawNavPlots();
  drawVPlugPlots();
  if(m_geo_settings.viewable("hash_viewable")) {
    drawHash(m_min_xpos-2000, m_max_xpos+2000, 
	     m_min_ypos-2000, m_max_ypos+2000);
  }

  ColorPack cpack("yellow");
  string msg = "--zoom=" + doubleToString(m_zoom,3) + "  ";
  msg += "--panx=" + doubleToString(m_vshift_x,2) + " ";
  msg += "--pany=" + doubleToString(m_vshift_y,2);

  drawTextX(10, 10, msg, cpack, 12);
}

//-------------------------------------------------------------
// Procedure: setCurrTime
//   Returns: true if the given time is within the min/max bounds

bool NavPlotViewer::setCurrTime(double gtime)
{
  m_curr_time = gtime;

  double min_time = m_dbroker.getPrunedMinTime();
  double max_time = m_dbroker.getPrunedMaxTime();

  if(m_curr_time < min_time) {
    m_curr_time = min_time;
    return(false);
  }

  if(m_curr_time > max_time) {
    m_curr_time = max_time;
    return(false);
  }

  return(true);
}

//-------------------------------------------------------------
// Procedure: stepTime
//   Returns: true if the newly calculated time is within min/max bounds

bool NavPlotViewer::stepTime(double amt)
{
  bool ok_set_curr_time = setCurrTime(m_curr_time + amt);
  if(!ok_set_curr_time)
    return(false);

  if(m_center_refresh) 
    setCenterView("ctr_of_vehicles");

  return(true);
}


//-------------------------------------------------------------
// Procedure: getCurrTime

double NavPlotViewer::getCurrTime()
{
  return(m_curr_time);
}

//-------------------------------------------------------------
// Procedure: getStartTimeHint()

double NavPlotViewer::getStartTimeHint()
{
  double min_time = m_dbroker.getPrunedMinTime();
  double max_time = m_dbroker.getPrunedMaxTime();
  
  double time_window = max_time - min_time;
  double time_hint   = min_time + (time_window / 4);
  return(time_hint);
}

//-------------------------------------------------------------
// Procedure: drawNavPlots

void NavPlotViewer::drawNavPlots()
{
  for(unsigned int i=0; i<m_navx_plot.size(); i++) {
    drawNavPlot(i);
    // If any alt nav (e.g., NAV_GT_X) info available, then draw.
    if(m_navx_gt_plot[i].size() > 0) 
      drawNavPlot(i, true);
  }

}

//-------------------------------------------------------------
// Procedure: drawNavPlot
//      Note: The [index] argument refers to the vehicle index.

void NavPlotViewer::drawNavPlot(unsigned int index, bool alt_nav)
{
  if(index >= m_navx_plot.size())
    return;
  if(m_navx_plot[index].size() == 0)
    return;

  double ctime = getCurrTime();
  double x = 0;
  double y = 0;
  double heading = 0;

  if(index < m_navx_plot.size()) {
    if(alt_nav)
      x = m_navx_gt_plot[index].getValueByTime(ctime);
    else
      x = m_navx_plot[index].getValueByTime(ctime);
  }
  if(index < m_navy_plot.size()) {
    if(alt_nav) 
      y = m_navy_gt_plot[index].getValueByTime(ctime);
    else
      y = m_navy_plot[index].getValueByTime(ctime);
  }

  if(index < m_hdg_plot.size()) {
    if(alt_nav) 
      heading = m_hdg_gt_plot[index].getValueByTime(ctime);
    else
      heading = m_hdg_plot[index].getValueByTime(ctime);
  }
  ColorPack  vehi_color = m_vehi_settings.getColorInactiveVehicle();
  vehi_color = m_vehi_settings.getColorActiveVehicle();

  // Check if the color was specified individually (eg NODE_REPORT)
  if(m_vcolors[index] != "")
    vehi_color.setColor(m_vcolors[index]);
  
  ColorPack vname_color = m_vehi_settings.getColorVehicleName();  
  string    vnames_mode = m_vehi_settings.getVehiclesNameMode();
  //double    shape_scale = m_vehi_settings.getVehiclesShapeScale();

  string    vehi_type   = m_vtypes[index];
  double    vehi_length = m_vlengths[index] * m_shape_scale;
  string    vehi_name   = m_vnames[index];

  NodeRecord record(vehi_name, vehi_type);
  record.setX(x);
  record.setY(y);
  record.setHeading(heading);
  record.setLength(vehi_length);

  bool vname_draw = true;
  if(vnames_mode == "off")
    vname_draw = false;
  

  // For now we don't support depth. It would mean reading in a LogPlot for NAV_DEPTH
  // which could substantially bloat alogview memory footprint. Users can see depth
  // by opening NAV_DEPTH in the LogPlotViewer. If we can support a "collapse" 
  // function for logplots to collapse series of same values into one, then 
  // perhaps it makes sense here. Surface vehicles still get NAV_DEPTH
  // 
#if 0
  if(vnames_mode == "names+depth") {
    string str_depth = dstringCompact(doubleToString(record.getDepth(),2));
    vehi_name += " (depth=" + str_depth + ")";
    record.setName(vehi_name);
  }
  cout << "vehi_name:[" << vehi_name << "]" << endl;
#endif

  // We do not handle bearing reports - yet.
  BearingLine bng_line;
  
  if(m_vehi_settings.isViewableVehicles())
    drawCommonVehicle(record, bng_line, vehi_color, vname_color, vname_draw);
}


//-------------------------------------------------------------
// Procedure: drawTrails()

void NavPlotViewer::drawTrails()
{
  if(m_trails == "none")
    return;

  for(unsigned int i=0; i<m_navx_plot.size(); i++)
    drawTrail(i);
}

//-------------------------------------------------------------
// Procedure: drawTrail()

void NavPlotViewer::drawTrail(unsigned int index)
{
  double    ctime = getCurrTime();
  double    pt_size = m_vehi_settings.getTrailsPointSize();
  bool      connect = m_vehi_settings.isViewableTrailsConnect();
  ColorPack tcolor  = m_vehi_settings.getColorTrails();

  // We cannot assume there are equal numbers of NAV_X and NAV_Y entries.
  // Especially true if the alog file has been "pared" with alogpare.

  // Determine the absolute min and max times between x and y plots
  double starttime = m_navx_plot[index].getMinTime();
  if(starttime < m_navy_plot[index].getMinTime())
    starttime = m_navy_plot[index].getMinTime();
  double endtime = m_navx_plot[index].getMaxTime();
  if(endtime > m_navy_plot[index].getMaxTime())
    endtime = m_navy_plot[index].getMaxTime();

  double tgap = 0.5;

  XYSegList segl;

  if(m_trails == "window") {
    unsigned int trails_length = m_vehi_settings.getTrailsLength();
    double timewindow = (double)(trails_length) * tgap;
    starttime = ctime - timewindow;
    if(starttime < m_navx_plot[index].getMinTime())
      starttime = m_navx_plot[index].getMinTime();
    if(starttime < m_navy_plot[index].getMinTime())
      starttime = m_navy_plot[index].getMinTime();
    endtime = ctime;
  }
  else if(m_trails == "to-present")  // (m_trails == to-present)
    endtime = ctime;
  //else  m_trails=all - we just keep the global min/max times as
  // our start and end times.

  for(double time=starttime; time <= endtime; time += tgap) {
    double x = m_navx_plot[index].getValueByTime(time);
    double y = m_navy_plot[index].getValueByTime(time);
    segl.add_vertex(x, y);
  }
  
  segl.set_color("vertex", tcolor);
  segl.set_vertex_size(pt_size);

  if(connect)
    segl.set_color("edge", tcolor);
  else
    segl.set_color("edge", "invisible");

  drawSegList(segl);
}

//-------------------------------------------------------------
// Procedure: drawVPlugPlots

void NavPlotViewer::drawVPlugPlots()
{
  for(unsigned int i=0; i<m_vplug_plot.size(); i++)
    drawVPlugPlot(i);
}

//-------------------------------------------------------------
// Procedure: drawVPlugPlot

void NavPlotViewer::drawVPlugPlot(unsigned int index)
{
  if(index >= m_vplug_plot.size())
    return;
  
  VPlug_GeoShapes geo_shapes;
  geo_shapes = m_vplug_plot[index].getVPlugByTime(m_curr_time);

  vector<XYPolygon>    polys   = geo_shapes.getPolygons();
  vector<XYGrid>       grids   = geo_shapes.getGrids();
  vector<XYSegList>    segls   = geo_shapes.getSegLists();
  //vector<XYSeglr>      seglrs  = geo_shapes.getSeglrs();
  vector<XYRangePulse> rpulses = geo_shapes.getRangePulses();
  vector<XYCommsPulse> cpulses = geo_shapes.getCommsPulses();
  const map<string, XYPoint>&  points  = geo_shapes.getPoints();
  const map<string, XYCircle>& circles = geo_shapes.getCircles();
  const map<string, XYMarker>& markers = geo_shapes.getMarkers();

  drawPolygons(polys);
  drawGrids(grids);
  drawSegLists(segls);
  //drawSeglrs(seglrs);
  drawCircles(circles);
  drawPoints(points);
  drawMarkers(markers);

  double global_logstart = m_dbroker.getGlobalLogStart();
  double utc_timestamp = global_logstart + m_curr_time;

  drawRangePulses(rpulses, utc_timestamp);
  drawCommsPulses(cpulses, utc_timestamp);
}

//-------------------------------------------------------------
// Procedure: setCenterview

void NavPlotViewer::setCenterView(string centering_style)
{
  if((m_navx_plot.size()==0) || (m_navy_plot.size() == 0))
    return;

  double pos_x = 0;
  double pos_y = 0;
  if(centering_style == "ctr_of_bounding") {
    // Median values of logplots are used.
    double min_x=0, min_y=0, max_x=0, max_y=0;
    unsigned int i, x_size = m_navx_plot.size();
    for(i=0; i<x_size; i++) {
      double this_value = m_navx_plot[i].getMedian();
      if((i==0) || (this_value < min_x))
	min_x = this_value;
      if((i==0) || (this_value > max_x))
	max_x = this_value;
    }    
    unsigned int j, y_size = m_navy_plot.size();
    for(j=0; j<y_size; j++) {
      double this_value = m_navy_plot[j].getMedian();
      if((j==0) || (this_value < min_y))
	min_y = this_value;
      if((j==0) || (this_value > max_y))
	max_y = this_value;
    }    
    pos_x = ((max_x - min_x) / 2) + min_x;
    pos_y = ((max_y - min_y) / 2) + min_y;
  }
  else if(centering_style == "ctr_of_vehicles") {
    double nav_x_total = 0;
    for(unsigned int i=0; i<m_navx_plot.size(); i++) 
      nav_x_total += m_navx_plot[i].getValueByTime(m_curr_time, true);
    double nav_y_total = 0;
    for(unsigned int i=0; i<m_navy_plot.size(); i++) 
      nav_y_total += m_navy_plot[i].getValueByTime(m_curr_time, true);

    pos_x = nav_x_total / (double)(m_navx_plot.size());
    pos_y = nav_y_total / (double)(m_navy_plot.size());
  }
  else
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
// Procedure: setStepType
//      Note: 

void NavPlotViewer::setStepType(const string& step_type)
{
  if(step_type == "seconds")
    m_step_by_secs = true;
  else if(step_type == "helm_iterations")
    m_step_by_secs = false;
}










