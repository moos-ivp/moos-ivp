/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VZAIC_GUI.h                                          */
/*    DATE: May 4th, 2010                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
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
