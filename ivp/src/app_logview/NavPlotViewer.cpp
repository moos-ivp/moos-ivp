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
#include "KayakShapes.h"

using namespace std;

NavPlotViewer::NavPlotViewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
{
  m_global_ix    = 0;
  m_local_ix     = 0;
  m_trail_gap    = 1;
  m_trail_size   = 1;
  m_alltrail     = true;
  m_vehibody     = "kayak";  // "auv" or "kayak"
}

//-------------------------------------------------------------
// Procedure: draw()

void NavPlotViewer::draw()
{
  MarineViewer::draw();

  if(m_poly_offon)
    drawPolys();

  drawNavPlots();
  drawFrame();
}

// ----------------------------------------------------------
// Procedure: getMetersX
//   Purpose: For a given x position, return its position, in 
//            terms of delta meters from the zero position.

#if 1
float NavPlotViewer::getMetersX()
{
  if(m_cross_offon) {
    int iwidth = m_back_img.get_img_width();
    float x_pos = ((float)(iwidth) / 2.0) - (float)(m_vshift_x);
    float x_pct = m_back_img.pixToPctX(x_pos);
    float x_pct_cent = m_back_img.get_img_centx();
    float x_pct_mtrs = m_back_img.get_img_meters();
    float meters = (x_pct - x_pct_cent) / (x_pct_mtrs / 100.0);
    float offset = m_back_img.get_img_offset_x();
    meters += offset;
    return(meters);
  }
  else
    return(m_navplots[m_global_ix].get_xval_by_index(m_local_ix));
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
    float offset = m_back_img.get_img_offset_y();
    meters += offset;
    return(meters);
  }
  else
    return(m_navplots[m_global_ix].get_yval_by_index(m_local_ix));
}
#endif

//-------------------------------------------------------------
// Procedure: setCurrIndex
//      Note: returns true if the value changes

bool NavPlotViewer::setCurrIndex(int v)
{
  int new_index = v;

  int max_new_index = m_navplots[m_global_ix].get_size() - 1;

  if(v < 0)
    new_index = 0;
  else if(v >= max_new_index) 
    new_index = max_new_index;

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

bool NavPlotViewer::jumpCurrIndex(int v)
{
  if(v==0)
    return(setCurrIndex(0));
  else if(v==1) {
    int end_ix = m_navplots[m_global_ix].get_size() - 1;
    return(setCurrIndex(end_ix));
  }
  else {
    int end_ix = m_navplots[m_global_ix].get_size() / 2;
    return(setCurrIndex(end_ix));
  }
}


//-------------------------------------------------------------
// Procedure: vehicle

void NavPlotViewer::setGlobalIndex(int new_ix)
{
  cout << "In NavPlotViewer::setGlobalIndex() " << new_ix << endl;
  if(new_ix < 0)
    new_ix = 0;
  if(new_ix >= m_navplots.size())
    new_ix = m_navplots.size()-1;
  m_global_ix = new_ix;
}

//-------------------------------------------------------------
// Procedure: addNavPlot
//      Note: A local copy of the given navplot is created here.

void NavPlotViewer::addNavPlot(const NavPlot &given_navplot)
{
  NavPlot new_navplot(given_navplot);
  m_navplots.push_back(new_navplot); 
  m_local_ix=0;
}


//-------------------------------------------------------------
// Procedure: getCurrTime

float NavPlotViewer::getCurrTime()
{
  if(m_navplots.size() > 0)
    return(m_navplots[m_global_ix].get_time_by_index(m_local_ix));
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
  if(m_navplots.size() == 0)
    return(0);

  float max_time = m_navplots[m_global_ix].get_max_time();
  float min_time = m_navplots[m_global_ix].get_min_time();
  float plt_size = m_navplots[m_global_ix].get_size();
  
  return((max_time - min_time) / (plt_size - 1));
}

//-------------------------------------------------------------
// Procedure: drawNavPlots

void NavPlotViewer::drawNavPlots()
{
  for(int i=0; i<m_navplots.size(); i++)
    drawNavPlot(i);
}

//-------------------------------------------------------------
// Procedure: drawNavPlot

void NavPlotViewer::drawNavPlot(int index)
{
  int npsize = m_navplots[index].get_size();
  if(npsize == 0)
    return;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  double ctime = getCurrTime();
  
  int tmp_trail_gap = m_trail_gap;

  // Draw all the non_current points
  if(m_trails) {
    glPointSize(m_trail_size * m_zoom);

#if 0
      glColor3f(0.0, 0.0, 0.5);  // darkblue
#endif

#if 1
    if(index == m_global_ix)
      glColor3f(1.0, 1.0, 1.0);  // white
    else
      glColor3f(1.0, 1.0, 1.0);  // white
#endif

#if 0
    if(index == 0)
      glColor3f(0.5, 0.0, 0.0);  // DarkRed
    else if(index == 1)
      glColor3f(0.0, 0.5, 0.0);  // DarkGreen
    else if(index == 2)
      glColor3f(0.0, 0.0, 0.5);  // DarkBlue
    else
      glColor3f(0.7, 0.7, 0.7);  // Lighter
    if(index==2)
      m_trail_gap += 5;
#endif

#if 0
    glColor3f(0.0, 0.0, 0.0);  // Lighter
#endif

    glBegin(GL_POINTS);
    for(int i=0; i<npsize; i++) {
      double itime = m_navplots[index].get_time_by_index(i);
      if(m_alltrail || (itime < ctime)) {
	
	//if((ctime - itime) < 110) {
	if(1) {
	  if(i != m_local_ix) {
	    if((i % m_trail_gap) == 0) {
	      double x = m_navplots[index].get_xval_by_index(i);
	      double y = m_navplots[index].get_yval_by_index(i);
	      glVertex2f(x,y);
	    }
	  }
	}
      }
    }
    glEnd();
  }
  m_trail_gap = tmp_trail_gap;

  // Draw the current_index point, larger and w/ diff color
  double x, y, theta;
  if(index == m_global_ix) {
    x     = m_navplots[index].get_xval_by_index(m_local_ix);
    y     = m_navplots[index].get_yval_by_index(m_local_ix);
    theta = m_navplots[index].get_cval_by_index(m_local_ix);
  }
  else {
    double curr_time = getCurrTime();
    x     = m_navplots[index].get_xval_by_time(curr_time);
    y     = m_navplots[index].get_yval_by_time(curr_time);
    theta = m_navplots[index].get_cval_by_time(curr_time);
  }

  bool show_kayaks = true;

  if(!show_kayaks) {
    glPointSize(m_trail_size * 3.0 * m_zoom);
    glColor3f(1.0,0.0,0.0);
    glBegin(GL_POINTS);
    glVertex2f(x,y);
    glEnd();
  }
  else {
    glTranslatef(x, y, 0);     // these are in pixel units
    glScalef(m_shape_scale, m_shape_scale, m_shape_scale);
    glRotatef(-theta,0,0,1);  

    if(m_vehibody == "auv") {
      drawGLPoly(g_auvBody, g_auvBodySize, 1.0, 0.843, 0.0);
      drawGLPoly(g_auvBody, g_auvBodySize, 0.0, 0.0,   0.0, 2.0);
      drawGLPoly(g_propUnit, g_propUnitSize, 0.0,0.0, 1.0);
    }
    else {
      if(index==0)
	drawGLPoly(g_kayakBody, g_kayakBodySize, 1.0, 0.906, 0.243);
      else if(index==1)
	drawGLPoly(g_kayakBody, g_kayakBodySize, 1.0, 0.0, 0.0);
      else if(index==2)
	drawGLPoly(g_kayakBody, g_kayakBodySize, 0.0, 1.0, 0.0);
      else if(index==3)
	drawGLPoly(g_kayakBody, g_kayakBodySize, 0.0, 0.0, 1.0);
      else
	drawGLPoly(g_kayakBody, g_kayakBodySize, 1.0, 0.906, 0.243);
      drawGLPoly(g_kayakMidOpen, g_kayakMidOpenSize, 0.5, 0.5, 0.5);
      drawGLPoly(g_kayakBody, g_kayakBodySize, 0.0, 0.0, 0.0, 1.0); // HERE
    }
  }

#if 0
    if(index == m_global_ix)
      drawGLPoly(g_kayakBody, g_kayakBodySize, 1.0, 0.0, 0.0);
    else
      drawGLPoly(g_kayakBody, g_kayakBodySize, 1.0, 0.906, 0.243);
    drawGLPoly(g_kayakMidOpen, g_kayakMidOpenSize, 0.5, 0.5, 0.5);
#endif

  glFlush();
  glPopMatrix();
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







