/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ARC_GUI.h                                            */
/*    DATE: Jan 16th, 2011                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ARC_GUI_HEADER
#define ARC_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "ARCViewer.h"
#include <FL/Fl_Hold_Browser.H>
#include "MY_Output.h"

class ARC_GUI : Fl_Window {
public:
  ARC_GUI(int w, int h, const char *l=0);
  ~ARC_GUI() {};
  
  static Fl_Menu_Item menu_[];

  void updateOutput();
  int  handle(int);

public:
  Fl_Menu_Bar *mbar;
  ARCViewer   *arc_viewer;

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
