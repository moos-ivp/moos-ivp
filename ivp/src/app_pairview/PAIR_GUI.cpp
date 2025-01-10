/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PAIR_GUI.cpp                                         */
/*    DATE: Jan 9th, 2025                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "PAIR_GUI.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor()

PAIR_GUI::PAIR_GUI(int wid, int hgt, const char *label)
  : MarineGUI(wid, hgt, label) 
{
  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();
  this->size_range(950,900, 900,500, 0,0, 1);

  m_start_hgt = hgt;
  m_start_wid = wid;

  m_pair_model = new PairModel;
  m_curr_time  = 0;
  
  augmentMenu();
  setMenuItemColors();

  initWidgets();
  resizeWidgetsShape();
  resizeWidgetsText();
  
  this->end();
  this->resizable(this);
  this->show();
}

//----------------------------------------------------------------
// Procedure: setPairModel()

void PAIR_GUI::setPairModel(PairModel new_pair_model)
{
  if(!m_pair_model)
    m_pair_model = new PairModel;

  m_pair_model->copy(new_pair_model);
  m_pair_model_set.copy(new_pair_model);
}

//----------------------------------------------------------------
// Procedure: handleActionSubGUI()

void PAIR_GUI::handleActionSubGUI(int val)
{
  if(val == 1)
    cb_LinearMoveShips_i(10);
  else if(val == 2)
    cb_LinearMoveShips_i(-10);

  if(val == 11)
    cb_LinearMoveShips_i(1);
  else if(val == 12)
    cb_LinearMoveShips_i(-1);

  if(val == 21)
    cb_LinearMoveOwnship_i(10);
  else if(val == 22)
    cb_LinearMoveOwnship_i(-10);

  if(val == 31)
    cb_LinearMoveContact_i(10);
  else if(val == 32)
    cb_LinearMoveContact_i(-10);

  else if(val == 101)
    cb_SwapPair_i();
  else if(val == 102)
    cb_ResetPair_i(0);
  else if(val == 120)   // CTRL-A
    cb_AltCNV_i(1);
  else if(val == 121)   // CTRL-Z
    cb_AltCNV_i(-1);
  else if(val == 130)   // A
    cb_AltOSV_i(1);
  else if(val == 131)   // Z
    cb_AltOSV_i(-1);
}

//--------------------------------------------------------------------------- 
// Procedure: initWidgets()     

void PAIR_GUI::initWidgets()
{
  Fl_Color fcolor_blue  = fl_rgb_color(140, 140, 220);
  //Fl_Color fcolor_beige = fl_rgb_color(223, 219, 191);
  //Fl_Color fcolor_green = fl_rgb_color(200, 230, 190);
  //Fl_Color fcolor_dark_goldenrod  = fl_rgb_color(184, 136, 11);
  //Fl_Color fcolor_dark_goldenrodx = fl_rgb_color(136, 136, 11);

  //=================================================
  // Main Ship Viewer
  //=================================================
  m_pair_viewer = new PAIR_Viewer(m_pair_model, 1, 1, 1, 1);
  m_mviewer    = m_pair_viewer;

  //=================================================
  // Field Panel
  //=================================================
  // Field Panel - Column ONE Ownship Fields
  m_fld_os_x = new Fl_Output(0, 0, 1, 1, "osx:");
  m_fld_os_x->color(fcolor_blue);
  m_fld_os_x->clear_visible_focus();

  m_fld_os_y = new Fl_Output(0, 0, 1, 1, "osy:");
  m_fld_os_y->color(fcolor_blue);
  m_fld_os_y->clear_visible_focus();

  // Field Panel - Column TWO Ownship Fields
  m_fld_os_h = new Fl_Output(0, 0, 1, 1, "osh:");
  m_fld_os_h->color(fcolor_blue);
  m_fld_os_h->clear_visible_focus();

  m_fld_os_v = new Fl_Output(0, 0, 1, 1, "osv:");
  m_fld_os_v->color(fcolor_blue);
  m_fld_os_v->clear_visible_focus();

  // Field Panel - Column THREE Contact Fields
  m_fld_cn_x = new Fl_Output(0, 0, 1, 1, "cnx:");
  m_fld_cn_x->color(fcolor_blue);
  m_fld_cn_x->clear_visible_focus();

  m_fld_cn_y = new Fl_Output(0, 0, 1, 1, "cny:");
  m_fld_cn_y->color(fcolor_blue);
  m_fld_cn_y->clear_visible_focus();

  // Field Panel - Column FOUR Contact Fields
  m_fld_cn_h = new Fl_Output(0, 0, 1, 1, "cnh:");
  m_fld_cn_h->color(fcolor_blue);
  m_fld_cn_h->clear_visible_focus();

  m_fld_cn_v = new Fl_Output(0, 0, 1, 1, "cnv:");
  m_fld_cn_v->color(fcolor_blue);
  m_fld_cn_v->clear_visible_focus();


  // Field Panel - Column FIVE Contact Fields
  m_fld_range = new Fl_Output(0, 0, 1, 1, "range:");
  m_fld_range->clear_visible_focus();

  m_fld_range_rate = new Fl_Output(0, 0, 1, 1, "rng_rate:");
  m_fld_range_rate->clear_visible_focus();

  // Field Panel - Column SIX CPA Field
  m_fld_cpa = new Fl_Output(0, 0, 1, 1, "cpa:");
  m_fld_cpa->clear_visible_focus();

  m_fld_roc = new Fl_Output(0, 0, 1, 1, "roc:");
  m_fld_roc->clear_visible_focus();

  // Field Panel - Column SEVEN alert fields
  m_fld_alert_rng = new Fl_Output(0, 0, 1, 1, "alert_rng:");
  m_fld_alert_rng->clear_visible_focus();

  m_fld_warn_time = new Fl_Output(0, 0, 1, 1, "warn_time:");
  m_fld_warn_time->clear_visible_focus();
  
  // Field Panel - Column EIGHT stat fields
  m_fld_alerted = new Fl_Output(0, 0, 1, 1, "alerted:");
  m_fld_alerted->clear_visible_focus();

  m_fld_warned = new Fl_Output(0, 0, 1, 1, "warned:");
  m_fld_warned->clear_visible_focus();
  
  // Field Panel - Column NINE time-of fields
  m_fld_to_alerted = new Fl_Output(0, 0, 1, 1, "to_alerted:");
  m_fld_to_alerted->clear_visible_focus();

  m_fld_to_warned = new Fl_Output(0, 0, 1, 1, "to_warned:");
  m_fld_to_warned->clear_visible_focus();
  
  // Field Panel - Column TEN time fields
  m_fld_curr_time = new Fl_Output(0, 0, 1, 1, "curr_time:");
  m_fld_curr_time->clear_visible_focus();

  m_fld_delt_time = new Fl_Output(0, 0, 1, 1, "delt_time:");
  m_fld_delt_time->clear_visible_focus();
  
}

//--------------------------------------------------------------------------- 
// Procedure: resizeWidgetsShape()     

void PAIR_GUI::resizeWidgetsShape()
{

  //int lmarg = 50;
  int extra_wid = w() - m_start_wid;
  if(extra_wid < 0)
    extra_wid = 0;
  int field_hgt = 20;

  int row0 = h() -60;
  int row1 = row0 + 25;
  //int row2 = row1 + 25;
  //int row3 = row2 + 25;

  //int row4 = row3 + 35;
  //int row5 = row4 + 25;
  //int row6 = row5 + 25;
  //int row7 = row6 + 25;
  //int row8 = row7 + 25;
  
  int col1 = 40;
  int col2 = col1 + 80;
  int col3 = col2 + 75;
  int col4 = col3 + 80;
  int col5 = col4 + 100;
  int col6 = col5 + 85  + (0.2 * extra_wid);
  int col7 = col6 + 115 + (0.2 * extra_wid);
  int col8 = col7 + 95 + (0.2 * extra_wid);
  int col9 = col8 + 105 + (0.2 * extra_wid);
  int col10 = col9 + 105 + (0.2 * extra_wid);

  //===================================================
  // Main Ship Viewer
  //===================================================
  m_pair_viewer->resize(0, 30, w(), h()-95);

  //===================================================
  // Field Panel - Column ONE Ownship Fields
  //===================================================
  int osx_x = col1;
  int osx_y = row0;
  int osx_wid = 45;
  m_fld_os_x->resize(osx_x, osx_y, osx_wid, field_hgt);

  int osy_x = col1;
  int osy_y = row1;
  int osy_wid = 45;
  m_fld_os_y->resize(osy_x, osy_y, osy_wid, field_hgt);

  //===================================================
  // Field Panel - Column TWO Ownship Fields
  //===================================================
  int osh_x = col2;
  int osh_y = row0;
  int osh_wid = 40;
  m_fld_os_h->resize(osh_x, osh_y, osh_wid, field_hgt);

  int osv_x = col2;
  int osv_y = row1;
  int osv_wid = 40;
  m_fld_os_v->resize(osv_x, osv_y, osv_wid, field_hgt);

  //===================================================
  // Field Panel - Column THREE Contact Fields
  //===================================================
  int cnx_x = col3;
  int cnx_y = row0;
  int cnx_wid = 45;
  m_fld_cn_x->resize(cnx_x, cnx_y, cnx_wid, field_hgt);

  int cny_x = col3;
  int cny_y = row1;
  int cny_wid = 45;
  m_fld_cn_y->resize(cny_x, cny_y, cny_wid, field_hgt);

  //===================================================
  // Field Panel - Column FOUR Contact Fields
  //===================================================
  int cnh_x = col4;
  int cnh_y = row0;
  int cnh_wid = 40;
  m_fld_cn_h->resize(cnh_x, cnh_y, cnh_wid, field_hgt);

  int cnv_x = col4;
  int cnv_y = row1;
  int cnv_wid = 40;
  m_fld_cn_v->resize(cnv_x, cnv_y, cnv_wid, field_hgt);

  //===================================================
  // Field Panel - Column FIVE Range Fields
  //===================================================
  int rng_x = col5;
  int rng_y = row0;
  int rng_wid = 50 + (extra_wid/4);
  m_fld_range->resize(rng_x, rng_y, rng_wid, field_hgt);

  int rrat_x = col5;
  int rrat_y = row1;
  int rrat_wid = 50 + (extra_wid/4);
  m_fld_range_rate->resize(rrat_x, rrat_y, rrat_wid, field_hgt);

  //===================================================
  // Field Panel - Column SIX CPA Field
  //===================================================
  int cpa_x = col6;
  int cpa_y = row0;
  int cpa_wid = 50 + (extra_wid/4);
  m_fld_cpa->resize(cpa_x, cpa_y, cpa_wid, field_hgt);

  int roc_x = col6;
  int roc_y = row1;
  int roc_wid = 50 + (extra_wid/4);
  m_fld_roc->resize(roc_x, roc_y, roc_wid, field_hgt);
  
  //===================================================
  // Field Panel - Column SEVEN Alert fields
  //===================================================
  int arng_x = col7;
  int arng_y = row0;
  int arng_wid = 40 + (extra_wid/4);
  m_fld_alert_rng->resize(arng_x, arng_y, arng_wid, field_hgt);

  int war_x = col7;
  int war_y = row1;
  int war_wid = 40 + (extra_wid/4);
  m_fld_warn_time->resize(war_x, war_y, war_wid, field_hgt);

  //===================================================
  // Field Panel - Column EIGHT Status fields
  //===================================================
  int aed_x = col8;
  int aed_y = row0;
  int aed_wid = 40 + (extra_wid/4);
  m_fld_alerted->resize(aed_x, aed_y, aed_wid, field_hgt);

  int wed_x = col8;
  int wed_y = row1;
  int wed_wid = 40 + (extra_wid/4);
  m_fld_warned->resize(wed_x, wed_y, wed_wid, field_hgt);

  //===================================================
  // Field Panel - Column NINE Time-OF fields
  //===================================================
  int toa_x = col9;
  int toa_y = row0;
  int toa_wid = 40 + (extra_wid/4);
  m_fld_to_alerted->resize(toa_x, toa_y, toa_wid, field_hgt);

  int tow_x = col9;
  int tow_y = row1;
  int tow_wid = 40 + (extra_wid/4);
  m_fld_to_warned->resize(tow_x, tow_y, tow_wid, field_hgt);

  //===================================================
  // Field Panel - Column TEN Time fields
  //===================================================
  int cut_x = col10;
  int cut_y = row0;
  int cut_wid = 40 + (extra_wid/4);
  m_fld_curr_time->resize(cut_x, cut_y, cut_wid, field_hgt);

  int det_x = col10;
  int det_y = row1;
  int det_wid = 40 + (extra_wid/4);
  m_fld_delt_time->resize(det_x, det_y, det_wid, field_hgt);
}
  
//--------------------------------------------------------------------------- 
// Procedure: resizeWidgetsText()

void PAIR_GUI::resizeWidgetsText()
{
  int text_size  = 12;
  int label_size = 12;

  // Field Panel - Column ONE Ownship Fields
  m_fld_os_x->textsize(text_size);
  m_fld_os_x->labelsize(label_size);

  m_fld_os_y->textsize(text_size);
  m_fld_os_y->labelsize(label_size);

 // Field Panel - Column TWO
  m_fld_os_h->textsize(text_size);
  m_fld_os_h->labelsize(label_size);

  m_fld_os_v->textsize(text_size);
  m_fld_os_v->labelsize(label_size);

 // Field Panel - Column THREE
  m_fld_cn_x->textsize(text_size);
  m_fld_cn_x->labelsize(label_size);

  m_fld_cn_y->textsize(text_size);
  m_fld_cn_y->labelsize(label_size);

 // Field Panel - Column FOUR
  m_fld_cn_h->textsize(text_size);
  m_fld_cn_h->labelsize(label_size);

  m_fld_cn_v->textsize(text_size);
  m_fld_cn_v->labelsize(label_size);

  // Field Panel - Column FIVE
  m_fld_range->textsize(text_size);
  m_fld_range->labelsize(label_size);

  m_fld_range_rate->textsize(text_size);
  m_fld_range_rate->labelsize(label_size);

  // Field Panel - Column SIX
  m_fld_cpa->textsize(text_size);
  m_fld_cpa->labelsize(label_size);

  m_fld_roc->textsize(text_size);
  m_fld_roc->labelsize(label_size);
  
  // Field Panel - Column SEVEN
  m_fld_alert_rng->textsize(text_size);
  m_fld_alert_rng->labelsize(label_size);

  m_fld_warn_time->textsize(text_size);
  m_fld_warn_time->labelsize(label_size);

  // Field Panel - Column EIGHT
  m_fld_alerted->textsize(text_size);
  m_fld_alerted->labelsize(label_size);

  m_fld_warned->textsize(text_size);
  m_fld_warned->labelsize(label_size);

  // Field Panel - Column NINE
  m_fld_to_alerted->textsize(text_size);
  m_fld_to_alerted->labelsize(label_size);

  m_fld_to_warned->textsize(text_size);
  m_fld_to_warned->labelsize(label_size);

  // Field Panel - Column TEN
  m_fld_curr_time->textsize(text_size);
  m_fld_curr_time->labelsize(label_size);

  m_fld_delt_time->textsize(text_size);
  m_fld_delt_time->labelsize(label_size);
}

//---------------------------------------------------------- 
// Procedure: resize()

void PAIR_GUI::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgetsShape();
  resizeWidgetsText();
}

//----------------------------------------------------------------
// Procedure: augmentMenu()

void PAIR_GUI::augmentMenu() 
{
  // First we remove some items at the superclass level so we can use the 
  // hot keys differently. 
  removeMenuItem("BackView/Zoom Reset");

  removeMenuItem("BackView/Pan Up (slow) ");
  removeMenuItem("BackView/Pan Down (slow) ");
  removeMenuItem("BackView/Pan Left (slow) ");
  removeMenuItem("BackView/Pan Right (slow)");

  removeMenuItem("BackView/Pan Up (v. slow) ");
  removeMenuItem("BackView/Pan Down (v. slow) ");
  removeMenuItem("BackView/Pan Left (v. slow) ");
  removeMenuItem("BackView/Pan Right (v. slow)");

  //====================================================================
  // The Pair SubMenu
  //====================================================================

  m_menubar->add("Pair/Set",   0, (Fl_Callback*)PAIR_GUI::cb_SetPair, (void*)0, 0);
  m_menubar->add("Pair/Swap",  's', (Fl_Callback*)PAIR_GUI::cb_SwapPair, (void*)0, 0);
  m_menubar->add("Pair/Reset", 'y',
		 (Fl_Callback*)PAIR_GUI::cb_ResetPair, (void*)0, FL_MENU_DIVIDER);

  m_menubar->add("Pair/toggle_alert_circle", FL_CTRL+'r',
		 (Fl_Callback*)PAIR_GUI::cb_ToggleCircle, (void*)1, 0);
  m_menubar->add("Pair/toggle_warning_circle", 'r',
		 (Fl_Callback*)PAIR_GUI::cb_ToggleCircle, (void*)2,FL_MENU_DIVIDER);

  m_menubar->add("Pair/vehicles_shape_scale = larger",  '+',
		 (Fl_Callback*)PAIR_GUI::cb_AltShapeScale, (void*)+1, 0);
  m_menubar->add("Pair/vehicles_shape_scale = smaller", '-',
		 (Fl_Callback*)PAIR_GUI::cb_AltShapeScale, (void*)-1, 0);
  m_menubar->add("Pair/center_view", 'c',
		 (Fl_Callback*)PAIR_GUI::cb_CenterView, (void*)-1, FL_MENU_DIVIDER);

  m_menubar->add("Pair/bothships forward (1 sec)",  ']',
		 (Fl_Callback*)PAIR_GUI::cb_LinearMoveShips, (void*)10, 0);
  m_menubar->add("Pair/bothships backward (1 sec)", '[',
		 (Fl_Callback*)PAIR_GUI::cb_LinearMoveShips, (void*)-10,FL_MENU_DIVIDER);
  m_menubar->add("Pair/bothships forward (0.1 sec)",  FL_CTRL+']',
		 (Fl_Callback*)PAIR_GUI::cb_LinearMoveShips, (void*)1, 0);
  m_menubar->add("Pair/bothships backward (0.1 sec)", FL_CTRL+'[',
		 (Fl_Callback*)PAIR_GUI::cb_LinearMoveShips, (void*)-1, 0);

  m_menubar->add("Pair/ownship forward (1 sec)",  FL_ALT+']',
		 (Fl_Callback*)PAIR_GUI::cb_LinearMoveOwnship, (void*)10, 0);
  m_menubar->add("Pair/ownship backward (1 sec)", FL_ALT+'[',
		 (Fl_Callback*)PAIR_GUI::cb_LinearMoveOwnship, (void*)-10, 0);

  m_menubar->add("Pair/contact forward (1 sec)",  FL_ALT+FL_CTRL+']',
		 (Fl_Callback*)PAIR_GUI::cb_LinearMoveContact, (void*)10, 0);
  m_menubar->add("Pair/contact backward (1 sec)", FL_ALT+FL_CTRL+'[',
		 (Fl_Callback*)PAIR_GUI::cb_LinearMoveContact, (void*)-10, 0);


  m_menubar->add("Pair/ownship range open",  '>',
		 (Fl_Callback*)PAIR_GUI::cb_AltOwnshipRange,  (void*)1, 0);
  m_menubar->add("Pair/ownship range close",  '<',
		 (Fl_Callback*)PAIR_GUI::cb_AltOwnshipRange,(void*)-1, FL_MENU_DIVIDER);
  m_menubar->add("Pair/inc os bearing", FL_ALT+FL_Right,
		 (Fl_Callback*)PAIR_GUI::cb_AltBearing, (void*)1,  0);
  m_menubar->add("Pair/dec os bearing", FL_ALT+FL_Left,
		 (Fl_Callback*)PAIR_GUI::cb_AltBearing, (void*)2, 0);
  m_menubar->add("Pair/inc os bearing, hold rb", ')',
		 (Fl_Callback*)PAIR_GUI::cb_AltBearing, (void*)11,  0);
  m_menubar->add("Pair/dec os bearing, hold rb", '(',
		 (Fl_Callback*)PAIR_GUI::cb_AltBearing, (void*)12, FL_MENU_DIVIDER);
  m_menubar->add("Pair/dump cmdline args", 'd',
		 (Fl_Callback*)PAIR_GUI::cb_DumpCmdLineArgs, (void*)0, FL_MENU_DIVIDER);

  //====================================================================
  // The Ownship SubMenu
  //====================================================================

  m_menubar->add("Ownship/Reset", FL_ALT+'y',
		 (Fl_Callback*)PAIR_GUI::cb_ResetPair, (void*)1, 0);
  m_menubar->add("Ownship/Heading -", 'n',
		 (Fl_Callback*)PAIR_GUI::cb_AltOSH, (void*)-1, 0);
  m_menubar->add("Ownship/Heading +", 'm',
		 (Fl_Callback*)PAIR_GUI::cb_AltOSH, (void*)1, 0);
  m_menubar->add("Ownship/Heading --", FL_SHIFT+'n',
		 (Fl_Callback*)PAIR_GUI::cb_AltOSH, (void*)-5, 0);
  m_menubar->add("Ownship/Heading ++", FL_SHIFT+'m',
		 (Fl_Callback*)PAIR_GUI::cb_AltOSH, (void*)5, 0);
  m_menubar->add("Ownship/Speed   -", 'z',
		 (Fl_Callback*)PAIR_GUI::cb_AltOSV, (void*)-1, 0);
  m_menubar->add("Ownship/Speed   +", 'a',
		 (Fl_Callback*)PAIR_GUI::cb_AltOSV, (void*)1, FL_MENU_DIVIDER);
  m_menubar->add("Ownship/Up",   FL_SHIFT+FL_Up,
		 (Fl_Callback*)PAIR_GUI::cb_AltOSY, (void*)1, 0);
  m_menubar->add("Ownship/Down", FL_SHIFT+FL_Down,
		 (Fl_Callback*)PAIR_GUI::cb_AltOSY, (void*)-1, 0);
  m_menubar->add("Ownship/Right",FL_SHIFT+FL_Right,
		 (Fl_Callback*)PAIR_GUI::cb_AltOSX, (void*)1, 0);
  m_menubar->add("Ownship/Left", FL_SHIFT+FL_Left,
		 (Fl_Callback*)PAIR_GUI::cb_AltOSX, (void*)-1, 0);

  //====================================================================
  // The Contact SubMenu
  //====================================================================

  m_menubar->add("Contact/Reset", FL_CTRL+'y',
		 (Fl_Callback*)PAIR_GUI::cb_ResetPair, (void*)2, 0);
  m_menubar->add("Contact/Heading -", FL_CTRL+'n',
		 (Fl_Callback*)PAIR_GUI::cb_AltCNH, (void*)-1,  0);
  m_menubar->add("Contact/Heading --", FL_SHIFT+FL_CTRL+'n',
		 (Fl_Callback*)PAIR_GUI::cb_AltCNH, (void*)-5,  0);
  m_menubar->add("Contact/Heading +", FL_CTRL+'m',
		 (Fl_Callback*)PAIR_GUI::cb_AltCNH, (void*)1, 0);
  m_menubar->add("Contact/Heading ++", FL_SHIFT+FL_CTRL+'m',
		 (Fl_Callback*)PAIR_GUI::cb_AltCNH, (void*)5, 0);
  m_menubar->add("Contact/Speed   -", FL_CTRL+'z',
		 (Fl_Callback*)PAIR_GUI::cb_AltCNV, (void*)-1,  0);
  m_menubar->add("Contact/Speed   +", FL_CTRL+'a',
		 (Fl_Callback*)PAIR_GUI::cb_AltCNV, (void*)1, FL_MENU_DIVIDER);
  m_menubar->add("Contact/Up",   FL_CTRL+FL_Up,
		 (Fl_Callback*)PAIR_GUI::cb_AltCNY, (void*)1, 0);
  m_menubar->add("Contact/Down", FL_CTRL+FL_Down,
		 (Fl_Callback*)PAIR_GUI::cb_AltCNY, (void*)-1, 0);
  m_menubar->add("Contact/Right",FL_CTRL+FL_Right,
		 (Fl_Callback*)PAIR_GUI::cb_AltCNX, (void*)1, 0);
  m_menubar->add("Contact/Left", FL_CTRL+FL_Left,
		 (Fl_Callback*)PAIR_GUI::cb_AltCNX, (void*)-1, 0);
}

//----------------------------------------------------------
// Procedure: handle()

int PAIR_GUI::handle(int event) 
{
  switch(event) {
  case FL_PUSH:
    Fl_Window::handle(event);
    updateXY();
    return(1);
    break;
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------- CenterView
inline void PAIR_GUI::cb_CenterView_i() {
  m_pair_viewer->initCenterView();
  m_pair_viewer->redraw();
}
void PAIR_GUI::cb_CenterView(Fl_Widget* o) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_CenterView_i();
}

//----------------------------------------- ToggleCircle
inline void PAIR_GUI::cb_ToggleCircle_i(int i) {
  if(i==1)
    m_pair_viewer->toggleAlertCircle();
  else if(i==2)
    m_pair_viewer->toggleWarningCircle();
  else
    return;

  m_pair_viewer->redraw();
}
void PAIR_GUI::cb_ToggleCircle(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_ToggleCircle_i(v);
}

//----------------------------------------- LinearMoveShips
inline void PAIR_GUI::cb_LinearMoveShips_i(int tenths_of_secs)
{
  double secs = ((double)(tenths_of_secs)) / 10.0;
  
  m_pair_model->move_os(secs);
  m_pair_model->move_cn(secs);
  m_curr_time += secs;
  m_pair_model->set_curr_time(m_curr_time);
  m_pair_viewer->redraw();
  updateXY();
}
void PAIR_GUI::cb_LinearMoveShips(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_LinearMoveShips_i(v);
}

//----------------------------------------- LinearMoveOwnship
inline void PAIR_GUI::cb_LinearMoveOwnship_i(int tenths_of_secs)
{
  double secs = ((double)(tenths_of_secs)) / 10.0; 
  m_pair_model->move_os(secs);
  m_curr_time += secs;
  m_pair_model->set_curr_time(m_curr_time);  
}
void PAIR_GUI::cb_LinearMoveOwnship(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_LinearMoveOwnship_i(v);
}

//----------------------------------------- LinearMoveContact
inline void PAIR_GUI::cb_LinearMoveContact_i(int tenths_of_secs)
{
  double secs = ((double)(tenths_of_secs)) / 10.0; 
  m_pair_model->move_cn(secs);
  m_curr_time += secs;
  m_pair_model->set_curr_time(m_curr_time);
}
void PAIR_GUI::cb_LinearMoveContact(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_LinearMoveContact_i(v);
}

//----------------------------------------- AltOwnshipRange
inline void PAIR_GUI::cb_AltOwnshipRange_i(int i) {
  m_pair_model->alt_os_range((double)(i));
  m_pair_viewer->redraw();
  updateXY();
}
void PAIR_GUI::cb_AltOwnshipRange(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_AltOwnshipRange_i(v);
}

//----------------------------------------- AltBearing
inline void PAIR_GUI::cb_AltBearing_i(int i) {
  if(i == 1)
    m_pair_model->alt_os_bearing(1);
  else if(i == 2)
    m_pair_model->alt_os_bearing(-1);
  else if(i == 11)
    m_pair_model->alt_os_bearing_holdrb(1);
  else if(i == 12)
    m_pair_model->alt_os_bearing_holdrb(-1);
  m_pair_viewer->redraw();
  updateXY();
}
void PAIR_GUI::cb_AltBearing(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_AltBearing_i(v);
}

//----------------------------------------- SetPair
inline void PAIR_GUI::cb_SetPair_i() {
  m_pair_model_set.copy(*m_pair_model);
  m_pair_viewer->redraw();
  updateXY();
}
void PAIR_GUI::cb_SetPair(Fl_Widget* o) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_SetPair_i();
}

//----------------------------------------- SwapPair
inline void PAIR_GUI::cb_SwapPair_i() {
  m_pair_model->swap();
  m_pair_viewer->redraw();
  updateXY();
}
void PAIR_GUI::cb_SwapPair(Fl_Widget* o) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_SwapPair_i();
}

//----------------------------------------- ResetPair
inline void PAIR_GUI::cb_ResetPair_i(int i) {
  if(i==0)
    m_pair_model->copy(m_pair_model_set, "both");
  else if(i==1)
    m_pair_model->copy(m_pair_model_set, "ownship");
  else if(i==2)
    m_pair_model->copy(m_pair_model_set, "contact");

  m_curr_time = 0;
  //  m_plt_viewer->setCurrTime(0);
  m_pair_model->set_curr_time(0);
  m_pair_viewer->redraw();
  updateXY();

}
void PAIR_GUI::cb_ResetPair(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_ResetPair_i(v);
}

//----------------------------------------- AltOSX
inline void PAIR_GUI::cb_AltOSX_i(int amt) {
  m_pair_model->shift_osx((double)(amt));
  m_pair_viewer->redraw();
  updateXY();
}
void PAIR_GUI::cb_AltOSX(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_AltOSX_i(v);
}

//----------------------------------------- AltOSY
inline void PAIR_GUI::cb_AltOSY_i(int amt) {
  m_pair_model->shift_osy((double)(amt));
  m_pair_viewer->redraw();
  updateXY();
}
void PAIR_GUI::cb_AltOSY(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_AltOSY_i(v);
}

//----------------------------------------- AltOSH
inline void PAIR_GUI::cb_AltOSH_i(int amt) {
  m_pair_model->shift_osh((double)(amt));
  m_pair_viewer->redraw();
  updateXY();
}
void PAIR_GUI::cb_AltOSH(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_AltOSH_i(v);
}

//----------------------------------------- AltOSV
inline void PAIR_GUI::cb_AltOSV_i(int amt) {
  double delta = (double)(amt) * 0.1;
  m_pair_model->shift_osv(delta);
  m_pair_viewer->redraw();
  updateXY();
}
void PAIR_GUI::cb_AltOSV(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_AltOSV_i(v);
}

//----------------------------------------- AltCNX
inline void PAIR_GUI::cb_AltCNX_i(int amt) {
  m_pair_model->shift_cnx((double)(amt));
  m_pair_viewer->redraw();
  updateXY();
}
void PAIR_GUI::cb_AltCNX(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_AltCNX_i(v);
}

//----------------------------------------- AltCNY
inline void PAIR_GUI::cb_AltCNY_i(int amt) {
  m_pair_model->shift_cny((double)(amt));
  m_pair_viewer->redraw();
  updateXY();
}
void PAIR_GUI::cb_AltCNY(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_AltCNY_i(v);
}

//----------------------------------------- AltCNV
inline void PAIR_GUI::cb_AltCNV_i(int amt) {
  double delta = (double)(amt) * 0.1;
  m_pair_model->shift_cnv(delta);
  m_pair_viewer->redraw();
  updateXY();
}
void PAIR_GUI::cb_AltCNV(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_AltCNV_i(v);
}

//----------------------------------------- AltCNH
inline void PAIR_GUI::cb_AltCNH_i(int amt) {
  m_pair_model->shift_cnh((double)(amt));
  m_pair_viewer->redraw();
  updateXY();
}
void PAIR_GUI::cb_AltCNH(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_AltCNH_i(v);
}

//----------------------------------------- Alt Shape Scale
inline void PAIR_GUI::cb_AltShapeScale_i(int delta) {
  if(delta > 0)
    m_pair_viewer->shapeScaleBigger();
  else if(delta < 0)
    m_pair_viewer->shapeScaleSmaller();
  else 
    m_pair_viewer->shapeScaleReset();
  m_pair_viewer->redraw();
}
void PAIR_GUI::cb_AltShapeScale(Fl_Widget* o, int v) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_AltShapeScale_i(v);
}

//----------------------------------------- Dump CmdLine Args
inline void PAIR_GUI::cb_DumpCmdLineArgs_i() {
  if(!m_pair_model)
    return;
  double osx = m_pair_model->get_osx();
  double osy = m_pair_model->get_osy();
  double osh = m_pair_model->get_osh();
  double osv = m_pair_model->get_osv();

  double cnx = m_pair_model->get_cnx();
  double cny = m_pair_model->get_cny();
  double cnh = m_pair_model->get_cnh();
  double cnv = m_pair_model->get_cnv();

  cout << "pairview";
  cout << " --osx=" << doubleToStringX(osx,2);
  cout << " --osy=" << doubleToStringX(osy,2);
  cout << " --osh=" << doubleToStringX(osh,2);
  cout << " --osv=" << doubleToStringX(osv,2);
  cout << " --cnx=" << doubleToStringX(cnx,2);
  cout << " --cny=" << doubleToStringX(cny,2);
  cout << " --cnh=" << doubleToStringX(cnh,2);
  cout << " --cnv=" << doubleToStringX(cnv,2) << endl;
}
void PAIR_GUI::cb_DumpCmdLineArgs(Fl_Widget* o) {
  ((PAIR_GUI*)(o->parent()->user_data()))->cb_DumpCmdLineArgs_i();
}


//------------------------------------------------------
// Procedure  UpdateXY()

void PAIR_GUI::updateXY() 
{
  //double dval;
  string sval;

  //=================================================
  // Field Panel - Column ONE Ownship Values
  sval = doubleToStringX(m_pair_model->get_osx(),2);
  m_fld_os_x->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->get_osy(),2);
  m_fld_os_y->value(sval.c_str());

  // Field Panel - Column TWO Ownship Values
  sval = doubleToStringX(m_pair_model->get_osh(),2);
  m_fld_os_h->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->get_osv(),2);
  m_fld_os_v->value(sval.c_str());

  // Field Panel - Column THREE Contact Values
  sval = doubleToStringX(m_pair_model->get_cnx(),2);
  m_fld_cn_x->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->get_cny(),2);
  m_fld_cn_y->value(sval.c_str());

  // Field Panel - Column FOUR Contact Values
  sval = doubleToStringX(m_pair_model->get_cnh(),2);
  m_fld_cn_h->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->get_cnv(),2);
  m_fld_cn_v->value(sval.c_str());

  // Field Panel - Column FIVE Range Values
  sval = doubleToStringX(m_pair_model->range(),2);
  m_fld_range->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->range_rate(),2);
  m_fld_range_rate->value(sval.c_str());

  // Field Panel - Column SIX CPA
  sval = doubleToStringX(m_pair_model->get_cpa(3600),2);
  m_fld_cpa->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->rate_of_closure(),2);
  m_fld_roc->value(sval.c_str());

  // Field Panel - Column SEVEN ALERT
  sval = doubleToStringX(m_pair_model->get_alert_range(),2);
  m_fld_alert_rng->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->get_warning_time(),2);
  m_fld_warn_time->value(sval.c_str());

  // Field Panel - Column EIGHT Status fields
  sval = boolToString(m_pair_model->get_alerted());
  m_fld_alerted->value(sval.c_str());

  sval = boolToString(m_pair_model->get_warned());
  m_fld_warned->value(sval.c_str());

  // Field Panel - Column NINE TIME-OF fields
  sval = doubleToStringX(m_pair_model->get_alerted_time(),2);
  m_fld_to_alerted->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->get_warned_time(),2);
  m_fld_to_warned->value(sval.c_str());

  // Field Panel - Column TEN TIME fields
  sval = doubleToStringX(m_pair_model->get_curr_time(),2);
  m_fld_curr_time->value(sval.c_str());

  sval = doubleToStringX(m_pair_model->get_delt_time(),2);
  m_fld_delt_time->value(sval.c_str());
}





