/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NavPlotViewer.cpp                                    */
/*    DATE: May 31st, 2005                                       */
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
#include <string>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "NavPlotViewer.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "GeomUtils.h"
#include "IO_GeomUtils.h"
#ifdef _WIN32
#include "tiffio.h"
#else
#include <tiffio.h>
#endif

using namespace std;

//---------------------------------------------------------------
// Constructor

NavPlotViewer::NavPlotViewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
{
  m_hash_offon     = true;
  m_hash_shade     = 0.35;

  m_trails         = "to-present"; // "none, to-present, window, all"
  m_trail_gap      = 1;

  m_curr_time      = 0;
  m_min_time_utc   = 0;
  m_max_time_utc   = 0;

  // Bounding box of all vehicle positions over all time
  m_min_xpos       = 0;
  m_min_ypos       = 0;
  m_max_xpos       = 0;
  m_max_ypos       = 0;

  m_time_format_utc = true;
}

//-------------------------------------------------------------
// Procedure: setMinMaxTimeUTC

void NavPlotViewer::setMinMaxTimeUTC(double mintime, double maxtime)
{
  if(mintime > maxtime)
    return;
  
  m_min_time_utc = mintime;
  m_max_time_utc = maxtime;
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
  
  else if(param == "time_format") {
    if(value=="utc")
      m_time_format_utc = true;
    else if(value == "local")
      m_time_format_utc = false;
    else if(value == "toggle")
      m_time_format_utc = !m_time_format_utc;
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
  
  else if(param == "trail_gap") {
    if(m_trail_gap + value >= 1)      
      m_trail_gap += (int)value;
  }
  else
    return(false);
  
  return(true);
}

//-------------------------------------------------------------
// Procedure: addElement

void NavPlotViewer::addElement(const NavPlotElement& element)
{
  m_elements.push_back(element);
  unsigned int vsize = m_elements.size(); 

  if((vsize == 1) || (element.getMinX() < m_min_xpos))
    m_min_xpos = element.getMinX();
  if((vsize == 1) || (element.getMaxX() > m_max_xpos))
    m_max_xpos = element.getMaxX();
  if((vsize == 1) || (element.getMinY() < m_min_ypos))
    m_min_ypos = element.getMinY();
  if((vsize == 1) || (element.getMaxY() > m_max_ypos))
    m_max_ypos = element.getMaxY();
}


//-------------------------------------------------------------
// Procedure: draw()

void NavPlotViewer::draw()
{
  MarineViewer::draw();
  drawTrails();
  drawNavPlots();
  drawVPlugPlots();
  if(m_hash_offon) {
    drawHash(m_min_xpos-2000, m_max_xpos+2000, 
	     m_min_ypos-2000, m_max_ypos+2000);
  }
}

//-------------------------------------------------------------
// Procedure: setCurrTime

bool NavPlotViewer::setCurrTime(double gtime, bool post)
{
  double prev_time = m_curr_time;
  m_curr_time = vclip(gtime, m_min_time_utc, m_max_time_utc);

  if(m_curr_time != prev_time) {
    redraw();
    if(post)
      m_pbuffer.pushPending("ALOGVIEW_TIME", m_curr_time);
    return(true);
  }
  return(false);
}

//-------------------------------------------------------------
// Procedure: stepTime

bool NavPlotViewer::stepTime(double amt)
{
  double newtime  = m_curr_time + amt;
  return(setCurrTime(newtime));
}

//-------------------------------------------------------------
// Procedure: getCurrTime

double NavPlotViewer::getCurrTime()
{
  return(m_curr_time);
}

//-------------------------------------------------------------
// Procedure: getCurrTimeStr

string NavPlotViewer::getCurrTimeStr()
{
  if(m_time_format_utc)
    return(doubleToString(m_curr_time, 2));
  else
    return(doubleToString((m_curr_time - m_min_time_utc), 2));
}

//-------------------------------------------------------------
// Procedure: drawNavPlots

void NavPlotViewer::drawNavPlots()
{
  unsigned int i, vsize = m_elements.size();
  for(i=0; i<vsize; i++)
    drawNavPlot(i);
}

//-------------------------------------------------------------
// Procedure: drawNavPlot
//      Note: The [index] argument refers to the vehicle index.

void NavPlotViewer::drawNavPlot(unsigned int index)
{
  if(index >= m_elements.size())
    return;

  double ctime = getCurrTime();

  const LogTPlot& navx_plot = m_elements[index].getLogPlotX();
  const LogTPlot& navy_plot = m_elements[index].getLogPlotY();
  const LogTPlot& hdg_plot  = m_elements[index].getLogPlotHDG();

  double x = navx_plot.getValueByTimeUTC(ctime);
  double y = navy_plot.getValueByTimeUTC(ctime);
  double heading = hdg_plot.getValueByTimeUTC(ctime);

  ColorPack vehi_color  = m_vehi_settings.getColorActiveVehicle();
  ColorPack vname_color = m_vehi_settings.getColorVehicleName();  
  string    vnames_mode = m_vehi_settings.getVehiclesNameMode();
  double    shape_scale = m_vehi_settings.getVehiclesShapeScale();

  string    vehi_name   = m_elements[index].getVName();
  string    vehi_type   = m_elements[index].getVType();
  double    vehi_length = m_elements[index].getVLength();

  if(vehi_name == "")
    vehi_name = "V" + uintToString(index);
  if(vehi_type == "")
    vehi_type = "glider";
  if(vehi_length == 0)
    vehi_length = 5;

  // Apply the shape scale
  vehi_length *= shape_scale;

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
  drawCommonVehicle(record, bng_line, vehi_color, vname_color, 
		    vname_draw);
}


//-------------------------------------------------------------
// Procedure: drawTrails()

void NavPlotViewer::drawTrails()
{
  unsigned int i, vsize = m_elements.size();
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

  const LogTPlot& logplotx = m_elements[index].getLogPlotX();
  const LogTPlot& logploty = m_elements[index].getLogPlotY();
  unsigned int i, vsize = logplotx.size();
  for(i=0; i<vsize; i++) {
    double itime = logplotx.getTimeUTCByIndex(i);
    if(alltrail || (itime < ctime)) {
      if((i % m_trail_gap) == 0) {
	double x = logplotx.getValueByIndex(i);
	//double y = logploty.getValueByTime(itime); // safer????
	double y = logploty.getValueByIndex(i); 
	segl.add_vertex(x, y);
      }
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
  unsigned int i, vsize = m_elements.size();
  for(i=0; i<vsize; i++)
    drawVPlugPlot(i);
}

//-------------------------------------------------------------
// Procedure: drawVPlugPlot

void NavPlotViewer::drawVPlugPlot(unsigned int index)
{
  if(index >= m_elements.size())
    return;
  
  const VPlugTPlot& vplug_plot = m_elements[index].getVPlugPlot();
  VPlug_GeoShapes   geo_shapes = vplug_plot.getVPlugByTimeUTC(m_curr_time);

  vector<XYPolygon>    polys   = geo_shapes.getPolygons();
  vector<XYGrid>       grids   = geo_shapes.getGrids();
  vector<XYPoint>      points  = geo_shapes.getPoints();
  vector<XYSegList>    segls   = geo_shapes.getSegLists();
  vector<XYCircle>     circles = geo_shapes.getCircles();
  vector<XYRangePulse> pulses  = geo_shapes.getRangePulses();
  vector<XYMarker>     markers = geo_shapes.getMarkers();

  drawPolygons(polys);
  drawGrids(grids);
  drawSegLists(segls);
  drawCircles(circles);
  drawPoints(points);
  drawMarkers(markers);

  double logstart_utc = m_elements[index].getLogStartUTC();
  double utc_timestamp = logstart_utc + m_curr_time;
  drawRangePulses(pulses, utc_timestamp);
}

//-------------------------------------------------------------
// Procedure: setCenterview
//      Note: 

void NavPlotViewer::setCenterView(string centering_style)
{
  double pos_x = m_min_xpos + ((m_max_xpos - m_min_xpos) / 2);
  double pos_y = m_min_ypos + ((m_max_ypos - m_min_ypos) / 2);

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

