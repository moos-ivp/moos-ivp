/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MVS_GUI.h                                            */
/*    DATE: May 14th 2005 Sat move from 221B to 422              */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef KAYAK_MVS_GUI_HEADER
#define KAYAK_MVS_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Hold_Browser.H>
#include "MarineViewer.h"
#include "MY_Output.h"
#include "MY_Hold_Browser.h"
#include "MV_GUI.h"
#include "SIM.h"
#include "IB_HelmIvP.h"

class MVS_GUI : public MV_GUI {
public:
  MVS_GUI(int w, int h, const char *l=0);

  int  handle(int);

  void set_sim(SIM *val)          {sim = val;};
  void set_ibuff(IB_HelmIvP *val) {info_buffer = val;};
  void add_steps(int v)           {steps_to_do += v; pause=false;};
  void conditional_step();

  void step_act();
  void step_decide();
  void step_render();

protected:
  void handle_drawofs();

protected:
  SIM*        sim;
  IB_HelmIvP* info_buffer;
  int         draw_ofs;
  int         steps_to_do;
  int         last_active;
  bool        pause;
  bool        first_step;

  inline void cb_ImageRes_i(int);
  static void cb_ImageRes(Fl_Widget*, int);

  inline void cb_ImageRadius_i(int);
  static void cb_ImageRadius(Fl_Widget*, int);

  static void cb_AddSteps(Fl_Widget*, int);

  inline void cb_Images_i();
  static void cb_Images(Fl_Widget*);

  inline void cb_FunctionSave_i();
  static void cb_FunctionSave(Fl_Widget*);

  inline void cb_DeltaCrs_i(int);
  static void cb_DeltaCrs(Fl_Widget*, int);

  inline void cb_DeltaSpd_i(int);
  static void cb_DeltaSpd(Fl_Widget*, int);

  inline void cb_Pause_i();
  static void cb_Pause(Fl_Widget*);

  inline void cb_Reset_i();
  static void cb_Reset(Fl_Widget*);

};
#endif




