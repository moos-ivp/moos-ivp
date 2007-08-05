/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GUI.h                                                */
/*    DATE: Apr 15th 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef SIMPLE_A_GUI_HEADER
#define SIMPLE_A_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "Viewer.h"

class GUI : Fl_Window {
public:
  GUI(int w, int h, const char *l=0);

  static Fl_Menu_Item menu_[];

  int  handle(int);
  void addIPF(IvPFunction* ipf)   {viewer->addIPF(ipf);};
  void setAOF(AOF* aof, int ix=0)           
    {viewer->setAOF(aof, ix); viewer->applyFColorMap();};

public:
  Fl_Menu_Bar *mbar;
  Viewer      *viewer;

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

  inline void cb_ModBaseAOF_i(int);
  static void cb_ModBaseAOF(Fl_Widget*, int);

  inline void cb_ModBaseIPF_i(int);
  static void cb_ModBaseIPF(Fl_Widget*, int);

  inline void cb_ToggleAOF_i();
  static void cb_ToggleAOF(Fl_Widget*);

  inline void cb_ToggleIPF_i();
  static void cb_ToggleIPF(Fl_Widget*);

  inline void cb_ToggleFrame_i();
  static void cb_ToggleFrame(Fl_Widget*);

  inline void cb_ToggleStrict_i();
  static void cb_ToggleStrict(Fl_Widget*);

  inline void cb_ModPatchAOF_i(int);
  static void cb_ModPatchAOF(Fl_Widget*, int);

  inline void cb_ModIndexIPF_i(int);
  static void cb_ModIndexIPF(Fl_Widget*, int);

  inline void cb_MakeUniform_i(int);
  static void cb_MakeUniform(Fl_Widget*, int);

  inline void cb_ColorMap_i(int);
  static void cb_ColorMap(Fl_Widget*, int);

  inline void cb_ModIntensity_i(int);
  static void cb_ModIntensity(Fl_Widget*, int);

  inline void cb_ModBIntensity_i(int);
  static void cb_ModBIntensity(Fl_Widget*, int);

  inline void cb_ModPolar_i(int);
  static void cb_ModPolar(Fl_Widget*, int);

  inline void cb_ModZSlice_i(int);
  static void cb_ModZSlice(Fl_Widget*, int);

  static void cb_Quit();
};
#endif
