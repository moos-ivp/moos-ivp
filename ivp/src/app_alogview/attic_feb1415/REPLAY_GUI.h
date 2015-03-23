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

#include <string>
#include <list>
#include "NavPlotViewer.h"
#include "LogPlotViewer.h"
#include "MY_Output.h"
#include "MY_Button.h"
#include "MY_Repeat_Button.h"
#include "MBTimer.h"
#include "MarineVehiGUI.h"
#include "GUI_IPF.h"
#include "ALogDataBroker.h"

class REPLAY_GUI : public MarineVehiGUI {
 public:
  REPLAY_GUI(int w, int h, const char *l=0);
  ~REPLAY_GUI();
  
  void   resize(int, int, int, int);
  int    handle(int);

  void   setDataBroker(ALogDataBroker);
  void   setLogPlotMenus();
  void   setIPFPlotMenus();
  void   initLogPlotChoiceA(std::string vname, std::string varname);
  void   initLogPlotChoiceB(std::string vname, std::string varname);

  void   conditionalStep();
  void   updateXY();

  bool   inNavPlotViewer() const;
  bool   inLogPlotViewer() const;

  void   steptime(int val=0);

  void   setWindowLayout(std::string layout="normal");
  void   setCurrTime(double=-1);
  void   setClipTimeMin(double);
  void   setClipTimeMax(double);

 protected:
  void   augmentMenu();

  void   resizeWidgets();
  void   resizeDataText(int);
  void   updateTimeSubGUI();
  
 private:
  // Variants of the base class functions. Invoked in REPLAY_GUI::handle
  void   handleLeftRight(int);
  void   handleUpDown(int);
  void   zoom(int);

  // Implement these base class virtual functions so that the base
  // class version is not called. We handle all these keys in this class.
  void   cb_HandleLeftRight_i(int) {};
  void   cb_HandleUpDown_i(int) {};
  void   cb_Zoom_i(int) {};

  inline bool cb_Step_i(int);
  static void cb_Step(Fl_Widget*, int);

  inline bool cb_StepType_i(int);
  static void cb_StepType(Fl_Widget*, int);

  inline void cb_LeftLogPlot_i(int);
  static void cb_LeftLogPlot(Fl_Widget*, int);

  inline void cb_RightLogPlot_i(int);
  static void cb_RightLogPlot(Fl_Widget*, int);

  inline void cb_IPF_GUI_i(int);
  static void cb_IPF_GUI(Fl_Widget*, int);

  inline void cb_Streaming_i(int);
  static void cb_Streaming(Fl_Widget*, int);

  inline void cb_StreamStep_i(int);
  static void cb_StreamStep(Fl_Widget*, int);

  inline void cb_StreamSpeed_i(bool);
  static void cb_StreamSpeed(Fl_Widget*, bool);

  inline void cb_TimeZoom_i(int);
  static void cb_TimeZoom(Fl_Widget*, int);

 public:
  NavPlotViewer *np_viewer;
  LogPlotViewer *lp_viewer;
  
  std::list<GUI_IPF*> m_sub_guis;

 protected:
  ALogDataBroker m_dbroker;
  
  int m_left_mix;
  int m_right_mix;

 protected:
  double      m_np_viewer_hgt;
  double      m_lp_viewer_hgt;

  double      m_clip_time_min;
  double      m_clip_time_max;

  MY_Output*  m_disp_time;

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
