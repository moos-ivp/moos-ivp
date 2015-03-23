/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NavPlotViewer.cpp                                    */
/*    DATE: May 31st, 2005                                       */
/*    DATE: Feb 9th, 2015 Major ovrhaul by mikerb                */
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
#include <cmath>
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
  m_hash_shade     = 0.35;

  m_trails         = "to-present"; // "none, to-present, window, all"
  m_curr_time      = 0;
  m_step_by_secs   = true;

  // Bounding box of all vehicle positions and min/max time globally
  m_min_time       = -1;
  m_max_time       = -1;
  m_min_xpos       = 0;
  m_min_ypos       = 0;
  m_max_xpos       = 0;
  m_max_ypos       = 0;

  m_shape_scale    = 2;

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
    if(value == "average")
      setCenterView("ctr_of_bounding");
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
// Procedure: setDataBroker()

void NavPlotViewer::setDataBroker(ALogDataBroker dbroker)
{
  m_dbroker = dbroker;

  m_min_time = dbroker.getGlobalMinTime();
  m_max_time = dbroker.getGlobalMaxTime();
  
  m_vnames.clear();
  m_vtypes.clear();
  m_vlengths.clear();
  for(unsigned int aix=0; aix<m_dbroker.sizeALogs(); aix++) {
    string vehicle_name = m_dbroker.getVNameFromAix(aix);
    string vehicle_type = m_dbroker.getVTypeFromAix(aix);
    double vehicle_length = m_dbroker.getVLengthFromAix(aix);
    m_vnames.push_back(vehicle_name);
    m_vtypes.push_back(vehicle_type);
    m_vlengths.push_back(vehicle_length);
  }

}

//-------------------------------------------------------------
// Procedure: initPlots()

void NavPlotViewer::initPlots()
{
  unsigned int tmin = 0;  // Use later?
  unsigned int tmax = 0;

  unsigned int alogs = m_dbroker.sizeALogs();
  for(unsigned int aix=0; aix<alogs; aix++) {
    string vname = m_dbroker.getVNameFromAix(aix);
    unsigned int mix;

    mix = m_dbroker.getMixFromVNameVarName(vname, "NAV_X");
    LogPlot logplot_navx = m_dbroker.getLogPlot(mix, tmin, tmax);
    addLogPlotNAVX(logplot_navx);

    mix = m_dbroker.getMixFromVNameVarName(vname, "NAV_Y");
    LogPlot logplot_navy = m_dbroker.getLogPlot(mix, tmin, tmax);
    addLogPlotNAVY(logplot_navy);

    mix = m_dbroker.getMixFromVNameVarName(vname, "NAV_HEADING");
    LogPlot logplot_hdg = m_dbroker.getLogPlot(mix, tmin, tmax);
    addLogPlotHDG(logplot_hdg);
  }

  for(unsigned int aix=0; aix<alogs; aix++) {
    VPlugPlot vplot = m_dbroker.getVPlugPlot(aix, tmin, tmax);
    cout << "VPlot size: " << vplot.size() << endl;
    addVPlugPlot(vplot);
  }
}

//-------------------------------------------------------------
// Procedure: addLogPlotNAVX

void NavPlotViewer::addLogPlotNAVX(const LogPlot& lp)
{
  // First see if the new logplot expands the x or time bounds
  double lp_min_xpos = lp.getMinVal();
  double lp_max_xpos = lp.getMaxVal();
  if((m_navx_plot.size() == 0) || (lp_min_xpos < m_min_xpos))
    m_min_xpos = lp_min_xpos;
  if((m_navx_plot.size() == 0) || (lp_max_xpos > m_max_xpos))
    m_max_xpos = lp_max_xpos;

  // Then add the new logplot 
  m_navx_plot.push_back(lp);
}

//-------------------------------------------------------------
// Procedure: addLogPlotNAVY

void NavPlotViewer::addLogPlotNAVY(const LogPlot& lp)
{
  // First see if the new logplot expands the Y or Time bounds
  double lp_min_ypos = lp.getMinVal();
  double lp_max_ypos = lp.getMaxVal();
  if((m_navy_plot.size() == 0) ||(lp_min_ypos < m_min_ypos)) 
    m_min_ypos = lp_min_ypos;
  if((m_navy_plot.size() == 0) ||(lp_max_ypos > m_max_ypos)) 
    m_max_ypos = lp_max_ypos;

  // Then add the new logplot 
  m_navy_plot.push_back(lp);
}

//-------------------------------------------------------------
// Procedure: addLogPlotHDG

void NavPlotViewer::addLogPlotHDG(const LogPlot& lp)
{
  m_hdg_plot.push_back(lp);
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
}

//-------------------------------------------------------------
// Procedure: setCurrTime

void NavPlotViewer::setCurrTime(double gtime)
{
  // If min/max times are not set, just take the given time directly.
  // Otherwise clip the given time to be within the min/max range.
  if((m_min_time == -1) || (m_max_time == -1))
    m_curr_time = gtime;
  else
    m_curr_time = vclip(gtime, m_min_time, m_max_time);
  redraw();
}

//-------------------------------------------------------------
// Procedure: stepTime

void NavPlotViewer::stepTime(double amt)
{
  double newtime = m_curr_time;
  newtime = m_curr_time + amt;

  cout << "newtime" << newtime << endl;
  cout << "min_time:" << m_min_time << " max_time:" << m_max_time << endl;

  // If min/max times are not set, just take the given time directly.
  // Otherwise clip the given time to be within the min/max range.
  if((m_min_time != -1) || (m_max_time != -1))
    m_curr_time = vclip(newtime, m_min_time, m_max_time);
  redraw();
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
  double time_window = m_max_time - m_min_time;
  double time_hint   = m_min_time + (time_window / 4);
  return(time_hint);
}

//-------------------------------------------------------------
// Procedure: drawNavPlots

void NavPlotViewer::drawNavPlots()
{
  for(unsigned int i=0; i<m_navx_plot.size(); i++)
    drawNavPlot(i);
}

//-------------------------------------------------------------
// Procedure: drawNavPlot
//      Note: The [index] argument refers to the vehicle index.

void NavPlotViewer::drawNavPlot(unsigned int index)
{
  if(index >= m_navx_plot.size())
    return;
  
  if(m_navx_plot[index].size() == 0)
    return;

  double ctime = getCurrTime();

  // The size of the navx, navy, hdg vectors *should* all be the 
  // same, one added for each vehicle. Do some checking here anyway.
  double x = 0;
  double y = 0;
  double heading = 0;
  if(index < m_navx_plot.size())
    x = m_navx_plot[index].getValueByTime(ctime);
  if(index < m_navy_plot.size())
    y = m_navy_plot[index].getValueByTime(ctime);
  if(index < m_hdg_plot.size())
    heading = m_hdg_plot[index].getValueByTime(ctime);

  ColorPack  vehi_color = m_vehi_settings.getColorInactiveVehicle();
  vehi_color = m_vehi_settings.getColorActiveVehicle();
  
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
  
  if(vnames_mode == "names+depth") {
    string str_depth = "0";
    vehi_name += " (depth=" + str_depth + ")";
  }

  // We do not handle bearing reports - yet.
  BearingLine bng_line;
  drawCommonVehicle(record, bng_line, vehi_color, vname_color, vname_draw);
}


//-------------------------------------------------------------
// Procedure: drawTrails()

void NavPlotViewer::drawTrails()
{
  unsigned int i, vsize = m_navx_plot.size();
  for(i=0; i<vsize; i++)
    drawTrail(i);
}

//-------------------------------------------------------------
// Procedure: drawTrail()

void NavPlotViewer::drawTrail(unsigned int index)
{
  if(m_trails == "none")
    return;

  double ctime = getCurrTime();
  
  double    pt_size = m_vehi_settings.getTrailsPointSize();
  bool      connect = m_vehi_settings.isViewableTrailsConnect();
  ColorPack tcolor  = m_vehi_settings.getColorTrails();

  bool alltrail = false;
  if(m_trails == "all")
    alltrail = true;

  XYSegList segl;

  unsigned int i, npsize = m_navx_plot[index].size();
  for(i=0; i<npsize; i++) {
    double itime = m_navx_plot[index].getTimeByIndex(i);
    if(alltrail || (itime < ctime)) {
      double x = m_navx_plot[index].getValueByIndex(i);
      double y = m_navy_plot[index].getValueByIndex(i);
      segl.add_vertex(x, y);
    }
  }

  if(m_trails == "window") {
    double trails_length = m_vehi_settings.getTrailsLength();
    unsigned int points = segl.size();
    if(trails_length < points) {
      XYSegList new_segl;
      unsigned int cutpoints = points - (unsigned int)(trails_length);
      for(i=cutpoints; i<points; i++)
	new_segl.add_vertex(segl.get_vx(i), segl.get_vy(i));
      segl = new_segl;
    }
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
  vector<XYRangePulse> pulses  = geo_shapes.getRangePulses();
  const map<string, XYPoint>&  points  = geo_shapes.getPoints();
  const map<string, XYCircle>& circles = geo_shapes.getCircles();
  const map<string, XYMarker>& markers = geo_shapes.getMarkers();

  drawPolygons(polys);
  drawGrids(grids);
  drawSegLists(segls);
  drawCircles(circles);
  drawPoints(points);
  drawMarkers(markers);

  if(index < m_start_time.size()) {
    double utc_timestamp = m_start_time[index] + m_curr_time;
    drawRangePulses(pulses, utc_timestamp);
  }
}

//-------------------------------------------------------------
// Procedure: setCenterview
//      Note: 

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






