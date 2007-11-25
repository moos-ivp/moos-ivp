/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: FV_GUI.h                                             */
/*    DATE: May 13th 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef FUNCTION_VIEWER_GUI_HEADER
#define FUNCTION_VIEWER_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "FV_Viewer.h"

class FV_GUI : Fl_Window {
public:
  FV_GUI(int w, int h, const char *l=0);

  static Fl_Menu_Item menu_[];

  int  handle(int);
  void setIPF(std::string str)   {viewer->setIPF(str);};

public:
  Fl_Menu_Bar *mbar;
  FV_Viewer   *viewer;

private:
  inline void cb_Zoom_i(int);
  static void cb_Zoom(Fl_Widget*, int);
  inline void cb_PanY_i(int);
  static void cb_PanY(Fl_Widget*, int);
  inline void cb_PanX_i(int);
  static void cb_PanX(Fl_Widget*, int);

  inline void cb_Reset_i();
  static void cb_Reset(Fl_Widget*);

  inline void cb_RotateX_i(int);
  static void cb_RotateX(Fl_Widget*, int);

  inline void cb_RotateY_i(int);
  static void cb_RotateY(Fl_Widget*, int);

  inline void cb_RotateZ_i(int);
  static void cb_RotateZ(Fl_Widget*, int);

  inline void cb_StretchX_i(int);
  static void cb_StretchX(Fl_Widget*, int);

  inline void cb_StretchY_i(int);
  static void cb_StretchY(Fl_Widget*, int);

  inline void cb_ModScale_i(int);
  static void cb_ModScale(Fl_Widget*, int);

  inline void cb_ModBaseIPF_i(int);
  static void cb_ModBaseIPF(Fl_Widget*, int);

  inline void cb_ToggleFrame_i();
  static void cb_ToggleFrame(Fl_Widget*);

  inline void cb_ColorMap_i(int);
  static void cb_ColorMap(Fl_Widget*, int);

  inline void cb_ModIntensity_i(int);
  static void cb_ModIntensity(Fl_Widget*, int);

  inline void cb_ModPolar_i(int);
  static void cb_ModPolar(Fl_Widget*, int);

  inline void cb_ModZSlice_i(int);
  static void cb_ModZSlice(Fl_Widget*, int);

  static void cb_Quit();
};
#endif





