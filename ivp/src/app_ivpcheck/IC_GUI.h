/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: IC_GUI.h                                            */
/*    DATE: Apr 19th 2011                                       */
/****************************************************************/

#ifndef IVP_CHECK_GUI_HEADER
#define IVP_CHECK_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "IC_Viewer.h"
#include "MY_Output.h"

class IC_GUI : Fl_Window {
public:
  IC_GUI(int w, int h, const char *l=0);

  static Fl_Menu_Item menu_[];

  int  handle(int);
  void setViewer(IC_Viewer* g_viewer) {viewer = g_viewer;};

  void updateFields();
  
  IC_Viewer* getViewer() {return(viewer);};

protected:
  Fl_Menu_Bar *mbar;
  MY_Output   *curr_function;
  MY_Output   *curr_func_size;
  MY_Output   *curr_func_pwt;
  MY_Output   *total_funcs;
  MY_Output   *ivp_domain;
  MY_Output   *focus_point;
  IC_Viewer   *viewer;

private:
  inline void cb_Zoom_i(int);
  static void cb_Zoom(Fl_Widget*, int);

  inline void cb_Reset_i(int);
  static void cb_Reset(Fl_Widget*, int);

  inline void cb_RotateX_i(int);
  static void cb_RotateX(Fl_Widget*, int);

  inline void cb_RotateZ_i(int);
  static void cb_RotateZ(Fl_Widget*, int);

  inline void cb_StretchRad_i(int);
  static void cb_StretchRad(Fl_Widget*, int);

  inline void cb_Solve_i(int);
  static void cb_Solve(Fl_Widget*, int);

  inline void cb_EvalFocusPoint_i();
  static void cb_EvalFocusPoint(Fl_Widget*);

  inline void cb_ToggleFrame_i();
  static void cb_ToggleFrame(Fl_Widget*);

  inline void cb_ColorMap_i(int);
  static void cb_ColorMap(Fl_Widget*, int);

  inline void cb_IncCurrFunction_i(int);
  static void cb_IncCurrFunction(Fl_Widget*, int);

  inline void cb_ToggleCollectiveView_i();
  static void cb_ToggleCollectiveView(Fl_Widget*);

  static void cb_Quit();
  
};
#endif
