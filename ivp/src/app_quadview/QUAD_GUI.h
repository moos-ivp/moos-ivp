/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Quad_GUI.h                                           */
/*    DATE: June 20th, 2005                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef QUAD_GUI_HEADER
#define QUAD_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "QuadViewer.h"
#include <FL/Fl_Hold_Browser.H>
#include "MY_Output.h"

class QUAD_GUI : Fl_Window {
public:
  QUAD_GUI(int w, int h, const char *l=0);
  ~QUAD_GUI() {};
  
  static Fl_Menu_Item menu_[];

  void updateOutput();
  int  handle(int);

public:
  Fl_Menu_Bar *mbar;
  QuadViewer  *q_viewer;

  MY_Output   *pt1;
  MY_Output   *pt2;
  MY_Output   *pt3;

  MY_Output   *x2;
  MY_Output   *x1;
  MY_Output   *x0;

  MY_Output   *min_y;
  MY_Output   *min_x;
  MY_Output   *max_y;
  MY_Output   *max_x;


private:
  static void cb_Quit();

  inline void cb_MoveY_i(int);
  static void cb_MoveY(Fl_Widget*, int);

  inline void cb_MoveX_i(int);
  static void cb_MoveX(Fl_Widget*, int);

  inline void cb_CurrPt_i(int);
  static void cb_CurrPt(Fl_Widget*, int);


};
#endif





