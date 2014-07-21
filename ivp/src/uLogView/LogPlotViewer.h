/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogPlotViewer.h                                      */
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

#ifndef LOGPLOT_VIEWER_HEADER
#define LOGPLOT_VIEWER_HEADER

#include <vector>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "LogTPlot.h"
//#include "XYPolygon.h"

class LogPlotViewer : public Fl_Gl_Window
{
 public:
  LogPlotViewer(int x,int y,int w,int h,const char *l=0);
  ~LogPlotViewer() {};
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);
  void  resize(int, int, int, int);

 public:
  unsigned int addLogPlot(const LogTPlot&);
  void   setMinMaxTimeUTC(double, double);

  void   setCurrTime(double v);
  double getCurrTime() {return(m_curr_time);};

  void   adjustZoom(std::string);

  void   setLeftPlot(unsigned int);
  void   setRightPlot(unsigned int);

  double getTimeLow() {return(m_display_min_time);};
  double getTimeHigh() {return(m_display_max_time);};

  double getCurrVal1(double time);
  double getCurrVal2(double time);

  std::string getVariable1();
  std::string getVariable2();
  
  std::string getMinVal1();
  std::string getMinVal2();

  std::string getMaxVal1();
  std::string getMaxVal2();

 protected:
  int   handleLeftMouse(int, int);
  void  drawLogPlot();
  bool  fillCache();

 protected:
  std::vector<LogTPlot> m_logplots;
  double m_curr_time;

  std::vector<double>  cache_x1;
  std::vector<double>  cache_y1;
  std::vector<double>  cache_x2;
  std::vector<double>  cache_y2;
  bool   m_valid_cache;
  
  unsigned int  view_index1;  // Index in logplot vector being viewed
  unsigned int  view_index2;  // Index in logplot vector being viewed

  double   m_min_time_utc;
  double   m_max_time_utc;  
  double   m_display_min_time;
  double   m_display_max_time;

  double m_margin;

  //double m_extreme_min_time;
  //double m_extreme_max_time;

  double m_step;
};

#endif 





