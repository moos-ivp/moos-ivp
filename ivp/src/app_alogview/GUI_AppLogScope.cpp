/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
  this->size_range(450,350);

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
  if(m_fld_time)      
    delete(m_fld_time);
  if(m_but_truncate)  
    delete(m_but_truncate);
  if(m_but_separate)  
    delete(m_but_separate);
  if(m_brw_info)
    delete(m_brw_info);
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
  //Fl_Color bcolor_gray = fl_rgb_color(200, 220, 200); // light blueish gray

  m_brw_info = new Fl_Browser(0, 0, 1, 1); 
  m_brw_info->align(FL_ALIGN_LEFT);
  m_brw_info->color(fcolor2);
  m_brw_info->tooltip("Terminal Output");
  m_brw_info->callback((Fl_Callback*)GUI_AppLogScope::cb_BrowserInfo);
  m_brw_info->when(FL_WHEN_RELEASE);
  m_brw_info->textfont(FL_COURIER_BOLD);

  m_but_truncate = new Fl_Check_Button(0, 0, 0, 0, "Truncate");
  m_but_truncate->labelcolor(bcolor);
  m_but_truncate->callback((Fl_Callback*)GUI_AppLogScope::cb_ButtonTruncate, (void*)0);
  m_but_truncate->clear_visible_focus();
  m_but_truncate->value(0);

  m_but_separate = new Fl_Check_Button(0, 0, 0, 0, "separate");
  m_but_separate->labelcolor(bcolor);
  m_but_separate->callback((Fl_Callback*)GUI_AppLogScope::cb_ButtonSeparate, (void*)0);
  m_but_separate->clear_visible_focus();
  m_but_separate->value(0);

  m_but_wrapline = new Fl_Check_Button(0, 0, 0, 0, "wrapline");
  m_but_wrapline->labelcolor(bcolor);
  m_but_wrapline->callback((Fl_Callback*)GUI_AppLogScope::cb_ButtonWrapLine, (void*)0);
  m_but_wrapline->clear_visible_focus();
  m_but_wrapline->value(0);

  m_fld_time = new Fl_Output(0, 0, 0, 0, "Time:"); 
  m_fld_time->clear_visible_focus();
}

//---------------------------------------------------------------------------
// Procedure: resizeWidgetsShape()

void GUI_AppLogScope::resizeWidgetsShape()
{
  // General params
  int lmarg = 10;
  int rmarg = 10;
  int bwid  = w() - (lmarg + rmarg);
  int total_dat_hgt = 70;
  int total_brw_hgt = h() - total_dat_hgt;

  // Part 1: Set top row: time, add, set, remove buttons
  m_fld_time->resize(40, 5, 95, 20);

  int trunc_x = 150;
  int trunc_y = 5; 
  int trunc_wid = ((w()-155) / 3) - 5;
  int trunc_hgt = 20;
  m_but_truncate->resize(trunc_x, trunc_y, trunc_wid, trunc_hgt);

  int sep_x = 150;
  int sep_y = 30; 
  int sep_wid = 50;
  int sep_hgt = 20;
  m_but_separate->resize(sep_x, sep_y, sep_wid, sep_hgt);

  int wrp_x = sep_x + sep_wid + 30;
  int wrp_y = 30; 
  int wrp_wid = 50;
  int wrp_hgt = 20;
  m_but_wrapline->resize(wrp_x, wrp_y, wrp_wid, wrp_hgt);

  int y_info = total_dat_hgt;
  int h_info = total_brw_hgt;
  m_brw_info->resize(lmarg, y_info, bwid, h_info); 
  m_brw_info->bottomline(m_brw_info->size());
}

//---------------------------------------------------------------------------
// Procedure: resizeWidgetsText()

void GUI_AppLogScope::resizeWidgetsText()
{
  int blab_size = 12; // blab=button_label size

  m_brw_info->textsize(m_mutable_text_size); 
  m_brw_info->labelsize(m_mutable_text_size);

  m_but_truncate->labelsize(blab_size);
  m_but_separate->labelsize(blab_size);
  m_but_wrapline->labelsize(blab_size);

  m_fld_time->textsize(m_mutable_text_size); 
  m_fld_time->labelsize(m_mutable_text_size);
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
  updateBrowser();
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

  updateBrowser();
}
void GUI_AppLogScope::cb_ButtonTruncate(Fl_Widget* o, int val) {
  ((GUI_AppLogScope*)(o->parent()->user_data()))->cb_ButtonTruncate_i(val);
}

//----------------------------------------- ButtonSeparate
inline void GUI_AppLogScope::cb_ButtonSeparate_i(int v) {
  bool sep = m_but_separate->value();
  m_alsmodel.setShowSeparator(sep);

  updateBrowser();
}
void GUI_AppLogScope::cb_ButtonSeparate(Fl_Widget* o, int val) {
  ((GUI_AppLogScope*)(o->parent()->user_data()))->cb_ButtonSeparate_i(val);
}

//----------------------------------------- ButtonWrapLine
inline void GUI_AppLogScope::cb_ButtonWrapLine_i(int v) {
  bool wrap = m_but_wrapline->value();
  m_alsmodel.setWrapVal(wrap);

  updateBrowser();
}
void GUI_AppLogScope::cb_ButtonWrapLine(Fl_Widget* o, int val) {
  ((GUI_AppLogScope*)(o->parent()->user_data()))->cb_ButtonWrapLine_i(val);
}

//----------------------------------------- Step
inline void GUI_AppLogScope::cb_Step_i(int val) {
  if(m_parent_gui)
    m_parent_gui->steptime(val);
  updateBrowser();
  updateXY();
}

void GUI_AppLogScope::cb_Step(Fl_Widget* o, int val) {
  ((GUI_AppLogScope*)(o->parent()->user_data()))->cb_Step_i(val);
}

//----------------------------------------- UpdateXY
void GUI_AppLogScope::updateXY() 
{
  string title = m_vname + " " + m_replay_warp_msg;
  copy_label(title.c_str());

  double vtime_loc = m_alsmodel.getCurrTime();
  string stime_loc = doubleToString(vtime_loc, 3);
  m_fld_time->value(stime_loc.c_str());

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
  updateBrowser();
}


//----------------------------------------- UpdateBrowser
void GUI_AppLogScope::updateBrowser()
{
  m_brw_info->clear();

  bool show_sep = m_alsmodel.getShowSeparator();
  
  vector<string> pvector = m_alsmodel.getLinesUpToNow(show_sep);
  for(unsigned i=0; i<pvector.size(); i++) 
      m_brw_info->add(pvector[i].c_str());

  m_brw_info->bottomline(m_brw_info->size());
}

