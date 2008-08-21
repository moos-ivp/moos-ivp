/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: FV_GUI.h                                            */
/*    DATE: May 13th 2006                                       */
/****************************************************************/

#ifndef FUNCTION_VIEWER_GUI_HEADER
#define FUNCTION_VIEWER_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "FV_Viewer.h"
#include "FV_Model.h"
#include "MY_Output.h"

class FV_GUI : Fl_Window {
public:
  FV_GUI(int w, int h, const char *l=0);

  static Fl_Menu_Item menu_[];

  int  handle(int);
  void setModel(FV_Model* g_model)
    {model = g_model; viewer->setModel(g_model);};

  void updateFields();
  
  FV_Viewer* getViewer() {return(viewer);};

protected:
  Fl_Menu_Bar *mbar;
  MY_Output   *curr_func_size;
  MY_Output   *curr_function;
  MY_Output   *curr_iteration;
  FV_Model    *model;
  FV_Viewer   *viewer;

private:
  inline void cb_Zoom_i(int);
  static void cb_Zoom(Fl_Widget*, int);
  inline void cb_PanY_i(int);
  static void cb_PanY(Fl_Widget*, int);
  inline void cb_PanX_i(int);
  static void cb_PanX(Fl_Widget*, int);

  inline void cb_Reset_i(int);
  static void cb_Reset(Fl_Widget*, int);

  inline void cb_RotateX_i(int);
  static void cb_RotateX(Fl_Widget*, int);

  inline void cb_RotateY_i(int);
  static void cb_RotateY(Fl_Widget*, int);

  inline void cb_RotateZ_i(int);
  static void cb_RotateZ(Fl_Widget*, int);

  inline void cb_StretchRad_i(int);
  static void cb_StretchRad(Fl_Widget*, int);

  inline void cb_ModBase_i(int);
  static void cb_ModBase(Fl_Widget*, int);

  inline void cb_ModScale_i(int);
  static void cb_ModScale(Fl_Widget*, int);

  inline void cb_ToggleFrame_i();
  static void cb_ToggleFrame(Fl_Widget*);

  inline void cb_ToggleLockIPF_i();
  static void cb_ToggleLockIPF(Fl_Widget*);

  inline void cb_ColorMap_i(int);
  static void cb_ColorMap(Fl_Widget*, int);

  inline void cb_ModColorRange_i(int);
  static void cb_ModColorRange(Fl_Widget*, int);

  inline void cb_ModZSlice_i(int);
  static void cb_ModZSlice(Fl_Widget*, int);

  inline void cb_IncCurrFunction_i(int);
  static void cb_IncCurrFunction(Fl_Widget*, int);

  inline void cb_ToggleCollectiveView_i();
  static void cb_ToggleCollectiveView(Fl_Widget*);

  static void cb_Quit();

};
#endif
