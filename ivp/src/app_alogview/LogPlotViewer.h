/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include <string>
#include "FL/Fl_Gl_Window.H"
#include "ALogDataBroker.h"
#include "LogPlot.h"

class LogPlotViewer : public Fl_Gl_Window
{
 public:
  LogPlotViewer(int x,int y,int w,int h,const char *l=0);
  ~LogPlotViewer() {}
  
  // Pure virtuals that need to be defined
  void   draw();
  int    handle(int);
  void   resize(int, int, int, int);

 public:
  void   setCurrTime(double v);
  void   setDataBroker(const ALogDataBroker& dbroker);
  double getCurrTime() {return(m_curr_time);}
  void   adjustZoom(std::string);

  void   setLeftPlot(unsigned int);
  void   setRightPlot(unsigned int);

  void   toggleLeftLogPlot()  {m_show_left_logplot  = !m_show_left_logplot;}
  void   toggleRightLogPlot() {m_show_right_logplot = !m_show_right_logplot;};
  void   showLeftLogPlot(bool v)  {m_show_left_logplot=v;}
  void   showRightLogPlot(bool v) {m_show_right_logplot=v;}

  double getTimeLow() {return(m_display_min_time);}
  double getTimeHigh() {return(m_display_max_time);}

  double get_curr_val1(double time);
  double get_curr_val2(double time);

  std::string getFullVar1() const {return(m_fullvar1);}
  std::string getFullVar2() const {return(m_fullvar2);}
  
  double getMinVal1();
  double getMinVal2();

  double getMaxVal1();
  double getMaxVal2();

  void   setSyncScales(std::string);

 protected:
  void  handleLeftMouse(int, int);
  bool  fillCache();
  void  adjustTimeBounds();
  void  drawLogPlot();
  void  drawTimeText();
  void  drawMinMaxVarText();
  void  drawText(double px, double py, const std::string&, 
		 double r, double g, double b);
  std::string doubleToStringXX(double) const;

 protected:
  ALogDataBroker m_dbroker;

  LogPlot m_logplot1;
  LogPlot m_logplot2;
  
  bool m_show_left_logplot;
  bool m_show_right_logplot;

  std::string m_fullvar1;
  std::string m_fullvar2;

  unsigned int m_left_mix;
  unsigned int m_right_mix;

  double  m_curr_time;
  
  std::vector<double>  cache_x1;
  std::vector<double>  cache_y1;
  std::vector<double>  cache_x2;
  std::vector<double>  cache_y2;
  bool   m_valid_cache;
  bool   m_sync_scales;

  double m_margin;
  double m_lft_marg;
  double m_rgt_marg;
  double m_bot_marg;
  
  bool   m_zoomed_in;
  double m_extreme_min_time;
  double m_extreme_max_time;
  double m_display_min_time;
  double m_display_max_time;
  double m_step;
};

#endif 





