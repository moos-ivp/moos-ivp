/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
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

#include <set>
#include <map>
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Button.H>
#include "NavPlotViewer.h"
#include "LogPlotViewer.h"
#include "IvPFuncViewer.h"
#include <FL/Fl_Hold_Browser.H>
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
  
  void addLogPlot(const LogPlot&);
  void addHelmPlot(const HelmPlot&);
  void addIPF_Plot(const IPF_Plot &v); 
  void setLogFile(std::string str)  {m_log_file = str;};
  void conditional_step();
  void capture_to_file();
  void updateXY();

  bool inNavPlotViewer();
  bool inLogPlotViewer();
  bool inIPFViewerA();
  bool inIPFViewerB();

  void setWindowLayout(std::string layout="normal");
  void setCurrTime(double=-1);

  void initChoicesIPF();

protected:
  void augmentMenu();
  int  handle(int);
 
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

  inline void cb_LeftHelmPlot_i(int);
  static void cb_LeftHelmPlot(Fl_Widget*, int);

  inline void cb_RightHelmPlot_i(int);
  static void cb_RightHelmPlot(Fl_Widget*, int);

  inline void cb_CollectToggle_i();
  static void cb_CollectToggle(Fl_Widget*);

  inline void cb_StreamToggle_i();
  static void cb_StreamToggle(Fl_Widget*);

  inline void cb_StreamStep_i(int);
  static void cb_StreamStep(Fl_Widget*, int);

  inline void cb_StreamSpeed_i(bool);
  static void cb_StreamSpeed(Fl_Widget*, bool);

  inline void cb_AllTrail_i();
  static void cb_AllTrail(Fl_Widget*);

  inline void cb_TimeZoom_i(int);
  static void cb_TimeZoom(Fl_Widget*, int);

  inline void cb_ToggleSet_i(int);
  static void cb_ToggleSet(Fl_Widget*, int);

  inline void cb_TogglePin_i(int);
  static void cb_TogglePin(Fl_Widget*, int);

  inline void cb_ToggleCollective_i(int);
  static void cb_ToggleCollective(Fl_Widget*, int);

  inline void cb_Delete_i();
  static void cb_Delete(Fl_Widget*);

  inline void cb_TopSelectVName_i(int);
  static void cb_TopSelectVName(Fl_Widget*, int);

  inline void cb_TopSelectSource_i(int);
  static void cb_TopSelectSource(Fl_Widget*, int);

  inline void cb_BotSelectVName_i(int);
  static void cb_BotSelectVName(Fl_Widget*, int);

  inline void cb_BotSelectSource_i(int);
  static void cb_BotSelectSource(Fl_Widget*, int);

public:
  NavPlotViewer *np_viewer;
  LogPlotViewer *lp_viewer;
  IvPFuncViewer *ipf_viewer_a;
  IvPFuncViewer *ipf_viewer_b;
  
protected:
  double      m_np_viewer_hgt;
  double      m_lp_viewer_hgt;
  std::string m_window_layout;

  MY_Output   *ipf_pcs_a;
  MY_Output   *ipf_pcs_b;
  MY_Output   *ipf_pwt_a;
  MY_Output   *ipf_pwt_b;
  MY_Output   *ipf_iter_a;
  MY_Output   *ipf_iter_b;
  MY_Output   *ipf_domain_a;
  MY_Output   *ipf_domain_b;
  MY_Button   *m_but_ipf_set_a;
  MY_Button   *m_but_ipf_set_b;
  MY_Button   *m_but_ipf_pin_a;
  MY_Button   *m_but_ipf_pin_b;

  MY_Menu_Button *m_but_ipf_vname_a;
  MY_Menu_Button *m_but_ipf_vname_b;
  MY_Menu_Button *m_but_ipf_source_a;
  MY_Menu_Button *m_but_ipf_source_b;
  std::string     m_but_ipf_source_a_str;
  std::string     m_but_ipf_source_b_str;
  
  MY_Output   *disp_time;
  MY_Output   *play_rate;
  MY_Output   *collect_state;

  MY_Output   *vname1;
  MY_Output   *label1;
  MY_Output   *low1;
  MY_Output   *high1;
  MY_Output   *curr1;

  MY_Output   *m_fld_time_low;
  MY_Output   *m_fld_time_high;

  MY_Output   *vname2;
  MY_Output   *label2;
  MY_Output   *low2;
  MY_Output   *high2;
  MY_Output   *curr2;

  MY_Output   *m_fld_bhvs_act_1;
  MY_Output   *m_fld_bhvs_run_1;
  MY_Output   *m_fld_bhvs_idle_1;
  MY_Output   *m_fld_bhvs_cplt_1;
  MY_Output   *m_fld_bhvs_vname_1;
  MY_Output   *m_fld_bhvs_mode_1;
  MY_Output   *m_fld_bhvs_iter_1;
  MY_Output   *m_fld_bhvs_dec_1;

  MY_Output   *m_fld_bhvs_act_2;
  MY_Output   *m_fld_bhvs_run_2;
  MY_Output   *m_fld_bhvs_idle_2;
  MY_Output   *m_fld_bhvs_cplt_2;
  MY_Output   *m_fld_bhvs_vname_2;
  MY_Output   *m_fld_bhvs_mode_2;
  MY_Output   *m_fld_bhvs_iter_2;
  MY_Output   *m_fld_bhvs_dec_2;

  MY_Repeat_Button *m_but_zoom_in_time;
  MY_Repeat_Button *m_but_zoom_out_time;
  MY_Button        *m_but_zoom_reset_time;

  std::vector<std::string> m_vnames;
  std::vector<std::string> m_sources;
  std::map<std::string, std::set<std::string> > m_catalog;
  void  setVNameMenuButtonA(std::string vname);
  void  setVNameMenuButtonB(std::string vname);

  void  updateSourceMenuButtonA();
  void  updateSourceMenuButtonB();
  std::string m_vname_a;
  std::string m_vname_b;

  std::string m_source_a;
  std::string m_source_b;

  std::string m_log_file;
  std::string m_collect;

  int     m_num_ipfplots;
  bool    m_view_ipfs;
  bool    m_stream;
  MBTimer m_timer;
  double  m_step_time;
  int     m_step_amt;
  int     m_step_time_ix;
  int     m_save_file_ix;
};
#endif





