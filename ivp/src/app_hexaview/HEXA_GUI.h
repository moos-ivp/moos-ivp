/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HEXA_GUI.h                                           */
/*    DATE: Oct 31 2005                                          */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef HEXA_GUI_HEADER
#define HEXA_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "HexaViewer.h"
#include "MarineGUI.h"
#include "MY_Output.h"

class HEXA_GUI : public MarineGUI {
public:
  HEXA_GUI(int w, int h, const char *l=0);
  ~HEXA_GUI() {};
  
  void updateXY();
  int  handle(int);

public:
  HexaViewer *hviewer;

  MY_Output  *p_snap;
  MY_Output  *p_spec;

 protected:
  void augmentMenu();
 
private:
  inline void cb_Clear_i();
  static void cb_Clear(Fl_Widget*);

  inline void cb_ClearActive_i();
  static void cb_ClearActive(Fl_Widget*);

  inline void cb_AdjustActive_i(int);
  static void cb_AdjustActive(Fl_Widget*, int);

  inline void cb_ShiftX_i(int);
  static void cb_ShiftX(Fl_Widget*, int);

  inline void cb_ShiftY_i(int);
  static void cb_ShiftY(Fl_Widget*, int);

  inline void cb_SnapVal_i(int);
  static void cb_SnapVal(Fl_Widget*, int);

  inline void cb_ToggleView_i();
  static void cb_ToggleView(Fl_Widget*);

  inline void cb_ReApplySnapVal_i();
  static void cb_ReApplySnapVal(Fl_Widget*);

  inline void cb_DumpPolySpec_i(int);
  static void cb_DumpPolySpec(Fl_Widget*, int);
};
#endif

