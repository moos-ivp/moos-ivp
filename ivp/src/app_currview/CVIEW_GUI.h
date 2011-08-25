/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CVIEW_GUI.h                                          */
/*    DATE: January 4th, 2011                                    */
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

#ifndef CVIEW_GUI_HEADER
#define CVIEW_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "CurrViewer.h"
#include <FL/Fl_Hold_Browser.H>
#include "MarineGUI.h"
#include "MY_Output.h"

class CVIEW_GUI : public MarineGUI {
public:
  CVIEW_GUI(int w, int h, const char *l=0);
  ~CVIEW_GUI() {};
  
  void updateXY();
  int  handle(int);

public:
  CurrViewer  *pviewer;

  MY_Output   *v_ang;
  MY_Output   *v_mag;
  MY_Output   *x_mtr;
  MY_Output   *y_mtr;
  MY_Output   *v_lat;
  MY_Output   *v_lon;

  MY_Output   *v_amt;
  MY_Output   *v_idx;

  MY_Output   *p_mode;
  MY_Output   *p_snap;

 protected:
  void augmentMenu();
 
private:
  inline void cb_Clear_i();
  static void cb_Clear(Fl_Widget*);
  
  inline void cb_DeleteMarked_i();
  static void cb_DeleteMarked(Fl_Widget*);

  inline void cb_DeleteLastAdded_i();
  static void cb_DeleteLastAdded(Fl_Widget*);

  inline void cb_FileSave_i();
  static void cb_FileSave(Fl_Widget*);

  inline void cb_FileSaveAs_i();
  static void cb_FileSaveAs(Fl_Widget*);

  inline void cb_ClearActive_i();
  static void cb_ClearActive(Fl_Widget*);

  inline void cb_AdjustActive_i(int);
  static void cb_AdjustActive(Fl_Widget*, int);

  inline void cb_DropMode_i(int);
  static void cb_DropMode(Fl_Widget*, int);

  inline void cb_ShiftX_i(int);
  static void cb_ShiftX(Fl_Widget*, int);

  inline void cb_ShiftY_i(int);
  static void cb_ShiftY(Fl_Widget*, int);

  inline void cb_Rotate_i(int);
  static void cb_Rotate(Fl_Widget*, int);

  inline void cb_Magnitude_i(int);
  static void cb_Magnitude(Fl_Widget*, int);

  inline void cb_SnapVal_i(int);
  static void cb_SnapVal(Fl_Widget*, int);

  inline void cb_SetColor_i(int);
  static void cb_SetColor(Fl_Widget*, int);

  inline void cb_ApplySnapVal_i();
  static void cb_ApplySnapVal(Fl_Widget*);

  inline void cb_DumpPolySpec_i();
  static void cb_DumpPolySpec(Fl_Widget*);
};
#endif


