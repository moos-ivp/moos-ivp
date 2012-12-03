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

#include <string>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#ifdef _WIN32
#include "tiffio.h"
#else
#include <tiffio.h>
#endif
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
  m_hplot_left_ix  = 0;
  m_hplot_right_ix = 0;
  m_hash_shade     = 0.35;

  m_trails         = "to-present"; // "none, to-present, window, all"
  m_trail_gap      = 1;

  m_curr_time      = 0;
  m_min_time       = -1;
  m_max_time       = -1;
  m_step_by_secs   = true;

  // Bounding box of all vehicle positions over all time
  m_min_xpos       = 0;
  m_min_ypos       = 0;
  m_max_xpos       = 0;
  m_max_ypos       = 0;

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

  else if(param == "cycle_active") {
    if(m_hplot_left_ix < (m_navx_plot.size() - 1))
      m_hplot_left_ix++;
    else
      m_hplot_left_ix = 0;
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
// Procedure: addLogPlotNAVX

void NavPlotViewer::addLogPlotNAVX(const LogPlot& lp)
{
  // First see if the new logplot expands the x or time bounds
  double lp_min_xpos = lp.get_min_val();
  double lp_max_xpos = lp.get_max_val();
  double lp_min_time = lp.get_min_time();
  double lp_max_time = lp.get_max_time();
  if(m_navx_plot.size() == 0) {
    m_min_xpos = lp_min_xpos;
    m_max_xpos = lp_max_xpos;
    m_min_time = lp_min_time;
    m_max_time = lp_max_time;
  }
  else {
    if(lp_min_xpos < m_min_xpos)
      m_min_xpos = lp_min_xpos;
    if(lp_max_xpos > m_max_xpos)
      m_max_xpos = lp_max_xpos;
    if(lp_min_time < m_min_time)
      m_min_time = lp_min_time;
    if(lp_max_time > m_max_time)
      m_max_time = lp_max_time;
  }

  // Then add the new logplot 
  m_navx_plot.push_back(lp);

  // mikerb
  //m_curr_time = lp_min_time + ((lp_max_time-lp_min_time)/4);
}

//-------------------------------------------------------------
// Procedure: addLogPlotNAVY

void NavPlotViewer::addLogPlotNAVY(const LogPlot& lp)
{
  // First see if the new logplot expands the Y or Time bounds
  double lp_min_ypos = lp.get_min_val();
  double lp_max_ypos = lp.get_max_val();
  double lp_min_time = lp.get_min_time();
  double lp_max_time = lp.get_max_time();
  if(m_navy_plot.size() == 0) {
    m_min_ypos = lp_min_ypos;
    m_max_ypos = lp_max_ypos;
    m_min_time = lp_min_time;
    m_max_time = lp_max_time;
  }
  else {
    if(lp_min_ypos < m_min_ypos)
      m_min_ypos = lp_min_ypos;
    if(lp_max_ypos > m_max_ypos)
      m_max_ypos = lp_max_ypos;
    if(lp_min_time < m_min_time)
      m_min_time = lp_min_time;
    if(lp_max_time > m_max_time)
      m_max_time = lp_max_time;
  }

  // Then add the new logplot 
  m_navy_plot.push_back(lp);
}

//-------------------------------------------------------------
// Procedure: addLogPlotHDG

void NavPlotViewer::addLogPlotHDG(const LogPlot& lp)
{
  m_hdg_plot.push_back(lp);
  double lp_min_time = lp.get_min_time();
  double lp_max_time = lp.get_max_time();
  if((m_min_time == -1) || (lp_min_time < m_min_time))
    m_min_time = lp_min_time;
  if((m_max_time == -1) || (lp_max_time > m_max_time))
    m_max_time = lp_max_time;
}

//-------------------------------------------------------------
// Procedure: addLogPlotStartTime

void NavPlotViewer::addLogPlotStartTime(double start_time)
{
  m_start_time.push_back(start_time);
}

//-------------------------------------------------------------
// Procedure: addHelmPlot

unsigned int NavPlotViewer::addHelmPlot(const HelmPlot& hp)
{
  // Specia case: If this is the 2nd plot added, then it is 
  // convenient to set this plot to be displayed in the right pane.
  if(m_helm_plot.size() == 1)
    m_hplot_right_ix = 1;

  m_helm_plot.push_back(hp);
  double hp_min_time = hp.get_min_time();
  double hp_max_time = hp.get_max_time();
  if((m_min_time == -1) || (hp_min_time < m_min_time))
    m_min_time = hp_min_time;
  if((m_max_time == -1) || (hp_max_time > m_max_time))
    m_max_time = hp_max_time;

  return(m_helm_plot.size());
}

//-------------------------------------------------------------
// Procedure: addVPlugPlot

void NavPlotViewer::addVPlugPlot(const VPlugPlot& gp)
{
  m_vplug_plot.push_back(gp);
  double gp_min_time = gp.getMinTime();
  double gp_max_time = gp.getMaxTime();
  if((m_min_time == -1) || (gp_min_time < m_min_time))
    m_min_time = gp_min_time;
  if((m_max_time == -1) || (gp_max_time > m_max_time))
    m_max_time = gp_max_time;
}


//-------------------------------------------------------------
// Procedure: draw()

void NavPlotViewer::draw()
{
  MarineViewer::draw();
  drawTrails();
  drawNavPlots();
  drawVPlugPlots();
  drawFrame();
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
  if(m_step_by_secs)
    newtime = m_curr_time + amt;
  else {
    if(amt < 0) {
      unsigned int iter_steps = (unsigned int)(amt * -1);
      newtime = m_helm_plot[m_hplot_left_ix].get_time_by_iter_sub(m_curr_time, iter_steps);
    }
    else {
      unsigned int iter_steps = (unsigned int)(amt);
      newtime = m_helm_plot[m_hplot_left_ix].get_time_by_iter_add(m_curr_time, iter_steps);
    }
  }

  // If min/max times are not set, just take the given time directly.
  // Otherwise clip the given time to be within the min/max range.
  if((m_min_time != -1) || (m_max_time != -1))
    m_curr_time = vclip(newtime, m_min_time, m_max_time);
  

}

//-------------------------------------------------------------
// Procedure: setHPlotLeftIndex

void NavPlotViewer::setHPlotLeftIndex(unsigned int new_ix)
{
  if(new_ix >= m_helm_plot.size())
    return;
  m_hplot_left_ix = new_ix;
}

//-------------------------------------------------------------
// Procedure: setHPlotRightIndex

void NavPlotViewer::setHPlotRightIndex(unsigned int new_ix)
{
  if(new_ix >= m_helm_plot.size())
    return;
  m_hplot_right_ix = new_ix;
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
// Procedure: getHPlotVName

string NavPlotViewer::getHPlotVName(const string& side)
{
  unsigned int ix = m_hplot_left_ix;
  if(side == "right") 
    ix = m_hplot_right_ix;
  if(ix >= m_helm_plot.size())
    return("");

  string vname = m_helm_plot[ix].get_vehi_name();
  string iter  = m_helm_plot[ix].get_value_by_time("iter", m_curr_time);
  return(iter + "-" + vname);
}

//-------------------------------------------------------------
// Procedure: getHPlotDecision

string NavPlotViewer::getHPlotDecision(const string& side)
{
  unsigned int ix = m_hplot_left_ix;
  if(side == "right") 
    ix = m_hplot_right_ix;
  if(ix >= m_helm_plot.size())
    return("");

  return(m_helm_plot[ix].get_value_by_time("decision", m_curr_time));
}

//-------------------------------------------------------------
// Procedure: getHPlotMode

string NavPlotViewer::getHPlotMode(const string& side)
{
  unsigned int ix = m_hplot_left_ix;
  if(side == "right") 
    ix = m_hplot_right_ix;
  if(ix >= m_helm_plot.size())
    return("");

  return(m_helm_plot[ix].get_value_by_time("mode_short", m_curr_time));
}

//-------------------------------------------------------------
// Procedure: getHPlotBehaviors

string NavPlotViewer::getHPlotBehaviors(const string& side, 
					const string& bhv_type)
{
  unsigned int ix = m_hplot_left_ix;
  if(side == "right") 
    ix = m_hplot_right_ix;
  if(ix >= m_helm_plot.size())
    return("");

  string bhvs = m_helm_plot[ix].get_value_by_time(bhv_type, m_curr_time);
  if(!m_behaviors_verbose)
    bhvs = shortenBehaviors(bhvs);
  return(bhvs);
}

//-------------------------------------------------------------
// Procedure: getVIterMap

map<string, unsigned int> NavPlotViewer::getVIterMap()
{
  map<string, unsigned int> viter_map;
  unsigned int i, vsize = m_helm_plot.size();
  for(i=0; i<vsize; i++) {
    string vname = m_helm_plot[i].get_vehi_name();
    unsigned int iter = m_helm_plot[i].get_iter_by_time(m_curr_time);
    viter_map[vname] = iter;
  }

  return(viter_map);
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
    x = m_navx_plot[index].get_value_by_time(ctime);
  if(index < m_navy_plot.size())
    y = m_navy_plot[index].get_value_by_time(ctime);
  if(index < m_hdg_plot.size())
    heading = m_hdg_plot[index].get_value_by_time(ctime);

  ColorPack  vehi_color = m_vehi_settings.getColorInactiveVehicle();
  if(index == m_hplot_left_ix) 
    vehi_color = m_vehi_settings.getColorActiveVehicle();
  
  ColorPack vname_color = m_vehi_settings.getColorVehicleName();  
  string    vnames_mode = m_vehi_settings.getVehiclesNameMode();
  double    shape_scale = m_vehi_settings.getVehiclesShapeScale();

  string    vehi_type   = "unknown";
  double    vehi_length = 20;
  string    vehi_name   = "unknown";

  unsigned int hp_size = m_helm_plot.size();
  if(index < hp_size) {
    vehi_type   = m_helm_plot[index].get_vehi_type();
    vehi_length = m_helm_plot[index].get_vehi_length() * shape_scale;
    vehi_name   = m_helm_plot[index].get_vehi_name();
  }

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
    double itime = m_navx_plot[index].get_time_by_index(i);
    if(alltrail || (itime < ctime)) {
      if((i % m_trail_gap) == 0) {
	double x = m_navx_plot[index].get_value_by_index(i);
	double y = m_navy_plot[index].get_value_by_index(i);
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
// Procedure: drawFrame
//      Note: Frame string format: "640x480+0+0"

void NavPlotViewer::drawFrame()
{
  if(m_frame == "")
    return;
  
  vector<string> svector = parseString(m_frame, '+');
  if(svector.size() != 3)
    return;
  
  int x = atoi(svector[1].c_str());
  int y = atoi(svector[2].c_str());
  
  svector = parseString(svector[0], 'x');
  if(svector.size() != 2)
    return;

  int wid = atoi(svector[0].c_str());
  int hgt = atoi(svector[1].c_str());
  
  y = ((h() - hgt) - y);

  x = x-1;
  wid = wid + 2;
  y = y-1;
  hgt = hgt + 2;

  glBegin(GL_LINE_STRIP);
  glColor3f(1.000, 0.800, 1.000);
  glVertex2f(x, y);
  glVertex2f(x+wid, y);
  glVertex2f(x+wid, y+hgt);
  glVertex2f(x, y+hgt);
  glVertex2f(x, y);
  glEnd();
}


//-------------------------------------------------------------
// Procedure: shortenBehaviors
//   Purpose: Produce a shortened version of a behavior list
//   
//  "waypt_survey$0.5$0/0:waypt_return$0.5$0/0" --> 
//                                  "waypt_survey, waypt_return"

string NavPlotViewer::shortenBehaviors(string bhvs)
{
  string rstr;
  vector<string> kvector = parseString(bhvs, ':');
  unsigned int k, ksize  = kvector.size();
  for(k=0; k<ksize; k++) {
    if(rstr != "")
      rstr += ", ";
    rstr += biteString(kvector[k], '$');
  }
  return(rstr);
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
    double min_x, min_y, max_x, max_y;
    unsigned int i, x_size = m_navx_plot.size();
    for(i=0; i<x_size; i++) {
      double this_value = m_navx_plot[i].get_median();
      if((i==0) || (this_value < min_x))
	min_x = this_value;
      if((i==0) || (this_value > max_x))
	max_x = this_value;
    }    
    unsigned int j, y_size = m_navy_plot.size();
    for(j=0; j<y_size; j++) {
      double this_value = m_navy_plot[j].get_median();
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



