/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GUI_VarScope.h                                       */
/*    DATE: Feb 28th, 2015                                       */
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

#ifndef GUI_VARSCOPE_HEADER
#define GUI_VARSCOPE_HEADER

#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Menu_Button.H>
#include "ModelVarScope.h"
#include "ALogDataBroker.h"

class REPLAY_GUI;

class GUI_VarScope : public Fl_Window {
public:
  GUI_VarScope(int w, int h, const char *l=0);
  ~GUI_VarScope();
  
  static Fl_Menu_Item menu_[];
  Fl_Menu_Bar *mbar;

  void initWidgets();

  void setParentGUI(REPLAY_GUI *gui) {m_parent_gui=gui;}
  void setDataBroker(ALogDataBroker, unsigned int mix);
  void setCurrTime(double=-1);
  void setReplayWarpMsg(std::string s) {m_replay_warp_msg=s; updateXY();}

  void resize(int, int, int, int);
  int  handle(int);

 protected:
  void resizeWidgetsShape();
  void resizeWidgetsText();
  void updateBrowsers();
  void updateXY();
  void initMenuVarButtons();
  void updateMenuDelVarButton();
  void updateMutableTextSize(std::string);

private:
  inline void cb_BrowserPast_i();
  static void cb_BrowserPast(Fl_Widget*);

  inline void cb_BrowserSoon_i();
  static void cb_BrowserSoon(Fl_Widget*);

  inline void cb_ButtonFilter_i(int);
  static void cb_ButtonFilter(Fl_Widget*, int);

  inline void cb_ButtonAddVar_i(int);
  static void cb_ButtonAddVar(Fl_Widget*, int);

  inline void cb_ButtonSetVar_i(int);
  static void cb_ButtonSetVar(Fl_Widget*, int);

  inline void cb_ButtonDelVar_i(int);
  static void cb_ButtonDelVar(Fl_Widget*, int);

  inline void cb_Step_i(int);
  static void cb_Step(Fl_Widget*, int);

 public:
  ModelVarScope   m_vsmodel;
  REPLAY_GUI     *m_parent_gui;
  ALogDataBroker  m_dbroker;

protected:
  Fl_Output   *m_fld_time;

  Fl_Check_Button  *m_but_vname;
  Fl_Check_Button  *m_but_source;
  Fl_Check_Button  *m_but_srcaux;
  Fl_Check_Button  *m_but_varname;

  Fl_Menu_Button *m_but_addvar;
  Fl_Menu_Button *m_but_setvar;
  Fl_Menu_Button *m_but_delvar;

  Fl_Browser *m_brw_past;
  Fl_Browser *m_brw_soon;

  std::string m_replay_warp_msg;
  std::string m_vname;

  int m_mutable_text_size;
  
  std::vector<unsigned int> m_current_mix_vars;
};

#endif




