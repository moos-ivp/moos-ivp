/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GUI_Encounters.h                                     */
/*    DATE: Jan 11th, 2016                                       */
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

#ifndef GUI_ENCOUNTERS_HEADER
#define GUI_ENCOUNTERS_HEADER

#include <set>
#include <map>
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Check_Button.H>
#include "FL/Fl_Hold_Browser.H"
#include "FL/Fl_Output.H"
#include "FL/Fl_Button.H"
#include "EncounterViewer.h"
#include "ALogDataBroker.h"

class REPLAY_GUI;

class GUI_Encounters : public Fl_Window {
public:
  GUI_Encounters(int w, int h, const char *l=0);
  ~GUI_Encounters();
  
  static Fl_Menu_Item menu_[];
  Fl_Menu_Bar *mbar;

  void  setDataBroker(ALogDataBroker, std::string vname);
  void  setParentGUI(REPLAY_GUI*);
  void  setCurrTime(double=-1);
  void  setCurrTimeX(double=-1);
  void  setReplayWarpMsg(std::string s) {m_replay_warp_msg=s; updateXY();}
  void  updateXY();

 protected:
  void  resize(int, int, int, int);
  int   handle(int);
  void  initMenuButton();
  void  initWidgets();
  void  resizeWidgetsShape();
  void  resizeWidgetsText();

  void  toggleFullScreen();

 private:
  inline void cb_Step_i(int);
  static void cb_Step(Fl_Widget*, int);

  inline void cb_SelectShowPts_i();
  static void cb_SelectShowPts(Fl_Widget*);
  
  inline void cb_SelectMinCPA_i();
  static void cb_SelectMinCPA(Fl_Widget*);

  inline void cb_SelectAvgCPA_i();
  static void cb_SelectAvgCPA(Fl_Widget*);
  
  inline void cb_SelectMinEff_i();
  static void cb_SelectMinEff(Fl_Widget*);

  inline void cb_SelectAvgEff_i();
  static void cb_SelectAvgEff(Fl_Widget*);
  
  inline void cb_SelectGo_i();
  static void cb_SelectGo(Fl_Widget*);
  
public:
  EncounterViewer *m_eviewer;
  REPLAY_GUI      *m_parent_gui;
  ALogDataBroker   m_dbroker;

protected:
  Fl_Output   *m_fld_loc_time;
  Fl_Output   *m_fld_encounters;

  Fl_Output   *m_fld_min_cpa;
  Fl_Output   *m_fld_min_eff;
  Fl_Output   *m_fld_avg_cpa;
  Fl_Output   *m_fld_avg_eff;

  Fl_Output   *m_fld_collision_range;
  Fl_Output   *m_fld_near_miss_range;
  Fl_Output   *m_fld_encounter_range;

  Fl_Button   *m_but_curr_go;
  Fl_Output   *m_fld_curr_id;
  Fl_Output   *m_fld_curr_time;
  Fl_Output   *m_fld_curr_cpa;
  Fl_Output   *m_fld_curr_eff;
  Fl_Output   *m_fld_curr_contact;

  Fl_Check_Button *m_but_draw_mineff;
  Fl_Check_Button *m_but_draw_avgeff;
  Fl_Check_Button *m_but_draw_mincpa;
  Fl_Check_Button *m_but_draw_avgcpa;
  Fl_Check_Button *m_but_show_allpts;
  
  std::vector<std::string> m_sources;

  std::string    m_vname;
  std::string    m_source;
  std::string    m_replay_warp_msg;
  bool           m_fullscreen;
};
#endif




