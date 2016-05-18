/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_SPD_GUI.h                                       */
/*    DATE: June 2nd, 2015                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ZAIC_SPD_GUI_HEADER
#define ZAIC_SPD_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Hold_Browser.H>
#include "ZAIC_Viewer.h"
#include "ZAIC_SPD_Model.h"

class ZAIC_SPD_GUI : Fl_Window {
public:
  ZAIC_SPD_GUI(int w, int h, const char *l=0);
  ~ZAIC_SPD_GUI() {};
  
  static Fl_Menu_Item menu_[];

  void setVerbose(bool v)        {m_zaic_viewer->setVerbose(v);}
  void updateOutput();
  int  handle(int);

public:
  Fl_Menu_Bar     *mbar;
  ZAIC_Viewer     *m_zaic_viewer;
  ZAIC_SPD_Model  *m_zaic_model;
  
  Fl_Output   *m_medval;
  Fl_Output   *m_lowval;
  Fl_Output   *m_hghval;
  Fl_Output   *m_lowval_util;
  Fl_Output   *m_hghval_util;

private:
  static void cb_Quit();

  inline void cb_MoveX_i(int);
  static void cb_MoveX(Fl_Widget*, int);

  inline void cb_CurrMode_i(int);
  static void cb_CurrMode(Fl_Widget*, int);

  inline void cb_GridSize_i(int);
  static void cb_GridSize(Fl_Widget*, int);

  inline void cb_GridShade_i(int);
  static void cb_GridShade(Fl_Widget*, int);

  inline void cb_LineShade_i(int);
  static void cb_LineShade(Fl_Widget*, int);

  inline void cb_BackShade_i(int);
  static void cb_BackShade(Fl_Widget*, int);

  inline void cb_ToggleVerbose_i();
  static void cb_ToggleVerbose(Fl_Widget*);

  inline void cb_ToggleLabels_i();
  static void cb_ToggleLabels(Fl_Widget*);

  inline void cb_ToggleColorScheme_i();
  static void cb_ToggleColorScheme(Fl_Widget*);
};
#endif
