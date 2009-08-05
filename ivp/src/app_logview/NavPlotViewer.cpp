/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
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
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
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

NavPlotViewer::NavPlotViewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
{
  m_vehicle_ix   = 0;
  m_data_ix      = 0;
  m_trail_gap    = 1;
  m_trail_size   = 0.5;
  m_alltrail     = true;
  m_shape_scale  = 1;
  m_trails       = true;
  m_draw_vname   = false;
}

//-------------------------------------------------------------
// Procedure: setParam

bool NavPlotViewer::setParam(string param, string value)
{
  if(param == "trails_point_size") {
    if(value == "bigger")
      m_trail_size *= 1.25;
    else if(value == "smaller")
      m_trail_size *= 0.80;
    else if(value == "reset")
      m_trail_size = 1.0;
    m_trail_size = vclip(m_trail_size, 1, 25);
    return(true);
  }

  if(param == "center_view") {
    if(value == "average")
      setCenterView("ctr_of_bounding");
    return(true);
  }

  if(MarineViewer::setParam(param, value))
    return(true);
  
  if(param == "trails_viewable")
    return(setBooleanOnString(m_trails, value));
  else if(param=="display_vname")
    return(setBooleanOnString(m_draw_vname, value));
  else if(param == "vehicle_shape_scale") {
    if(value == "bigger")
      m_shape_scale *= 1.25;
    else if(value == "smaller")
      m_shape_scale *= 0.80;
    else if(value == "reset")
      m_shape_scale = 1.0;
  }
  else
    return(false);
  
  return(true);
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
// Procedure: draw()

void NavPlotViewer::draw()
{
  MarineViewer::draw();

  drawPolygons();
  drawNavPlots();
  drawFrame();
}

//-------------------------------------------------------------
// Procedure: setCurrIndex
//      Note: returns true if the value changes
//      Note: uint-check

bool NavPlotViewer::setCurrIndex(int v)
{
  if(m_navx_plot.size() == 0)
    return(false);

  unsigned int new_index = 0;
  if(v >= 0)
    new_index = (unsigned int)(v);
  
  unsigned int plotsize = m_navx_plot[m_vehicle_ix].size();

  if(plotsize == 0)
    return(false);

  if(new_index >= plotsize) 
    new_index = plotsize - 1;

  if(m_data_ix != new_index) {
    m_data_ix = new_index;
    return(true);
  }
  else
    return(false);
}

//-------------------------------------------------------------
// Procedure: incCurrIndex
//      Note: returns true if the value changes

bool NavPlotViewer::incCurrIndex(int v)
{
  if(m_navx_plot.size() == 0)
    return(false);

  unsigned int max_index = m_navx_plot[m_vehicle_ix].size();
  
  int new_index = m_data_ix + v;
  new_index = (int)(vclip(new_index, 0, max_index));
    
  return(setCurrIndex(m_data_ix + v));
}

//-------------------------------------------------------------
// Procedure: setCurrIndexByTime
//      Note: returns true if the value changes

bool NavPlotViewer::setCurrIndexByTime(double gtime)
{
  if(m_navx_plot.size() == 0)
    return(false);

  cout << "m_navx_plot.size(): " << m_navx_plot.size() << endl;
  unsigned int ix = m_navx_plot[m_vehicle_ix].get_index_by_time(gtime);
  redraw();

  return(setCurrIndex(ix));
}


//-------------------------------------------------------------
// Procedure: jumpCurrIndex
//      Note: returns true if the value changes
//            0 sets index at the beginning
//            1 sets index at the end
//            2 sets index at the middle

bool NavPlotViewer::jumpCurrIndex(unsigned int v)
{
  if(m_navx_plot.size() == 0)
    return(false);

  if(v==0)
    return(setCurrIndex(0));
  else if(v==1) {
    int end_ix = m_navx_plot[m_vehicle_ix].size() - 1;
    return(setCurrIndex(end_ix));
  }
  else {
    int end_ix = m_navx_plot[m_vehicle_ix].size() / 2;
    return(setCurrIndex(end_ix));
  }
}

//-------------------------------------------------------------
// Procedure: vehicle

void NavPlotViewer::setGlobalIndex(unsigned int new_ix)
{
  if(m_navx_plot.size() == 0)
    return;

  if(new_ix < 0)
    new_ix = 0;
  if(new_ix >= m_navx_plot.size())
    new_ix = m_navx_plot.size()-1;
  m_vehicle_ix = new_ix;
}

//-------------------------------------------------------------
// Procedure: getCurrTime

double NavPlotViewer::getCurrTime()
{
  if(m_navx_plot.size() > 0)
    return(m_navx_plot[m_vehicle_ix].get_time_by_index(m_data_ix));
  else
    return(0);
}

//-------------------------------------------------------------
// Procedure: getAvgStepTime
//      Note: Returns the avg delta time between indices in the 
//            NavPlot. Useful for determining a delay in between
//            index increments to simulate "real-time" stepping.

double NavPlotViewer::getAvgStepTime()
{
  if(m_navx_plot.size() == 0)
    return(0);
  
  double max_time = m_navx_plot[m_vehicle_ix].get_max_time();
  double min_time = m_navx_plot[m_vehicle_ix].get_min_time();
  double plt_size = m_navx_plot[m_vehicle_ix].size();
  
  return((max_time - min_time) / (plt_size - 1));
}

//-------------------------------------------------------------
// Procedure: getHPlotVName

string NavPlotViewer::getHPlotVName()
{
  return(m_helm_plot[m_vehicle_ix].get_vehi_name());
}

#if 0
//-------------------------------------------------------------
// Procedure: drawPoints

void NavPlotViewer::drawPoints(const vector<double>& vx, 
			       const vector<double>& vy,
			       double cr, double cg, double cb, double sz)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // Determine position in terms of image percentage
  double pt_ix = meters2img('x', px);
  double pt_iy = meters2img('y', py);

  // Determine position in terms of view percentage
  double pt_vx = img2view('x', pt_ix);
  double pt_vy = img2view('y', pt_iy);

  glTranslatef(pt_vx, pt_vy, 0); // theses are in pixel units

  glEnable(GL_POINT_SMOOTH);
  glPointSize(sz);
  glColor3f(cr, cg, cb);
  
  glBegin(GL_POINTS);
  glVertex2f(0, 0);
  glEnd();
  glDisable(GL_POINT_SMOOTH);

  glFlush();
  glPopMatrix();
}
#endif

//-------------------------------------------------------------
// Procedure: drawNavPlots

void NavPlotViewer::drawNavPlots()
{
  for(unsigned int i=0; i<m_navx_plot.size(); i++)
    drawNavPlot(i);
}

//-------------------------------------------------------------
// Procedure: drawNavPlot

void NavPlotViewer::drawNavPlot(unsigned int index)
{
  if((index < 0) || (index >= m_navx_plot.size()))
    return;

  int npsize = m_navx_plot[index].size();
  if(npsize == 0)
    return;

  double ctime = getCurrTime();
  
  // Draw all the non_current points
  if(m_trails) {
    //double pt_size = m_trail_size * m_zoom;
    double pt_size = m_trail_size;
    vector<double> cvect = colorParse("red");

    if(index == 0) 
      cvect = colorParse("green");
    else if(index == 1) 
      cvect = colorParse("red");
    else if(index == 2)
      cvect = colorParse("yellow");
    else if(index == 3) 
      cvect = colorParse("white");
    
    vector<double> xvect, yvect;
    for(int i=0; i<npsize; i++) {
      double itime = m_navx_plot[index].get_time_by_index(i);
      if(m_alltrail || (itime < ctime)) {
	if((i % m_trail_gap) == 0) {
	  xvect.push_back(m_navx_plot[index].get_value_by_index(i));
	  yvect.push_back(m_navy_plot[index].get_value_by_index(i));
	}
      }
    }
    drawPointList(xvect, yvect, pt_size, cvect);
  }

  // [index] refers to the vehicle index.

  // The size of the navx, navy, hdg vectors *should* all be the 
  // same, one added for each vehicle. Do some checking here anyway.
  double x = 0;
  double y = 0;
  double theta = 0;
  if(index < m_navx_plot.size())
    x = m_navx_plot[index].get_value_by_time(ctime);
  if(index < m_navy_plot.size())
    y = m_navy_plot[index].get_value_by_time(ctime);
  if(index < m_hdg_plot.size())
    theta = m_hdg_plot[index].get_value_by_time(ctime);

  ObjectPose opose(x,y,theta,0,0);
  vector<double> cvect = colorParse("1.0, 0.906, 0.243");
  if(index==1) 
    cvect = colorParse("red");    
  if(index==2)
    cvect = colorParse("green");
  if(index==2) 
    cvect = colorParse("blue");

  vector<double> vname_color(3,1); // Vehicle name will be drawn "white"

  // Not sure how we handle vehicle lengths - hard code 3 meters for now.
  string vehi_body = m_helm_plot[index].get_vehi_type();
  double shape_length = m_helm_plot[index].get_vehi_length() * m_shape_scale;


  drawCommonVehicle("", opose, cvect, vname_color, vehi_body, 
		    shape_length, m_draw_vname);
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
  double pix_per_mtr = m_back_img.get_pix_per_mtr();

  double x_pixels = pix_per_mtr * delta_x;
  double y_pixels = pix_per_mtr * delta_y;
  
  m_vshift_x = -x_pixels;
  m_vshift_y = -y_pixels;
}




