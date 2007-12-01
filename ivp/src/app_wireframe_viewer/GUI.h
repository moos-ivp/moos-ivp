/*****************************************************************/
/*    NAME: Doug Maxwell ane Michael Benjamin                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GUI.h                                                */
/*    DATE: November 2007                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef WIREFRAME_GUI_HEADER
#define FIREFRAME_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "WireframeViewer.h"

class GUI : Fl_Window {
public:
  GUI(int w, int h, const char *l=0);

  static Fl_Menu_Item menu_[];

  int  handle(int);

public:
  Fl_Menu_Bar      *mbar;
  WireframeViewer  *viewer;

private:
  inline void cb_Zoom_i(int);
  static void cb_Zoom(Fl_Widget*, int);

  inline void cb_PanLR_i(int);
  static void cb_PanLR(Fl_Widget*, int);

  inline void cb_PanUD_i(int);
  static void cb_PanUD(Fl_Widget*, int);

  inline void cb_Reset_i();
  static void cb_Reset(Fl_Widget*);

  inline void cb_CameraX_i(int);
  static void cb_CameraX(Fl_Widget*, int);

  inline void cb_CameraY_i(int);
  static void cb_CameraY(Fl_Widget*, int);

  inline void cb_CameraZ_i(int);
  static void cb_CameraZ(Fl_Widget*, int);

  inline void cb_Rotate_i(int);
  static void cb_Rotate(Fl_Widget*, int);

  inline void cb_ToggleAnimation_i();
  static void cb_ToggleAnimation(Fl_Widget*);

  inline void cb_ViewMode_i(int);
  static void cb_ViewMode(Fl_Widget*, int);

  static void cb_Quit();
};
#endif

