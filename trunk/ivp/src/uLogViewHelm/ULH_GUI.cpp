/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ULH_GUI.cpp                                          */
/*    DATE: Oct0411                                              */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "ULH_GUI.h"
#include "MBUtils.h"
#include "FL/fl_ask.H"

using namespace std;

//-------------------------------------------------------------------
// Constructor

ULH_GUI::ULH_GUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) 
{
  mbar = new Fl_Menu_Bar(0, 0, 0, 0);
  mbar->menu(menu_);

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  m_fld_utc_time  = 0;
  m_fld_loc_time  = 0;
  m_fld_mode      = 0;
  m_fld_iter      = 0;
  m_fld_decision  = 0;
  m_fld_engaged   = 0;
  m_brw_active    = 0;
  m_brw_running   = 0;
  m_brw_idle      = 0;
  m_brw_completed = 0;
  m_brw_general   = 0;

  m_but_gen_errors = 0;
  m_but_gen_warnings = 0;
  m_but_gen_modetree = 0;

  m_cnt_active    = 0;
  m_cnt_running   = 0;
  m_cnt_idle      = 0;
  m_cnt_completed = 0;

  setWindowLayout("normal");

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------

Fl_Menu_Item ULH_GUI::menu_[] = {
 {"Invisible", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Replay/Step+1",  ']', (Fl_Callback*)ULH_GUI::cb_Step, (void*)1, 0},
 {"Replay/Step-",   '[', (Fl_Callback*)ULH_GUI::cb_Step, (void*)-1, 0},
 {"BrowserHeader",  'h', (Fl_Callback*)ULH_GUI::cb_BrowserHeader},
 {0}
};


//-------------------------------------------------------------------
// Destructor

ULH_GUI::~ULH_GUI()
{
  if(m_fld_utc_time)  delete(m_fld_utc_time);
  if(m_fld_mode)      delete(m_fld_mode);
  if(m_fld_decision)  delete(m_fld_decision);
  if(m_fld_engaged)   delete(m_fld_engaged);
  //if(m_fld_)  delete(m_fld_);
}

//----------------------------------------------------------
// Procedure: handle

int ULH_GUI::handle(int event) 
{
  switch(event) {
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

//-------------------------------------------------------------------
// Procedure: setWindowLayout()

void ULH_GUI::setWindowLayout(string layout)
{
  int info_size = 10;

  Fl_Color fcolor1 = fl_rgb_color(200, 120, 120);
  Fl_Color fcolor2 = fl_rgb_color(190, 255, 190); // green
  Fl_Color fcolor3 = fl_rgb_color(245, 245, 170); // yellow
  Fl_Color fcolor4 = fl_rgb_color(225, 170, 170); // red

  if(!m_brw_active) {
    m_brw_active = new Fl_Browser(60, 75, 400, 80, "Active\n Behaviors:"); 
    m_brw_active->align(FL_ALIGN_LEFT);
    m_brw_active->color(fcolor2);
    m_brw_active->textsize(info_size); 
    m_brw_active->labelsize(info_size);
    m_brw_active->tooltip("List of Active Behaviors");
    m_brw_active->callback((Fl_Callback*)ULH_GUI::cb_Browser);
    m_brw_active->when(FL_WHEN_RELEASE);
    m_brw_active->textfont(FL_COURIER_BOLD);
  }
  if(!m_brw_running) {
    m_brw_running = new Fl_Browser(60, 165, 400, 50, "Running\n Behaviors:"); 
    m_brw_running->align(FL_ALIGN_LEFT);
    m_brw_running->color(fcolor3);
    m_brw_running->textsize(info_size); 
    m_brw_running->labelsize(info_size);
    m_brw_running->tooltip("List of Active Behaviors");
    m_brw_running->callback((Fl_Callback*)ULH_GUI::cb_Browser);
    m_brw_running->when(FL_WHEN_RELEASE);
    m_brw_running->textfont(FL_COURIER_BOLD);
  }
  if(!m_brw_idle) {
    m_brw_idle = new Fl_Browser(60, 225, 400, 80, "Idle\n Behaviors:"); 
    m_brw_idle->align(FL_ALIGN_LEFT);
    m_brw_idle->color(fcolor4);
    m_brw_idle->textsize(info_size); 
    m_brw_idle->labelsize(info_size);
    m_brw_idle->tooltip("List of Active Behaviors");
    m_brw_idle->callback((Fl_Callback*)ULH_GUI::cb_Browser);
    m_brw_idle->when(FL_WHEN_RELEASE);
    m_brw_idle->textfont(FL_COURIER_BOLD);
  }
  if(!m_brw_completed) {
    m_brw_completed = new Fl_Browser(60, 315, 400, 50, "Completed\n Behaviors:"); 
    m_brw_completed->align(FL_ALIGN_LEFT);
    m_brw_completed->textsize(info_size); 
    m_brw_completed->labelsize(info_size);
    m_brw_completed->tooltip("List of Active Behaviors");
    m_brw_completed->callback((Fl_Callback*)ULH_GUI::cb_Browser);
    m_brw_completed->when(FL_WHEN_RELEASE);
    m_brw_completed->textfont(FL_COURIER_BOLD);
  }

  if(!m_brw_general) {
    m_brw_general = new Fl_Browser(60, 400, 400, 80, "Behavior\n Errors:"); 
    m_brw_general->align(FL_ALIGN_LEFT);
    m_brw_general->textsize(info_size); 
    m_brw_general->labelsize(info_size);
    m_brw_general->tooltip("List of Behavior Errors");
    m_brw_general->callback((Fl_Callback*)ULH_GUI::cb_Browser);
    m_brw_general->when(FL_WHEN_RELEASE);
    m_brw_general->textfont(FL_COURIER_BOLD);
  }

  Fl_Color bcolor = fl_rgb_color(0, 0, 120); // dark blue
  if(!m_but_gen_errors) {
    m_but_gen_errors = new Fl_Check_Button(60, 380, 20, 20, "Errors");
    m_but_gen_errors->labelsize(12);
    m_but_gen_errors->labelcolor(bcolor); 
    m_but_gen_errors->callback((Fl_Callback*)ULH_GUI::cb_ButtonErrors);
  }

  if(!m_but_gen_warnings) {
    m_but_gen_warnings = new Fl_Check_Button(120, 380, 20, 20, "Warnings");
    m_but_gen_warnings->labelcolor(bcolor); 
    m_but_gen_warnings->labelsize(12);
    m_but_gen_warnings->callback((Fl_Callback*)ULH_GUI::cb_ButtonWarnings);
  }

  if(!m_but_gen_modetree) {
    m_but_gen_modetree = new Fl_Check_Button(200, 380, 20, 20, "ModeTree");
    m_but_gen_modetree->labelcolor(bcolor); 
    m_but_gen_modetree->labelsize(12);
    m_but_gen_modetree->callback((Fl_Callback*)ULH_GUI::cb_ButtonModeTree);
  }


  if(!m_fld_utc_time) {
    m_fld_utc_time = new MY_Output(60, 5, 95, 20, "Time:"); 
    m_fld_utc_time->textsize(info_size); 
    m_fld_utc_time->labelsize(info_size);
  }

  if(!m_fld_loc_time) {
    m_fld_loc_time = new MY_Output(160, 5, 60, 20); 
    m_fld_loc_time->textsize(info_size); 
    m_fld_loc_time->labelsize(info_size);
  }

  if(!m_fld_iter) {
    m_fld_iter = new MY_Output(410, 5, 50, 20, "Iter:"); 
    m_fld_iter->textsize(info_size); 
    m_fld_iter->color(fcolor1); 
    m_fld_iter->labelsize(info_size);
  }

  if(!m_fld_mode) {
    m_fld_mode = new MY_Output(60, 35, 160, 20, "Mode:"); 
    m_fld_mode->textsize(info_size); 
    m_fld_mode->labelsize(info_size);
  }

  if(!m_fld_decision) {
    m_fld_decision = new MY_Output(260, 35, 200, 20, "Dec:"); 
    m_fld_decision->textsize(info_size); 
    m_fld_decision->labelsize(info_size);
  }

  if(!m_fld_engaged) {
    m_fld_engaged = new MY_Output(260, 5, 120, 20, "Eng:"); 
    m_fld_engaged->textsize(info_size); 
    m_fld_engaged->labelsize(info_size);
  }
}

//----------------------------------------------------------
// Procedure: setHelmPlot

void ULH_GUI::setHelmPlot(const HelmTPlot& helm_plot)
{
  string vname  = helm_plot.getVName();
  if(m_vname == "")
    m_vname = vname;

  if(m_vname != vname)
    return;

  hsm.setHelmPlot(helm_plot);    
}

//----------------------------------------------------------
// Procedure: setCurrTime

void ULH_GUI::setCurrTime(double curr_time)
{
  hsm.setTime(curr_time);
  updateBrowsers();
  updateXY();
}


//----------------------------------------- Browser
inline void ULH_GUI::cb_Browser_i() {
  cout << "In cb_Browser_i()" << endl;
}
void ULH_GUI::cb_Browser(Fl_Widget* o) {
  ((ULH_GUI*)(o->parent()->user_data()))->cb_Browser_i();
}

//----------------------------------------- BrowserHeader
inline void ULH_GUI::cb_BrowserHeader_i() {
  cout << "In cb_BrowserHeader_i()" << endl;
  hsm.toggleHeadersBHV();
  updateBrowsers();
}
void ULH_GUI::cb_BrowserHeader(Fl_Widget* o) {
  ((ULH_GUI*)(o->parent()->user_data()))->cb_BrowserHeader_i();
}

//----------------------------------------- ButtonErrors
inline void ULH_GUI::cb_ButtonErrors_i() {
  cout << "In cb_ButtonErrors_i()" << endl;
  if(m_but_gen_errors->value()) {
    m_but_gen_warnings->value(0);
    m_but_gen_modetree->value(0);
  }
}
void ULH_GUI::cb_ButtonErrors(Fl_Widget* o) {
  ((ULH_GUI*)(o->parent()->user_data()))->cb_ButtonErrors_i();
}

//----------------------------------------- ButtonWarnings
inline void ULH_GUI::cb_ButtonWarnings_i() {
  cout << "In cb_ButtonWarnings_i()" << endl;
  if(m_but_gen_warnings->value()) {
    m_but_gen_errors->value(0);
    m_but_gen_modetree->value(0);
  }
}
void ULH_GUI::cb_ButtonWarnings(Fl_Widget* o) {
  ((ULH_GUI*)(o->parent()->user_data()))->cb_ButtonWarnings_i();
}

//----------------------------------------- ButtonModeTree
inline void ULH_GUI::cb_ButtonModeTree_i() {
  cout << "In cb_ButtonModeTree_i()" << endl;
  if(m_but_gen_modetree->value()) {
    m_but_gen_errors->value(0);
    m_but_gen_warnings->value(0);
  }
}
void ULH_GUI::cb_ButtonModeTree(Fl_Widget* o) {
  ((ULH_GUI*)(o->parent()->user_data()))->cb_ButtonModeTree_i();
}

//----------------------------------------- Step
inline void ULH_GUI::cb_Step_i(int val) {
  hsm.incrementIter(val);
  updateBrowsers();
  updateXY();
}

void ULH_GUI::cb_Step(Fl_Widget* o, int val) {
  ((ULH_GUI*)(o->parent()->user_data()))->cb_Step_i(val);
}

//----------------------------------------- UpdateXY
void ULH_GUI::updateXY() 
{
  // IPF fields
  double vtime_utc = hsm.getCurrTimeUTC();
  string stime_utc = doubleToString(vtime_utc, 2);
  m_fld_utc_time->value(stime_utc.c_str());

  double vtime_loc  = hsm.getCurrTime();
  string stime_loc = doubleToString(vtime_loc, 2);
  m_fld_loc_time->value(stime_loc.c_str());

  string mode = hsm.getCurrMode();
  m_fld_mode->value(mode.c_str());

  string iter = hsm.getCurrIter();
  m_fld_iter->value(iter.c_str());

  string dec = hsm.getCurrDecision();
  m_fld_decision->value(dec.c_str());

  string eng = hsm.getCurrEngaged();
  m_fld_engaged->value(eng.c_str());

  string label_active = "Active\nBehaviors\n(";
  label_active += uintToString(m_cnt_active) + ")";
  m_brw_active->copy_label(label_active.c_str());

  string label_running = "Running\nBehaviors\n(";
  label_running += uintToString(m_cnt_running) + ")";
  m_brw_running->copy_label(label_running.c_str());

  string label_idle = "Idle\nBehaviors\n(";
  label_idle += uintToString(m_cnt_idle) + ")";
  m_brw_idle->copy_label(label_idle.c_str());

  string label_completed = "Completed\nBehaviors\n(";
  label_completed += uintToString(m_cnt_completed) + ")";
  m_brw_completed->copy_label(label_completed.c_str());
}


//----------------------------------------- UpdateBrowsers
void ULH_GUI::updateBrowsers()
{
  unsigned int header_lines = 0;
  if(hsm.getHeadersBHV())
    header_lines = 2;
  if(m_brw_active) {
    m_brw_active->clear();
    const vector<string>& svector = hsm.getActiveList();
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++)
      m_brw_active->add(svector[i].c_str());
    m_cnt_active = vsize;
    if(m_cnt_active >= header_lines)
      m_cnt_active -= header_lines;
  }

  if(m_brw_running) {
    m_brw_running->clear();
    const vector<string>& svector = hsm.getNonActiveList("running");
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++)
      m_brw_running->add(svector[i].c_str());
    m_cnt_running = vsize;
    if(m_cnt_running >= header_lines)
      m_cnt_running -= header_lines;
  }

  if(m_brw_idle) {
    m_brw_idle->clear();
    const vector<string>& svector = hsm.getNonActiveList("idle");
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++)
      m_brw_idle->add(svector[i].c_str());
    m_cnt_idle = vsize;
    if(m_cnt_idle >= header_lines)
      m_cnt_idle -= header_lines;
  }

  if(m_brw_completed) {
    m_brw_completed->clear();
    const vector<string>& svector = hsm.getNonActiveList("completed");
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++)
      m_brw_completed->add(svector[i].c_str());
    m_cnt_completed = vsize;
    if(m_cnt_completed >= header_lines)
      m_cnt_completed -= header_lines;
  }

  if(m_brw_general) {
    m_brw_general->clear();
    const vector<string>& svector = hsm.getWarningsList();
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++)
      m_brw_general->add(svector[(vsize-1)-i].c_str());
    m_cnt_general = vsize;
    if(m_cnt_general >= header_lines)
      m_cnt_general -= header_lines;
    m_brw_general->bottomline(m_brw_general->size()-1);

  }
}

