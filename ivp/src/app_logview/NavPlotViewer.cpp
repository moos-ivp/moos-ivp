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
#include <tiffio.h>
#include "NavPlotViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "IO_GeomUtils.h"

using namespace std;

NavPlotViewer::NavPlotViewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
{
  m_global_ix    = 0;
  m_gridplot_ix  = 0;
  m_local_ix     = 0;
  m_trail_gap    = 1;
  m_trail_size   = 1;
  m_alltrail     = true;
  m_vehibody     = "kayak";  // "auv" or "kayak"
}

//-------------------------------------------------------------
// Procedure: addLogPlot

void NavPlotViewer::addLogPlot(const LogPlot& lp, string vid, string type)
{
  type = tolower(type);
  if((type == "nav_x") || (type == "navx"))
    m_plotmap_navx[vid] = lp;
  else if((type == "nav_y") || (type == "navy"))
    m_plotmap_navy[vid] = lp;
  else if((type == "nav_heading") || (type == "navheading"))
    m_plotmap_hdg[vid] = lp;
}

//-------------------------------------------------------------
// Procedure: draw()

void NavPlotViewer::draw()
{
  MarineViewer::draw();

  if(m_poly_offon)
    drawPolys();

  if(m_grid_offon)
    drawGridPlots();

  drawNavPlots();
  drawFrame();
}

// ----------------------------------------------------------
// Procedure: getMetersX
//   Purpose: For a given x position, return its position, in 
//            terms of delta meters from the zero position.

float NavPlotViewer::getMetersX()
{
  if(m_cross_offon) {
    int iwidth = m_back_img.get_img_width();
    float x_pos = ((float)(iwidth) / 2.0) - (float)(m_vshift_x);
    float x_pct = m_back_img.pixToPctX(x_pos);
    float x_pct_cent = m_back_img.get_img_centx();
    float x_pct_mtrs = m_back_img.get_img_meters();
    float meters = (x_pct - x_pct_cent) / (x_pct_mtrs / 100.0);
    return(meters);
  }
  else {
    if(m_global_ix < m_navx_plot.size()) {
      return(m_navx_plot[m_global_ix].get_value_by_index(m_local_ix));
    }
    else
      return(0);
  }
}

// ----------------------------------------------------------
// Procedure: getMetersY
//   Purpose: For a given y position, return its position, in 
//            terms of delta meters from the zero position.

float NavPlotViewer::getMetersY()
{
  if(m_cross_offon) {
    int iheight = m_back_img.get_img_height();
    float y_pos = ((float)(iheight) / 2.0) - (float)(m_vshift_y);
    float y_pct = m_back_img.pixToPctY(y_pos);
    float y_pct_cent = m_back_img.get_img_centy();
    float y_pct_mtrs = m_back_img.get_img_meters();
    float meters = (y_pct - y_pct_cent) / (y_pct_mtrs / 100.0);
    return(meters);
  }
  else {
    if(m_global_ix < m_navx_plot.size()) {
      return(m_navy_plot[m_global_ix].get_value_by_index(m_local_ix));
    }
    else
      return(0);
  }
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
  
  unsigned int plotsize = m_navx_plot[m_global_ix].size();

  if(plotsize == 0)
    return(false);

  if(new_index >= plotsize) 
    new_index = plotsize - 1;

  if(m_local_ix != new_index) {
    m_local_ix = new_index;
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
  return(setCurrIndex(m_local_ix + v));
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
    int end_ix = m_navx_plot[m_global_ix].size() - 1;
    return(setCurrIndex(end_ix));
  }
  else {
    int end_ix = m_navx_plot[m_global_ix].size() / 2;
    return(setCurrIndex(end_ix));
  }
}


//-------------------------------------------------------------
// Procedure: setGridPlotIndex

void NavPlotViewer::setGridPlotIndex(unsigned int ix)
{
  if((ix < 0) || (ix >= m_gridplots.size()))
    return;
  else
    m_gridplot_ix = ix;
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
  m_global_ix = new_ix;
}

//-------------------------------------------------------------
// Procedure: addGridPlot
//      Note: A local copy of the given Gridplot is created here.

int NavPlotViewer::addGridPlot(const GridPlot &given_gridplot)
{
  GridPlot new_gridplot(given_gridplot);
  m_gridplots.push_back(new_gridplot); 
  return(m_gridplots.size());
}


//-------------------------------------------------------------
// Procedure: getCurrTime

float NavPlotViewer::getCurrTime()
{
  if(m_navx_plot.size() > 0)
    return(m_navx_plot[m_global_ix].get_time_by_index(m_local_ix));
  else
    return(0);
}

//-------------------------------------------------------------
// Procedure: getAvgStepTime
//      Note: Returns the avg delta time between indices in the 
//            NavPlot. Useful for determining a delay in between
//            index increments to simulate "real-time" stepping.

float NavPlotViewer::getAvgStepTime()
{
  if(m_navx_plot.size() == 0)
    return(0);
  
  float max_time = m_navx_plot[m_global_ix].get_max_time();
  float min_time = m_navx_plot[m_global_ix].get_min_time();
  float plt_size = m_navx_plot[m_global_ix].size();
  
  return((max_time - min_time) / (plt_size - 1));
}

//-------------------------------------------------------------
// Procedure: drawPoint

void NavPlotViewer::drawPoint(float px, float py, float cr, 
			      float cg, float cb, float sz)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // Determine position in terms of image percentage
  float pt_ix = meters2img('x', px);
  float pt_iy = meters2img('y', py);

  // Determine position in terms of view percentage
  float pt_vx = img2view('x', pt_ix);
  float pt_vy = img2view('y', pt_iy);

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

//-------------------------------------------------------------
// Procedure: drawNavPlots

void NavPlotViewer::drawNavPlots()
{
  for(unsigned int i=0; i<m_navx_plot.size(); i++)
    drawNavPlot(i);

#if 0
  map<string, LogPlot>::iterator p;
  p = m_plotmap
#endif

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
    double pt_size = m_trail_size * m_zoom;
    double pt_red  = 0;
    double pt_grn  = 0;
    double pt_blu  = 0;

    if(index == 0) {
      pt_red=0.5; pt_grn=0; pt_blu=0;
    }
    else if(index == 1) {
      pt_red=0; pt_grn=0.5; pt_blu=0;
    }
    else if(index == 2){
      pt_red=0; pt_grn=0; pt_blu=0.5;
    }
    else if(index == 3) {
      pt_red=0.7; pt_grn=0.7; pt_blu=0.7;
    }
    
    for(int i=0; i<npsize; i++) {
      double itime = m_navx_plot[index].get_time_by_index(i);
      if(m_alltrail || (itime < ctime)) {
	if((i % m_trail_gap) == 0) {
	  double x = m_navx_plot[index].get_value_by_index(i);
	  double y = m_navy_plot[index].get_value_by_index(i);
	  drawPoint(x, y, pt_red, pt_grn, pt_blu, pt_size);
	}
      }
    }
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
  double red=1.0, grn=0.906, blu=0.243;
  if(index==1) 
    {red=1.0; grn=0; blu=0;}
  if(index==2)
    {red=0; grn=1; blu=0;}
  if(index==2) 
    {red=0; grn=0; blu=2;}
  drawCommonVehicle("", opose, red, grn, blu, m_vehibody);
}


//-------------------------------------------------------------
// Procedure: drawGridPlots

void NavPlotViewer::drawGridPlots()
{
  if(m_gridplots.size() == 0)
    return;

  drawGridPlot(m_gridplot_ix);
}


//-------------------------------------------------------------
// Procedure: drawGridPlot

void NavPlotViewer::drawGridPlot(unsigned int index)
{
  int npsize = m_gridplots[index].size();
  if(npsize == 0)
    return;

  double ctime = getCurrTime();

  XYGrid grid = m_gridplots[index].getGridByTime(ctime);
  drawGrid(grid);
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








