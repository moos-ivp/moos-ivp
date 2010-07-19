/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CPA_GUI.cpp                                          */
/*    DATE: Feb 12, 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "CPA_GUI.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

CPA_GUI::CPA_GUI(int g_w, int g_h, const char *g_l)
  : MarineGUI(g_w, g_h, g_l) {

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  double osl = 25;

  os_maxspd_ctr = new Fl_Counter(osl+0, h()-95, 120, 20, "ownship max speed");
  os_maxspd_ctr->range(0, 10);
  os_maxspd_ctr->precision(1);
  os_maxspd_ctr->step(0.1);
  os_maxspd_ctr->lstep(1);
  os_maxspd_ctr->callback((Fl_Callback*)CPA_GUI::cb_AltOS_MAXSPD, (void *)0);

  os_maxtol_ctr = new Fl_Counter(osl+0, h()-45, 120, 20, "ownship time-on-leg");
  os_maxtol_ctr->range(0, 600);
  os_maxtol_ctr->precision(0);
  os_maxtol_ctr->step(1);
  os_maxtol_ctr->lstep(10);
  os_maxspd_ctr->callback((Fl_Callback*)CPA_GUI::cb_AltOS_MAXTOL, (void *)0);

  double cnl = 300;

  cn_hdg_ctr = new Fl_Counter(cnl+0, h()-95, 120, 20, "contact heading");
  cn_hdg_ctr->range(-360, 360);
  cn_hdg_ctr->precision(0);
  cn_hdg_ctr->step(1);
  cn_hdg_ctr->lstep(10);
  cn_hdg_ctr->callback((Fl_Callback*)CPA_GUI::cb_AltCNCRS, (void *)0);

  cn_spd_ctr = new Fl_Counter(cnl+0, h()-45, 120, 20, "contact speed");
  cn_spd_ctr->range(0, 10);
  cn_spd_ctr->precision(1);
  cn_spd_ctr->step(0.1);
  cn_spd_ctr->lstep(1);
  cn_spd_ctr->callback((Fl_Callback*)CPA_GUI::cb_AltCNSPD, (void *)0);

  cn_rng_ctr = new Fl_Counter(cnl+150, h()-95, 120, 20, "contact range");
  cn_rng_ctr->range(0, 1000);
  cn_rng_ctr->precision(0);
  cn_rng_ctr->step(1);
  cn_rng_ctr->lstep(10);
  cn_rng_ctr->callback((Fl_Callback*)CPA_GUI::cb_AltCNRNG, (void *)0);

  cn_bng_ctr = new Fl_Counter(cnl+150, h()-45, 120, 20, "contact bearing");
  cn_bng_ctr->range(-360, 360);
  cn_bng_ctr->precision(0);
  cn_bng_ctr->step(1);
  cn_bng_ctr->lstep(10);
  cn_bng_ctr->callback((Fl_Callback*)CPA_GUI::cb_AltCNBNG, (void *)0);

  cn_xpos_ctr = new Fl_Counter(cnl+300, h()-95, 120, 20, "contact x-pos");
  cn_xpos_ctr->range(-1000, 1000);
  cn_xpos_ctr->precision(0);
  cn_xpos_ctr->step(1);
  cn_xpos_ctr->lstep(10);
  cn_xpos_ctr->callback((Fl_Callback*)CPA_GUI::cb_AltCNX, (void *)0);

  cn_ypos_ctr = new Fl_Counter(cnl+300, h()-45, 120, 20, "contact y-pos");
  cn_ypos_ctr->range(-1000, 1000);
  cn_ypos_ctr->precision(0);
  cn_ypos_ctr->step(1);
  cn_ypos_ctr->lstep(10);
  cn_ypos_ctr->callback((Fl_Callback*)CPA_GUI::cb_AltCNY, (void *)0);

  cpa_model      = new CPA_Model;
  contact_index  = 0;
  ipf_resolution = 10;
  
  int info_size = 10;

  cpa_viewer = new CPAViewer(cpa_model, 0, 30, w()/2, h()-140);
  cmviewer   = cpa_viewer;

  ipf_viewer = new IPFViewer(cpa_model, (w()/2)+5, 30, (w()/2)-5, h()-140);

  augmentMenu();

  p_os_maxspd = new MY_Output(200, h()-60, 50, 20, "OS Top SPD:"); 
  p_os_maxspd->textsize(info_size); 
  p_os_maxspd->labelsize(info_size);

  p_os_maxtol = new MY_Output(200, h()-30, 50, 20, "OS Max TOL:"); 
  p_os_maxtol->textsize(info_size); 
  p_os_maxtol->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();

  ipf_viewer->resetIPF(0, ipf_resolution);
  ipf_viewer->redraw();
}

//----------------------------------------------------------------
// Procedure: augmentMenu

void CPA_GUI::augmentMenu() 
{
  mbar->add("Ownship/Reset", FL_ALT+'r', (Fl_Callback*)CPA_GUI::cb_ResetOS, (void*)0, 0);
  mbar->add("Ownship/MaxSpd+",         's', (Fl_Callback*)CPA_GUI::cb_AltOS_MAXSPD, (void*)1, 0);
  mbar->add("Ownship/MaxSpd-",         'x', (Fl_Callback*)CPA_GUI::cb_AltOS_MAXSPD, (void*)-1, 0);
  mbar->add("Ownship/MaxSpd++", FL_ALT+'s', (Fl_Callback*)CPA_GUI::cb_AltOS_MAXSPD, (void*)10, 0);
  mbar->add("Ownship/MaxSpd--", FL_ALT+'x', (Fl_Callback*)CPA_GUI::cb_AltOS_MAXSPD, (void*)-10, FL_MENU_DIVIDER);
  mbar->add("Ownship/MaxTOL+",         'f', (Fl_Callback*)CPA_GUI::cb_AltOS_MAXTOL, (void*)10, 0);
  mbar->add("Ownship/MaxTOL-",         'v', (Fl_Callback*)CPA_GUI::cb_AltOS_MAXTOL, (void*)-10, 0);
  mbar->add("Ownship/MaxTOL++", FL_ALT+'f', (Fl_Callback*)CPA_GUI::cb_AltOS_MAXTOL, (void*)100, 0);
  mbar->add("Ownship/MaxTOL--", FL_ALT+'v', (Fl_Callback*)CPA_GUI::cb_AltOS_MAXTOL, (void*)-100, 0);
  
  mbar->add("Contact/Reset", 0, (Fl_Callback*)CPA_GUI::cb_ResetCN, (void*)0, 0);
  mbar->add("Contact/Index-0", FL_CTRL+'0', (Fl_Callback*)CPA_GUI::cb_CN_INDEX, (void*)0, 0);
  mbar->add("Contact/Index-1", FL_CTRL+'1', (Fl_Callback*)CPA_GUI::cb_CN_INDEX, (void*)1, 0);
  mbar->add("Contact/Index-2", FL_CTRL+'3', (Fl_Callback*)CPA_GUI::cb_CN_INDEX, (void*)2, FL_MENU_DIVIDER);
  mbar->add("Contact/Move N", 'A', (Fl_Callback*)CPA_GUI::cb_AltCNY, (void*)5, 0);
  mbar->add("Contact/Move S", 'Z', (Fl_Callback*)CPA_GUI::cb_AltCNY, (void*)-5, 0);
  mbar->add("Contact/Move E", 'M', (Fl_Callback*)CPA_GUI::cb_AltCNX, (void*)5, 0);
  mbar->add("Contact/Move W", 'N', (Fl_Callback*)CPA_GUI::cb_AltCNX, (void*)-5, FL_MENU_DIVIDER);
  mbar->add("Contact/Heading -", 'n', (Fl_Callback*)CPA_GUI::cb_AltCNCRS, (void*)-5, 0);
  mbar->add("Contact/Heading +", 'm', (Fl_Callback*)CPA_GUI::cb_AltCNCRS, (void*)5, 0);
  mbar->add("Contact/Speed   -", 'a', (Fl_Callback*)CPA_GUI::cb_AltCNSPD, (void*)2, 0);
  mbar->add("Contact/Speed   +", 'z', (Fl_Callback*)CPA_GUI::cb_AltCNSPD, (void*)-2, 0);

  mbar->add("Pan-Zoom/Rotate X- ", FL_CTRL+FL_SHIFT+FL_Down,  (Fl_Callback*)CPA_GUI::cb_RotateX, (void*)-1, 0);
  mbar->add("Pan-Zoom/Rotate X+ ", FL_CTRL+FL_SHIFT+FL_Up,  (Fl_Callback*)CPA_GUI::cb_RotateX, (void*)1, 0);
  mbar->add("Pan-Zoom/Rotate Z- ", FL_CTRL+FL_SHIFT+FL_Left,  (Fl_Callback*)CPA_GUI::cb_RotateZ, (void*)-1, 0);
  mbar->add("Pan-Zoom/Rotate Z+ ", FL_CTRL+FL_SHIFT+FL_Right,  (Fl_Callback*)CPA_GUI::cb_RotateZ, (void*)1, FL_MENU_DIVIDER);
  mbar->add("Pan-Zoom/Reset 1",         '1',  (Fl_Callback*)CPA_GUI::cb_Reset, (void*)1, 0);
  mbar->add("Pan-Zoom/Reset 2",         '2',  (Fl_Callback*)CPA_GUI::cb_Reset, (void*)2, 0);
  mbar->add("Pan-Zoom/Reset 3",         '3',  (Fl_Callback*)CPA_GUI::cb_Reset, (void*)3, FL_MENU_DIVIDER);
  mbar->add("Pan-Zoom/Toggle Frame ",   'F',  (Fl_Callback*)CPA_GUI::cb_ToggleFrame, (void*)-1, FL_MENU_DIVIDER);
  mbar->add("Pan-Zoom/Expand Radius ",  '}',  (Fl_Callback*)CPA_GUI::cb_StretchRad, (void*)1, 0);
  mbar->add("Pan-Zoom/Shrink Radius ",  '{',  (Fl_Callback*)CPA_GUI::cb_StretchRad, (void*)-1, FL_MENU_DIVIDER);
  mbar->add("Pan-Zoom/Base Higher ",    'B',  (Fl_Callback*)CPA_GUI::cb_ModBase, (void*)1, 0);
  mbar->add("Pan-Zoom/Base Lower ",     'b',  (Fl_Callback*)CPA_GUI::cb_ModBase, (void*)-1, FL_MENU_DIVIDER);
  mbar->add("Pan-Zoom/Scale Higher ",   'S',  (Fl_Callback*)CPA_GUI::cb_ModScale, (void*)1, 0);
  mbar->add("Pan-Zoom/Scale Lower ",    's',  (Fl_Callback*)CPA_GUI::cb_ModScale, (void*)-1, FL_MENU_DIVIDER);
  mbar->add("Pan-Zoom/AdjustPrecision 1",'*',  (Fl_Callback*)CPA_GUI::cb_AdjustPrecisionIPF, (void*)0, 0);
  mbar->add("Pan-Zoom/AdjustPrecision +",FL_CTRL+'>',  (Fl_Callback*)CPA_GUI::cb_AdjustPrecisionIPF, (void*)-1, 0);
  mbar->add("Pan-Zoom/AdjustPrecision -",FL_CTRL+'<',  (Fl_Callback*)CPA_GUI::cb_AdjustPrecisionIPF, (void*)1, 0);

  mbar->add("TypeIPF/CPA", 0, (Fl_Callback*)CPA_GUI::cb_SetTypeIPF, (void*)0, 0);
  mbar->add("TypeIPF/Waypoint", 0, (Fl_Callback*)CPA_GUI::cb_SetTypeIPF, (void*)1, 0);
  mbar->add("TypeIPF/Avoid", 0, (Fl_Callback*)CPA_GUI::cb_SetTypeIPF, (void*)2, 0);
  mbar->add("TypeIPF/ROC", 0, (Fl_Callback*)CPA_GUI::cb_SetTypeIPF, (void*)3, 0);
  mbar->add("TypeIPF/R16", 0, (Fl_Callback*)CPA_GUI::cb_SetTypeIPF, (void*)4, 0);
}

//----------------------------------------------------------
// Procedure: handle
//     Notes: We want the various "Output" widgets to ignore keyboard
//            events (as they should, right?!), so we wrote a MY_Output
//            subclass to do just that. However the keyboard arrow keys
//            still seem to be grabbed by Fl_Window to change focuse
//            between sub-widgets. We over-ride that here to do the 
//            panning on the image by invoking the pan callbacks. By
//            then returning (1), we've indicated that the event has
//            been handled.

int CPA_GUI::handle(int event) 
{
  switch(event) {
  case FL_PUSH:
    Fl_Window::handle(event);
    ipf_viewer->resetIPF(contact_index, ipf_resolution);
    ipf_viewer->redraw();
    updateXY();
    return(1);
    break;
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------- ResetOS
inline void CPA_GUI::cb_ResetOS_i() {
  cpa_viewer->resetPan();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(contact_index, ipf_resolution);
  ipf_viewer->redraw();
}
void CPA_GUI::cb_ResetOS(Fl_Widget* o) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_ResetOS_i();
}

//----------------------------------------- ResetCN
inline void CPA_GUI::cb_ResetCN_i() {
  cpa_model->shift_cnx((float)(0));
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(contact_index, ipf_resolution);
  ipf_viewer->redraw();
}
void CPA_GUI::cb_ResetCN(Fl_Widget* o) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_ResetCN_i();
}

//----------------------------------------- AltOS_MAXSPD
inline void CPA_GUI::cb_AltOS_MAXSPD_i() {
  cpa_model->set_os_maxspd(os_maxspd_ctr->value());
  cout << "os_maxspd: " << os_maxspd_ctr->value() << endl;
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(contact_index, ipf_resolution);
  ipf_viewer->redraw();
}
void CPA_GUI::cb_AltOS_MAXSPD(Fl_Widget* o) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_AltOS_MAXSPD_i();
}

//----------------------------------------- AltOS_MAXTOL
inline void CPA_GUI::cb_AltOS_MAXTOL_i() {
  cpa_model->set_os_maxtol(os_maxtol_ctr->value());
  updateXY();  
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(contact_index, ipf_resolution);
  ipf_viewer->redraw();
}
void CPA_GUI::cb_AltOS_MAXTOL(Fl_Widget* o) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_AltOS_MAXTOL_i();
}

//----------------------------------------- AltCNX
inline void CPA_GUI::cb_AltCNX_i(int amt) {
  if(amt == 0)
    cpa_model->set_cnx(cn_xpos_ctr->value());
  else
    cpa_model->shift_cnx((float)(amt), contact_index);
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(contact_index, ipf_resolution);
  ipf_viewer->redraw();
}
void CPA_GUI::cb_AltCNX(Fl_Widget* o, int v) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_AltCNX_i(v);
}

//----------------------------------------- AltCNY
inline void CPA_GUI::cb_AltCNY_i(int amt) {
  if(amt == 0)
    cpa_model->set_cny(cn_ypos_ctr->value());
  else
    cpa_model->shift_cny((float)(amt), contact_index);
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(contact_index, ipf_resolution);
  ipf_viewer->redraw();
}
void CPA_GUI::cb_AltCNY(Fl_Widget* o, int v) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_AltCNY_i(v);
}

//----------------------------------------- AltCNSPD
inline void CPA_GUI::cb_AltCNSPD_i(int amt) {
  if(amt == 0)
    cpa_model->set_cnspd(cn_spd_ctr->value());
  else
    cpa_model->shift_cnspd((float)(amt), contact_index);
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(contact_index, ipf_resolution);
  ipf_viewer->redraw();
}
void CPA_GUI::cb_AltCNSPD(Fl_Widget* o, int v) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_AltCNSPD_i(v);
}

//----------------------------------------- AltCNCRS
inline void CPA_GUI::cb_AltCNCRS_i(int amt) {
  if(amt == 0) { 
    double dval = angle360(cn_hdg_ctr->value());
    cn_hdg_ctr->value(dval);
    cpa_model->set_cncrs(cn_hdg_ctr->value());
  }
  else
    cpa_model->shift_cncrs((float)(amt), contact_index);
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(contact_index, ipf_resolution);
  ipf_viewer->redraw();
}
void CPA_GUI::cb_AltCNCRS(Fl_Widget* o, int v) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_AltCNCRS_i(v);
}

//----------------------------------------- AltCNBNG
inline void CPA_GUI::cb_AltCNBNG_i() {
  double dval = angle360(cn_bng_ctr->value());
  cn_bng_ctr->value(dval);
  cpa_model->set_cnbng(cn_bng_ctr->value());
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(contact_index, ipf_resolution);
  ipf_viewer->redraw();
}
void CPA_GUI::cb_AltCNBNG(Fl_Widget* o) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_AltCNBNG_i();
}

//----------------------------------------- AltCNRNG
inline void CPA_GUI::cb_AltCNRNG_i() {
  cpa_model->set_cnrng(cn_rng_ctr->value());
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(contact_index, ipf_resolution);
  ipf_viewer->redraw();
}
void CPA_GUI::cb_AltCNRNG(Fl_Widget* o) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_AltCNRNG_i();
}

//----------------------------------------- ROT_X
inline void CPA_GUI::cb_RotateX_i(int amt) {
  ipf_viewer->setParam("mod_x_rotation", (double)(amt));
  updateXY();
  ipf_viewer->redraw();
}
void CPA_GUI::cb_RotateX(Fl_Widget* o, int v) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_RotateX_i(v);
}

//----------------------------------------- ROT_Z
inline void CPA_GUI::cb_RotateZ_i(int amt) {
  ipf_viewer->setParam("mod_z_rotation", (double)(amt));
  updateXY();
  ipf_viewer->redraw();
}
void CPA_GUI::cb_RotateZ(Fl_Widget* o, int v) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_RotateZ_i(v);
}

//----------------------------------------- Reset
inline void CPA_GUI::cb_Reset_i(int val) {
  if(val==1)  ipf_viewer->setParam("reset_view", "1");
  if(val==2)  ipf_viewer->setParam("reset_view", "2");
  if(val==3)  ipf_viewer->setParam("reset_view", "3");
}
void CPA_GUI::cb_Reset(Fl_Widget* o, int v) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_Reset_i(v);
}

//----------------------------------------- Stretch Radius
inline void CPA_GUI::cb_StretchRad_i(int amt) {
  if(amt > 0) ipf_viewer->setParam("mod_radius", 1.25);
  if(amt < 0) ipf_viewer->setParam("mod_radius", 0.80);
}
void CPA_GUI::cb_StretchRad(Fl_Widget* o, int v) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_StretchRad_i(v);
}

//----------------------------------------- Mod Base
inline void CPA_GUI::cb_ModBase_i(int amt) {
  if(amt > 0) ipf_viewer->setParam("mod_base", 10);
  if(amt < 0) ipf_viewer->setParam("mod_base",-10);
}
void CPA_GUI::cb_ModBase(Fl_Widget* o, int v) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_ModBase_i(v);
}

//----------------------------------------- Mod Scale
inline void CPA_GUI::cb_ModScale_i(int amt) {
  if(amt > 0) ipf_viewer->setParam("mod_scale", 1.25);
  if(amt < 0) ipf_viewer->setParam("mod_scale", 0.80);
}
void CPA_GUI::cb_ModScale(Fl_Widget* o, int v) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_ModScale_i(v);
}

//----------------------------------------- SetTypeIPF
inline void CPA_GUI::cb_SetTypeIPF_i(int val) {
  if(val == 0) 
    ipf_viewer->setTypeIPF("cpa");
  if(val == 1) 
    ipf_viewer->setTypeIPF("wpt");
  if(val == 2) 
    ipf_viewer->setTypeIPF("avd");
  if(val == 3) 
    ipf_viewer->setTypeIPF("roc");
  if(val == 4) 
    ipf_viewer->setTypeIPF("r16");
}
void CPA_GUI::cb_SetTypeIPF(Fl_Widget* o, int v) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_SetTypeIPF_i(v);
}

//----------------------------------------- Toggle Frame
inline void CPA_GUI::cb_ToggleFrame_i() {
  ipf_viewer->setParam("draw_frame", "toggle");
}
void CPA_GUI::cb_ToggleFrame(Fl_Widget* o) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_ToggleFrame_i();
}

//----------------------------------------- UnitOnePrecision
inline void CPA_GUI::cb_AdjustPrecisionIPF_i(int v) {
  if(v == 0)
    ipf_resolution = 1;
  else if(v == 1) {
    ipf_resolution++;
  }
  else if(v == -1) {
    ipf_resolution--;
    if(ipf_resolution < 1)
      ipf_resolution = 1;
  }
  else
    return;
  ipf_viewer->resetIPF(contact_index, ipf_resolution);
  ipf_viewer->redraw();
}
void CPA_GUI::cb_AdjustPrecisionIPF(Fl_Widget* o, int v) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_AdjustPrecisionIPF_i(v);
}

//----------------------------------------- Contact-Index
inline void CPA_GUI::cb_CN_INDEX_i(int index) {
  contact_index = index;
  updateXY();
  ipf_viewer->resetIPF(contact_index, ipf_resolution);
  ipf_viewer->redraw();

}
void CPA_GUI::cb_CN_INDEX(Fl_Widget* o, int ix) {
  ((CPA_GUI*)(o->parent()->user_data()))->cb_CN_INDEX_i(ix);
}



//----------------------------------------- UpdateXY
void CPA_GUI::updateXY() 
{
  double dval;
  string str;
  int    ix = contact_index;

  dval = cpa_model->get_param("cnx", ix);
  str = doubleToString(dval, 1);
  cn_xpos_ctr->value(dval);

  dval = cpa_model->get_param("cny", ix);
  str = doubleToString(cpa_model->get_param("cny", ix), 1);
  cn_ypos_ctr->value(dval);

  dval = cpa_model->get_param("cn_range", ix);
  cn_rng_ctr->value(dval);

  dval = cpa_model->get_param("cn_bearing", ix);
  cn_bng_ctr->value(dval);

  
  dval = cpa_model->get_param("cncrs", ix);
  str  = doubleToString(dval, 1);
  cn_hdg_ctr->value(dval);

  dval = cpa_model->get_param("cnspd", ix);
  str = doubleToString(dval, 1);
  cn_spd_ctr->value(dval);

  dval = cpa_model->get_param("os_maxtol");
  str = doubleToString(dval,1);
  p_os_maxtol->value(str.c_str());
  os_maxtol_ctr->value(dval);

  dval = cpa_model->get_param("os_maxspd");
  str = doubleToString(dval,1);
  p_os_maxspd->value(str.c_str());
  os_maxspd_ctr->value(dval);
}
