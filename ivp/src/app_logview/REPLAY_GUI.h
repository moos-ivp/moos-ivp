/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: REPLAY_GUI.h                                         */
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

#ifndef REPLAY_GUI_HEADER
#define REPLAY_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "NavPlotViewer.h"
#include "LogPlotViewer.h"
#include <FL/Fl_Hold_Browser.H>
#include "MY_Output.h"
#include "MBTimer.h"
#include "MarineVehiGUI.h"

class REPLAY_GUI : public MarineVehiGUI {
public:
  REPLAY_GUI(int w, int h, const char *l=0);
  ~REPLAY_GUI() {};
  
  void addGridPlot(const GridPlot &v);
  void addLogPlot(const LogPlot&);
  void setLogFile(std::string str)  {log_file = str;};
  void conditional_step();
  void capture_to_file();
  void updateXY();

protected:
  void augmentMenu();
  int  handle(int);

private:
  inline bool cb_Step_i(int);
  static void cb_Step(Fl_Widget*, int);

  inline void cb_Jump_i(int);
  static void cb_Jump(Fl_Widget*, int);

  inline void cb_PolyView_i();
  static void cb_PolyView(Fl_Widget*);

  inline void cb_GridView_i();
  static void cb_GridView(Fl_Widget*);

  inline void cb_CrossView_i();
  static void cb_CrossView(Fl_Widget*);

  inline void cb_SelectGridPlot_i(int);
  static void cb_SelectGridPlot(Fl_Widget*, int);

  inline void cb_LeftLogPlot_i(int);
  static void cb_LeftLogPlot(Fl_Widget*, int);

  inline void cb_RightLogPlot_i(int);
  static void cb_RightLogPlot(Fl_Widget*, int);

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

  inline void cb_Delete_i();
  static void cb_Delete(Fl_Widget*);

public:
  NavPlotViewer *np_viewer;
  LogPlotViewer *lp_viewer;

protected:
  MY_Output   *x_mtr;
  MY_Output   *y_mtr;
  MY_Output   *disp_time;
  MY_Output   *play_rate;
  MY_Output   *collect_state;

  MY_Output   *vname1;
  MY_Output   *label1;
  MY_Output   *low1;
  MY_Output   *high1;
  MY_Output   *curr1;

  MY_Output   *vname2;
  MY_Output   *label2;
  MY_Output   *low2;
  MY_Output   *high2;
  MY_Output   *curr2;

  std::string log_file;
  std::string collect;

  bool    stream;
  MBTimer timer;
  float   step_time;
  int     step_amt;
  int     step_time_ix;
  int     save_file_ix;
};
#endif







