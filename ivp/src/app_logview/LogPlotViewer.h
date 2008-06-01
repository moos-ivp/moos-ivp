/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LogPlotViewer.h                                      */
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

#ifndef LOGPLOT_VIEWER_HEADER
#define LOGPLOT_VIEWER_HEADER

#include <vector>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "LogPlot.h"
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
  unsigned int add_logplot(const LogPlot&);
  void  set_curr_time(double v) {curr_time = v;};

  void  set_left_plot(unsigned int);
  void  set_right_plot(unsigned int);

  double get_curr_val1(double time);
  double get_curr_val2(double time);

  std::string get_variable1();
  std::string get_variable2();
  
  std::string get_min_val1();
  std::string get_min_val2();

  std::string get_max_val1();
  std::string get_max_val2();

 protected:
  int   handle_left_mouse(int, int);
  int   handle_right_mouse(int, int);
  void  draw_logplot();
  bool  fill_cache();

 protected:
  std::vector<LogPlot>     logplot;
  double curr_time;

  std::vector<double>  cache_x1;
  std::vector<double>  cache_y1;
  std::vector<double>  cache_x2;
  std::vector<double>  cache_y2;
  bool   valid_cache;
  
  unsigned int  view_index1;  // Index in logplot vector being viewed
  unsigned int  view_index2;  // Index in logplot vector being viewed
  double margin;

  double min_time;
  double max_time;
  double w_step;
};

#endif 
