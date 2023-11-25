/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: GUI_AppLogScope.cpp                                  */
/*    DATE: Oct 16th, 2021                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <FL/fl_ask.H>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "GUI_AppLogScope.h"
#include "REPLAY_GUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

GUI_AppLogScope::GUI_AppLogScope(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) 
{
  mbar = new Fl_Menu_Bar(0, 0, 0, 0);
  mbar->menu(menu_);

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();
  this->size_range(450,250);

  m_replay_warp_msg = "(PAUSED)";
  m_parent_gui = 0;

  m_mutable_text_size = 10;

  initWidgets();  
  resizeWidgetsShape();
  resizeWidgetsText();

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
// Destructor: Must carefully implement since master GUI may create
//             and destroy instances of this GUI all in same session

GUI_AppLogScope::~GUI_AppLogScope()
{
  // Fields
  if(m_fld_time)      
    delete(m_fld_time);
  if(m_fld_grep1)      
    delete(m_fld_grep1);
  if(m_fld_grep2)      
    delete(m_fld_grep2);

  // Check-box buttons
  if(m_but_truncate)  
    delete(m_but_truncate);
  if(m_but_separate)  
    delete(m_but_separate);
  if(m_but_wrapline)  
    delete(m_but_wrapline);
  if(m_but_future)  
    delete(m_but_future);
  if(m_but_apply_grep1)  
    delete(m_but_apply_grep1);
  if(m_but_apply_grep2)  
    delete(m_but_apply_grep2);

  // Standard buttons
  if(m_but_mod_grep1)
    delete(m_but_mod_grep1);
  if(m_but_mod_grep2)
    delete(m_but_mod_grep2);
  if(m_but_text_more)
    delete(m_but_text_more);
  if(m_but_text_less)
    delete(m_but_text_less);

  // Main content browser
  if(m_brw_info1)
    delete(m_brw_info1);
  if(m_brw_info2)
    delete(m_brw_info2);
}

//---------------------------------------------------------------------------
// Procedure: initWidgets()

void GUI_AppLogScope::initWidgets()
{
  Fl_Color fcolor2 = fl_rgb_color(190, 235, 210); // green
  //Fl_Color fcolor2 = fl_rgb_color(190, 255, 190); // green
  //Fl_Color fcolor3 = fl_rgb_color(245, 245, 170); // yellow
  //Fl_Color fcolor4 = fl_rgb_color(225, 170, 170); // red
  Fl_Color bcolor = fl_rgb_color(0, 0, 120);     // dark blue
  Fl_Color bcolor_gray = fl_rgb_color(200, 220, 200); // light blueish gray

  //==========================================================
  // Row 1
  //==========================================================
  m_fld_time = new Fl_Output(0, 0, 0, 0, "Time:"); 
  m_fld_time->clear_visible_focus();

  m_but_separate = new Fl_Check_Button(0, 0, 0, 0, "Separate");
  m_but_separate->labelcolor(bcolor);
  m_but_separate->callback((Fl_Callback*)GUI_AppLogScope::cb_ButtonSeparate, (void*)0);
  m_but_separate->clear_visible_focus();
  m_but_separate->value(0);

  m_but_truncate = new Fl_Check_Button(0, 0, 0, 0, "Truncate");
  m_but_truncate->labelcolor(bcolor);
  m_but_truncate->callback((Fl_Callback*)GUI_AppLogScope::cb_ButtonTruncate, (void*)0);
  m_but_truncate->clear_visible_focus();
  m_but_truncate->value(0);

  m_but_wrapline = new Fl_Check_Button(0, 0, 0, 0, "Wrapline");
  m_but_wrapline->labelcolor(bcolor);
  m_but_wrapline->callback((Fl_Callback*)GUI_AppLogScope::cb_ButtonWrapLine, (void*)0);
  m_but_wrapline->clear_visible_focus();
  m_but_wrapline->value(0);

  m_but_future = new Fl_Check_Button(0, 0, 0, 0, "Future");
  m_but_future->labelcolor(bcolor);
  m_but_future->callback((Fl_Callback*)GUI_AppLogScope::cb_ButtonFuture, (void*)0);
  m_but_future->clear_visible_focus();
  m_but_future->value(0);

  //==========================================================
  // Row 2: First Grep option
  //==========================================================
  m_fld_grep1 = new Fl_Output(0, 0, 0, 0, "Grep1:"); 
  m_fld_grep1->clear_visible_focus();

  m_but_mod_grep1 = new Fl_Button(0, 0, 0, 0, "Modify");
  m_but_mod_grep1->clear_visible_focus();
  m_but_mod_grep1->callback((Fl_Callback*)GUI_AppLogScope::cb_ModGrep, (void*)1);

  m_but_apply_grep1 = new Fl_Check_Button(0, 0, 0, 0, "ApplyGrep1");
  m_but_apply_grep1->labelcolor(bcolor);
  m_but_apply_grep1->callback((Fl_Callback*)GUI_AppLogScope::cb_ButtonApplyGrep, (void*)1);
  m_but_apply_grep1->clear_visible_focus();
  m_but_apply_grep1->value(0);

  m_but_text_more = new Fl_Button(0, 0, 0, 0, "Text");
  m_but_text_more->clear_visible_focus();
  m_but_text_more->callback((Fl_Callback*)GUI_AppLogScope::cb_ModText, (void*)1);


  //==========================================================
  // Row 3: Second Grep option
  //==========================================================
  m_but_apply_grep2 = new Fl_Check_Button(0, 0, 0, 0, "ApplyGrep2");
  m_but_apply_grep2->labelcolor(bcolor);
  m_but_apply_grep2->callback((Fl_Callback*)GUI_AppLogScope::cb_ButtonApplyGrep, (void*)2);
  m_but_apply_grep2->clear_visible_focus();
  m_but_apply_grep2->value(0);

  m_fld_grep2 = new Fl_Output(0, 0, 0, 0, "Grep2:"); 
  m_fld_grep2->clear_visible_focus();

  m_but_mod_grep2 = new Fl_Button(0, 0, 0, 0, "Modify");
  m_but_mod_grep2->clear_visible_focus();
  m_but_mod_grep2->callback((Fl_Callback*)GUI_AppLogScope::cb_ModGrep, (void*)2);

  m_but_text_less = new Fl_Button(0, 0, 0, 0, "Text");
  m_but_text_less->clear_visible_focus();
  m_but_text_less->callback((Fl_Callback*)GUI_AppLogScope::cb_ModText, (void*)2);

  
  //==========================================================
  // Row 4/5 Browser panels
  //==========================================================
  m_brw_info1 = new Fl_Browser(0, 0, 1, 1); 
  m_brw_info1->align(FL_ALIGN_LEFT);
  m_brw_info1->color(fcolor2);
  m_brw_info1->tooltip("Terminal Output Now/Past");
  m_brw_info1->callback((Fl_Callback*)GUI_AppLogScope::cb_BrowserInfo);
  m_brw_info1->when(FL_WHEN_RELEASE);
  m_brw_info1->textfont(FL_COURIER_BOLD);

  m_brw_info2 = new Fl_Browser(0, 0, 1, 1); 
  m_brw_info2->align(FL_ALIGN_LEFT);
  m_brw_info2->color(bcolor_gray);
  m_brw_info2->tooltip("Terminal Output Future");
  m_brw_info2->callback((Fl_Callback*)GUI_AppLogScope::cb_BrowserInfo);
  m_brw_info2->when(FL_WHEN_RELEASE);
  m_brw_info2->textfont(FL_COURIER_BOLD);

}

//---------------------------------------------------------------------------
// Procedure: resizeWidgetsShape()

void GUI_AppLogScope::resizeWidgetsShape()
{
  // General params
  int lmarg = 10;
  int rmarg = 10;
  int row1  = 5;
  int row2  = 30;
  int row3  = 55;
    
  int bwid  = w() - (lmarg + rmarg);
  int total_dat_hgt = 90;
  int total_brw_hgt = h() - total_dat_hgt - 5;

  // Part 1: Set top row: time, add, set, remove buttons
  m_fld_time->resize(40, row1, 95, 20);

  int sep_x = 150;
  int sep_y = row1; 
  int sep_wid = 50;
  int sep_hgt = 20;
  m_but_separate->resize(sep_x, sep_y, sep_wid, sep_hgt);

  int trunc_x = sep_x + sep_wid + 25;
  int trunc_y = row1; 
  int trunc_wid = 50;
  int trunc_hgt = 20;
  m_but_truncate->resize(trunc_x, trunc_y, trunc_wid, trunc_hgt);

  int wrp_x = trunc_x + trunc_wid + 25;
  int wrp_y = row1; 
  int wrp_wid = 50;
  int wrp_hgt = 20;
  m_but_wrapline->resize(wrp_x, wrp_y, wrp_wid, wrp_hgt);

  int fut_x = wrp_x + wrp_wid + 25;
  int fut_y = row1; 
  int fut_wid = 50;
  int fut_hgt = 20;
  m_but_future->resize(fut_x, fut_y, fut_wid, fut_hgt);

  // Row 2: First Grep Field
  m_fld_grep1->resize(40, row2, 95, 20);

  int mgr1_x = sep_x;
  int mgr1_y = row2; 
  int mgr1_wid = 40;
  int mgr1_hgt = 20;
  m_but_mod_grep1->resize(mgr1_x, mgr1_y, mgr1_wid, mgr1_hgt);

  int bag1_x = mgr1_x + mgr1_wid + 10;
  int bag1_y = row2; 
  int bag1_wid = 50;
  int bag1_hgt = 20;
  m_but_apply_grep1->resize(bag1_x, bag1_y, bag1_wid, bag1_hgt);

  int txtm_x = w() - lmarg - 50;
  int txtm_y = row2; 
  int txtm_wid = 50;
  int txtm_hgt = 20;
  m_but_text_more->resize(txtm_x, txtm_y, txtm_wid, txtm_hgt);
  
  // Row 3: Second Grep Field
  m_fld_grep2->resize(40, row3, 95, 20);
  
  int mgr2_x = sep_x;
  int mgr2_y = row3; 
  int mgr2_wid = 40;
  int mgr2_hgt = 20;
  m_but_mod_grep2->resize(mgr2_x, mgr2_y, mgr2_wid, mgr2_hgt);

  int bag2_x = mgr2_x + mgr2_wid + 10;
  int bag2_y = row3; 
  int bag2_wid = 50;
  int bag2_hgt = 20;
  m_but_apply_grep2->resize(bag2_x, bag2_y, bag2_wid, bag2_hgt);

  int txtl_x = w() - lmarg - 50;
  int txtl_y = row3; 
  int txtl_wid = 50;
  int txtl_hgt = 20;
  m_but_text_less->resize(txtl_x, txtl_y, txtl_wid, txtl_hgt);
  
  // Row 4/5: The browser Window(s)
  if(m_alsmodel.getFutureVal() == false) {
    int brw_x = lmarg;
    int brw_y = total_dat_hgt;
    int brw_wid = bwid;
    int brw_hgt = total_brw_hgt;
    m_brw_info1->resize(brw_x, brw_y, brw_wid, brw_hgt); 
    m_brw_info1->bottomline(m_brw_info1->size());
    m_brw_info2->resize(0, 0, 0, 0); 
  }
  else { 
    int brw1_x = lmarg;
    int brw1_y = total_dat_hgt;
    int brw1_wid = bwid;
    int brw1_hgt = total_brw_hgt/2 - 5;
    m_brw_info1->resize(brw1_x, brw1_y, brw1_wid, brw1_hgt); 
    m_brw_info1->bottomline(m_brw_info1->size());
    int brw2_x = lmarg;
    int brw2_y = brw1_y + brw1_hgt + 5;
    int brw2_wid = bwid;
    int brw2_hgt = total_brw_hgt/2 - 5;
    m_brw_info2->resize(brw2_x, brw2_y, brw2_wid, brw2_hgt); 
    m_brw_info2->bottomline(0);
  }
}


//---------------------------------------------------------------------------
// Procedure: resizeWidgetsText()

void GUI_AppLogScope::resizeWidgetsText()
{
  int blab_size = 10; // blab=button_label size

  if(m_mutable_text_size < 9)
    blab_size = 8;
  if(m_mutable_text_size > 14)
    blab_size = 12;

  m_brw_info1->textsize(m_mutable_text_size); 
  m_brw_info1->labelsize(m_mutable_text_size);

  m_brw_info2->textsize(m_mutable_text_size); 
  m_brw_info2->labelsize(m_mutable_text_size);

  m_but_truncate->labelsize(blab_size);
  m_but_separate->labelsize(blab_size);
  m_but_separate->set();

  m_but_wrapline->labelsize(blab_size);
  m_but_future->labelsize(blab_size);
  m_but_apply_grep1->labelsize(blab_size);
  m_but_apply_grep2->labelsize(blab_size);

  m_fld_time->textsize(blab_size); 
  m_fld_time->labelsize(blab_size);

  m_fld_grep1->textsize(blab_size); 
  m_fld_grep1->labelsize(blab_size);

  m_fld_grep2->textsize(blab_size); 
  m_fld_grep2->labelsize(blab_size);

  m_but_mod_grep1->labelsize(blab_size);
  m_but_mod_grep2->labelsize(blab_size);

  m_but_text_more->labelsize(14);
  m_but_text_less->labelsize(9);
}

//-------------------------------------------------------------------

Fl_Menu_Item GUI_AppLogScope::menu_[] = {
 {"Invisible", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Replay/Step+1",  FL_CTRL + ']', (Fl_Callback*)GUI_AppLogScope::cb_Step, (void*)100, 0},
 {"Replay/Step-",   FL_CTRL + '[', (Fl_Callback*)GUI_AppLogScope::cb_Step, (void*)-100, 0},
 {"Replay/Step+1",  ']', (Fl_Callback*)GUI_AppLogScope::cb_Step, (void*)1000, 0},
 {"Replay/Step-",   '[', (Fl_Callback*)GUI_AppLogScope::cb_Step, (void*)-1000, 0},
 {"Replay/Step+5",  '}', (Fl_Callback*)GUI_AppLogScope::cb_Step, (void*)5000, 0},
 {"Replay/Step-5",  '{', (Fl_Callback*)GUI_AppLogScope::cb_Step, (void*)-5000, 0},
 {0}
};

//----------------------------------------------------------
// Procedure: setDataBroker

void GUI_AppLogScope::setDataBroker(ALogDataBroker dbroker, unsigned int alix)
{
  m_dbroker = dbroker;

  AppLogPlot alplot = dbroker.getAppLogPlot(alix);
  
  m_alsmodel.setAppLogPlot(alplot);
  m_vname = dbroker.getVNameFromALix(alix);
}

//----------------------------------------------------------
// Procedure: resize

void GUI_AppLogScope::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgetsShape();
  resizeWidgetsText();
}

//----------------------------------------------------------
// Procedure: handle

int GUI_AppLogScope::handle(int event) 
{
  switch(event) {
  case FL_KEYDOWN:
    if(Fl::event_key() == 32) 
      m_parent_gui->streaming(2);
    else if(Fl::event_key() == 'a') 
      m_parent_gui->streamspeed(true);
    else if(Fl::event_key() == 'z') 
      m_parent_gui->streamspeed(false);
    else if(Fl::event_key() == '=') 
      m_parent_gui->streaming(2);
    else if(Fl::event_key() == 'f') {
      bool future = m_but_future->value();
      m_but_future->value(!future);
      cb_ButtonFuture_i(0);
    }
    else if(Fl::event_key() == 's') {
      bool sep = m_but_separate->value();
      m_but_separate->value(!sep);
      cb_ButtonSeparate_i(0);
    }
    else if(Fl::event_key() == 't') {
      bool trunc = m_but_truncate->value();
      m_but_truncate->value(!trunc);
      cb_ButtonTruncate_i(0);
    }
    else if(Fl::event_key() == 'w') {
      bool wrp = m_but_wrapline->value();
      m_but_wrapline->value(!wrp);
      cb_ButtonWrapLine_i(0);
    }
    else if(Fl::event_key() == '+') 
      updateMutableTextSize("bigger");
    else if(Fl::event_key() == '-') 
      updateMutableTextSize("smaller");
    else if(Fl::event_key() == FL_Left) 
      return(1);
    else if(Fl::event_key() == FL_Right) 
      return(1);
    else if(Fl::event_key() == 'i') 
      return(1);
    else if(Fl::event_key() == 'o') 
      return(1);
    else if(Fl::event_key() == 'h') 
      return(1);
    else if(Fl::event_key() == 'b') 
      return(1);
    else if(Fl::event_key() == FL_Up)
      return(Fl_Window::handle(event));
    else if(Fl::event_key() == FL_Down)
      return(Fl_Window::handle(event));
    else 
      return(Fl_Window::handle(event));
    return(1);
    break;
  case FL_PUSH:
    Fl_Window::handle(event);
    return(1);
    break;
  case FL_RELEASE:
    return(1);
    break;
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------------------------
// Procedure: setCurrTime

void GUI_AppLogScope::setCurrTime(double curr_time)
{
  // Check if there is an actual time difference. It's possible that
  // the time update is coming from the parent gui, prompted by this
  // gui.
  if(m_alsmodel.getCurrTime() == curr_time)
    return;
  m_alsmodel.setTime(curr_time);
  updateBrowsers();
  updateXY();
}


//----------------------------------------- BrowserInfo
inline void GUI_AppLogScope::cb_BrowserInfo_i() {
  //cout << "In cb_BrowserInfo_i()" << endl;
}
void GUI_AppLogScope::cb_BrowserInfo(Fl_Widget* o) {
  ((GUI_AppLogScope*)(o->parent()->user_data()))->cb_BrowserInfo_i();
}

//----------------------------------------- ButtonTruncate
inline void GUI_AppLogScope::cb_ButtonTruncate_i(int v) {
  bool trunc = m_but_truncate->value();
  m_alsmodel.setTruncateVal(trunc);

  updateBrowsers();
}
void GUI_AppLogScope::cb_ButtonTruncate(Fl_Widget* o, int val) {
  ((GUI_AppLogScope*)(o->parent()->user_data()))->cb_ButtonTruncate_i(val);
}

//----------------------------------------- ButtonSeparate
inline void GUI_AppLogScope::cb_ButtonSeparate_i(int v) {
  bool sep = m_but_separate->value();
  m_alsmodel.setShowSeparator(sep);
  updateBrowsers();
}
void GUI_AppLogScope::cb_ButtonSeparate(Fl_Widget* o, int val) {
  ((GUI_AppLogScope*)(o->parent()->user_data()))->cb_ButtonSeparate_i(val);
}

//----------------------------------------- ButtonWrapLine
inline void GUI_AppLogScope::cb_ButtonWrapLine_i(int v) {
  bool wrap = m_but_wrapline->value();
  m_alsmodel.setWrapVal(wrap);

  updateBrowsers();
}
void GUI_AppLogScope::cb_ButtonWrapLine(Fl_Widget* o, int val) {
  ((GUI_AppLogScope*)(o->parent()->user_data()))->cb_ButtonWrapLine_i(val);
}

//----------------------------------------- ButtonFuture
inline void GUI_AppLogScope::cb_ButtonFuture_i(int v) {
  bool future = m_but_future->value();
  m_alsmodel.setFutureVal(future);

  resizeWidgetsShape();
  updateBrowsers();
  updateXY();
}
void GUI_AppLogScope::cb_ButtonFuture(Fl_Widget* o, int val) {
  ((GUI_AppLogScope*)(o->parent()->user_data()))->cb_ButtonFuture_i(val);
}

//----------------------------------------- ButtonApplyGrep
inline void GUI_AppLogScope::cb_ButtonApplyGrep_i(int v) {
  if(v==1) {
    bool agrep = m_but_apply_grep1->value();
    m_alsmodel.setGrepApply1(agrep);
  }
  if(v==2) {
    bool agrep = m_but_apply_grep2->value();
    m_alsmodel.setGrepApply2(agrep);
  }

  updateBrowsers();
}
void GUI_AppLogScope::cb_ButtonApplyGrep(Fl_Widget* o, int val) {
  ((GUI_AppLogScope*)(o->parent()->user_data()))->cb_ButtonApplyGrep_i(val);
}

//----------------------------------------- Step
inline void GUI_AppLogScope::cb_Step_i(int val) {
  if(m_parent_gui)
    m_parent_gui->steptime(val);
  updateBrowsers();
  updateXY();
}

void GUI_AppLogScope::cb_Step(Fl_Widget* o, int val) {
  ((GUI_AppLogScope*)(o->parent()->user_data()))->cb_Step_i(val);
}

//----------------------------------------- ModGrep
inline void GUI_AppLogScope::cb_ModGrep_i(int val) {
  if(val == 1) {
    string curr_grepstr = m_alsmodel.getGrepStr1();
    const char *str = fl_input("Enter grep1 string:", curr_grepstr.c_str());
    if(str != 0) {
      string new_str = stripBlankEnds(str);
      if(!strContainsWhite(new_str)) 
	m_alsmodel.setGrepStr1(new_str);
    }
  }
  if(val == 2) {
    string curr_grepstr = m_alsmodel.getGrepStr2();
    const char *str = fl_input("Enter grep2 string:", curr_grepstr.c_str());
    if(str != 0) {
      string new_str = stripBlankEnds(str);
      if(!strContainsWhite(new_str)) 
	m_alsmodel.setGrepStr2(new_str);
    }
  }
  updateBrowsers();
  updateXY();
}

void GUI_AppLogScope::cb_ModGrep(Fl_Widget* o, int val) {
  ((GUI_AppLogScope*)(o->parent()->user_data()))->cb_ModGrep_i(val);
}

//----------------------------------------- ModText
inline void GUI_AppLogScope::cb_ModText_i(int val) {
  if(val == 1)
    updateMutableTextSize("bigger");
  if(val == 2)
    updateMutableTextSize("smaller");
}

void GUI_AppLogScope::cb_ModText(Fl_Widget* o, int val) {
  ((GUI_AppLogScope*)(o->parent()->user_data()))->cb_ModText_i(val);
}

//----------------------------------------- UpdateXY
void GUI_AppLogScope::updateXY() 
{
  string title = m_vname + " " + m_replay_warp_msg;
  copy_label(title.c_str());

  double vtime_loc = m_alsmodel.getCurrTime();
  string stime_loc = doubleToString(vtime_loc, 3);
  m_fld_time->value(stime_loc.c_str());
  
  string grep_str1 = m_alsmodel.getGrepStr1();
  m_fld_grep1->value(grep_str1.c_str());
  
  string grep_str2 = m_alsmodel.getGrepStr2();
  m_fld_grep2->value(grep_str2.c_str());
  
  redraw();
}


//----------------------------------------------------------
// Procedure: updateMutableTextSize()

void GUI_AppLogScope::updateMutableTextSize(string val) 
{
  if(val == "bigger") {
    if(m_mutable_text_size == 8)
      m_mutable_text_size = 9;
    else if(m_mutable_text_size == 9)
      m_mutable_text_size = 10;
    else if(m_mutable_text_size == 10)
      m_mutable_text_size = 12;
    else if(m_mutable_text_size == 12)
      m_mutable_text_size = 14;
    else if(m_mutable_text_size == 14)
      m_mutable_text_size = 16;
    else if(m_mutable_text_size == 16)
      m_mutable_text_size = 18;
  }
  else if(val == "smaller") {
    if(m_mutable_text_size == 18)
      m_mutable_text_size = 16;
    else if(m_mutable_text_size == 16)
      m_mutable_text_size = 14;
    else if(m_mutable_text_size == 14)
      m_mutable_text_size = 12;
    else if(m_mutable_text_size == 12)
      m_mutable_text_size = 10;
    else if(m_mutable_text_size == 10)
      m_mutable_text_size = 9;
    else if(m_mutable_text_size == 9)
      m_mutable_text_size = 8;
  }
  else
    return;

  resizeWidgetsText();
  updateBrowsers();
  updateXY();
  redraw();
}

//----------------------------------------- UpdateBrowsers
void GUI_AppLogScope::updateBrowsers()
{
  m_brw_info1->clear();
  m_brw_info2->clear();

  updateBrowser1();
  if(m_alsmodel.getFutureVal())
    updateBrowser2();
}

//----------------------------------------- UpdateBrowser1
void GUI_AppLogScope::updateBrowser1()
{
  vector<string> pvector = m_alsmodel.getLinesUpToNow();
  for(unsigned i=0; i<pvector.size(); i++) 
      m_brw_info1->add(pvector[i].c_str());

  m_brw_info1->bottomline(m_brw_info1->size());
}

//----------------------------------------- UpdateBrowser2
void GUI_AppLogScope::updateBrowser2()
{
  vector<string> pvector = m_alsmodel.getLinesPastNow();
  for(unsigned i=0; i<pvector.size(); i++) 
      m_brw_info2->add(pvector[i].c_str());

  m_brw_info2->bottomline(0);
  //m_brw_info2->bottomline(m_brw_info1->size());
}


