/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GEO_GUI.h                                            */
/*    DATE: May 1st, 2005 (Major Modifications November 2007)    */
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

#ifndef GEO_GUI_HEADER
#define GEO_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "GeoViewer.h"
#include <FL/Fl_Hold_Browser.H>
#include "MarineGUI.h"
#include "MY_Output.h"

class GEO_GUI : public MarineGUI {
public:
  GEO_GUI(int w, int h, const char *l=0);
  ~GEO_GUI() {}
  
  void updateXY();
  int  handle(int);

public:
  GeoViewer  *pviewer;

  MY_Output   *x_mtr;
  MY_Output   *y_mtr;

  MY_Output   *p_mode;
  MY_Output   *p_snap;
  MY_Output   *p_spec;

 protected:
  void augmentMenu();
 
private:
  inline void cb_Clear_i();
  static void cb_Clear(Fl_Widget*);
  
  inline void cb_CreateNew_i();
  static void cb_CreateNew(Fl_Widget*);

  inline void cb_ClearActive_i();
  static void cb_ClearActive(Fl_Widget*);

  inline void cb_AdjustActive_i(int);
  static void cb_AdjustActive(Fl_Widget*, int);

  inline void cb_DropMode_i(int);
  static void cb_DropMode(Fl_Widget*, int);

  inline void cb_Rotate_i(int);
  static void cb_Rotate(Fl_Widget*, int);

  inline void cb_ShiftX_i(int);
  static void cb_ShiftX(Fl_Widget*, int);

  inline void cb_ShiftY_i(int);
  static void cb_ShiftY(Fl_Widget*, int);

  inline void cb_Grow_i(int);
  static void cb_Grow(Fl_Widget*, int);

  inline void cb_Reverse_i();
  static void cb_Reverse(Fl_Widget*);

  inline void cb_SnapVal_i(int);
  static void cb_SnapVal(Fl_Widget*, int);

  inline void cb_SetColor_i(int);
  static void cb_SetColor(Fl_Widget*, int);

  inline void cb_ReApplySnapVal_i();
  static void cb_ReApplySnapVal(Fl_Widget*);

  inline void cb_Duplicate_i();
  static void cb_Duplicate(Fl_Widget*);

  inline void cb_DumpPolySpec_i();
  static void cb_DumpPolySpec(Fl_Widget*);
};
#endif










