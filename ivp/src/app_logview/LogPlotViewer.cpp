/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
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

using namespace std;

LogPlotViewer::LogPlotViewer(int gx, int gy, int gw, int gh, const char *gl)
  : Fl_Gl_Window(gx,gy,gw,gh,gl)
{
  valid_cache = false;
  view_index1 = 0;
  view_index2 = 0;
  margin      = 10.0;
  w_step      = 0.0;
  min_time    = 0.0;
  curr_time   = 0.0;
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
	handle_left_mouse(vx, vy);
      if(Fl::event_button() == FL_RIGHT_MOUSE)
	handle_right_mouse(vx, vy);
    }
    return(1);
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

  draw_logplot();
}

//-------------------------------------------------------------
// Procedure: resize()

void LogPlotViewer::resize(int gx, int gy, int gw, int gh)
{
  valid_cache = false;
  Fl_Gl_Window::resize(gx, gy, gw, gh);
}

//-------------------------------------------------------------
// Procedure: add_logplot

unsigned int LogPlotViewer::add_logplot(const LogPlot& given_logplot)
{
  LogPlot new_logplot(given_logplot);
  logplot.push_back(new_logplot);
  return(logplot.size());
}

//-------------------------------------------------------------
// Procedure: set_left_plot()

void LogPlotViewer::set_left_plot(unsigned int index)
{
  if(index == view_index1)
    return;
  if((index >= 0) && (index < logplot.size()))
    view_index1 = index;
  valid_cache = false;
}

//-------------------------------------------------------------
// Procedure: set_right_plot()

void LogPlotViewer::set_right_plot(unsigned int index)
{
  if(index == view_index2)
    return;
  if((index >= 0) && (index < logplot.size()))
    view_index2 = index;
  valid_cache = false;
}

//-------------------------------------------------------------
// Procedure: get_curr_val1()

double LogPlotViewer::get_curr_val1(double gtime)
{
  if(logplot.size() == 0)
    return(0);
  else
    return(logplot[view_index1].get_value_by_time(gtime));
}

//-------------------------------------------------------------
// Procedure: get_curr_val2()

double LogPlotViewer::get_curr_val2(double gtime)
{
  if(logplot.size() == 0)
    return(0);
  else
    return(logplot[view_index2].get_value_by_time(gtime));
}

//-------------------------------------------------------------
// Procedure: get_label1

string LogPlotViewer::get_variable1()
{
  if((view_index1 < 0) || (view_index1 >= logplot.size()))
    return("none");
  else {
    string vehicle = logplot[view_index1].get_vehicle();
    string varname = logplot[view_index1].get_varname();
    if(vehicle != "")
      return(vehicle + "/" + varname);
    else
      return(varname);
  }
}

//-------------------------------------------------------------
// Procedure: get_label2

string LogPlotViewer::get_variable2()
{
  if(view_index1 == view_index2)
    return("none");

  if((view_index2 < 0) || (view_index2 >= logplot.size()))
    return("none");
  else {
    string vehicle = logplot[view_index2].get_vehicle();
    string varname = logplot[view_index2].get_varname();
    if(vehicle != "")
      return(vehicle + "/" + varname);
    else
      return(varname);
  }
}

//-------------------------------------------------------------
// Procedure: get_min_val1()

string LogPlotViewer::get_min_val1()
{
  if((view_index1 < 0) || (view_index1 >= logplot.size()))
    return("n/a");
  else
    return(doubleToString(logplot[view_index1].get_min_val(), 3));
}

//-------------------------------------------------------------
// Procedure: get_max_val1()

string LogPlotViewer::get_max_val1()
{
  if((view_index1 < 0) || (view_index1 >= logplot.size()))
    return("n/a");
  else
    return(doubleToString(logplot[view_index1].get_max_val(), 3));
}

//-------------------------------------------------------------
// Procedure: get_min_val2()

string LogPlotViewer::get_min_val2()
{
  if(view_index1 == view_index2)
    return("n/a");

  if((view_index2 < 0) || (view_index2 >= logplot.size()))
    return("n/a");
  else
    return(doubleToString(logplot[view_index2].get_min_val(), 3));
}

//-------------------------------------------------------------
// Procedure: get_max_val2()

string LogPlotViewer::get_max_val2()
{
  if(view_index1 == view_index2)
    return("n/a");

  if((view_index2 < 0) || (view_index2 >= logplot.size()))
    return("n/a");
  else
    return(doubleToString(logplot[view_index2].get_max_val(), 3));
}

//-------------------------------------------------------------
// Procedure: handle_left_mouse

int LogPlotViewer::handle_left_mouse(int vx, int vy)
{
  return(0);
}

//-------------------------------------------------------------
// Procedure: handle_right_mouse

int LogPlotViewer::handle_right_mouse(int vx, int vy)
{
  return(0);
}

//-------------------------------------------------------------
// Procedure: fill_cache

bool LogPlotViewer::fill_cache()
{
  //cout << "In fill_cache" << view_index1 << "  " << view_index2 << endl;

  valid_cache = false;
  if((view_index1 < 0) || (view_index1 >= logplot.size()))
    return(false);
  if((view_index2 < 0) || (view_index2 >= logplot.size()))
    return(false);

  LogPlot vplot1 = logplot[view_index1];
  LogPlot vplot2 = logplot[view_index2];

  cache_x1.clear();
  cache_y1.clear();
  cache_x2.clear();
  cache_y2.clear();

  min_time = vplot1.get_min_time();
  max_time = vplot1.get_max_time();

  if(vplot2.get_min_time() < min_time)
    min_time = vplot2.get_min_time();
  if(vplot2.get_max_time() > max_time)
    max_time = vplot2.get_max_time();

  w_step = (w()-margin) / (max_time - min_time);

  int    vpsize1  = vplot1.size();
  double min_val1 = vplot1.get_min_val();
  double max_val1 = vplot1.get_max_val();
  double h_step1 = (h()-margin) / (max_val1  - min_val1);

  for(int i=0; i<vpsize1; i++) {
    double rawt = vplot1.get_time_by_index(i);
    double rawv = vplot1.get_value_by_index(i);
    double scalet = ((rawt - min_time)  * w_step) + (margin/2.0); 
    double scalev = ((rawv - min_val1 ) * h_step1) + (margin/2.0);
    cache_x1.push_back(scalet);
    cache_y1.push_back(scalev);
  }

  if(view_index1 != view_index2) {
    int    vpsize2  = vplot2.size();
    double min_val2 = vplot2.get_min_val();
    double max_val2 = vplot2.get_max_val();
    double h_step2 = (h()-margin) / (max_val2  - min_val2);
    
    for(int j=0; j<vpsize2; j++) {
      double rawt = vplot2.get_time_by_index(j);
      double rawv = vplot2.get_value_by_index(j);
      double scalet = ((rawt - min_time)  * w_step) + (margin/2.0); 
      double scalev = ((rawv - min_val2 ) * h_step2) + (margin/2.0);
      cache_x2.push_back(scalet);
      cache_y2.push_back(scalev);
    }
  }

  valid_cache = true;
  return(true);
}


//-------------------------------------------------------------
// Procedure: draw_logplot
//  483d8b   .282 .239 .545   DarSlateBlue
//  8b0000   .545 .000 .000   DarkRed
//  2f4f4f   .184 .310 .310   DarkSlateGrey
//  daa520   .855 .647 .125   GoldenRod
//  e9967a   .914 .588 .478   DarkSalmon
//  8fbc8f   .561 .737 .561   DarkSeaGreen
//  bdb76b                    DarkKhaki
//  fafad2   .980 .980 .824   LightGoldenRodYellow

void LogPlotViewer::draw_logplot()
{
  if(!valid_cache)
    fill_cache();
  if(!valid_cache)
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

  // ----- Handle the FIRST Plot ----------
  // Draw the LINES of the plot
  if(color_scheme == 1)
    glColor4f(0.282,  0.239,  0.545,  0.1);  // DarkSlateBlue
  else
    glColor4f(0.433,  0.520,  0.284,  0.1);  // DarkOliveGreen-ish
  glBegin(GL_LINE_STRIP);
  for(unsigned int i=0; i<cache_x1.size(); i++)
    glVertex2f(cache_x1[i], cache_y1[i]);
  glEnd();

  // Draw the POINTS of the plot
  glPointSize(2.0);
  if(color_scheme == 1)
    glColor4f(0.561,  0.737,  0.561,  0.1);  // DarkSeaGreen
  else
    glColor4f(0.433,  0.520,  0.284,  0.1);  // DarkOliveGreen-ish
  glBegin(GL_POINTS);
  for(unsigned int i=0; i<cache_x1.size(); i++)
    glVertex2f(cache_x1[i], cache_y1[i]);
  glEnd();

  if(view_index1 != view_index2) {
    // ----- Handle the SECOND Plot ----------
    // Draw the LINES of the plot
    if(color_scheme == 1)
      glColor4f(0.545,  0.000,  0.000,  0.1);  // DarkRed
    else
      glColor4f(0.282,  0.239,  0.545,  0.1);  // DarkSlateBlue
    glBegin(GL_LINE_STRIP);
    for(unsigned int i=0; i<cache_x2.size(); i++)
      glVertex2f(cache_x2[i], cache_y2[i]);
    glEnd();

    // Draw the POINTS of the plot
    glPointSize(2.0);
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
  double scalet = ((curr_time - min_time) * w_step) + (margin/2.0); 
  glColor4f(1.0, 0.0, 0.0, 0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(scalet, 0);
  glVertex2f(scalet, h());
  glEnd();
  
  glFlush();
  glPopMatrix();
}








