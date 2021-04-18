/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: REPLAY_GUI.h                                         */
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

#ifndef REPLAY_GUI_HEADER
#define REPLAY_GUI_HEADER

#include <string>
#include <list>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Repeat_Button.H>
#include "NavPlotViewer.h"
#include "LogPlotViewer.h"
#include "MY_Repeat_Button.h"
#include "MBTimer.h"
#include "MarineVehiGUI.h"
#include "GUI_IPF.h"
#include "GUI_HelmScope.h"
#include "GUI_VarScope.h"
#include "GUI_Encounters.h"
#include "ALogDataBroker.h"

class REPLAY_GUI : public MarineVehiGUI {
 public:
  REPLAY_GUI(int w, int h, const char *l=0);
  ~REPLAY_GUI();
  
  void   resize(int, int, int, int);
  int    handle(int);

  void   setDataBroker(ALogDataBroker);
  void   setLogPlotMenus();
  void   setVarHistMenus();
  void   setBehaviorVarMap(std::map<std::string, std::string>);
  void   initLogPlotChoiceA(std::string vname, std::string varname);
  void   initLogPlotChoiceB(std::string vname, std::string varname);

  void   setIPFPlotMenus();
  void   setEncounterPlotMenus();
  void   setHelmPlotMenus();

  void   conditionalStep();
  void   updateXY();

  bool   inNavPlotViewer() const;
  bool   inLogPlotViewer() const;

  void   steptime(int val=0);
  void   streaming(int val=0);
  void   streamspeed(bool faster);

  void   setWindowLayout(std::string layout="normal");
  void   setCurrTime(double=-1);

  void   resizeWidgetsShape();

 protected:
  void   augmentMenu();
  
  void   initWidgets();
  void   resizeWidgetsText(int);
  void   updatePlayRateMsg();
  void   updateTimeSubGUI();
  void   toggleLeftLogPlot();
  void   toggleRightLogPlot();

 private:
  // Variants of the base class functions. Invoked in REPLAY_GUI::handle
  void   handleLeftRight(int);
  void   handleUpDown(int);
  void   zoom(int);

  // Implement these base class virtual functions so that the base
  // class version is not called. We handle all these keys in this class.
  void   cb_HandleLeftRight_i(int) {}
  void   cb_HandleUpDown_i(int) {}
  void   cb_Zoom_i(int) {}

  inline void cb_JumpTime_i(int);
  static void cb_JumpTime(Fl_Widget*, int);

  inline void cb_Step_i(int millisecs);
  static void cb_Step(Fl_Widget*, int millisecs);

  inline void cb_StepType_i(int);
  static void cb_StepType(Fl_Widget*, int);

  inline void cb_LeftLogPlot_i(int);
  static void cb_LeftLogPlot(Fl_Widget*, int);

  inline void cb_RightLogPlot_i(int);
  static void cb_RightLogPlot(Fl_Widget*, int);

  inline void cb_VarHist_i(int);
  static void cb_VarHist(Fl_Widget*, int);

  inline void cb_Encounter_i(int);
  static void cb_Encounter(Fl_Widget*, int);

  inline void cb_IPF_GUI_i(int);
  static void cb_IPF_GUI(Fl_Widget*, int);

  inline void cb_Helm_GUI_i(int);
  static void cb_Helm_GUI(Fl_Widget*, int);

  inline void cb_Streaming_i(int);
  static void cb_Streaming(Fl_Widget*, int);

  inline void cb_StreamStep_i(int);
  static void cb_StreamStep(Fl_Widget*, int);

  inline void cb_StreamSpeed_i(bool);
  static void cb_StreamSpeed(Fl_Widget*, bool);

  inline void cb_TimeZoom_i(int);
  static void cb_TimeZoom(Fl_Widget*, int);

  inline void cb_ToggleSyncScales_i(int);
  static void cb_ToggleSyncScales(Fl_Widget*, int);

  inline void cb_ButtonHideLogPlot_i(int);
  static void cb_ButtonHideLogPlot(Fl_Widget*, int);

 public:
  NavPlotViewer *np_viewer;
  LogPlotViewer *lp_viewer;
  
  std::list<GUI_IPF*>        m_sub_guis;
  std::list<GUI_HelmScope*>  m_sub_guis_h;
  std::list<GUI_VarScope*>   m_sub_guis_v;
  std::list<GUI_Encounters*> m_sub_guis_e;

 protected:
  ALogDataBroker m_dbroker;
  
  int m_left_mix;
  int m_right_mix;

 protected:
  double      m_np_viewer_hgt;
  double      m_lp_viewer_hgt;

  // map from behavior to scope vars for convenience in GUI_IPF
  std::map<std::string, std::string> m_map_bhv_vars;
  
  Fl_Output*  m_disp_time;

  Fl_Check_Button *m_but_hide_lp;
  Fl_Check_Button *m_but_hide_rp;
  
  Fl_Output*  m_label1;
  Fl_Output*  m_curr1;
  Fl_Output*  m_label2;
  Fl_Output*  m_curr2;

  Fl_Repeat_Button *m_but_zoom_in_time;
  Fl_Repeat_Button *m_but_zoom_out_time;
  Fl_Button        *m_but_zoom_reset_time;
  Fl_Button        *m_but_sync_scales;

 protected:
  bool    m_stream;
  MBTimer m_timer;

  std::string m_replay_warp_msg;
  int         m_replay_warp_ix;
  double      m_replay_warp;
  int         m_replay_disp_gap;  // milliseconds

  std::list<double> m_warp_gaps;
  double            m_replay_warp_actual;

};
#endif




