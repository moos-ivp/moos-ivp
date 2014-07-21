/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ULV_GUI.h                                            */
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

#ifndef ULV_GUI_HEADER
#define ULV_GUI_HEADER

#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Check_Button.H>
#include "NavPlotViewer.h"
#include "LogPlotViewer.h"
#include <FL/Fl_Hold_Browser.H>
#include "MY_Output.h"
#include "MY_Button.h"
#include "MY_Menu_Button.h"
#include "MY_Repeat_Button.h"
#include "MBTimer.h"
#include "MarineVehiGUI.h"

class ULV_GUI : public MarineVehiGUI {
public:
  ULV_GUI(int w, int h, const char *l=0);
  ~ULV_GUI();
  
  void addLogPlot(const LogTPlot&);
  void setLogFile(std::string str)  {m_log_file = str;};
  void conditional_step();
  void updateXY();

  bool inNavPlotViewer();

  void setWindowLayout(std::string layout="normal");
  void setCurrTime(double=-1);

  PostingBuffer getPostingBuffer(bool clear=true);

 protected:
  PostingBuffer m_pbuffer;


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

  inline void cb_ToggleUTC_i();
  static void cb_ToggleUTC(Fl_Widget*);

  inline void cb_Delete_i();
  static void cb_Delete(Fl_Widget*);

public:
  NavPlotViewer *np_viewer;
  LogPlotViewer *lp_viewer;
  
protected:
  double      m_np_viewer_hgt;
  double      m_lp_viewer_hgt;
  std::string m_window_layout;

  MY_Output   *disp_time;
  MY_Output   *play_rate;

  MY_Output   *m_fld_var_left;
  MY_Output   *m_fld_low_left;
  MY_Output   *m_fld_hgh_left;
  MY_Output   *m_fld_cur_left;

  MY_Output   *m_fld_var_right;
  MY_Output   *m_fld_low_right;
  MY_Output   *m_fld_hgh_right;
  MY_Output   *m_fld_cur_right;

  Fl_Check_Button  *m_but_time_utc;

  MY_Repeat_Button *m_but_zoom_in_time;
  MY_Repeat_Button *m_but_zoom_out_time;
  MY_Button        *m_but_zoom_reset_time;

  std::string m_log_file;

  bool    m_stream;
  MBTimer m_timer;
  double  m_step_time;
  int     m_step_amt;
  int     m_step_time_ix;
};
#endif





