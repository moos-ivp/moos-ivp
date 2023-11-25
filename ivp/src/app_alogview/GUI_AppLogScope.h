/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: GUI_AppLogScope.h                                    */
/*    DATE: Oct 16th, 2021                                       */
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

#ifndef GUI_APPLOG_SCOPE_HEADER
#define GUI_APPLOG_SCOPE_HEADER

#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Check_Button.H>
#include "FL/Fl_Button.H"
#include <FL/Fl_Browser.H>
#include <FL/Fl_Output.H>
#include "ModelAppLogScope.h"
#include "ALogDataBroker.h"

class REPLAY_GUI;

class GUI_AppLogScope : public Fl_Window {
public:
  GUI_AppLogScope(int w, int h, const char *l=0);
  ~GUI_AppLogScope();
  
  static Fl_Menu_Item menu_[];
  Fl_Menu_Bar *mbar;

  void initWidgets();

  void setParentGUI(REPLAY_GUI *gui) {m_parent_gui=gui;}
  void setDataBroker(ALogDataBroker, unsigned int mix);
  void setCurrTime(double=-1);
  void setReplayWarpMsg(std::string s) {m_replay_warp_msg=s; updateXY();}

  void setGrepStr1(std::string s) {m_alsmodel.setGrepStr1(s);}
  void setGrepStr2(std::string s) {m_alsmodel.setGrepStr2(s);}
    
  void resize(int, int, int, int);
  int  handle(int);

  
 protected:
  void resizeWidgetsShape();
  void resizeWidgetsText();
  void updateBrowsers();
  void updateXY();
  void updateMutableTextSize(std::string);

  void updateBrowser1();
  void updateBrowser2();

private:
  inline void cb_BrowserInfo_i();
  static void cb_BrowserInfo(Fl_Widget*);

  inline void cb_ButtonTruncate_i(int);
  static void cb_ButtonTruncate(Fl_Widget*, int);

  inline void cb_ButtonSeparate_i(int);
  static void cb_ButtonSeparate(Fl_Widget*, int);

  inline void cb_ButtonWrapLine_i(int);
  static void cb_ButtonWrapLine(Fl_Widget*, int);

  inline void cb_ButtonFuture_i(int);
  static void cb_ButtonFuture(Fl_Widget*, int);

  inline void cb_ModGrep_i(int);
  static void cb_ModGrep(Fl_Widget*, int);

  inline void cb_ModText_i(int);
  static void cb_ModText(Fl_Widget*, int);

  inline void cb_Step_i(int);
  static void cb_Step(Fl_Widget*, int);

  inline void cb_ButtonApplyGrep_i(int);
  static void cb_ButtonApplyGrep(Fl_Widget*, int);

 public:
  ModelAppLogScope m_alsmodel;
  REPLAY_GUI      *m_parent_gui;
  ALogDataBroker   m_dbroker;

 protected:
  Fl_Output   *m_fld_time;
  Fl_Output   *m_fld_grep1;
  Fl_Output   *m_fld_grep2;

  Fl_Check_Button *m_but_truncate;
  Fl_Check_Button *m_but_separate;
  Fl_Check_Button *m_but_wrapline;
  Fl_Check_Button *m_but_future;
  Fl_Check_Button *m_but_apply_grep1;
  Fl_Check_Button *m_but_apply_grep2;

  Fl_Browser *m_brw_info1;
  Fl_Browser *m_brw_info2;

  Fl_Button  *m_but_mod_grep1;
  Fl_Button  *m_but_mod_grep2;
  
  Fl_Button  *m_but_text_more;
  Fl_Button  *m_but_text_less;
  
  std::string m_replay_warp_msg;
  std::string m_vname;
  std::string m_app_name;

  int m_mutable_text_size;
};

#endif

