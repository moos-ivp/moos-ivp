/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogPlotViewer.cpp                                    */
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

// Defined to silence GL deprecation warnings in OSX 10.14+
#define GL_SILENCE_DEPRECATION

#include <string>
#include <iostream>
#include "FL/Fl.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "LogPlotViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"

using namespace std;

LogPlotViewer::LogPlotViewer(int gx, int gy, int gw, int gh, const char *gl)
  : Fl_Gl_Window(gx,gy,gw,gh,gl)
{
  m_valid_cache = false;
  m_sync_scales = false;
  m_margin    = 10.0;
  m_step      = 0.0;
  m_curr_time = 0.0;

  m_lft_marg = 80;
  m_rgt_marg = 80;
  m_bot_marg = 20;

  m_left_mix  = 99999;
  m_right_mix = 99999;

  m_show_left_logplot=true;
  m_show_right_logplot=true;

  m_fullvar1 = "none";
  m_fullvar2 = "none";

  m_zoomed_in = false;

  m_extreme_min_time = 0;
  m_extreme_max_time = 0;
  m_display_min_time = 0;
  m_display_max_time = 0;

  // The use_high_res_GL function is supported in more recent FLTK
  // packages. FLTK on older non-MacOS systems may not have this
  // feature. It is mostly needed to support Mac Retina displays.
#ifdef __APPLE__
  Fl::use_high_res_GL(1);
#endif
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
    break;
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

  // The pixel_w/h() functions are supported in more recent FLTK
  // packages. FLTK on older non-MacOS systems may not have this
  // feature. It is mostly needed to support Mac Retina displays.
#ifdef __APPLE__
  glViewport(0, 0, pixel_w(), pixel_h());
#else
  glViewport(0, 0, w(), h());
#endif

  drawLogPlot();
  drawTimeText();
  drawMinMaxVarText();
}

//-------------------------------------------------------------
// Procedure: resize()

void LogPlotViewer::resize(int gx, int gy, int gw, int gh)
{
  m_valid_cache = false;
  Fl_Gl_Window::resize(gx, gy, gw, gh);
}

//-------------------------------------------------------------
// Procedure: setDataBroker()

void LogPlotViewer::setDataBroker(ALogDataBroker dbroker)
{
  m_dbroker = dbroker;
}

//-------------------------------------------------------------
// Procedure: setLeftPlot()

void LogPlotViewer::setLeftPlot(unsigned int mix)
{
  cout << "================================================" << endl;
  cout << "LogPlotViewer::setLeftPlot() mix: " << mix << endl;
  cout << "================================================" << endl;
  
  cout << "LogPlotViewer::setLeftPlot() m_left_mix: " << m_left_mix << endl;
  // Check if mix represents a change and a viable change
  if((mix == m_left_mix) || (mix >= m_dbroker.sizeMix()))
    return;

  // old style: used to "shut off" one side. Now can do that with
  // the checkbuttons.
#if 0
  // Check if new left is same as right. If so, make left empty
  if(mix != m_right_mix) {
    m_logplot1 = m_dbroker.getLogPlot(mix); 
    string vname = m_dbroker.getVNameFromMix(mix);
    string varname = m_dbroker.getVarNameFromMix(mix);
    m_fullvar1 = vname + "/" + varname;
  }
  else {
    m_logplot1 = LogPlot();
    m_fullvar1 = "none";
  }
#endif
    
#if 1
  m_logplot1 = m_dbroker.getLogPlot(mix); 
  string vname = m_dbroker.getVNameFromMix(mix);
  string varname = m_dbroker.getVarNameFromMix(mix);
  m_fullvar1 = vname + "/" + varname;
#endif
    
  m_left_mix = mix;
  m_valid_cache = false;

  cout << "LogPlotViewer::setLeftPlot() new m_left_mix: " << m_left_mix << endl;
  if(!m_zoomed_in)
    adjustTimeBounds();
}

//-------------------------------------------------------------
// Procedure: setRightPlot()

void LogPlotViewer::setRightPlot(unsigned int mix)
{
  // Check if mix represents a change and a viable change
  if((mix == m_right_mix) || (mix >= m_dbroker.sizeMix()))
    return;

  // old style: used to "shut off" one side. Now can do that with
  // the checkbuttons.
#if 0
 // Check if new right is same as left. If so, make left empty
  if(mix != m_left_mix) {
    m_logplot2 = m_dbroker.getLogPlot(mix);
    string vname = m_dbroker.getVNameFromMix(mix);
    string varname = m_dbroker.getVarNameFromMix(mix);
    m_fullvar2 = vname + "/" + varname;
  }    
  else {
    m_logplot2 = LogPlot();
    m_fullvar2 = "none";
  }
#endif
#if 1
  m_logplot2 = m_dbroker.getLogPlot(mix);
  string vname = m_dbroker.getVNameFromMix(mix);
  string varname = m_dbroker.getVarNameFromMix(mix);
  m_fullvar2 = vname + "/" + varname;
#endif
  
  m_right_mix = mix;
  m_valid_cache = false;

  if(!m_zoomed_in)
    adjustTimeBounds();
}

//-------------------------------------------------------------
// Procedure: get_curr_val1()

double LogPlotViewer::get_curr_val1(double gtime)
{
  if(m_logplot1.size() == 0)
    return(0);
  return(m_logplot1.getValueByTime(gtime));
}

//-------------------------------------------------------------
// Procedure: get_curr_val2()

double LogPlotViewer::get_curr_val2(double gtime)
{
  if(m_logplot2.size() == 0)
    return(0);
  return(m_logplot2.getValueByTime(gtime));
}

//-------------------------------------------------------------
// Procedure: getMinVal1()

double LogPlotViewer::getMinVal1()
{
  if(m_logplot1.size() == 0)
    return(0);

  double min_val1 = m_logplot1.getMinVal();
  if(!m_sync_scales || (m_logplot2.size() == 0))
    return(min_val1);
  
  double min_val2 = m_logplot2.getMinVal();
  double min_val  = min_val1 < min_val2 ? min_val1 : min_val2;
  return(min_val);  
}

//-------------------------------------------------------------
// Procedure: getMaxVal1()

double LogPlotViewer::getMaxVal1()
{
  if(m_logplot1.size() == 0)
    return(0);

  double max_val1 = m_logplot1.getMaxVal();
  if(!m_sync_scales || (m_logplot2.size() == 0))
    return(max_val1);

  double max_val2 = m_logplot2.getMaxVal();
  double max_val  = max_val1 > max_val2 ? max_val1 : max_val2;
  return(max_val);
}

//-------------------------------------------------------------
// Procedure: getMinVal2()

double LogPlotViewer::getMinVal2()
{
  if(m_logplot2.size() == 0)
    return(0);

  double min_val2 = m_logplot2.getMinVal();
  if(!m_sync_scales || (m_logplot1.size() == 0))
    return(min_val2);

  double min_val1 = m_logplot1.getMinVal();
  double min_val  = min_val1 < min_val2 ? min_val1 : min_val2;
  return(min_val);
}

//-------------------------------------------------------------
// Procedure: getMaxVal2()

double LogPlotViewer::getMaxVal2()
{
  if(m_logplot2.size() == 0)
    return(0);

  double max_val2 = m_logplot2.getMaxVal();
  if(!m_sync_scales || (m_logplot1.size() == 0))
    return(max_val2);

  double max_val1 = m_logplot1.getMaxVal();
  double max_val  = max_val1 > max_val2 ? max_val1 : max_val2;
  return(max_val);
}

//-------------------------------------------------------------
// Procedure: handleLeftMouse

void LogPlotViewer::handleLeftMouse(int vx, int vy)
{
  double pct = (double)(vx-m_lft_marg)/(w()-m_lft_marg-m_rgt_marg);
  
  if(pct < 0)
    pct = 0;
  else if(pct > 1.0)
    pct = 1.0;

  m_curr_time = pct * (m_display_max_time - m_display_min_time);
  m_curr_time += m_display_min_time;
  redraw();
}

//-------------------------------------------------------------
// Procedure: setCurrTime

void LogPlotViewer::setCurrTime(double new_time)
{
  if(new_time < m_extreme_min_time)
    new_time = m_extreme_min_time;
  if(new_time > m_extreme_max_time)
    new_time = m_extreme_max_time;

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
    m_zoomed_in = true;
  }
  else if(ztype == "out") {
    double pad_amount = time_band * 0.25;
    double lower_pad_time = pad_amount * pct_band;
    double upper_pad_time = pad_amount * (1-pct_band);
    m_display_min_time -= lower_pad_time;
    m_display_max_time += upper_pad_time;
    if(m_display_min_time < m_extreme_min_time) {
      m_display_min_time = m_extreme_min_time;
      m_zoomed_in = false;
    }
    if(m_display_max_time > m_extreme_max_time) {
      m_display_max_time = m_extreme_max_time;
      m_zoomed_in = false;
    }
    m_valid_cache = false;
  }
  else if(ztype == "reset") {
    m_display_min_time = m_extreme_min_time;
    m_display_max_time = m_extreme_max_time;
    m_valid_cache = false;
    m_zoomed_in = false;
  }
  redraw();
}

//-------------------------------------------------------------
// Procedure: fillCache

bool LogPlotViewer::fillCache()
{
  // Part 1: Always clear both caches 
  cache_x1.clear();
  cache_y1.clear();
  cache_x2.clear();
  cache_y2.clear();
  m_valid_cache = true;

  // Recaclulate the step
  m_step = (w()-m_margin-m_lft_marg-m_rgt_marg) / 
    (m_display_max_time - m_display_min_time);

  double min_val1 = getMinVal1();
  double max_val1 = getMaxVal1();
  double min_val2 = getMinVal2();
  double max_val2 = getMaxVal2();

  // Part 2: Handle the Left LogPlot (LogPlot1)
  unsigned int vpsize1 = m_logplot1.size();
  double h_step1  = (h()-m_margin-m_bot_marg) / (max_val1  - min_val1);

  bool excepted_right = false;
  for(unsigned int i=0; i<vpsize1; i++) {
    double rawt = m_logplot1.getTimeByIndex(i);
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
      double rawv = m_logplot1.getValueByIndex(i);
      double scalet = ((rawt - m_display_min_time)  * m_step) + (m_margin/2.0); 
      scalet += m_lft_marg;
      double scalev = ((rawv - min_val1 ) * h_step1) + (m_margin/2.0);
      scalev += m_bot_marg;
      if((scalet >= m_lft_marg) && (scalet <= w()-m_rgt_marg)) {
	cache_x1.push_back(scalet);
	cache_y1.push_back(scalev);
      }
    }
  }


  // Part 3: Handle the Right LogPlot (LogPlot2)
  unsigned int vpsize2 = m_logplot2.size();
  double h_step2 = (h()-m_margin-m_bot_marg) / (max_val2  - min_val2);
  
  excepted_right = false;
  for(unsigned int j=0; j<vpsize2; j++) {      
    double rawt = m_logplot2.getTimeByIndex(j);
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
      double rawv = m_logplot2.getValueByIndex(j);
      double scalet = ((rawt - m_display_min_time)  * m_step) + (m_margin/2.0); 
      scalet += m_lft_marg;
      double scalev = ((rawv - min_val2 ) * h_step2) + (m_margin/2.0);
      scalev += m_bot_marg;
      if((scalet >= m_lft_marg) && (scalet <= w()-m_rgt_marg)) {
	cache_x2.push_back(scalet);
	cache_y2.push_back(scalev);
      }
    }
  }

  return(true);
}

//-------------------------------------------------------------
// Procedure: setSyncScales

void LogPlotViewer::setSyncScales(string str)
{
  if(str == "toggle")
    m_sync_scales = !m_sync_scales;
  else if(str == "off")
    m_sync_scales = false;
  else if(str == "on")
    m_sync_scales = true;
  m_valid_cache = false;
  redraw();
}

//-------------------------------------------------------------
// Procedure: adjustTimeBounds

void LogPlotViewer::adjustTimeBounds()
{
  m_extreme_min_time = 0;
  m_extreme_max_time = 0;

  if(m_logplot1.size() > 0) {
    double min_time = m_logplot1.getMinTime();
    if((m_extreme_min_time == 0) || (min_time < m_extreme_min_time))
      m_extreme_min_time = min_time;
    double max_time = m_logplot1.getMaxTime();
    if((m_extreme_max_time == 0) || (max_time > m_extreme_max_time))
      m_extreme_max_time = max_time;
  }

  if(m_logplot2.size() > 0) {
    double min_time = m_logplot2.getMinTime();
    if((m_extreme_min_time == 0) || (min_time < m_extreme_min_time))
      m_extreme_min_time = min_time;
    double max_time = m_logplot2.getMaxTime();
    if((m_extreme_max_time == 0) || (max_time > m_extreme_max_time))
      m_extreme_max_time = max_time;
  }

  m_display_min_time = m_extreme_min_time;
  m_display_max_time = m_extreme_max_time;
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
    
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  int color_scheme = 2;

  glLineWidth(1.0);

  // Draw Left Margin
  glColor4f(0.8,  0.8,  0.8,  0.1); 
  glBegin(GL_POLYGON);
  glVertex2f(0, 0);
  glVertex2f(m_lft_marg, 0);
  glVertex2f(m_lft_marg, h());
  glVertex2f(0, h());
  glEnd();
  // Draw Bottom Margin
  glColor4f(0.8,  0.8,  0.8,  0.1); 
  glBegin(GL_POLYGON);
  glVertex2f(0, 0);
  glVertex2f(w(), 0);
  glVertex2f(w(), m_bot_marg);
  glVertex2f(0, m_bot_marg);
  glEnd();
  // Draw Right Margin
  glColor4f(0.8,  0.8,  0.8,  0.1); 
  glBegin(GL_POLYGON);
  glVertex2f(w()-m_rgt_marg, 0);
  glVertex2f(w(), 0);
  glVertex2f(w(), h());
  glVertex2f(w()-m_rgt_marg, h());
  glEnd();

  // Draw the left border line
  glColor4f(0.35,  0.35,  0.4,  0.1); 
  glBegin(GL_LINE_STRIP);
  glVertex2f(m_lft_marg, m_bot_marg-5);
  glVertex2f(m_lft_marg, h());
  glEnd();
      
  // Draw the right border line
  glColor4f(0.35,  0.35,  0.4,  0.1); 
  glBegin(GL_LINE_STRIP);
  glVertex2f(w()-m_rgt_marg, m_bot_marg-5);
  glVertex2f(w()-m_rgt_marg, h());
  glEnd();
      
  // Draw the top border line
  glColor4f(0.35,  0.35,  0.4,  0.1); 
  glBegin(GL_LINE_STRIP);
  glVertex2f(m_lft_marg-5, h()-1);
  glVertex2f(w()-m_rgt_marg+5, h()-1);
  glEnd();
      
  // Draw the bottom borderline
  glColor4f(0.35,  0.35,  0.4,  0.1); 
  glBegin(GL_LINE_STRIP);
  glVertex2f(m_lft_marg-5, m_bot_marg);
  glVertex2f(w()-m_rgt_marg+5, m_bot_marg);
  glEnd();
      
  // ----- Handle the FIRST Plot ----------
  if(m_show_left_logplot && (m_logplot1.size()!=0)) {
    unsigned int cache_size = cache_x1.size();
    
    // Draw the LINES of the plot
    if(color_scheme == 1)
      glColor4f(0.282,  0.239,  0.545,  0.1);  // DarkSlateBlue
    else
      glColor4f(0.433,  0.520,  0.284,  0.1);  // DarkOliveGreen-ish
    for(unsigned int i=0; i<cache_size; i++) {
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
    else  glPointSize(6.0);
    if(color_scheme == 1)
      glColor4f(0.561,  0.737,  0.561,  0.1);  // DarkSeaGreen
    else
      glColor4f(0.433,  0.520,  0.284,  0.1);  // DarkOliveGreen-ish
    glBegin(GL_POINTS);
    for(unsigned int i=0; i<cache_size; i++)
      glVertex2f(cache_x1[i], cache_y1[i]);
    glEnd();
  }
    
  // ----- Handle the SECOND Plot ----------
  if(m_show_right_logplot && (m_logplot2.size()!=0)) {
    unsigned int i, cache_size = cache_x2.size();
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
    else  glPointSize(6.0);
    if(color_scheme == 1)
      glColor4f(0.99,  0.99,  0.99,  0.1);     // White
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
  scalet += m_lft_marg;
  glColor4f(1.0, 0.0, 0.0, 0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(scalet, m_bot_marg);
  glVertex2f(scalet, h());
  glEnd();
  
  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawTimeText

void LogPlotViewer::drawTimeText()
{
  string tlow = doubleToString(m_display_min_time, 3);
  string thgh = doubleToString(m_display_max_time, 3);

  // variable shift depending on the size of the string
  double vshift = thgh.length() * 5.75;

  drawText(w()/2-10, m_bot_marg/2, "time (secs)", 0.1, 0.1, 0.1);
  drawText(m_lft_marg+5, m_bot_marg/2, tlow, 0.1, 0.1, 0.1);
  drawText(w()-m_rgt_marg-vshift, m_bot_marg/2, thgh, 0.1, 0.1, 0.1);
}

//-------------------------------------------------------------
// Procedure: drawMinMaxVarText

void LogPlotViewer::drawMinMaxVarText()
{
  string min_val1 = "n/a";
  string max_val1 = "n/a";
  string min_val2 = "n/a";
  string max_val2 = "n/a";

  if(m_logplot1.size() != 0) {
    min_val1 = doubleToStringXX(getMinVal1());
    max_val1 = doubleToStringXX(getMaxVal1());
  }
  if(m_logplot2.size() != 0) {
    min_val2 = doubleToStringXX(getMinVal2());
    max_val2 = doubleToStringXX(getMaxVal2());
  }

  // variable shift depending on the size of the string
  double vshift_min = 7 + min_val1.length() * 5.75;
  double vshift_max = 7 + max_val1.length() * 5.75;
  drawText(m_lft_marg-vshift_min, m_bot_marg+4, min_val1, 0.1, 0.1, 0.1);
  drawText(m_lft_marg-vshift_max, h()-12, max_val1, 0.1, 0.1, 0.1);
  drawText(w()-m_rgt_marg+7, h()-12, max_val2, 0.1, 0.1, 0.1);
  drawText(w()-m_rgt_marg+7, m_bot_marg+4, min_val2, 0.1, 0.1, 0.1);
}

//-------------------------------------------------------------
// Procedure: drawText

void LogPlotViewer::drawText(double px, double py, const string& text,
			     double r, double g, double b)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glColor3f(r, g, b);
  gl_font(1, 10);
  glRasterPos3f(px, py, 0);
  gl_draw(text.c_str());

  glFlush();
  glPopMatrix();
}


//-------------------------------------------------------------
// Procedure: doubleToStringXX

string LogPlotViewer::doubleToStringXX(double val) const
{
  if(val > 12345678)
    return(doubleToStringX(val, 0));
  else if(val > 1234567)
    return(doubleToStringX(val, 1));
  else if(val > 123456)
    return(doubleToStringX(val, 2));
  return(doubleToStringX(val, 3));
}




