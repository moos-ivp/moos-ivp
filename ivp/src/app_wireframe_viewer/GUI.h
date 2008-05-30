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
#include <FL/Fl_File_Chooser.H>
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

  inline void cb_CameraYawRotate_i(int);
  static void cb_CameraYawRotate(Fl_Widget*, int);

  inline void cb_CameraPitchRotate_i(int);
  static void cb_CameraPitchRotate(Fl_Widget*, int);

  inline void cb_COI_Origin_i();
  static void cb_COI_Origin(Fl_Widget*);

  inline void cb_COI_Next_i();
  static void cb_COI_Next(Fl_Widget*);

  inline void cb_Toggle_Grid_i();
  static void cb_Toggle_Grid(Fl_Widget*);

  inline void cb_Toggle_Floor_i();
  static void cb_Toggle_Floor(Fl_Widget*);

  inline void cb_Toggle_Cam_Pos_i();
  static void cb_Toggle_Cam_Pos(Fl_Widget*);

  inline void cb_Floor_Depth_i(int);
  static void cb_Floor_Depth(Fl_Widget*, int);

  inline void cb_ToggleAnimation_i();
  static void cb_ToggleAnimation(Fl_Widget*);

  inline void cb_ViewMode_i(int);
  static void cb_ViewMode(Fl_Widget*, int);

  inline void cb_ViewSpeed_i(int);
  static void cb_ViewSpeed(Fl_Widget*, int);

  inline void cb_Open_i();
  static void cb_Open(Fl_Widget*);

  static void cb_Quit();
};
#endif
