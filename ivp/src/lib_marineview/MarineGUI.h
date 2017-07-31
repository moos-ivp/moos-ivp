/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MarineGUI.h                                          */
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

#ifndef COMMON_MARINE_GUI_HEADER
#define COMMON_MARINE_GUI_HEADER

#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "MarineViewer.h"

class MarineGUI : public Fl_Window {
public:
  MarineGUI(int w, int h, const char *l=0);
  ~MarineGUI() {}

  static Fl_Menu_Item menu_[];

  int  handle(int);
  virtual void updateXY() {}

  void addGeoAttrMenu();
  void updateRadios();
  void setMenuItemColors();

  //void setMenuItemColor(std::string item, int r=31, int g=71, int b=195);
  void setMenuItemColor(std::string item, int r=31, int g=71, int b=155);
  bool setMenuAttrib(std::string menu, std::string attr, std::string val="");

  bool removeMenuItem(std::string);

  MarineViewer* getMarineViewer() {return(m_mviewer);}

 protected:
  Fl_Menu_Bar  *m_menubar;
  MarineViewer *m_mviewer;

protected:
  std::vector<std::string> m_pending_vars;
  std::vector<std::string> m_pending_vals;

  virtual void cb_Zoom_i(int);
  static  void cb_Zoom(Fl_Widget*, int);

  virtual void cb_HandleUpDown_i(int);
  static  void cb_HandleUpDown(Fl_Widget*, int);

  virtual void cb_HandleLeftRight_i(int);
  static  void cb_HandleLeftRight(Fl_Widget*, int);

  inline void cb_ToggleTiffType_i();
  static void cb_ToggleTiffType(Fl_Widget*);

  inline void cb_ToggleHash_i();
  static void cb_ToggleHash(Fl_Widget*);

  inline void cb_HashDelta_i(int);
  static void cb_HashDelta(Fl_Widget*, int);

  inline void cb_HashShade_i(int);
  static void cb_HashShade(Fl_Widget*, int);

  inline void cb_SetGeoAttr_i(int);
  static void cb_SetGeoAttr(Fl_Widget*, int);

  inline void cb_DatumSize_i(int);
  static void cb_DatumSize(Fl_Widget*, int);

  inline void cb_BackShade_i(int);
  static void cb_BackShade(Fl_Widget*, int);

  static void cb_Quit();
};
#endif








