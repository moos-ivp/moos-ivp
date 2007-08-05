/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_GUI.h                                           */
/*    DATE: June 17th, 2006                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
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
  ~ZAIC_GUI() {};
  
  static Fl_Menu_Item menu_[];

  void updateOutput();
  int  handle(int);

public:
  Fl_Menu_Bar *mbar;
  ZAICViewer  *zaic_viewer;

  MY_Output   *summit;
  MY_Output   *p_width;
  MY_Output   *b_width;
  MY_Output   *s_height;
  MY_Output   *s_delta;

private:
  static void cb_Quit();

  inline void cb_MoveX_i(int);
  static void cb_MoveX(Fl_Widget*, int);

  inline void cb_CurrMode_i(int);
  static void cb_CurrMode(Fl_Widget*, int);

  inline void cb_ToggleWRAP_i();
  static void cb_ToggleWRAP(Fl_Widget*, int);

  inline void cb_ToggleTotal_i();
  static void cb_ToggleTotal(Fl_Widget*, int);

  inline void cb_CurrSumm_i(int);
  static void cb_CurrSumm(Fl_Widget*, int);
};
#endif
