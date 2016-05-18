/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_LEQ_GUI.h                                       */
/*    DATE: Apr 6th, 2008                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ZAIC_LEQ_GUI_HEADER
#define ZAIC_LEQ_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Output.H>
#include "ZAIC_LEQ_Viewer.h"

class ZAIC_LEQ_GUI : Fl_Window {
public:
  ZAIC_LEQ_GUI(int w, int h, const char *l=0);
  ~ZAIC_LEQ_GUI() {};
  
  static Fl_Menu_Item menu_[];

  void updateOutput();
  int  handle(int);

 protected:
  Fl_Menu_Bar      *mbar;
  ZAIC_LEQ_Viewer  *zaic_viewer;

  Fl_Output   *m_fld_summit;
  Fl_Output   *m_fld_bwidth;

  Fl_Output   *m_fld_minutil;
  Fl_Output   *m_fld_maxutil;

  Fl_Output   *m_fld_sumdelta;

 private:
  static void cb_Quit();

  inline void cb_MoveX_i(int);
  static void cb_MoveX(Fl_Widget*, int);

  inline void cb_CurrMode_i(int);
  static void cb_CurrMode(Fl_Widget*, int);

  inline void cb_ToggleHLEQ_i();
  static void cb_ToggleHLEQ(Fl_Widget*);

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
