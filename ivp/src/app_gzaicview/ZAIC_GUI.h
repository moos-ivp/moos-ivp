/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_GUI.h                                           */
/*    DATE: June 17th, 2006                                      */
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

#ifndef ZAIC_GUI_HEADER
#define ZAIC_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "ZAICViewer.h"
#include <FL/Fl_Hold_Browser.H>
#include "MY_Output.h"

class ZAIC_GUI : Fl_Window {
public:
  ZAIC_GUI(int w, int h, const char *l=0);
  ~ZAIC_GUI() {}
  
  static Fl_Menu_Item menu_[];

  void updateOutput();
  int  handle(int);

public:
  Fl_Menu_Bar *mbar;
  ZAICViewer  *zaic_viewer;

  MY_Output   *summit;
  MY_Output   *p_width;
  MY_Output   *b_width;
  MY_Output   *s_maxutil;
  MY_Output   *s_minutil;
  MY_Output   *s_delta;

private:
  static void cb_Quit();

  inline void cb_MoveX_i(int);
  static void cb_MoveX(Fl_Widget*, int);

  inline void cb_CurrMode_i(int);
  static void cb_CurrMode(Fl_Widget*, int);

  inline void cb_ToggleWRAP_i();
  static void cb_ToggleWRAP(Fl_Widget*);

  inline void cb_DrawMode_i(int);
  static void cb_DrawMode(Fl_Widget*, int);

  inline void cb_CurrZAIC_i();
  static void cb_CurrZAIC(Fl_Widget*);

  inline void cb_GridSize_i(int);
  static void cb_GridSize(Fl_Widget*, int);

  inline void cb_GridShade_i(int);
  static void cb_GridShade(Fl_Widget*, int);

  inline void cb_LineShade_i(int);
  static void cb_LineShade(Fl_Widget*, int);

  inline void cb_BackShade_i(int);
  static void cb_BackShade(Fl_Widget*, int);
};
#endif





