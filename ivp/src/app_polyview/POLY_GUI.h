/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: POLY_GUI.h                                           */
/*    DATE: May 1st, 2005                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef POLY_GUI_HEADER
#define POLY_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "PolyViewer.h"
#include <FL/Fl_Hold_Browser.H>
#include "MarineGUI.h"
#include "MY_Output.h"

class POLY_GUI : public MarineGUI {
public:
  POLY_GUI(int w, int h, const char *l=0);
  ~POLY_GUI() {};
  
  void updateXY();
  int  handle(int);
  void addCircle(XYCircle);
  void addArc(XYArc);

public:
  PolyViewer  *pviewer;

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

  inline void cb_Mode8Angle_i(int);
  static void cb_Mode8Angle(Fl_Widget*, int);

  inline void cb_ReApplySnapVal_i();
  static void cb_ReApplySnapVal(Fl_Widget*);

  inline void cb_Duplicate_i();
  static void cb_Duplicate(Fl_Widget*);

  inline void cb_DumpPolySpec_i();
  static void cb_DumpPolySpec(Fl_Widget*);
};
#endif


