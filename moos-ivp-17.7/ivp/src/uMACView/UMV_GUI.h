/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
#include "MY_Fl_Hold_Browser.h"
#include "VPlug_AppCastSettings.h"
#include "AppCastRepo.h"

class UMV_GUI : Fl_Window {
public:
  UMV_GUI(int w, int h, const char *l=0);
  ~UMV_GUI() {}
  
  void augmentMenu();
  void resize(int, int, int, int);

  void updateNodes(bool clear=false);
  void updateProcs(bool clear=false);
  void updateAppCast();

  void setAppCastRepo(AppCastRepo* repo) {m_repo = repo;}

  bool setRadioCastAttrib(std::string attr, std::string val="");

  void updateRadios();

 protected:
  void resizeWidgets();

 private:
  inline void cb_SelectNode_i();
  static void cb_SelectNode(Fl_Widget*, long);

  inline void cb_SelectProc_i();
  static void cb_SelectProc(Fl_Widget*, long);

  inline void cb_AppCastSetting_i(unsigned int);
  static void cb_AppCastSetting(Fl_Widget*, unsigned int);

  static void cb_Quit();

 protected:
  MY_Fl_Hold_Browser *m_brw_nodes;
  MY_Fl_Hold_Browser *m_brw_procs;
  MY_Fl_Hold_Browser *m_brw_casts;
  Fl_Menu_Bar        *m_menubar;
  AppCastRepo        *m_repo;

  VPlug_AppCastSettings m_ac_settings;

  Fl_Color     m_color_runw;
  Fl_Color     m_color_cfgw;
};
#endif









