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
  this->when(FL_WHEN_CHANGED);  this->begin();

  int col1 = 40;
  int col2 = 140;
  int col3 = 310;
  int col4 = 480;
  int col5 = 650;
  int col6 = 800;
  int col7 = 1000;
  int col8 = 1200;
  int col9 = 1350;

  int row0 = h() - 130;
  int row1 = h() - 105;
  int row2 = h() - 80;
  int row3 = h() - 55;
  int row4 = h() - 30;

  int info_size  = 11;
  int label_size = 11;

  // Column ONE Ownship Fields
  p_os_x = new MY_Output(col1, row0, 50, 20, "osx:");
  p_os_x->textsize(info_size);
  p_os_x->labelsize(label_size);

  p_os_y = new MY_Output(col1, row1, 50, 20, "osy:");
  p_os_y->textsize(info_size);
  p_os_y->labelsize(label_size);

  p_os_h = new MY_Output(col1, row2, 50, 20, "osh:");
  p_os_h->textsize(info_size);
  p_os_h->labelsize(label_size);

  p_os_v = new MY_Output(col1, row3, 50, 20, "osv:");
  p_os_v->textsize(info_size);
  p_os_v->labelsize(label_size);

  // Column TWO Contact Fields
  p_cn_x = new MY_Output(col2, row0, 50, 20, "cnx:");
  p_cn_x->textsize(info_size);
  p_cn_x->labelsize(label_size);

  p_cn_y = new MY_Output(col2, row1, 50, 20, "cny:");
  p_cn_y->textsize(info_size);
  p_cn_y->labelsize(label_size);

  p_cn_h = new MY_Output(col2, row2, 50, 20, "cnh:");
  p_cn_h->textsize(info_size);
  p_cn_h->labelsize(label_size);

  p_cn_v = new MY_Output(col2, row3, 50, 20, "cnv:");
  p_cn_v->textsize(info_size);
  p_cn_v->labelsize(label_size);

  // Column THREE Ownship-Contact Fields
  p_os_cn_bng = new MY_Output(col3, row0, 60, 20, "os_cn_bng:");
  p_os_cn_bng->textsize(info_size);
  p_os_cn_bng->labelsize(label_size);

  p_os_cn_rel_bng = new MY_Output(col3, row1, 60, 20, "os_cn_rel_bng:");
  p_os_cn_rel_bng->textsize(info_size);
  p_os_cn_rel_bng->labelsize(label_size);

  p_os_cn_abs_rel_bng = new MY_Output(col3, row2, 60, 20, "os_cn_abs_rel_bng:");
  p_os_cn_abs_rel_bng->textsize(info_size);
  p_os_cn_abs_rel_bng->labelsize(label_size);

  // Column  FOUR Contact-Ownship Fields
  p_cn_os_bng = new MY_Output(col4, row0, 60, 20, "cn_os_bng:");
  p_cn_os_bng->textsize(info_size);
  p_cn_os_bng->labelsize(label_size);

  p_cn_os_rel_bng = new MY_Output(col4, row1, 60, 20, "cn_os_rel_bng:");
  p_cn_os_rel_bng->textsize(info_size);
  p_cn_os_rel_bng->labelsize(label_size);

  p_cn_os_abs_rel_bng = new MY_Output(col4, row2, 60, 20, "cn_os_abs_rel_bng:");
  p_cn_os_abs_rel_bng->textsize(info_size);
  p_cn_os_abs_rel_bng->labelsize(label_size);

  // Column FIVE Pair Fiends
  p_range = new MY_Output(col5, row0, 60, 20, "range:");
  p_range->textsize(info_size);
  p_range->labelsize(label_size);

  p_cpa = new MY_Output(col5, row1, 60, 20, "cpa:");
  p_cpa->textsize(info_size);
  p_cpa->labelsize(label_size);

  p_tarb = new MY_Output(col5, row2, 60, 20, "tarb:");
  p_tarb->textsize(info_size);
  p_tarb->labelsize(label_size);

  p_spd_in_cn_hdg = new MY_Output(col5, row3, 60, 20, "spd_in_cn_hdg:");
  p_spd_in_cn_hdg->textsize(info_size);
  p_spd_in_cn_hdg->labelsize(label_size);

  // Column SIX Relative Position fields
  p_fore_of_cn = new MY_Output(col6, row0, 60, 20, "fore_of_cn:");
  p_fore_of_cn->textsize(info_size);
  p_fore_of_cn->labelsize(label_size);

  p_aft_of_cn = new MY_Output(col6, row1, 60, 20, "aft_of_cn:");
  p_aft_of_cn->textsize(info_size);
  p_aft_of_cn->labelsize(label_size);

  p_port_of_cn = new MY_Output(col6, row2, 60, 20, "port_of_cn:");
  p_port_of_cn->textsize(info_size);
  p_port_of_cn->labelsize(label_size);

  p_starboard_of_cn = new MY_Output(col6, row3, 60, 20, "star_of_cn:");
  p_starboard_of_cn->textsize(info_size);
  p_starboard_of_cn->labelsize(label_size);

  // Column SEVEN Passing fields
  p_passes_cn = new MY_Output(col7, row0, 60, 20, "passes_cn:");
  p_passes_cn->textsize(info_size);
  p_passes_cn->labelsize(label_size);

  p_passes_cn_port = new MY_Output(col7, row1, 60, 20, "passes_port:");
  p_passes_cn_port->textsize(info_size);
  p_passes_cn_port->labelsize(label_size);

  p_passes_cn_star = new MY_Output(col7, row2, 60, 20, "passes_star:");
  p_passes_cn_star->textsize(info_size);
  p_passes_cn_star->labelsize(label_size);

  // Column EIGHT Crossing fields
  p_crosses_bow = new MY_Output(col8, row0, 80, 20, "crosses bow:");
  p_crosses_bow->textsize(info_size);
  p_crosses_bow->labelsize(label_size);

  p_crosses_stern = new MY_Output(col8, row1, 80, 20, "crosses stern:");
  p_crosses_stern->textsize(info_size);
  p_crosses_stern->labelsize(label_size);

  p_crosses_either = new MY_Output(col8, row2, 80, 20, "crosses either:");
  p_crosses_either->textsize(info_size);
  p_crosses_either->labelsize(label_size);

  p_crosses_bow_dist = new MY_Output(col8, row3, 80, 20, "crosses bow dist:");
  p_crosses_bow_dist->textsize(info_size);
  p_crosses_bow_dist->labelsize(label_size);

  p_crosses_stern_dist = new MY_Output(col8, row4, 80, 20, "crosses stern dist:");
  p_crosses_stern_dist->textsize(info_size);
  p_crosses_stern_dist->labelsize(label_size);

  //---------------------------------------------------------------------
  // Column NINE Behavior Fields
  p_bhv_debug1 = new MY_Output(col9, row0, 220, 20, "debug1:");
  p_bhv_debug1->textsize(info_size);
  p_bhv_debug1->labelsize(label_size);

  p_bhv_debug2 = new MY_Output(col9, row1, 220, 20, "debug2:");
  p_bhv_debug2->textsize(info_size);
  p_bhv_debug2->labelsize(label_size);

  p_bhv_debug3 = new MY_Output(col9, row2, 220, 20, "debug3:");
  p_bhv_debug3->textsize(info_size);
  p_bhv_debug3->labelsize(label_size);

  p_bhv_debug4 = new MY_Output(col9, row3, 220, 20, "debug4:");
  p_bhv_debug4->textsize(info_size);
  p_bhv_debug4->labelsize(label_size);

  p_bhv_mode = new MY_Output(col9, row4, 220, 20, "mode:");
  p_bhv_mode->textsize(info_size);
  p_bhv_mode->labelsize(label_size);

  //------------------------------------------------------------------------


  m_pair_model   = new PairModel;
  m_ipf_resolution = 10;
  
  m_bng_viewer = new BNGViewer(m_pair_model, 0, 30, w()/2, h()-165);
  m_mviewer  = m_bng_viewer;

  m_ipf_viewer = new IPFViewer(m_pair_model, (w()/2)+5, 30, (w()/2)-5, h()-165);

  augmentMenu();

  setMenuItemColors();

  this->end();
  this->resizable(this);
  this->show();

  //m_ipf_viewer->resetIPF(m_ipf_resolution);
  //m_ipf_viewer->redraw();
  updateXY();
}

//----------------------------------------------------------------
// Procedure: augmentMenu

void BNG_GUI::augmentMenu() 
{
  // First we remove some items at the superclass level so we can use the 
  // hot keys differently.
  removeMenuItem("BackView/Pan Up (slow) ");
  removeMenuItem("BackView/Pan Down (slow) ");
  removeMenuItem("BackView/Pan Left (slow) ");
  removeMenuItem("BackView/Pan Right (slow)");

  removeMenuItem("BackView/Pan Up (v. slow) ");
  removeMenuItem("BackView/Pan Down (v. slow) ");
  removeMenuItem("BackView/Pan Left (v. slow) ");
  removeMenuItem("BackView/Pan Right (v. slow)");

  m_menubar->add("Pair/Set",   0, (Fl_Callback*)BNG_GUI::cb_SetPair,   (void*)0, 0);
  m_menubar->add("Pair/Reset", 0, (Fl_Callback*)BNG_GUI::cb_ResetPair, (void*)0, FL_MENU_DIVIDER);
  m_menubar->add("Pair/toggle_current_range_circle", FL_CTRL+'r', (Fl_Callback*)BNG_GUI::cb_ToggleRangeCircle, (void*)1, 0);
  m_menubar->add("Pair/toggle_trigger_range_circle", 'r', (Fl_Callback*)BNG_GUI::cb_ToggleRangeCircle, (void*)2, FL_MENU_DIVIDER);
  m_menubar->add("Pair/vehicles_shape_scale = larger",  '+', (Fl_Callback*)BNG_GUI::cb_AltShapeScale, (void*)+1, 0);
  m_menubar->add("Pair/vehicles_shape_scale = smaller", '-', (Fl_Callback*)BNG_GUI::cb_AltShapeScale, (void*)-1, 0);
  m_menubar->add("Pair/center_view", 'c', (Fl_Callback*)BNG_GUI::cb_CenterView, (void*)-1, FL_MENU_DIVIDER);
  m_menubar->add("Pair/ownship forward",  FL_ALT+']', (Fl_Callback*)BNG_GUI::cb_LinearMoveShips, (void*)1, 0);
  m_menubar->add("Pair/ownship backward", FL_ALT+'[', (Fl_Callback*)BNG_GUI::cb_LinearMoveShips, (void*)2, 0);
  m_menubar->add("Pair/contact forward",  FL_CTRL+']', (Fl_Callback*)BNG_GUI::cb_LinearMoveShips, (void*)11, 0);
  m_menubar->add("Pair/contact backward", FL_CTRL+'[', (Fl_Callback*)BNG_GUI::cb_LinearMoveShips, (void*)12, 0);
  m_menubar->add("Pair/bothships forward",  ']', (Fl_Callback*)BNG_GUI::cb_LinearMoveShips, (void*)21, 0);
  m_menubar->add("Pair/bothships backward", '[', (Fl_Callback*)BNG_GUI::cb_LinearMoveShips, (void*)22, FL_MENU_DIVIDER);
  m_menubar->add("Pair/ownship range open",  '>', (Fl_Callback*)BNG_GUI::cb_AltOwnshipRange,  (void*)1, 0);
  m_menubar->add("Pair/ownship range close",  '<', (Fl_Callback*)BNG_GUI::cb_AltOwnshipRange, (void*)-1, FL_MENU_DIVIDER);
  m_menubar->add("Pair/inc os bearing", FL_ALT+FL_Left, (Fl_Callback*)BNG_GUI::cb_AltOwnshipBearing,  (void*)1,  0);
  m_menubar->add("Pair/dec os bearing", FL_ALT+FL_Right, (Fl_Callback*)BNG_GUI::cb_AltOwnshipBearing,  (void*)2, 0);
  m_menubar->add("Pair/inc os bearing, hold rb", ')', (Fl_Callback*)BNG_GUI::cb_AltOwnshipBearing,  (void*)11,  0);
  m_menubar->add("Pair/dec os bearing, hold rb", '(', (Fl_Callback*)BNG_GUI::cb_AltOwnshipBearing,  (void*)12, 0);

  // Next Augment with Items for Ownship
  m_menubar->add("Ownship/Reset", FL_ALT+'r', (Fl_Callback*)BNG_GUI::cb_ResetOS, (void*)0, 0);

  m_menubar->add("Ownship/Heading -", 'n', (Fl_Callback*)BNG_GUI::cb_AltOSH, (void*)-1, 0);
  m_menubar->add("Ownship/Heading +", 'm', (Fl_Callback*)BNG_GUI::cb_AltOSH, (void*)1, 0);
  m_menubar->add("Ownship/Heading --", FL_SHIFT+'n', (Fl_Callback*)BNG_GUI::cb_AltOSH, (void*)-5, 0);
  m_menubar->add("Ownship/Heading ++", FL_SHIFT+'m', (Fl_Callback*)BNG_GUI::cb_AltOSH, (void*)5, 0);
  m_menubar->add("Ownship/Speed   -", 'z', (Fl_Callback*)BNG_GUI::cb_AltOSV, (void*)-1, 0);
  m_menubar->add("Ownship/Speed   +", 'a', (Fl_Callback*)BNG_GUI::cb_AltOSV, (void*)1, FL_MENU_DIVIDER);

  m_menubar->add("Ownship/Up",   FL_SHIFT+FL_Up,   (Fl_Callback*)BNG_GUI::cb_AltOSY, (void*)1, 0);
  m_menubar->add("Ownship/Down", FL_SHIFT+FL_Down, (Fl_Callback*)BNG_GUI::cb_AltOSY, (void*)-1, 0);
  m_menubar->add("Ownship/Right",FL_SHIFT+FL_Right,(Fl_Callback*)BNG_GUI::cb_AltOSX, (void*)1, 0);
  m_menubar->add("Ownship/Left", FL_SHIFT+FL_Left, (Fl_Callback*)BNG_GUI::cb_AltOSX, (void*)-1, 0);

  // Next Augment with Items for Contact
  m_menubar->add("Contact/Reset", 0, (Fl_Callback*)BNG_GUI::cb_ResetCN, (void*)0, 0);

  m_menubar->add("Contact/Heading -", FL_CTRL+'n', (Fl_Callback*)BNG_GUI::cb_AltCNH, (void*)-1,  0);
  m_menubar->add("Contact/Heading +", FL_CTRL+'m', (Fl_Callback*)BNG_GUI::cb_AltCNH, (void*)1, 0);
  m_menubar->add("Contact/Speed   -", FL_ALT+'z', (Fl_Callback*)BNG_GUI::cb_AltCNV, (void*)-1,  0);
  m_menubar->add("Contact/Speed   +", FL_CTRL+'a', (Fl_Callback*)BNG_GUI::cb_AltCNV, (void*)1, FL_MENU_DIVIDER);

  m_menubar->add("Contact/Up",   FL_CTRL+FL_Up,   (Fl_Callback*)BNG_GUI::cb_AltCNY, (void*)1, 0);
  m_menubar->add("Contact/Down", FL_CTRL+FL_Down, (Fl_Callback*)BNG_GUI::cb_AltCNY, (void*)-1, 0);
  m_menubar->add("Contact/Right",FL_CTRL+FL_Right,(Fl_Callback*)BNG_GUI::cb_AltCNX, (void*)1, 0);
  m_menubar->add("Contact/Left", FL_CTRL+FL_Left, (Fl_Callback*)BNG_GUI::cb_AltCNX, (void*)-1, 0);

  // Next Handle the IPV Viewer

  m_menubar->add("IPF Pan-Zoom/Rotate X- ", FL_ALT+FL_SHIFT+FL_Down,  (Fl_Callback*)BNG_GUI::cb_RotateX, (void*)-1, 0);
  m_menubar->add("IPF Pan-Zoom/Rotate X+ ", FL_ALT+FL_SHIFT+FL_Up,  (Fl_Callback*)BNG_GUI::cb_RotateX, (void*)1, 0);
  m_menubar->add("IPF Pan-Zoom/Rotate Z- ", FL_ALT+FL_SHIFT+FL_Left,  (Fl_Callback*)BNG_GUI::cb_RotateZ, (void*)-1, 0);
  m_menubar->add("IPF Pan-Zoom/Rotate Z+ ", FL_ALT+FL_SHIFT+FL_Right,  (Fl_Callback*)BNG_GUI::cb_RotateZ, (void*)1, FL_MENU_DIVIDER);
  m_menubar->add("IPF Pan-Zoom/Reset 1",         '1',  (Fl_Callback*)BNG_GUI::cb_Reset, (void*)1, 0);
  m_menubar->add("IPF Pan-Zoom/Reset 2",         '2',  (Fl_Callback*)BNG_GUI::cb_Reset, (void*)2, 0);
  m_menubar->add("IPF Pan-Zoom/Reset 3",         '3',  (Fl_Callback*)BNG_GUI::cb_Reset, (void*)3, FL_MENU_DIVIDER);
  m_menubar->add("IPF Pan-Zoom/Toggle Frame ",   'F',  (Fl_Callback*)BNG_GUI::cb_ToggleFrame, (void*)-1, FL_MENU_DIVIDER);
  m_menubar->add("IPF Pan-Zoom/Expand Radius ",  '}',  (Fl_Callback*)BNG_GUI::cb_StretchRad, (void*)1, 0);
  m_menubar->add("IPF Pan-Zoom/Shrink Radius ",  '{',  (Fl_Callback*)BNG_GUI::cb_StretchRad, (void*)-1, FL_MENU_DIVIDER);
  m_menubar->add("IPF Pan-Zoom/Base Higher ",    'B',  (Fl_Callback*)BNG_GUI::cb_ModBase, (void*)1, 0);
  m_menubar->add("IPF Pan-Zoom/Base Lower ",     'b',  (Fl_Callback*)BNG_GUI::cb_ModBase, (void*)-1, FL_MENU_DIVIDER);
  m_menubar->add("IPF Pan-Zoom/Scale Higher ",   'S',  (Fl_Callback*)BNG_GUI::cb_ModScale, (void*)1, 0);
  m_menubar->add("IPF Pan-Zoom/Scale Lower ",    's',  (Fl_Callback*)BNG_GUI::cb_ModScale, (void*)-1, FL_MENU_DIVIDER);
  m_menubar->add("IPF Pan-Zoom/AdjustPrecision 1",'*',  (Fl_Callback*)BNG_GUI::cb_AdjustPrecisionIPF, (void*)0, 0);
  m_menubar->add("IPF Pan-Zoom/AdjustPrecision +",FL_CTRL+'>',  (Fl_Callback*)BNG_GUI::cb_AdjustPrecisionIPF, (void*)-1, 0);
  m_menubar->add("IPF Pan-Zoom/AdjustPrecision -",FL_CTRL+'<',  (Fl_Callback*)BNG_GUI::cb_AdjustPrecisionIPF, (void*)1, 0);

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
    m_ipf_viewer->resetIPF(m_ipf_resolution);
    m_ipf_viewer->redraw();
    updateXY();
    return(1);
    break;
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------- CenterView
inline void BNG_GUI::cb_CenterView_i() {
  m_bng_viewer->setCenterView();
  m_bng_viewer->redraw();
}
void BNG_GUI::cb_CenterView(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_CenterView_i();
}

//----------------------------------------- ToggleRangeCircle
inline void BNG_GUI::cb_ToggleRangeCircle_i(int i) {
  if(i==1)
    m_bng_viewer->toggleCurrentCircle();
  else if(i==2)
    m_bng_viewer->toggleTriggerCircle();
  else
    return;

  m_bng_viewer->redraw();
}
void BNG_GUI::cb_ToggleRangeCircle(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_ToggleRangeCircle_i(v);
}

//----------------------------------------- LinearMoveShips
inline void BNG_GUI::cb_LinearMoveShips_i(int i) {
  if(i==1)
    m_pair_model->move_os(1);
  else if(i==2)
    m_pair_model->move_os(-1);
  else if(i==11)
    m_pair_model->move_cn(1);
  else if(i==12)
    m_pair_model->move_cn(-1);
  else if(i==21) {
    m_pair_model->move_os(1);
    m_pair_model->move_cn(1);
  }
  else if(i==22) {
    m_pair_model->move_os(-1);
    m_pair_model->move_cn(-1);
  }

  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution, false);
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_LinearMoveShips(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_LinearMoveShips_i(v);
}

//----------------------------------------- AltOwnshipRange
inline void BNG_GUI::cb_AltOwnshipRange_i(int i) {
  m_pair_model->alt_os_range((double)(i));
  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_AltOwnshipRange(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltOwnshipRange_i(v);
}

//----------------------------------------- AltOwnshipBearing
inline void BNG_GUI::cb_AltOwnshipBearing_i(int i) {
  if(i == 1)
    m_pair_model->alt_os_bearing(1);
  else if(i == 2)
    m_pair_model->alt_os_bearing(-1);
  else if(i == 11)
    m_pair_model->alt_os_bearing_holdrb(1);
  else if(i == 12)
    m_pair_model->alt_os_bearing_holdrb(-1);

  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_AltOwnshipBearing(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltOwnshipBearing_i(v);
}

//----------------------------------------- SetPair
inline void BNG_GUI::cb_SetPair_i() {
  //  m_vehi_pair_set = m_vehi_pair;
  m_bng_viewer->redraw();
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_SetPair(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_SetPair_i();
}

//----------------------------------------- ResetPair
inline void BNG_GUI::cb_ResetPair_i() {
  //m_vehi_pair = m_vehi_pair_set;
  m_bng_viewer->redraw();
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_ResetPair(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_ResetPair_i();
}

//----------------------------------------- ResetOS
inline void BNG_GUI::cb_ResetOS_i() {
  m_bng_viewer->resetPan();
  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_ResetOS(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_ResetOS_i();
}

//----------------------------------------- ResetCN
inline void BNG_GUI::cb_ResetCN_i() {
#if 0
  m_pair_model->shift_cnx();
    //cpa_model->shift_cnx((float)(0));
  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
#endif
}
void BNG_GUI::cb_ResetCN(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_ResetCN_i();
}

//----------------------------------------- AltOSX
inline void BNG_GUI::cb_AltOSX_i(int amt) {
  m_pair_model->shift_osx((double)(amt));
  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_AltOSX(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltOSX_i(v);
}

//----------------------------------------- AltOSY
inline void BNG_GUI::cb_AltOSY_i(int amt) {
  m_pair_model->shift_osy((double)(amt));
  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_AltOSY(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltOSY_i(v);
}

//----------------------------------------- AltOSH
inline void BNG_GUI::cb_AltOSH_i(int amt) {
  m_pair_model->shift_osh((double)(amt));
  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_AltOSH(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltOSH_i(v);
}

//----------------------------------------- AltOSV
inline void BNG_GUI::cb_AltOSV_i(int amt) {
  double delta = (double)(amt) * 0.1;
  m_pair_model->shift_osv(delta);
  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_AltOSV(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltOSV_i(v);
}



//----------------------------------------- AltCNX
inline void BNG_GUI::cb_AltCNX_i(int amt) {
  m_pair_model->shift_cnx((double)(amt));
  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_AltCNX(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltCNX_i(v);
}

//----------------------------------------- AltCNY
inline void BNG_GUI::cb_AltCNY_i(int amt) {
  m_pair_model->shift_cny((double)(amt));
  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_AltCNY(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltCNY_i(v);
}

//----------------------------------------- AltCNV
inline void BNG_GUI::cb_AltCNV_i(int amt) {
  double delta = (double)(amt) * 0.1;
  m_pair_model->shift_cnv(delta);
  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_AltCNV(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltCNV_i(v);
}

//----------------------------------------- AltCNH
inline void BNG_GUI::cb_AltCNH_i(int amt) {
  m_pair_model->shift_cnh((double)(amt));
  updateXY();
  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_AltCNH(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltCNH_i(v);
}

//----------------------------------------- AltCNBNG
inline void BNG_GUI::cb_AltCNBNG_i() {
#if 0
  double dval = angle360(cn_bng_ctr->value());
  cn_bng_ctr->value(dval);
  cpa_model->set_cnbng(cn_bng_ctr->value());
  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
#endif
}
void BNG_GUI::cb_AltCNBNG(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltCNBNG_i();
}

//----------------------------------------- Alt Shape Scale
inline void BNG_GUI::cb_AltShapeScale_i(int delta) {
  if(delta > 0)
    m_bng_viewer->shapeScaleBigger();
  else if(delta < 0)
    m_bng_viewer->shapeScaleSmaller();
  else 
    m_bng_viewer->shapeScaleReset();
  m_bng_viewer->redraw();
}
void BNG_GUI::cb_AltShapeScale(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltShapeScale_i(v);
}


//----------------------------------------- AltCNRNG
inline void BNG_GUI::cb_AltCNRNG_i() {
#if 0
  cpa_model->set_cnrng(cn_rng_ctr->value());
  m_bng_viewer->redraw();
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
#endif
}
void BNG_GUI::cb_AltCNRNG(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AltCNRNG_i();
}

//----------------------------------------- ROT_X
inline void BNG_GUI::cb_RotateX_i(int amt) {
  m_ipf_viewer->setParam("mod_x_rotation", (double)(amt));
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_RotateX(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_RotateX_i(v);
}

//----------------------------------------- ROT_Z
inline void BNG_GUI::cb_RotateZ_i(int amt) {
  m_ipf_viewer->setParam("mod_z_rotation", (double)(amt));
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_RotateZ(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_RotateZ_i(v);
}

//----------------------------------------- Reset
inline void BNG_GUI::cb_Reset_i(int val) {
  if(val==1)  m_ipf_viewer->setParam("reset_view", "1");
  if(val==2)  m_ipf_viewer->setParam("reset_view", "2");
  if(val==3)  m_ipf_viewer->setParam("reset_view", "3");
}
void BNG_GUI::cb_Reset(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_Reset_i(v);
}

//----------------------------------------- Stretch Radius
inline void BNG_GUI::cb_StretchRad_i(int amt) {
  if(amt > 0) m_ipf_viewer->setParam("mod_radius", 1.25);
  if(amt < 0) m_ipf_viewer->setParam("mod_radius", 0.80);
}
void BNG_GUI::cb_StretchRad(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_StretchRad_i(v);
}

//----------------------------------------- Mod Base
inline void BNG_GUI::cb_ModBase_i(int amt) {
  if(amt > 0) m_ipf_viewer->setParam("mod_base", 10);
  if(amt < 0) m_ipf_viewer->setParam("mod_base",-10);
}
void BNG_GUI::cb_ModBase(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_ModBase_i(v);
}

//----------------------------------------- Mod Scale
inline void BNG_GUI::cb_ModScale_i(int amt) {
  if(amt > 0) m_ipf_viewer->setParam("mod_scale", 1.25);
  if(amt < 0) m_ipf_viewer->setParam("mod_scale", 0.80);
}
void BNG_GUI::cb_ModScale(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_ModScale_i(v);
}


//----------------------------------------- Toggle Frame
inline void BNG_GUI::cb_ToggleFrame_i() {
  m_ipf_viewer->setParam("draw_frame", "toggle");
}
void BNG_GUI::cb_ToggleFrame(Fl_Widget* o) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_ToggleFrame_i();
}

//----------------------------------------- UnitOnePrecision
inline void BNG_GUI::cb_AdjustPrecisionIPF_i(int v) {
  if(v == 0)
    m_ipf_resolution = 1;
  else if(v == 1) {
    m_ipf_resolution++;
  }
  else if(v == -1) {
    if(m_ipf_resolution >= 1)
      m_ipf_resolution--;
  }
  else
    return;
  m_ipf_viewer->resetIPF(m_ipf_resolution);
  m_ipf_viewer->redraw();
  updateXY();
}
void BNG_GUI::cb_AdjustPrecisionIPF(Fl_Widget* o, int v) {
  ((BNG_GUI*)(o->parent()->user_data()))->cb_AdjustPrecisionIPF_i(v);
}


//----------------------------------------- UpdateXY
void BNG_GUI::updateXY() 
{
  string sval;

  // Column ONE Ownship Values
  sval = doubleToStringX(m_pair_model->get_osx());
  p_os_x->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->get_osy());
  p_os_y->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->get_osh());
  p_os_h->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->get_osv());
  p_os_v->value(sval.c_str());

  // Column TWO Ownship Values
  sval = doubleToStringX(m_pair_model->get_cnx());
  p_cn_x->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->get_cny());
  p_cn_y->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->get_cnh());
  p_cn_h->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->get_cnv());
  p_cn_v->value(sval.c_str());

  // Column THREE Ownship-Contact Fields
  sval = doubleToStringX(m_pair_model->os_cn_bearing(),1);
  p_os_cn_bng->value(sval.c_str());  

  sval = doubleToStringX(m_pair_model->os_cn_relbearing(),1);
  p_os_cn_rel_bng->value(sval.c_str());  

  sval = doubleToStringX(m_pair_model->os_cn_absrelbearing(),1);
  p_os_cn_abs_rel_bng->value(sval.c_str());  

  // Column FOUR Contact-Ownship Fields
  sval = doubleToStringX(m_pair_model->cn_os_bearing(),1);
  p_cn_os_bng->value(sval.c_str());  

  sval = doubleToStringX(m_pair_model->cn_os_relbearing(),1);
  p_cn_os_rel_bng->value(sval.c_str());  

  sval = doubleToStringX(m_pair_model->cn_os_absrelbearing(),1);
  p_cn_os_abs_rel_bng->value(sval.c_str());  

  // Column FIVE Pair Fields
  sval = doubleToStringX(m_pair_model->range(),1);
  p_range->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->get_cpa(),1);
  p_cpa->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->total_absrelbearing(),1);
  p_tarb->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->os_spd_in_cn_hdg());
  p_spd_in_cn_hdg->value(sval.c_str());  

  // Column SIX Relative Position Fields
  sval = boolToString(m_pair_model->os_fore_of_cn());
  p_fore_of_cn->value(sval.c_str());  

  sval = boolToString(m_pair_model->os_aft_of_cn());
  p_aft_of_cn->value(sval.c_str());  

  sval = boolToString(m_pair_model->os_port_of_cn());
  p_port_of_cn->value(sval.c_str());  

  sval = boolToString(m_pair_model->os_starboard_of_cn());
  p_starboard_of_cn->value(sval.c_str());  

  // Column SEVEN Passing Fields
  sval = boolToString(m_pair_model->os_passes_cn());
  p_passes_cn->value(sval.c_str());  

  sval = boolToString(m_pair_model->os_passes_cn_port());
  p_passes_cn_port->value(sval.c_str());  

  sval = boolToString(m_pair_model->os_passes_cn_starboard());
  p_passes_cn_star->value(sval.c_str());  

  // Column EIGHT Crossing Fields
  sval = boolToString(m_pair_model->os_crosses_cn_bow());
  p_crosses_bow->value(sval.c_str());  

  sval = boolToString(m_pair_model->os_crosses_cn_stern());
  p_crosses_stern->value(sval.c_str());  

  sval = boolToString(m_pair_model->os_crosses_cn_either());
  p_crosses_either->value(sval.c_str());  

  sval = doubleToString(m_pair_model->os_crosses_cn_bow_dist());
  p_crosses_bow_dist->value(sval.c_str());  

  sval = doubleToString(m_pair_model->os_crosses_cn_stern_dist());
  p_crosses_stern_dist->value(sval.c_str());  

  // Column NINE Behavior Fields
  if(m_ipf_viewer) {
    string mode = m_ipf_viewer->getInfo("full_avoid_mode");
    p_bhv_mode->value(mode.c_str());  

    string debug1 = m_ipf_viewer->getInfo("debug1");
    p_bhv_debug1->value(debug1.c_str());  

    string debug2 = m_ipf_viewer->getInfo("debug2");
    p_bhv_debug2->value(debug2.c_str());  

    string debug3 = m_ipf_viewer->getInfo("debug3");
    p_bhv_debug3->value(debug3.c_str());  

    string debug4 = m_ipf_viewer->getInfo("debug4");
    p_bhv_debug4->value(debug4.c_str());  
  }
}
