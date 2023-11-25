/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: UMV_GUI.h                                            */
/*    DATE: Aug 11th, 2012                                       */
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

#ifndef UMV_PLOT_GUI_HEADER
#define UMV_PLOT_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Button.H>
#include "MY_Fl_Hold_Browser.h"
#include "InfoCastSettings.h"
#include "AppCastRepo.h"
#include "RealmRepo.h"

class UMV_GUI : Fl_Window {
public:
  UMV_GUI(int w, int h, const char *l=0);
  ~UMV_GUI() {}
  
  void augmentMenu();
  void resize(int, int, int, int);

  void updateAppCastNodes(bool clear=false);
  void updateAppCastProcs(bool clear=false);
  void updateAppCast();

  void setAppCastRepo(AppCastRepo* repo) {m_ac_repo = repo;}
  void setRealmRepo(RealmRepo* repo)     {m_rc_repo = repo;}

  void updateRealmCastNodes(bool clear=false);
  void updateRealmCastProcs(bool clear=false);
  void updateRealmCastProcsRC(bool);
  void updateRealmCastProcsWC(bool);


  void updateRealmCast();

  bool setRadioCastAttrib(std::string attr, std::string val="");
  void setMenuItemColor(std::string item, int r=31, int g=71, int b=155);

  void updateRadios();
  void setMenuItemColors();

  InfoCastSettings getInfoCastSettings() const {return(m_icast_settings);}
  
 protected:
  void resizeWidgets();

 private:
  inline void cb_REALM_Button_i(unsigned int);
  static void cb_REALM_Button(Fl_Widget*, unsigned int);
  
  inline void cb_SelectAppCastNode_i();
  static void cb_SelectAppCastNode(Fl_Widget*, long);
  inline void cb_SelectAppCastProc_i();
  static void cb_SelectAppCastProc(Fl_Widget*, long);

  inline void cb_SelectRealmCastNode_i();
  static void cb_SelectRealmCastNode(Fl_Widget*, long);
  inline void cb_SelectRealmCastProc_i();
  static void cb_SelectRealmCastProc(Fl_Widget*, long);

  inline void cb_InfoCastSetting_i(unsigned int);
  static void cb_InfoCastSetting(Fl_Widget*, unsigned int);

  static void cb_Quit();

 protected:
  Fl_Menu_Bar        *m_menubar;

 protected: // Member variables added for AppCasting 
  MY_Fl_Hold_Browser *m_ac_brw_nodes;
  MY_Fl_Hold_Browser *m_ac_brw_procs;
  MY_Fl_Hold_Browser *m_ac_brw_casts;
  AppCastRepo        *m_ac_repo;

  MY_Fl_Hold_Browser *m_rc_brw_nodes;
  MY_Fl_Hold_Browser *m_rc_brw_procs;
  MY_Fl_Hold_Browser *m_rc_brw_casts;
  RealmRepo          *m_rc_repo;

  Fl_Button  *m_rc_button_src;
  Fl_Button  *m_rc_button_com;
  Fl_Button  *m_rc_button_wrp;

  Fl_Button  *m_rc_button_sub;
  Fl_Button  *m_rc_button_msk;
  Fl_Button  *m_rc_button_trc;

  Fl_Button  *m_rc_button_utc;

  InfoCastSettings m_icast_settings;
  
  Fl_Color     m_color_runw;
  Fl_Color     m_color_cfgw;
};
#endif

