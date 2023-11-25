/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: GUI_HelmScope.h                                      */
/*    DATE: Oct 4th 2011                                         */
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

#ifndef ULH_GUIX_HEADER
#define ULH_GUIX_HEADER

#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Output.H>
#include "ModelHelmScope.h"
#include "ALogDataBroker.h"

class REPLAY_GUI;

class GUI_HelmScope : public Fl_Window {
public:
  GUI_HelmScope(int w, int h, const char *l=0);
  ~GUI_HelmScope();
  
  static Fl_Menu_Item menu_[];
  Fl_Menu_Bar *mbar;

  void initWidgets();

  void setParentGUI(REPLAY_GUI *gui) {m_parent_gui=gui;}
  void setDataBroker(ALogDataBroker, std::string vname);
  void setCurrTime(double=-1);
  void setReplayWarpMsg(std::string s) {m_replay_warp_msg=s; updateXY();}

  void resize(int, int, int, int);
  int  handle(int);

 protected:
  void resizeWidgetsShape();
  void resizeWidgetsText();
  void updateTopBrowsers();
  void updateBotBrowser();
  void updateXY();
  void updateMutableTextSize(std::string);

private:
  inline void cb_Browser_i();
  static void cb_Browser(Fl_Widget*);

  inline void cb_BrowserHeader_i();
  static void cb_BrowserHeader(Fl_Widget*);

  inline void cb_ButtonWarnings_i();
  static void cb_ButtonWarnings(Fl_Widget*);

  inline void cb_ButtonErrors_i();
  static void cb_ButtonErrors(Fl_Widget*);

  inline void cb_ButtonModeTree_i();
  static void cb_ButtonModeTree(Fl_Widget*);

  inline void cb_ButtonLifeEvents_i();
  static void cb_ButtonLifeEvents(Fl_Widget*);

  inline void cb_Step_i(int);
  static void cb_Step(Fl_Widget*, int);

 public:
  ModelHelmScope  m_hsmodel;
  REPLAY_GUI     *m_parent_gui;
  ALogDataBroker  m_dbroker;

protected:
  std::string m_vname;

  Fl_Output   *m_fld_time;
  Fl_Output   *m_fld_mode;
  Fl_Output   *m_fld_iter;
  Fl_Output   *m_fld_decision;
  Fl_Output   *m_fld_state;

  Fl_Check_Button  *m_but_gen_errors;
  Fl_Check_Button  *m_but_gen_warnings;
  Fl_Check_Button  *m_but_gen_modetree;
  Fl_Check_Button  *m_but_gen_levents;

  Fl_Browser *m_brw_active;
  Fl_Browser *m_brw_running;
  Fl_Browser *m_brw_idle;
  Fl_Browser *m_brw_completed;  
  Fl_Browser *m_brw_general;

  unsigned int m_cnt_active;
  unsigned int m_cnt_running;
  unsigned int m_cnt_idle;
  unsigned int m_cnt_completed;
  unsigned int m_cnt_general;

  unsigned int m_cnt_warnings;
  unsigned int m_cnt_errors;
  unsigned int m_cnt_mevents;
  unsigned int m_cnt_levents;

  int m_mutable_text_size;

  std::string  m_gen_browser_mode; // warnings, errors, life_events, mode_tree
  std::string  m_replay_warp_msg;
};

#endif





