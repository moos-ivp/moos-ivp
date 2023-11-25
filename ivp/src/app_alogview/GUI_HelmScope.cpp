/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: GUI_HelmScope.cpp                                    */
/*    DATE: Oct 4th 2011                                         */
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

#include <iostream>
#include "GUI_HelmScope.h"
#include "REPLAY_GUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

GUI_HelmScope::GUI_HelmScope(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) 
{
  mbar = new Fl_Menu_Bar(0, 0, 0, 0);
  mbar->menu(menu_);

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();
  this->size_range(550,450);

  m_mutable_text_size = 10;

  initWidgets();
  m_replay_warp_msg = "(PAUSED)";
  m_gen_browser_mode = "warnings";
  m_but_gen_warnings->value(1);
  
  resizeWidgetsShape();
  resizeWidgetsText();

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
// Destructor: Must carefully implement since master GUI may create
//             and destroy instances of this GUI all in same session

GUI_HelmScope::~GUI_HelmScope()
{
  if(m_fld_time)      
    delete(m_fld_time);
  if(m_fld_mode)      
    delete(m_fld_mode);
  if(m_fld_iter)  
    delete(m_fld_iter);
  if(m_fld_decision)  
    delete(m_fld_decision);
  if(m_fld_state)     
    delete(m_fld_state);

  if(m_but_gen_errors)  
    delete(m_but_gen_errors);
  if(m_but_gen_warnings)  
    delete(m_but_gen_warnings);
  if(m_but_gen_modetree)  
    delete(m_but_gen_modetree);
  if(m_but_gen_levents)  
    delete(m_but_gen_levents);

  if(m_brw_active)  
    delete(m_brw_active);
  if(m_brw_running)  
    delete(m_brw_running);
  if(m_brw_idle)  
    delete(m_brw_idle);
  if(m_brw_completed)  
    delete(m_brw_completed);
  if(m_brw_general)  
    delete(m_brw_general);
}

//---------------------------------------------------------------------------
// Procedure: initWidgets()

void GUI_HelmScope::initWidgets()
{
  Fl_Color fcolor1 = fl_rgb_color(200, 120, 120);
  Fl_Color fcolor2 = fl_rgb_color(190, 255, 190); // green
  Fl_Color fcolor3 = fl_rgb_color(245, 245, 170); // yellow
  Fl_Color fcolor4 = fl_rgb_color(225, 170, 170); // red
  Fl_Color bcolor  = fl_rgb_color(0, 0, 120);     // dark blue

  m_brw_active = new Fl_Browser(0, 0, 1, 1, "Active\n Behaviors:"); 
  m_brw_active->align(FL_ALIGN_LEFT);
  m_brw_active->color(fcolor2);
  m_brw_active->tooltip("List of Active Behaviors");
  m_brw_active->callback((Fl_Callback*)GUI_HelmScope::cb_Browser);
  m_brw_active->when(FL_WHEN_RELEASE);
  m_brw_active->textfont(FL_COURIER_BOLD);

  m_brw_running = new Fl_Browser(0, 0, 1, 1, "Running\n Behaviors:"); 
  m_brw_running->align(FL_ALIGN_LEFT);
  m_brw_running->color(fcolor3);
  m_brw_running->tooltip("List of Running (but not active) Behaviors");
  m_brw_running->callback((Fl_Callback*)GUI_HelmScope::cb_Browser);
  m_brw_running->when(FL_WHEN_RELEASE);
  m_brw_running->textfont(FL_COURIER_BOLD);

  m_brw_idle = new Fl_Browser(0, 0, 1, 1, "Idle\n Behaviors:"); 
  m_brw_idle->align(FL_ALIGN_LEFT);
  m_brw_idle->color(fcolor4);
  m_brw_idle->tooltip("List of Idle Behaviors");
  m_brw_idle->callback((Fl_Callback*)GUI_HelmScope::cb_Browser);
  m_brw_idle->when(FL_WHEN_RELEASE);
  m_brw_idle->textfont(FL_COURIER_BOLD);

  m_brw_completed = new Fl_Browser(0, 0, 1, 1, "Completed\n Behaviors:"); 
  m_brw_completed->align(FL_ALIGN_LEFT);
  m_brw_completed->tooltip("List of Completed Behaviors");
  m_brw_completed->callback((Fl_Callback*)GUI_HelmScope::cb_Browser);
  m_brw_completed->when(FL_WHEN_RELEASE);
  m_brw_completed->textfont(FL_COURIER_BOLD);

  m_brw_general = new Fl_Browser(0, 0, 1, 1, "Behavior \n Warnings:"); 
  m_brw_general->align(FL_ALIGN_LEFT);
  m_brw_general->tooltip("Configurable List of Data");
  m_brw_general->callback((Fl_Callback*)GUI_HelmScope::cb_Browser);
  m_brw_general->when(FL_WHEN_RELEASE);
  m_brw_general->textfont(FL_COURIER_BOLD);
  
  m_but_gen_warnings = new Fl_Check_Button(0, 0, 0, 0, "Warnings");
  m_but_gen_warnings->labelcolor(bcolor);
  m_but_gen_warnings->callback((Fl_Callback*)GUI_HelmScope::cb_ButtonWarnings);

  m_but_gen_errors = new Fl_Check_Button(0, 0, 0, 0, "Errors");
  m_but_gen_errors->labelcolor(bcolor);
  m_but_gen_errors->callback((Fl_Callback*)GUI_HelmScope::cb_ButtonErrors);

  m_but_gen_modetree = new Fl_Check_Button(0, 0, 0, 0, "ModeHist");
  m_but_gen_modetree->labelcolor(bcolor);
  m_but_gen_modetree->callback((Fl_Callback*)GUI_HelmScope::cb_ButtonModeTree);

  m_but_gen_levents = new Fl_Check_Button(0, 0, 0, 0, "LifeEvents");
  m_but_gen_levents->labelcolor(bcolor);
  m_but_gen_levents->callback((Fl_Callback*)GUI_HelmScope::cb_ButtonLifeEvents);

  m_fld_time = new Fl_Output(0, 0, 0, 0, "Time:"); 
  m_fld_time->clear_visible_focus();

  m_fld_mode = new Fl_Output(0, 0, 0, 0, "Mode:"); 
  m_fld_mode->clear_visible_focus();

  m_fld_decision = new Fl_Output(0, 0, 0, 0, "Dec:"); 
  m_fld_decision->clear_visible_focus();

  m_fld_state = new Fl_Output(0, 0, 0, 0, "State:"); 
  m_fld_state->clear_visible_focus();

  m_fld_iter = new Fl_Output(0, 0, 0, 0, "Iter:"); 
  m_fld_iter->clear_visible_focus();
  m_fld_iter->color(fcolor1); 
}

//---------------------------------------------------------------------------
// Procedure: resizeWidgetsShape()

void GUI_HelmScope::resizeWidgetsShape()
{
  int lmarg = 60;
  int rmarg = 10;
  int bwid  = w() - (lmarg + rmarg);

  int total_dat_hgt = 70;
  int total_brw_hgt = h() - total_dat_hgt;
  
  double pct_act = 0.20;
  double pct_run = 0.20;
  double pct_idl = 0.20;
  double pct_cmp = 0.20;
  double pct_gen = 0.20;
  
  double sep = 10;

  int y_act = total_dat_hgt;
  int h_act = pct_act * total_brw_hgt;

  int y_run = y_act + h_act;
  int h_run = pct_run * total_brw_hgt;

  int y_idl = y_run + h_run;
  int h_idl = pct_idl * total_brw_hgt;

  int y_cmp = y_idl + h_idl;
  int h_cmp = pct_cmp * total_brw_hgt;

  int y_gen = y_cmp + h_cmp;
  int h_gen = pct_gen * total_brw_hgt;

  m_brw_active->resize(lmarg, y_act, bwid, h_act-sep); 
  m_brw_running->resize(lmarg, y_run, bwid, h_run-sep);
  m_brw_idle->resize(lmarg, y_idl, bwid, h_idl-sep);
  m_brw_completed->resize(lmarg, y_cmp, bwid, h_cmp-(3*sep));
  m_brw_general->resize(lmarg, y_gen, bwid, h_gen-sep);

  m_but_gen_warnings->resize(60, y_gen-25, 20, 20);
  m_but_gen_errors->resize(200, y_gen-25, 20, 20);
  m_but_gen_modetree->resize(320, y_gen-25, 20, 20);
  m_but_gen_levents->resize(440, y_gen-25, 20, 20);

  m_fld_time->resize(60, 5, 95, 20);
  m_fld_iter->resize(410, 5, 50, 20);
  m_fld_mode->resize(60, 35, 160, 20);
  m_fld_decision->resize(260, 35, 200, 20);
  m_fld_state->resize(260, 5, 120, 20);

  m_fld_time->redraw();
  m_fld_iter->redraw();
  m_fld_mode->redraw();
  m_fld_decision->redraw();
  m_fld_state->redraw();
}

//---------------------------------------------------------------------------
// Procedure: resizeWidgetsText()

void GUI_HelmScope::resizeWidgetsText()
{
  int info_size = 10;
  int blab_size = 12; // blab=button_label size

  m_brw_active->textsize(m_mutable_text_size); 
  m_brw_active->labelsize(info_size);

  m_brw_running->textsize(m_mutable_text_size); 
  m_brw_running->labelsize(info_size);

  m_brw_idle->textsize(m_mutable_text_size); 
  m_brw_idle->labelsize(info_size);

  m_brw_completed->textsize(m_mutable_text_size); 
  m_brw_completed->labelsize(info_size);

  m_brw_general->textsize(m_mutable_text_size); 
  m_brw_general->labelsize(info_size);

  m_but_gen_warnings->labelsize(blab_size);
  m_but_gen_errors->labelsize(blab_size);
  m_but_gen_modetree->labelsize(blab_size);
  m_but_gen_levents->labelsize(blab_size);

  m_fld_time->textsize(info_size); 
  m_fld_time->labelsize(info_size);
  m_fld_iter->textsize(info_size); 
  m_fld_iter->labelsize(info_size);
  m_fld_mode->textsize(info_size); 
  m_fld_mode->labelsize(info_size);
  m_fld_decision->textsize(info_size); 
  m_fld_decision->labelsize(info_size);
  m_fld_state->textsize(info_size); 
  m_fld_state->labelsize(info_size);
}

//-------------------------------------------------------------------

Fl_Menu_Item GUI_HelmScope::menu_[] = {
 {"Invisible", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Replay/Step+1",  FL_CTRL + ']', (Fl_Callback*)GUI_HelmScope::cb_Step, (void*)100, 0},
 {"Replay/Step-",   FL_CTRL + '[', (Fl_Callback*)GUI_HelmScope::cb_Step, (void*)-100, 0},
 {"Replay/Step+1",  ']', (Fl_Callback*)GUI_HelmScope::cb_Step, (void*)1000, 0},
 {"Replay/Step-",   '[', (Fl_Callback*)GUI_HelmScope::cb_Step, (void*)-1000, 0},
 {"Replay/Step+5",  '}', (Fl_Callback*)GUI_HelmScope::cb_Step, (void*)5000, 0},
 {"Replay/Step-5",  '{', (Fl_Callback*)GUI_HelmScope::cb_Step, (void*)-5000, 0},
 {"BrowserHeader",  'h', (Fl_Callback*)GUI_HelmScope::cb_BrowserHeader},
 {0}
};

//----------------------------------------------------------
// Procedure: setDataBroker

void GUI_HelmScope::setDataBroker(ALogDataBroker dbroker, string vname)
{
  m_dbroker = dbroker;
  m_vname   = vname;

  m_hsmodel.setDataBroker(dbroker, vname);
}

//----------------------------------------------------------
// Procedure: resize

void GUI_HelmScope::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgetsShape();
  resizeWidgetsText();
}

//----------------------------------------------------------
// Procedure: handle

int GUI_HelmScope::handle(int event) 
{
  switch(event) {
  case FL_KEYDOWN:
    if(Fl::event_key() == 32) 
      m_parent_gui->streaming(2);
    else if(Fl::event_key() == 'a') 
      m_parent_gui->streamspeed(true);
    else if(Fl::event_key() == 'z') 
      m_parent_gui->streamspeed(false);
    else if(Fl::event_key() == '+') 
      updateMutableTextSize("bigger");
    else if(Fl::event_key() == '-') 
      updateMutableTextSize("smaller");
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

void GUI_HelmScope::setCurrTime(double curr_time)
{
  m_hsmodel.setTime(curr_time);
  updateTopBrowsers();
  updateBotBrowser();
  updateXY();
}


//----------------------------------------- Browser
inline void GUI_HelmScope::cb_Browser_i() {
  cout << "In cb_Browser_i()" << endl;
}
void GUI_HelmScope::cb_Browser(Fl_Widget* o) {
  ((GUI_HelmScope*)(o->parent()->user_data()))->cb_Browser_i();
}

//----------------------------------------- BrowserHeader
inline void GUI_HelmScope::cb_BrowserHeader_i() {
  cout << "In cb_BrowserHeader_i()" << endl;
  m_hsmodel.toggleHeadersBHV();
  updateTopBrowsers();
  updateBotBrowser();
}
void GUI_HelmScope::cb_BrowserHeader(Fl_Widget* o) {
  ((GUI_HelmScope*)(o->parent()->user_data()))->cb_BrowserHeader_i();
}

//----------------------------------------- ButtonWarnings
inline void GUI_HelmScope::cb_ButtonWarnings_i() {
  if(m_but_gen_warnings->value()) {
    m_but_gen_errors->value(0);
    m_but_gen_modetree->value(0);
    m_but_gen_levents->value(0);
  }
  else
    m_but_gen_errors->value(1);
  
  m_brw_general->label("Behavior \n Warnings:"); 
  updateBotBrowser();
}
void GUI_HelmScope::cb_ButtonWarnings(Fl_Widget* o) {
  ((GUI_HelmScope*)(o->parent()->user_data()))->cb_ButtonWarnings_i();
}

//----------------------------------------- ButtonErrors
inline void GUI_HelmScope::cb_ButtonErrors_i() {
  if(m_but_gen_errors->value()) {
    m_but_gen_warnings->value(0);
    m_but_gen_modetree->value(0);
    m_but_gen_levents->value(0);
  }
  else
    m_but_gen_warnings->value(1);

  m_brw_general->label("Behavior \n Errors:"); 
  updateBotBrowser();
}
void GUI_HelmScope::cb_ButtonErrors(Fl_Widget* o) {
  ((GUI_HelmScope*)(o->parent()->user_data()))->cb_ButtonErrors_i();
}

//----------------------------------------- ButtonModeTree
inline void GUI_HelmScope::cb_ButtonModeTree_i() {
  if(m_but_gen_modetree->value()) {
    m_but_gen_errors->value(0);
    m_but_gen_warnings->value(0);
    m_but_gen_levents->value(0);
  }
  m_brw_general->label("Behavior \n Mode \n History:"); 
  updateBotBrowser();
}
void GUI_HelmScope::cb_ButtonModeTree(Fl_Widget* o) {
  ((GUI_HelmScope*)(o->parent()->user_data()))->cb_ButtonModeTree_i();
}

//----------------------------------------- ButtonLifeEvents
inline void GUI_HelmScope::cb_ButtonLifeEvents_i() {
  if(m_but_gen_levents->value()) {
    m_but_gen_errors->value(0);
    m_but_gen_warnings->value(0);
    m_but_gen_modetree->value(0);
  }
  m_brw_general->label("Behavior \n Life \n Events:"); 
  updateBotBrowser();
}
void GUI_HelmScope::cb_ButtonLifeEvents(Fl_Widget* o) {
  ((GUI_HelmScope*)(o->parent()->user_data()))->cb_ButtonLifeEvents_i();
}

//----------------------------------------- Step
inline void GUI_HelmScope::cb_Step_i(int val) {
  if(m_parent_gui)
    m_parent_gui->steptime(val);
  updateTopBrowsers();
  updateBotBrowser();
  updateXY();
}

void GUI_HelmScope::cb_Step(Fl_Widget* o, int val) {
  ((GUI_HelmScope*)(o->parent()->user_data()))->cb_Step_i(val);
}

//-------------------------------------------------------
// Procedure: updateXY()

void GUI_HelmScope::updateXY() 
{
  string title = m_vname + " " + m_replay_warp_msg;
  copy_label(title.c_str());

  double vtime_loc = m_hsmodel.getCurrTime();
  string stime_loc = doubleToString(vtime_loc, 3);
  m_fld_time->value(stime_loc.c_str());

  string mode = m_hsmodel.getCurrMode();
  m_fld_mode->value(mode.c_str());

  string iter = m_hsmodel.getCurrIter();
  m_fld_iter->value(iter.c_str());

  string dec = m_hsmodel.getCurrDecision();
  m_fld_decision->value(dec.c_str());

  string eng = m_hsmodel.getCurrHelmState();
  m_fld_state->value(eng.c_str());

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

  string label_warnings = " Warnings(";
  string total_warnings = uintToString(m_hsmodel.getVPlotSize("bhv_warning"));
  label_warnings += uintToString(m_cnt_warnings) + "/";
  label_warnings += total_warnings + ")";
  m_but_gen_warnings->copy_label(label_warnings.c_str());

  string label_errors = " Errors(";
  string total_errs = uintToString(m_hsmodel.getVPlotSize("bhv_error"));
  label_errors += uintToString(m_cnt_errors) + "/";
  label_errors += total_errs + ")";
  m_but_gen_errors->copy_label(label_errors.c_str());

  string label_levents = " LifeEvents(";
  string total_levents = uintToString(m_hsmodel.getVPlotSize("life_event"));
  label_levents += uintToString(m_cnt_levents) + "/";
  label_levents += total_levents + ")";
  m_but_gen_levents->copy_label(label_levents.c_str());
  redraw();
}


//----------------------------------------- UpdateTopBrowsers
void GUI_HelmScope::updateTopBrowsers()
{
  unsigned int header_lines = 0;
  if(m_hsmodel.getHeadersBHV())
    header_lines = 2;
  if(m_brw_active) {
    m_brw_active->clear();
    const vector<string>& svector = m_hsmodel.getActiveList();
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++)
      m_brw_active->add(svector[i].c_str());
    m_cnt_active = vsize;
    if(m_cnt_active >= header_lines)
      m_cnt_active -= header_lines;
  }

  if(m_brw_running) {
    m_brw_running->clear();
    const vector<string>& svector = m_hsmodel.getNonActiveList("running");
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++)
      m_brw_running->add(svector[i].c_str());
    m_cnt_running = vsize;
    if(m_cnt_running >= header_lines)
      m_cnt_running -= header_lines;
  }

  if(m_brw_idle) {
    m_brw_idle->clear();
    const vector<string>& svector = m_hsmodel.getNonActiveList("idle");
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++)
      m_brw_idle->add(svector[i].c_str());
    m_cnt_idle = vsize;
    if(m_cnt_idle >= header_lines)
      m_cnt_idle -= header_lines;
  }

  if(m_brw_completed) {
    m_brw_completed->clear();
    const vector<string>& svector = m_hsmodel.getNonActiveList("completed");
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++)
      m_brw_completed->add(svector[i].c_str());
    m_cnt_completed = vsize;
    if(m_cnt_completed >= header_lines)
      m_cnt_completed -= header_lines;
  }
}

//----------------------------------------- UpdateBotBrowser
void GUI_HelmScope::updateBotBrowser()
{
  unsigned int header_lines = 0;
  if(m_hsmodel.getHeadersBHV())
    header_lines = 2;

  if(m_brw_general) {
    m_brw_general->clear();
    vector<string> wvector = m_hsmodel.getWarnings();
    vector<string> evector = m_hsmodel.getErrors();
    vector<string> mvector = m_hsmodel.getModes();
    vector<string> lvector = m_hsmodel.getLifeEvents();

    m_cnt_warnings = wvector.size();
    m_cnt_errors   = evector.size();
    m_cnt_mevents  = mvector.size();
    m_cnt_levents  = lvector.size();

    if(m_cnt_warnings >= header_lines)
      m_cnt_warnings -= header_lines;
    if(m_cnt_errors >= header_lines)
      m_cnt_errors -= header_lines;
    if(m_cnt_mevents >= header_lines)
      m_cnt_mevents -= header_lines;
    if(m_cnt_levents >= header_lines)
      m_cnt_levents -= header_lines;

    vector<string> svector;

    if(m_but_gen_warnings->value()) 
      svector = wvector;
    else if(m_but_gen_errors->value()) 
      svector = evector;
    else if(m_but_gen_modetree->value()) 
      svector = mvector;
    else if(m_but_gen_levents->value())
      svector = lvector;
    
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++)
      m_brw_general->add(svector[i].c_str());
    m_cnt_general = vsize;
    if(m_cnt_general >= header_lines)
      m_cnt_general -= header_lines;
    //m_brw_general->bottomline(m_brw_general->size()-1);
  }
}


//----------------------------------------------------------
// Procedure: updateMutableTextSize()

void GUI_HelmScope::updateMutableTextSize(string val) 
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
  updateTopBrowsers();
  updateBotBrowser();
}





