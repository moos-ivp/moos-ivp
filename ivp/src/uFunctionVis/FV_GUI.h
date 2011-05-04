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
#include "MY_Button.h"

class FV_GUI : Fl_Window {
public:
  FV_GUI(int w, int h, const char *l=0);

  static Fl_Menu_Item menu_[];

  int  handle(int);
  void setModel(FV_Model* g_model)
    {m_model = g_model; m_viewer->setModel(g_model);};

  void updateFields();
  
  FV_Viewer* getViewer() {return(m_viewer);};

protected:
  Fl_Menu_Bar *m_menu_bar;
  MY_Output   *m_curr_plat;
  MY_Output   *m_curr_pcs;
  MY_Output   *m_curr_pwt;
  MY_Output   *m_curr_src;
  MY_Output   *m_curr_domain;
  MY_Output   *curr_iteration;
  FV_Model    *m_model;
  FV_Viewer   *m_viewer;

  MY_Button   *m_but_ipf_set;
  MY_Button   *m_but_ipf_pin;

private:
  inline void cb_Zoom_i(int);
  static void cb_Zoom(Fl_Widget*, int);

  inline void cb_RotateX_i(int);
  static void cb_RotateX(Fl_Widget*, int);

  inline void cb_RotateZ_i(int);
  static void cb_RotateZ(Fl_Widget*, int);

  inline void cb_StretchRad_i(int);
  static void cb_StretchRad(Fl_Widget*, int);

  inline void cb_ToggleFrame_i();
  static void cb_ToggleFrame(Fl_Widget*);

  inline void cb_ToggleLockIPF_i();
  static void cb_ToggleLockIPF(Fl_Widget*);

  inline void cb_ColorMap_i(int);
  static void cb_ColorMap(Fl_Widget*, int);

  inline void cb_IncCurrFunction_i(int);
  static void cb_IncCurrFunction(Fl_Widget*, int);

  inline void cb_ToggleCollectiveView_i();
  static void cb_ToggleCollectiveView(Fl_Widget*);

  inline void cb_ToggleSet_i();
  static void cb_ToggleSet(Fl_Widget*);

  inline void cb_TogglePin_i();
  static void cb_TogglePin(Fl_Widget*);

  static void cb_Quit();

};
#endif
