/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GUI_IPF.h                                            */
/*    DATE: Sep 25th, 2011                                       */
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

#ifndef GUI_IPFX_HEADER
#define GUI_IPFX_HEADER

#include <map>
#include <string>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Check_Button.H>
#include "FL/Fl_Hold_Browser.H"
#include "FL/Fl_Button.H"
#include "IvPFuncViewerX.h"
#include "ALogDataBroker.h"
#include "Common_IPF_GUI.h"

class REPLAY_GUI;

class GUI_IPF : public Common_IPF_GUI {
public:
  GUI_IPF(int w, int h, const char *l=0);
  ~GUI_IPF();
  
  void  augmentMenu();
  
  void  setDataBroker(ALogDataBroker, std::string vname);
  void  setParentGUI(REPLAY_GUI*);
  void  setCurrTime(double=-1);
  void  applyBehaviorVarMap(std::map<std::string, std::string>);
  void  setReplayWarpMsg(std::string s) {m_replay_warp_msg=s; updateXY();}

 protected:
  void  resize(int, int, int, int);
  int   handle(int);
  void  initMenuButton();
  void  initWidgets();
  void  resizeWidgetsShape();
  void  resizeWidgetsText();
  void  initSourceSelection();
  void  initMenuVarButtons();

  void  updateXY();
  void  updateBrowser();
  void  updateMutableTextSize(std::string);
  void  toggleFullScreen();

 private:
  void cb_HandleUpDown_i(int);
  void cb_HandleLeftRight_i(int);
  static void cb_HandleUpDown(Fl_Widget*, int);
  static void cb_HandleLeftRight(Fl_Widget*, int);

  void cb_Zoom_i(int);

  inline void cb_ToggleSet_i();
  static void cb_ToggleSet(Fl_Widget*);

  inline void cb_TogglePin_i();
  static void cb_TogglePin(Fl_Widget*);

  inline void cb_Step_i(int);
  static void cb_Step(Fl_Widget*, int);

  inline void cb_SelectSource_i();
  static void cb_SelectSource(Fl_Widget*);

  inline void cb_SelectCollective_i(int);
  static void cb_SelectCollective(Fl_Widget*, int);

  inline void cb_ButtonAddVarA_i(int);
  static void cb_ButtonAddVarA(Fl_Widget*, int);

  inline void cb_ButtonAddVarB_i(int);
  static void cb_ButtonAddVarB(Fl_Widget*, int);

public:
  IvPFuncViewerX *m_ipf_viewer;
  REPLAY_GUI     *m_parent_gui;
  ALogDataBroker  m_dbroker;
  
protected:
  Fl_Hold_Browser *m_brw_bhvs;

  Fl_Output   *m_fld_loc_time;
  Fl_Output   *m_fld_ipf_iter;
  Fl_Output   *m_fld_ipf_pcs;
  Fl_Output   *m_fld_ipf_dom;
  Fl_Button   *m_but_ipf_set;
  Fl_Button   *m_but_ipf_pin;

  Fl_Menu_Button  *m_but_addvar_a;
  Fl_Menu_Button  *m_but_addvar_b;

  Fl_Check_Button *m_but_collective;
  Fl_Check_Button *m_but_collective_dep;
  Fl_Check_Button *m_but_show_pcs;

  std::vector<std::string> m_sources;

  std::string    m_vname;
  std::string    m_source;
  std::string    m_replay_warp_msg;
  bool           m_fullscreen;
  bool           m_fullscreen_tmp;

  int            m_mutable_text_size;
};
#endif




