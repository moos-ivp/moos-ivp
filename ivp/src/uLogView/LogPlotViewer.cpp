/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogPlotViewer.cpp                                    */
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
#include "LogPlotViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "ColorParse.h"

using namespace std;

LogPlotViewer::LogPlotViewer(int gx, int gy, int gw, int gh, const char *gl)
  : Fl_Gl_Window(gx,gy,gw,gh,gl)
{
  m_valid_cache = false;
  view_index1 = 0;
  view_index2 = 0;
  m_margin    = 10.0;
  m_step      = 0.0;
  m_curr_time = 0.0;

  m_display_min_time = 0;
  m_display_max_time = 0;

  m_min_time_utc = 0;
  m_max_time_utc = 0;
}

//-------------------------------------------------------------
// Procedure: setMinMaxTimeUTC

void LogPlotViewer::setMinMaxTimeUTC(double mintime, double maxtime)
{
  if(mintime > maxtime)
    return;
  
  m_min_time_utc = mintime;
  m_max_time_utc = maxtime;

  m_display_min_time = m_min_time_utc;
  m_display_max_time = m_max_time_utc;
}

//-------------------------------------------------------------
// Procedure: handle()

int LogPlotViewer::handle(int event)
{
  int vx, vy;
  switch(event) {
  case FL_PUSH:
    vx = Fl::event_x();
    vy = h() - Fl::event_y();
    if(Fl_Window::handle(event) != 1) {
      if(Fl::event_button() == FL_LEFT_MOUSE)
	handleLeftMouse(vx, vy);
    }
  case FL_RELEASE:
    return(0);
    break;
  default:
    return(Fl_Gl_Window::handle(event));
  }
  return(0);
}

//-------------------------------------------------------------
// Procedure: draw()

void LogPlotViewer::draw()
{
  glClearColor(1.0, 1.0, 1.0, 0.0);
  //glClearColor(0.5,0.5,0.5 ,0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glViewport(0, 0, w(), h());

  drawLogPlot();
}

//-------------------------------------------------------------
// Procedure: resize()

void LogPlotViewer::resize(int gx, int gy, int gw, int gh)
{
  m_valid_cache = false;
  Fl_Gl_Window::resize(gx, gy, gw, gh);
}

//-------------------------------------------------------------
// Procedure: addLogPlot

unsigned int LogPlotViewer::addLogPlot(const LogTPlot& given_logplot)
{
#if 0  
  // Ensure that the extreme_min_time and extreme_max_time values are
  // the extremes over all added logplots.
  double this_min_time = given_logplot.getMinTime();
  double this_max_time = given_logplot.getMaxTime();
  if((m_logplots.size() == 0) || (this_min_time < m_min_time_utc))
    m_min_time_utc = this_min_time;
  if((m_logplots.size() == 0) || (this_max_time > m_max_time_utc))
    m_max_time_utc = this_max_time;

  // Every time a new plot is added, sync the display_max/min times
  // to the extreme_max/min times.

  m_display_min_time = m_min_time_utc;
  m_display_max_time = m_max_time_utc;
#endif

  LogTPlot new_logplot(given_logplot);
  m_logplots.push_back(new_logplot);

  return(m_logplots.size());
}

//-------------------------------------------------------------
// Procedure: setLeftPlot()

void LogPlotViewer::setLeftPlot(unsigned int index)
{
  if(index == view_index1)
    return;
  if(index < m_logplots.size())
    view_index1 = index;
  m_valid_cache = false;
}

//-------------------------------------------------------------
// Procedure: setRightPlot()

void LogPlotViewer::setRightPlot(unsigned int index)
{
  if(index == view_index2)
    return;
  if(index < m_logplots.size())
    view_index2 = index;
  m_valid_cache = false;
}

//-------------------------------------------------------------
// Procedure: getCurrVal1()

double LogPlotViewer::getCurrVal1(double gtime)
{
  if(m_logplots.size() == 0)
    return(0);
  else
    return(m_logplots[view_index1].getValueByTimeUTC(gtime));
}

//-------------------------------------------------------------
// Procedure: getCurrVal2()

double LogPlotViewer::getCurrVal2(double gtime)
{
  if(m_logplots.size() == 0)
    return(0);
  else
    return(m_logplots[view_index2].getValueByTimeUTC(gtime));
}

//-------------------------------------------------------------
// Procedure: getVariable1

string LogPlotViewer::getVariable1()
{
  if(view_index1 >= m_logplots.size())
    return("time!");
  else {
    string vehicle = m_logplots[view_index1].getVName();
    string varname = m_logplots[view_index1].getVarName();
    if(vehicle != "")
      return(vehicle + "/" + varname);
    else
      return(varname);
  }
}

//-------------------------------------------------------------
// Procedure: getVariable2

string LogPlotViewer::getVariable2()
{
  if(view_index1 == view_index2)
    return("none");

  if(view_index2 >= m_logplots.size())
    return("none");
  else {
    string vehicle = m_logplots[view_index2].getVName();
    string varname = m_logplots[view_index2].getVarName();
    if(vehicle != "")
      return(vehicle + "/" + varname);
    else
      return(varname);
  }
}

//-------------------------------------------------------------
// Procedure: getMinVal1()

string LogPlotViewer::getMinVal1()
{
  if((view_index1 < 0) || (view_index1 >= m_logplots.size()))
    return("n/a");
  else
    return(doubleToString(m_logplots[view_index1].getMinVal(), 3));
}

//-------------------------------------------------------------
// Procedure: getMaxVal1()

string LogPlotViewer::getMaxVal1()
{
  if(view_index1 >= m_logplots.size())
    return("n/a");
  else
    return(doubleToString(m_logplots[view_index1].getMaxVal(), 3));
} 


//-------------------------------------------------------------
// Procedure: getMinVal2()

string LogPlotViewer::getMinVal2()
{
  if(view_index1 == view_index2)
    return("n/a");

  if((view_index2 < 0) || (view_index2 >= m_logplots.size()))
    return("n/a");
  else
    return(doubleToString(m_logplots[view_index2].getMinVal(), 3));
}

//-------------------------------------------------------------
// Procedure: getMaxVal2()

string LogPlotViewer::getMaxVal2()
{
  if(view_index1 == view_index2)
    return("n/a");

  if((view_index2 < 0) || (view_index2 >= m_logplots.size()))
    return("n/a");
  else
    return(doubleToString(m_logplots[view_index2].getMaxVal(), 3));
}

//-------------------------------------------------------------
// Procedure: handleLeftMouse

int LogPlotViewer::handleLeftMouse(int vx, int vy)
{
  double pct = (double)(vx)/w();
  double newtime = m_min_time_utc;
  newtime += pct * (m_max_time_utc - m_min_time_utc);
  
  setCurrTime(newtime);

  redraw();
  return(0);
}

//-------------------------------------------------------------
// Procedure: setCurrTime

void LogPlotViewer::setCurrTime(double new_time)
{
  if(new_time < m_min_time_utc)
    new_time = m_min_time_utc;
  if(new_time > m_max_time_utc)
    new_time = m_max_time_utc;

  if(new_time < m_display_min_time) {
    m_display_max_time -= (m_display_min_time - new_time);
    m_display_min_time = new_time;
    m_valid_cache = false;
  }

  if(new_time > m_display_max_time) {
    m_display_min_time += (new_time - m_display_max_time);
    m_display_max_time = new_time;
    m_valid_cache = false;
  }

  m_curr_time = new_time;
}

//-------------------------------------------------------------
// Procedure: adjustZoom

void LogPlotViewer::adjustZoom(string ztype)
{
  // Ensure there is never a zero denominator time_band
  if(m_display_max_time <= m_display_min_time)
    return;

  // Ensure curr_time marker is always in the viewable range.
  if(m_curr_time < m_display_min_time)
    m_curr_time = m_display_min_time;
  if(m_curr_time > m_display_max_time)
    m_curr_time = m_display_max_time;

  double time_band = (m_display_max_time - m_display_min_time);
  double pct_band  = (m_curr_time - (m_display_min_time)) / time_band;
  
  if(ztype == "in") {
    double crop_amount = time_band * 0.20;
    double lower_crop_time = crop_amount * pct_band;
    double upper_crop_time = crop_amount * (1-pct_band);
    if((m_display_max_time - m_display_min_time) >2.5) {
      m_display_min_time += lower_crop_time;
      m_display_max_time -= upper_crop_time;
      m_valid_cache = false;
    }
  }
  else if(ztype == "out") {
    double pad_amount = time_band * 0.25;
    double lower_pad_time = pad_amount * pct_band;
    double upper_pad_time = pad_amount * (1-pct_band);
    m_display_min_time -= lower_pad_time;
    m_display_max_time += upper_pad_time;
    if(m_display_min_time < m_min_time_utc)
      m_display_min_time = m_min_time_utc;
    if(m_display_max_time > m_max_time_utc)
      m_display_max_time = m_max_time_utc;
    m_valid_cache = false;
  }
  else if(ztype == "reset") {
    m_display_min_time = m_min_time_utc;
    m_display_max_time = m_max_time_utc;
    m_valid_cache = false;
  }
  redraw();
}

//-------------------------------------------------------------
// Procedure: fillCache

bool LogPlotViewer::fillCache()
{
  m_valid_cache = false;
  if((view_index1 < 0) || (view_index1 >= m_logplots.size()))
    return(false);
  if((view_index2 < 0) || (view_index2 >= m_logplots.size()))
    return(false);
  m_valid_cache = true;
  
  LogTPlot vplot1 = m_logplots[view_index1];
  LogTPlot vplot2 = m_logplots[view_index2];

  cache_x1.clear();
  cache_y1.clear();
  cache_x2.clear();
  cache_y2.clear();

  m_step = (w()-m_margin) / (m_display_max_time - m_display_min_time);

  unsigned int i, vpsize1  = vplot1.size();
  double min_val1 = vplot1.getMinVal();
  double max_val1 = vplot1.getMaxVal();
  double h_step1 = (h()-m_margin) / (max_val1  - min_val1);

  bool excepted_right = false;
  for(i=0; i<vpsize1; i++) {
    double rawt = vplot1.getTimeUTCByIndex(i);
    bool add_to_cache = false;
    if((rawt >= m_display_min_time) && (rawt <= m_display_max_time))
      add_to_cache = true;
    else if(rawt <= m_display_min_time) {
      cache_x1.clear();
      cache_y1.clear();
      add_to_cache = true;
    }
    else if(!excepted_right) {
      add_to_cache = true;
      excepted_right = true;
    }

    if(add_to_cache) {
      double rawv = vplot1.getValueByIndex(i);
      double scalet = ((rawt - m_display_min_time)  * m_step) + (m_margin/2.0); 
      double scalev = ((rawv - min_val1 ) * h_step1) + (m_margin/2.0);
      cache_x1.push_back(scalet);
      cache_y1.push_back(scalev);
    }
  }

  // If the two plots are the same index, we're done.
  if(view_index1 == view_index2) 
    return(true);

  // Otherwise, fill in the cache for the second index
  unsigned int j, vpsize2  = vplot2.size();
  double min_val2 = vplot2.getMinVal();
  double max_val2 = vplot2.getMaxVal();
  double h_step2 = (h()-m_margin) / (max_val2  - min_val2);
  
  excepted_right = false;
  for(j=0; j<vpsize2; j++) {      
    double rawt = vplot2.getTimeUTCByIndex(j);
    bool add_to_cache = false;
    if((rawt >= m_display_min_time) && (rawt <= m_display_max_time))
      add_to_cache = true;
    else if(rawt <= m_display_min_time) {
      cache_x2.clear();
      cache_y2.clear();
      add_to_cache = true;
    }
    else if(!excepted_right) {
      add_to_cache = true;
      excepted_right = true;
    }
    
    if(add_to_cache) {
      double rawv = vplot2.getValueByIndex(j);
      double scalet = ((rawt - m_display_min_time)  * m_step) + (m_margin/2.0); 
      double scalev = ((rawv - min_val2 ) * h_step2) + (m_margin/2.0);
      cache_x2.push_back(scalet);
      cache_y2.push_back(scalev);
    }
  }
  return(true);
}


//-------------------------------------------------------------
// Procedure: drawLogPlot
//  483d8b   .282 .239 .545   DarSlateBlue
//  8b0000   .545 .000 .000   DarkRed
//  2f4f4f   .184 .310 .310   DarkSlateGrey
//  daa520   .855 .647 .125   GoldenRod
//  e9967a   .914 .588 .478   DarkSalmon
//  8fbc8f   .561 .737 .561   DarkSeaGreen
//  bdb76b                    DarkKhaki
//  fafad2   .980 .980 .824   LightGoldenRodYellow

void LogPlotViewer::drawLogPlot()
{
  if(!m_valid_cache)
    fillCache();
  if(!m_valid_cache)
    return;
    
  float r, g, b;
  r=0.2; g=0.8; b=0.2;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  int color_scheme = 2;

  glLineWidth(1.0);
  unsigned int i, cache_size = cache_x1.size();

  // ----- Handle the FIRST Plot ----------
  // Draw the LINES of the plot
  if(color_scheme == 1)
    glColor4f(0.282,  0.239,  0.545,  0.1);  // DarkSlateBlue
  else
    glColor4f(0.433,  0.520,  0.284,  0.1);  // DarkOliveGreen-ish
  for(i=0; i<cache_size; i++) {
    if(i>0) {
      glBegin(GL_LINE_STRIP);
      glVertex2f(cache_x1[i-1], cache_y1[i-1]);
      glVertex2f(cache_x1[i], cache_y1[i-1]);
      glVertex2f(cache_x1[i], cache_y1[i]);
      glEnd();
    }
  }

  // Draw the POINTS of the plot
  if(cache_size > 75)  glPointSize(2.0);
  else if(cache_size > 55) glPointSize(3.0);
  else if(cache_size > 40) glPointSize(4.0);
  else if(cache_size > 35) glPointSize(5.0);
  else if(cache_size > 30) glPointSize(6.0);
  else if(cache_size > 25) glPointSize(7.0);
  else  glPointSize(8.0);
  if(color_scheme == 1)
    glColor4f(0.561,  0.737,  0.561,  0.1);  // DarkSeaGreen
  else
    glColor4f(0.433,  0.520,  0.284,  0.1);  // DarkOliveGreen-ish
  glBegin(GL_POINTS);
  for(i=0; i<cache_size; i++)
    glVertex2f(cache_x1[i], cache_y1[i]);
  glEnd();

  if(view_index1 != view_index2) {
    unsigned int i, cache_size = cache_x2.size();
    // ----- Handle the SECOND Plot ----------
    // Draw the LINES of the plot
    if(color_scheme == 1)
      glColor4f(0.545,  0.000,  0.000,  0.1);  // DarkRed
    else
      glColor4f(0.282,  0.239,  0.545,  0.1);  // DarkSlateBlue
    glBegin(GL_LINE_STRIP);
    for(i=0; i<cache_size; i++) {
      if(i>0) {
	glBegin(GL_LINE_STRIP);
	glVertex2f(cache_x2[i-1], cache_y2[i-1]);
	glVertex2f(cache_x2[i], cache_y2[i-1]);
	glVertex2f(cache_x2[i], cache_y2[i]);
	glEnd();
      }
    }

    // Draw the POINTS of the plot
    if(cache_size > 75)  glPointSize(2.0);
    else if(cache_size > 55) glPointSize(3.0);
    else if(cache_size > 40) glPointSize(4.0);
    else if(cache_size > 35) glPointSize(5.0);
    else if(cache_size > 30) glPointSize(6.0);
    else if(cache_size > 25) glPointSize(7.0);
    else  glPointSize(8.0);
    if(color_scheme == 1)
      glColor4f(0.99,  0.99,  0.99,  0.1);  // WHite
    else
      glColor4f(0.282,  0.239,  0.545,  0.1);  // DarkSlateBlue
    glBegin(GL_POINTS);
    for(unsigned int i=0; i<cache_x2.size(); i++)
      glVertex2f(cache_x2[i], cache_y2[i]);
    glEnd();
  }

  // Draw the "current time" vertical bar
  double scalet = ((m_curr_time - m_display_min_time) * m_step) + 
    (m_margin/2.0); 
  glColor4f(1.0, 0.0, 0.0, 0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(scalet, 0);
  glVertex2f(scalet, h());
  glEnd();
  
  glFlush();
  glPopMatrix();
}









