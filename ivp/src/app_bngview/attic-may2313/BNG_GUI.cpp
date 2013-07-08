/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BNG_GUI.cpp                                          */
/*    DATE: May 21, 2013                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "BNG_GUI.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

BNG_GUI::BNG_GUI(int g_w, int g_h, const char *g_l)
  : MarineGUI(g_w, g_h, g_l) 
{
  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  double osl = 60;
  double cnl = 200;
  int    info_size  = 11;
  int    label_size = 11;

  p_os_x = new MY_Output(osl, h()-27, 60, 20, "osx:");
  p_os_x->textsize(info_size);
  p_os_x->labelsize(label_size);

  p_os_y = new MY_Output(osl, h()-52, 60, 20, "osy:");
  p_os_y->textsize(info_size);
  p_os_y->labelsize(label_size);

  p_os_h = new MY_Output(osl, h()-77, 60, 20, "osh:");
  p_os_h->textsize(info_size);
  p_os_h->labelsize(label_size);

  p_os_v = new MY_Output(osl, h()-102, 60, 20, "osv:");
  p_os_v->textsize(info_size);
  p_os_v->labelsize(label_size);


  p_cn_x = new MY_Output(cnl, h()-25, 60, 20, "cnx:");
  p_cn_x->textsize(info_size);
  p_cn_x->labelsize(label_size);

  p_cn_y = new MY_Output(cnl, h()-50, 60, 20, "cny:");
  p_cn_y->textsize(info_size);
  p_cn_y->labelsize(label_size);

  p_cn_h = new MY_Output(cnl, h()-75, 60, 20, "cnh:");
  p_cn_h->textsize(info_size);
  p_cn_h->labelsize(label_size);

  p_cn_v = new MY_Output(cnl, h()-100, 60, 20, "cnv:");
  p_cn_v->textsize(info_size);
  p_cn_v->labelsize(label_size);


  cn_rng_ctr = new Fl_Counter(cnl+150, h()-95, 120, 20, "contact range");
  cn_rng_ctr->range(0, 1000);
  cn_rng_ctr->precision(0);
  cn_rng_ctr->step(1);
  cn_rng_ctr->lstep(10);
  cn_rng_ctr->callback((Fl_Callback*)BNG_GUI::cb_AltCNRNG, (void *)0);

  cn_bng_ctr = new Fl_Counter(cnl+150, h()-45, 120, 20, "contact bearing");
  cn_bng_ctr->range(-360, 360);
  cn_bng_ctr->precision(0);
  cn_bng_ctr->step(1);
  cn_bng_ctr->lstep(10);
  cn_bng_ctr->callback((Fl_Callback*)BNG_GUI::cb_AltCNBNG, (void *)0);

  cpa_model      = new BNG_Model;
  ipf_resolution = 10;
  
  cpa_viewer = new BNGViewer(cpa_model, 0, 30, w()/2, h()-140);
  m_mviewer  = cpa_viewer;

  ipf_viewer = new IPFViewer(cpa_model, (w()/2)+5, 30, (w()/2)-5, h()-140);

  augmentMenu();

  this->end();
  this->resizable(this);
  this->show();

  ipf_viewer->resetIPF(0, ipf_resolution);
  ipf_viewer->redraw();
}

//----------------------------------------------------------------
// Procedure: augmentMenu

void BNG_GUI::augmentMenu() 
{
  m_menubar->add("Pair/Set",   0, (Fl_Callback*)BNG_GUI::cb_SetPair,   (void*)0, 0);
  m_menubar->add("Pair/Reset", 0, (Fl_Callback*)BNG_GUI::cb_ResetPair, (void*)0, 0);

  m_menubar->add("Ownship/Reset", FL_ALT+'r', (Fl_Callback*)BNG_GUI::cb_ResetOS, (void*)0, 0);
  
  m_menubar->add("Contact/Reset", 0, (Fl_Callback*)BNG_GUI::cb_ResetCN, (void*)0, 0);

  m_menubar->add("Contact/Move N", 'A', (Fl_Callback*)BNG_GUI::cb_AltCNY, (void*)5, 0);
  m_menubar->add("Contact/Move S", 'Z', (Fl_Callback*)BNG_GUI::cb_AltCNY, (void*)-5, 0);
  m_menubar->add("Contact/Move E", 'M', (Fl_Callback*)BNG_GUI::cb_AltCNX, (void*)5, 0);
  m_menubar->add("Contact/Move W", 'N', (Fl_Callback*)BNG_GUI::cb_AltCNX, (void*)-5, FL_MENU_DIVIDER);
  m_menubar->add("Contact/Heading -", 'n', (Fl_Callback*)BNG_GUI::cb_AltCNCRS, (void*)-5, 0);
  m_menubar->add("Contact/Heading +", 'm', (Fl_Callback*)BNG_GUI::cb_AltCNCRS, (void*)5, 0);
  m_menubar->add("Contact/Speed   -", 'a', (Fl_Callback*)BNG_GUI::cb_AltCNSPD, (void*)2, 0);
  m_menubar->add("Contact/Speed   +", 'z', (Fl_Callback*)BNG_GUI::cb_AltCNSPD, (void*)-2, 0);

  m_menubar->add("Pan-Zoom/Rotate X- ", FL_CTRL+FL_SHIFT+FL_Down,  (Fl_Callback*)BNG_GUI::cb_RotateX, (void*)-1, 0);
  m_menubar->add("Pan-Zoom/Rotate X+ ", FL_CTRL+FL_SHIFT+FL_Up,  (Fl_Callback*)BNG_GUI::cb_RotateX, (void*)1, 0);
  m_menubar->add("Pan-Zoom/Rotate Z- ", FL_CTRL+FL_SHIFT+FL_Left,  (Fl_Callback*)BNG_GUI::cb_RotateZ, (void*)-1, 0);
  m_menubar->add("Pan-Zoom/Rotate Z+ ", FL_CTRL+FL_SHIFT+FL_Right,  (Fl_Callback*)BNG_GUI::cb_RotateZ, (void*)1, FL_MENU_DIVIDER);
  m_menubar->add("Pan-Zoom/Reset 1",         '1',  (Fl_Callback*)BNG_GUI::cb_Reset, (void*)1, 0);
  m_menubar->add("Pan-Zoom/Reset 2",         '2',  (Fl_Callback*)BNG_GUI::cb_Reset, (void*)2, 0);
  m_menubar->add("Pan-Zoom/Reset 3",         '3',  (Fl_Callback*)BNG_GUI::cb_Reset, (void*)3, FL_MENU_DIVIDER);
  m_menubar->add("Pan-Zoom/Toggle Frame ",   'F',  (Fl_Callback*)BNG_GUI::cb_ToggleFrame, (void*)-1, FL_MENU_DIVIDER);
  m_menubar->add("Pan-Zoom/Expand Radius ",  '}',  (Fl_Callback*)BNG_GUI::cb_StretchRad, (void*)1, 0);
  m_menubar->add("Pan-Zoom/Shrink Radius ",  '{',  (Fl_Callback*)BNG_GUI::cb_StretchRad, (void*)-1, FL_MENU_DIVIDER);
  m_menubar->add("Pan-Zoom/Base Higher ",    'B',  (Fl_Callback*)BNG_GUI::cb_ModBase, (void*)1, 0);
  m_menubar->add("Pan-Zoom/Base Lower ",     'b',  (Fl_Callback*)BNG_GUI::cb_ModBase, (void*)-1, FL_MENU_DIVIDER);
  m_menubar->add("Pan-Zoom/Scale Higher ",   'S',  (Fl_Callback*)BNG_GUI::cb_ModScale, (void*)1, 0);
  m_menubar->add("Pan-Zoom/Scale Lower ",    's',  (Fl_Callback*)BNG_GUI::cb_ModScale, (void*)-1, FL_MENU_DIVIDER);
  m_menubar->add("Pan-Zoom/AdjustPrecision 1",'*',  (Fl_Callback*)BNG_GUI::cb_AdjustPrecisionIPF, (void*)0, 0);
  m_menubar->add("Pan-Zoom/AdjustPrecision +",FL_CTRL+'>',  (Fl_Callback*)BNG_GUI::cb_AdjustPrecisionIPF, (void*)-1, 0);
  m_menubar->add("Pan-Zoom/AdjustPrecision -",FL_CTRL+'<',  (Fl_Callback*)BNG_GUI::cb_AdjustPrecisionIPF, (void*)1, 0);

  m_menubar->add("TypeIPF/BNG", 0, (Fl_Callback*)BNG_GUI::cb_SetTypeIPF, (void*)0, 0);
  m_menubar->add("TypeIPF/Waypoint", 0, (Fl_Callback*)BNG_GUI::cb_SetTypeIPF, (void*)1, 0);
  m_menubar->add("TypeIPF/Avoid", 0, (Fl_Callback*)BNG_GUI::cb_SetTypeIPF, (void*)2, 0);
  m_menubar->add("TypeIPF/ROC", 0, (Fl_Callback*)BNG_GUI::cb_SetTypeIPF, (void*)3, 0);
  m_menubar->add("TypeIPF/R16", 0, (Fl_Callback*)BNG_GUI::cb_SetTypeIPF, (void*)4, 0);
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

int BNG_GUI::handle(int event) 
{
  switch(event) {
  case FL_PUSH:
    Fl_Window::handle(event);
    ipf_viewer->resetIPF(0, ipf_resolution);
    ipf_viewer->redraw();
    updateXY();
    return(1);
    break;
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------- SetPair
inline void BNG_GUI::cb_SetPair_i() {
  m_vehi_pair_set = m_vehi_pair;
  cpa_viewer->redraw();
  ipf_viewer->redraw();
}
void BNG_GUI::cb_SetPair(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_SetPair_i();
}

//----------------------------------------- ResetPair
inline void BNG_GUI::cb_ResetPair_i() {
  m_vehi_pair = m_vehi_pair_set;
  cpa_viewer->redraw();
  ipf_viewer->redraw();
}
void BNG_GUI::cb_ResetPair(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_ResetPair_i();
}

//----------------------------------------- ResetOS
inline void BNG_GUI::cb_ResetOS_i() {
  cpa_viewer->resetPan();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(0, ipf_resolution);
  ipf_viewer->redraw();
}
void BNG_GUI::cb_ResetOS(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_ResetOS_i();
}

//----------------------------------------- ResetCN
inline void BNG_GUI::cb_ResetCN_i() {
  //  m_vehi_model->shift_cnx()
  cpa_model->shift_cnx((float)(0));
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(0, ipf_resolution);
  ipf_viewer->redraw();
}
void BNG_GUI::cb_ResetCN(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_ResetCN_i();
}

//----------------------------------------- AltCNX
inline void BNG_GUI::cb_AltCNX_i(int amt) {
#if 0
  if(amt == 0)
    cpa_model->set_cnx(cn_xpos_ctr->value());
  else
    cpa_model->shift_cnx((float)(amt), 0);
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(, ipf_resolution);
  ipf_viewer->redraw();
#endif
}
void BNG_GUI::cb_AltCNX(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltCNX_i(v);
}

//----------------------------------------- AltCNY
inline void BNG_GUI::cb_AltCNY_i(int amt) {
#if 0
  if(amt == 0)
    cpa_model->set_cny(cn_ypos_ctr->value());
  else
    cpa_model->shift_cny((float)(amt), );
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(0, ipf_resolution);
  ipf_viewer->redraw();
#endif
}
void BNG_GUI::cb_AltCNY(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltCNY_i(v);
}

//----------------------------------------- AltCNSPD
inline void BNG_GUI::cb_AltCNSPD_i(int amt) {
  if(amt == 0)
    cpa_model->set_cnspd(cn_spd_ctr->value());
  else
    cpa_model->shift_cnspd((float)(amt), 0);
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(0, ipf_resolution);
  ipf_viewer->redraw();
}
void BNG_GUI::cb_AltCNSPD(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltCNSPD_i(v);
}

//----------------------------------------- AltCNCRS
inline void BNG_GUI::cb_AltCNCRS_i(int amt) {
  if(amt == 0) { 
    double dval = angle360(cn_hdg_ctr->value());
    cn_hdg_ctr->value(dval);
    cpa_model->set_cncrs(cn_hdg_ctr->value());
  }
  else
    cpa_model->shift_cncrs((float)(amt), 0);
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(0, ipf_resolution);
  ipf_viewer->redraw();
}
void BNG_GUI::cb_AltCNCRS(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltCNCRS_i(v);
}

//----------------------------------------- AltCNBNG
inline void BNG_GUI::cb_AltCNBNG_i() {
  double dval = angle360(cn_bng_ctr->value());
  cn_bng_ctr->value(dval);
  cpa_model->set_cnbng(cn_bng_ctr->value());
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(0, ipf_resolution);
  ipf_viewer->redraw();
}
void BNG_GUI::cb_AltCNBNG(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltCNBNG_i();
}

//----------------------------------------- AltCNRNG
inline void BNG_GUI::cb_AltCNRNG_i() {
  cpa_model->set_cnrng(cn_rng_ctr->value());
  updateXY();
  cpa_viewer->redraw();
  ipf_viewer->resetIPF(0, ipf_resolution);
  ipf_viewer->redraw();
}
void BNG_GUI::cb_AltCNRNG(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltCNRNG_i();
}

//----------------------------------------- ROT_X
inline void BNG_GUI::cb_RotateX_i(int amt) {
  ipf_viewer->setParam("mod_x_rotation", (double)(amt));
  updateXY();
  ipf_viewer->redraw();
}
void BNG_GUI::cb_RotateX(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_RotateX_i(v);
}

//----------------------------------------- ROT_Z
inline void BNG_GUI::cb_RotateZ_i(int amt) {
  ipf_viewer->setParam("mod_z_rotation", (double)(amt));
  updateXY();
  ipf_viewer->redraw();
}
void BNG_GUI::cb_RotateZ(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_RotateZ_i(v);
}

//----------------------------------------- Reset
inline void BNG_GUI::cb_Reset_i(int val) {
  if(val==1)  ipf_viewer->setParam("reset_view", "1");
  if(val==2)  ipf_viewer->setParam("reset_view", "2");
  if(val==3)  ipf_viewer->setParam("reset_view", "3");
}
void BNG_GUI::cb_Reset(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_Reset_i(v);
}

//----------------------------------------- Stretch Radius
inline void BNG_GUI::cb_StretchRad_i(int amt) {
  if(amt > 0) ipf_viewer->setParam("mod_radius", 1.25);
  if(amt < 0) ipf_viewer->setParam("mod_radius", 0.80);
}
void BNG_GUI::cb_StretchRad(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_StretchRad_i(v);
}

//----------------------------------------- Mod Base
inline void BNG_GUI::cb_ModBase_i(int amt) {
  if(amt > 0) ipf_viewer->setParam("mod_base", 10);
  if(amt < 0) ipf_viewer->setParam("mod_base",-10);
}
void BNG_GUI::cb_ModBase(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_ModBase_i(v);
}

//----------------------------------------- Mod Scale
inline void BNG_GUI::cb_ModScale_i(int amt) {
  if(amt > 0) ipf_viewer->setParam("mod_scale", 1.25);
  if(amt < 0) ipf_viewer->setParam("mod_scale", 0.80);
}
void BNG_GUI::cb_ModScale(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_ModScale_i(v);
}

//----------------------------------------- SetTypeIPF
inline void BNG_GUI::cb_SetTypeIPF_i(int val) {
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
void BNG_GUI::cb_SetTypeIPF(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_SetTypeIPF_i(v);
}

//----------------------------------------- Toggle Frame
inline void BNG_GUI::cb_ToggleFrame_i() {
  ipf_viewer->setParam("draw_frame", "toggle");
}
void BNG_GUI::cb_ToggleFrame(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_ToggleFrame_i();
}

//----------------------------------------- UnitOnePrecision
inline void BNG_GUI::cb_AdjustPrecisionIPF_i(int v) {
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
  ipf_viewer->resetIPF(0, ipf_resolution);
  ipf_viewer->redraw();
}
void BNG_GUI::cb_AdjustPrecisionIPF(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AdjustPrecisionIPF_i(v);
}


//----------------------------------------- UpdateXY
void BNG_GUI::updateXY() 
{
  double dval;
  string str;
  int    ix = 0;

  dval = cpa_model->get_param("cnx", ix);
  str = doubleToString(dval, 1);
  p_cn_x->value(str.c_str());

  dval = cpa_model->get_param("cny", ix);
  str = doubleToString(dval, 1);
  p_cn_y->value(str.c_str());

  dval = cpa_model->get_param("cncrs", ix);
  str  = doubleToString(dval, 1);
  p_cn_h->value(str.c_str());

  dval = cpa_model->get_param("cnspd", ix);
  str = doubleToString(dval, 1);
  p_cn_v->value(str.c_str());

  dval = cpa_model->get_param("cn_range", ix);
  cn_rng_ctr->value(dval);

  dval = cpa_model->get_param("cn_bearing", ix);
  cn_bng_ctr->value(dval);  
}


