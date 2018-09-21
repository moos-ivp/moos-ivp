/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PMV_GUI.h                                            */
/*    DATE: Nov 11th 2004                                        */
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

#ifndef PMV_GUI_HEADER
#define PMV_GUI_HEADER

#include "PMV_Viewer.h"
#include "MarineVehiGUI.h"
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Hold_Browser.H>
#include "MY_Fl_Hold_Browser.h"
#include "AppCastRepo.h"
#include "VPlug_AppCastSettings.h"
#include "CommandFolio.h"
#include "CommandSummary.h"
#include "UCMD_GUI.h"

class PMV_GUI : public MarineVehiGUI {
public:
  PMV_GUI(int w, int h, const char *l=0);
  virtual ~PMV_GUI() {}

  void         augmentMenu();
  void         resize(int, int, int, int);
  int          handle(int);

  bool         addButton(std::string button, std::string pairs);
  bool         addAction(std::string pair, bool separator=false);
  void         setCurrTime(double v)       {m_curr_time=v;}
  void         setTitleBase(std::string s) {m_title_base=s;}
  void         augmentTitle(std::string ip_str);
  bool         syncNodesAtoB();
  bool         syncNodesBtoA();
  void         updateXY();
  void         clearGeoShapes(std::string, std::string, std::string);

  void         setCommandFolio(CommandFolio v)     {m_cmd_folio=v;}
  void         setCommandSummary(CommandSummary v) {m_cmd_summary=v;}
  
  bool         clearStaleVehicles(bool force=false);
  double       getClearStaleTimeStamp() {return(m_clear_stale_timestamp);}

  std::string  getPendingVar(unsigned int index);
  std::string  getPendingVal(unsigned int index);

  void         clearPending();
  void         pushPending(std::string, std::string);
  unsigned int getPendingSize() {return(m_pending_vars.size());}
  bool         addScopeVariable(std::string);
  bool         addMousePoke(std::string, std::string, std::string);
  bool         addReferenceVehicle(std::string vname);
  bool         addFilterVehicle(std::string vname);

  UCMD_GUI*    getCmdGUI() {return(m_cmd_gui);}
  void         closeCmdGUI();
  
 public: // AppCasting Related Functions

  void         updateNodes(bool clear=false);
  void         updateProcs(bool clear=false);
  void         updateAppCast();
  void         setAppCastRepo(AppCastRepo* repo) {m_repo = repo;}
  bool         showingAppCasts() const;
  void         updateRadios();
  void         setMenuItemColors();
  bool         setRadioCastAttrib(std::string attr, std::string val="");

 protected:
  void         removeFilterVehicle(std::string vname);
  void         showDataFields();
  void         hideDataFields();
  void         resizeDataText(int);
  void         resizeWidgets();

 private:
  inline void cb_MOOS_Button_i(unsigned int);
  static void cb_MOOS_Button(Fl_Widget*, unsigned int);
  inline void cb_DoAction_i(unsigned int);
  static void cb_DoAction(Fl_Widget*, unsigned int);
  inline void cb_Scope_i(unsigned int);
  static void cb_Scope(Fl_Widget*, unsigned int);

  inline void cb_AppCastSetting_i(unsigned int);
  static void cb_AppCastSetting(Fl_Widget*, unsigned int);

  inline void cb_LeftContext_i(unsigned int);
  static void cb_LeftContext(Fl_Widget*, unsigned int);
  inline void cb_RightContext_i(unsigned int);
  static void cb_RightContext(Fl_Widget*, unsigned int);
  inline void cb_Reference_i(unsigned int);
  static void cb_Reference(Fl_Widget*, unsigned int);
  inline void cb_FilterOut_i(int);
  static void cb_FilterOut(Fl_Widget*, int);

  inline void cb_DeleteActiveNode_i(int);
  static void cb_DeleteActiveNode(Fl_Widget*, int);

  inline void cb_CommandGUI_i();
  static void cb_CommandGUI(Fl_Widget*);

  inline void cb_SelectNode_i();
  static void cb_SelectNode(Fl_Widget*, long);
  inline void cb_SelectProc_i();
  static void cb_SelectProc(Fl_Widget*, long);

 public:
  PMV_Viewer*  mviewer;

 protected:
  Fl_Output  *v_nam;
  Fl_Output  *v_typ;
  Fl_Output  *x_mtr;
  Fl_Output  *y_mtr;
  Fl_Output  *v_spd;
  Fl_Output  *v_lat;
  Fl_Output  *v_lon;
  Fl_Output  *v_crs;
  Fl_Output  *v_dep;
  Fl_Output  *v_ais;
  Fl_Output  *time;
  Fl_Output  *warp;
  Fl_Output  *v_range;
  Fl_Output  *v_bearing;

  Fl_Output  *m_scope_variable;
  Fl_Output  *m_scope_time;
  Fl_Output  *m_scope_value;

  Fl_Button  *m_user_button_1;
  Fl_Button  *m_user_button_2;
  Fl_Button  *m_user_button_3;
  Fl_Button  *m_user_button_4;

  std::vector<std::string> m_scope_vars;
  
  // Poking via on-screen buttons
  std::vector<std::string> m_button_keys;
  std::vector<std::string> m_button_vars;
  std::vector<std::string> m_button_vals;

  // Poking via pull-down menu selections
  std::vector<std::string> m_action_vars;
  std::vector<std::string> m_action_vals;
  std::vector<std::string> m_action_keys;

  // Poking via mouse clicking
  std::vector<std::string> m_left_mouse_keys;
  std::vector<std::string> m_right_mouse_keys;

  std::vector<std::string> m_reference_tags;
  std::vector<std::string> m_filter_tags;

  double    m_curr_time;
  double    m_clear_stale_timestamp;
  
 protected: // Member variables added for AppCasting
  AppCastRepo     *m_repo;
  MY_Fl_Hold_Browser *m_brw_nodes;
  MY_Fl_Hold_Browser *m_brw_procs;
  MY_Fl_Hold_Browser *m_brw_casts;

  VPlug_AppCastSettings m_ac_settings;

  Fl_Color    m_color_runw;
  Fl_Color    m_color_cfgw;
  Fl_Color    m_color_stlw;

  CommandFolio   m_cmd_folio;
  CommandSummary m_cmd_summary;
  UCMD_GUI*      m_cmd_gui;

  // Info about the current state of the command sub-gui so it
  // may be restored when/if it is re-launched.
  int            m_cmd_gui_start_wid;
  int            m_cmd_gui_start_hgt;
  unsigned int   m_cmd_gui_start_bpost_cnt;
  unsigned int   m_cmd_gui_start_cpost_cnt;
  bool           m_cmd_gui_start_show_posts;
  
  std::string m_title_base;
};
#endif









