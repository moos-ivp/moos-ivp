/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: REPLAY_GUI.h                                         */
/*    DATE: May 31st, 2005                                       */
/*    DATE: Feb 2nd, 2015 Major overhaul/simplification          */
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

#ifndef REPLAY_GUI_HEADER
#define REPLAY_GUI_HEADER

#include <set>
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "NavPlotViewer.h"
#include "LogPlotViewer.h"
#include "MY_Output.h"
#include "MY_Button.h"
#include "MY_Menu_Button.h"
#include "MY_Repeat_Button.h"
#include "MBTimer.h"
#include "MarineVehiGUI.h"

class REPLAY_GUI : public MarineVehiGUI {
 public:
  REPLAY_GUI(int w, int h, const char *l=0);
  ~REPLAY_GUI();
  
  void   resize(int, int, int, int);
  int    handle(int);

  void   addLogPlot(const LogPlot&);
  void   addHelmPlot(const HelmPlot&);
  void   conditional_step();
  void   updateXY();

  bool   inNavPlotViewer();
  bool   inLogPlotViewer();

  void   setWindowLayout(std::string layout="normal");
  void   setCurrTime(double=-1);

 protected:
  void augmentMenu();

  void resizeWidgets();
 
 private:
  void cb_HandleUpDown_i(int);
  void cb_HandleLeftRight_i(int);
  void cb_Zoom_i(int);

  inline bool cb_Step_i(int);
  static void cb_Step(Fl_Widget*, int);

  inline bool cb_StepType_i(int);
  static void cb_StepType(Fl_Widget*, int);

  inline void cb_LeftLogPlot_i(int);
  static void cb_LeftLogPlot(Fl_Widget*, int);

  inline void cb_RightLogPlot_i(int);
  static void cb_RightLogPlot(Fl_Widget*, int);

  inline void cb_StreamToggle_i();
  static void cb_StreamToggle(Fl_Widget*);

  inline void cb_StreamStep_i(int);
  static void cb_StreamStep(Fl_Widget*, int);

  inline void cb_StreamSpeed_i(bool);
  static void cb_StreamSpeed(Fl_Widget*, bool);

  inline void cb_TimeZoom_i(int);
  static void cb_TimeZoom(Fl_Widget*, int);

 public:
  NavPlotViewer *np_viewer;
  LogPlotViewer *lp_viewer;
  
 protected:
  double      m_np_viewer_hgt;
  double      m_lp_viewer_hgt;

 protected:
  MY_Output*  m_disp_time;
  MY_Output*  m_play_rate;

  MY_Output*  m_label1;
  MY_Output*  m_low1;
  MY_Output*  m_high1;
  MY_Output*  m_curr1;

  MY_Output*  m_fld_time_low;
  MY_Output*  m_fld_time_high;

  MY_Output*  m_label2;
  MY_Output*  m_low2;
  MY_Output*  m_high2;
  MY_Output*  m_curr2;

  MY_Repeat_Button *m_but_zoom_in_time;
  MY_Repeat_Button *m_but_zoom_out_time;
  MY_Button        *m_but_zoom_reset_time;

 protected:
  bool    m_stream;
  MBTimer m_timer;
  double  m_step_time;
  int     m_step_amt;
  int     m_step_time_ix;
};
#endif
