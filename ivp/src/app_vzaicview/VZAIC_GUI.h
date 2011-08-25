/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VZAIC_GUI.h                                          */
/*    DATE: May 4th, 2010                                        */
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

#ifndef VZAIC_GUI_HEADER
#define VZAIC_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "VZAIC_Viewer.h"
#include <FL/Fl_Hold_Browser.H>
#include "MY_Output.h"
#include "ZAIC_Vector.h"

class VZAIC_GUI : Fl_Window {
public:
  VZAIC_GUI(int w, int h, const char *l=0);
  ~VZAIC_GUI() {};
  
  static Fl_Menu_Item menu_[];

  void updateOutput();
  int  handle(int);

  void setZAIC(ZAIC_Vector *zaic);

public:
  Fl_Menu_Bar  *mbar;
  VZAIC_Viewer *m_zaic_viewer;

  MY_Output   *s_maxutil;
  MY_Output   *s_minutil;

private:
  static void cb_Quit();

  inline void cb_GridSize_i(int);
  static void cb_GridSize(Fl_Widget*, int);

  inline void cb_GridShade_i(int);
  static void cb_GridShade(Fl_Widget*, int);

  inline void cb_LineShade_i(int);
  static void cb_LineShade(Fl_Widget*, int);

  inline void cb_BackShade_i(int);
  static void cb_BackShade(Fl_Widget*, int);

  inline void cb_DrawLabels_i(int);
  static void cb_DrawLabels(Fl_Widget*, int);
};
#endif

