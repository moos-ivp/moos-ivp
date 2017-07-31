/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GUI_VarScope.cpp                                     */
/*    DATE: Feb 28th, 2015                                       */
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
#include "GUI_VarScope.h"
#include "REPLAY_GUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

GUI_VarScope::GUI_VarScope(int g_w, int g_h, const char *g_l)
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

GUI_VarScope::~GUI_VarScope()
{
  if(m_fld_time)      
    delete(m_fld_time);

  if(m_but_vname)  
    delete(m_but_vname);
  if(m_but_varname)  
    delete(m_but_varname);
  if(m_but_source)  
    delete(m_but_source);
  if(m_but_srcaux)  
    delete(m_but_srcaux);

  if(m_brw_past)  
    delete(m_brw_past);
  if(m_brw_soon)  
    delete(m_brw_soon);
}

//---------------------------------------------------------------------------
// Procedure: initWidgets()

void GUI_VarScope::initWidgets()
{
  //Fl_Color fcolor1 = fl_rgb_color(200, 120, 120);
  Fl_Color fcolor2 = fl_rgb_color(190, 255, 190); // green
  //Fl_Color fcolor3 = fl_rgb_color(245, 245, 170); // yellow
  //Fl_Color fcolor4 = fl_rgb_color(225, 170, 170); // red
  Fl_Color bcolor = fl_rgb_color(0, 0, 120);     // dark blue
  Fl_Color bcolor_gray = fl_rgb_color(200, 220, 200); // light blueish gray

  m_brw_past = new Fl_Browser(0, 0, 1, 1); 
  m_brw_past->align(FL_ALIGN_LEFT);
  m_brw_past->color(fcolor2);
  m_brw_past->tooltip("Past Events");
  m_brw_past->callback((Fl_Callback*)GUI_VarScope::cb_BrowserPast);
  m_brw_past->when(FL_WHEN_RELEASE);
  m_brw_past->textfont(FL_COURIER_BOLD);

  m_brw_soon = new Fl_Browser(0, 0, 1, 1, ""); 
  m_brw_soon->align(FL_ALIGN_LEFT);
  m_brw_soon->color(bcolor_gray);
  m_brw_soon->tooltip("Future Events");
  m_brw_soon->callback((Fl_Callback*)GUI_VarScope::cb_BrowserSoon);
  m_brw_soon->when(FL_WHEN_RELEASE);
  m_brw_soon->textfont(FL_COURIER_BOLD);

  m_but_vname = new Fl_Check_Button(0, 0, 0, 0, "Node");
  m_but_vname->labelcolor(bcolor);
  m_but_vname->callback((Fl_Callback*)GUI_VarScope::cb_ButtonFilter, (void*)0);
  m_but_vname->clear_visible_focus();
  m_but_vname->value(0);

  m_but_varname = new Fl_Check_Button(0, 0, 0, 0, "VarName");
  m_but_varname->labelcolor(bcolor);
  m_but_varname->callback((Fl_Callback*)GUI_VarScope::cb_ButtonFilter, (void*)1);
  m_but_varname->clear_visible_focus();
  m_but_varname->value(1);

  m_but_source = new Fl_Check_Button(0, 0, 0, 0, "Source");
  m_but_source->labelcolor(bcolor);
  m_but_source->callback((Fl_Callback*)GUI_VarScope::cb_ButtonFilter, (void*)2);
  m_but_source->clear_visible_focus();
  m_but_source->value(1);

  m_but_srcaux = new Fl_Check_Button(0, 0, 0, 0, "AuxSource");
  m_but_srcaux->labelcolor(bcolor);
  m_but_srcaux->callback((Fl_Callback*)GUI_VarScope::cb_ButtonFilter, (void*)3);
  m_but_srcaux->clear_visible_focus();
  m_but_srcaux->value(0);

  m_but_addvar = new Fl_Menu_Button(0, 0, 1, 1, "Add Variable");
  m_but_addvar->clear_visible_focus();
  m_but_addvar->callback((Fl_Callback*)GUI_VarScope::cb_ButtonAddVar,(void*)-1);

  m_but_setvar = new Fl_Menu_Button(0, 0, 1, 1, "Set Variable");
  m_but_setvar->clear_visible_focus();
  m_but_setvar->callback((Fl_Callback*)GUI_VarScope::cb_ButtonSetVar,(void*)-1);

  m_but_delvar = new Fl_Menu_Button(0, 0, 1, 1, "Del Variable");
  m_but_delvar->clear_visible_focus();
  m_but_delvar->callback((Fl_Callback*)GUI_VarScope::cb_ButtonDelVar,(void*)-1);

  m_fld_time = new Fl_Output(0, 0, 0, 0, "Time:"); 
  m_fld_time->clear_visible_focus();
}

//---------------------------------------------------------------------------
// Procedure: resizeWidgetsShape()

void GUI_VarScope::resizeWidgetsShape()
{
  // General params
  int lmarg = 10;
  int rmarg = 10;
  int bwid  = w() - (lmarg + rmarg);
  int total_dat_hgt = 70;
  int total_brw_hgt = h() - total_dat_hgt;

  // Part 1: Set top row: time, add, set, remove buttons
  m_fld_time->resize(40, 5, 95, 20);

  int add_x = 150;
  int add_y = 5; 
  int add_wid = ((w()-155) / 3) - 5;
  int add_hgt = 20;
  m_but_addvar->resize(add_x, add_y, add_wid, add_hgt);

  int set_x = add_x + add_wid + 5;
  int set_y = 5; 
  int set_wid = ((w()-155) / 3) - 5;
  int set_hgt = 20;
  m_but_setvar->resize(set_x, set_y, set_wid, set_hgt);

  int del_x = set_x + set_wid + 5;
  int del_y = 5; 
  int del_wid = ((w()-155) / 3) - 5;
  int del_hgt = 20;
  m_but_delvar->resize(del_x, del_y, del_wid, del_hgt);

  // Part 2: Set the check buttons
  int butrow = total_dat_hgt-25;
  m_but_vname->resize(lmarg, butrow, 20, 20);
  m_but_varname->resize(70, butrow, 20, 20);
  m_but_source->resize(150, butrow, 20, 20);
  m_but_srcaux->resize(210, butrow, 20, 20);

  // Part 3: Set the browser components
  double pct_past = 0.5;
  double pct_soon = 0.5;
  double sep = 10;

  int y_past = total_dat_hgt;
  int h_past = pct_past * total_brw_hgt;
  m_brw_past->resize(lmarg, y_past, bwid, h_past-sep); 
  m_brw_past->bottomline(m_brw_past->size());

  int y_soon = y_past + h_past;
  int h_soon = pct_soon * total_brw_hgt;
  m_brw_soon->resize(lmarg, y_soon, bwid, h_soon-sep); 
  //m_brw_soon->bottomline(m_brw_soon->size());
}

//---------------------------------------------------------------------------
// Procedure: resizeWidgetsText()

void GUI_VarScope::resizeWidgetsText()
{
  int blab_size = 12; // blab=button_label size

  m_brw_past->textsize(m_mutable_text_size); 
  m_brw_past->labelsize(m_mutable_text_size);

  m_brw_soon->textsize(m_mutable_text_size); 
  m_brw_soon->labelsize(m_mutable_text_size);

  m_but_vname->labelsize(blab_size);
  m_but_varname->labelsize(blab_size);
  m_but_source->labelsize(blab_size);
  m_but_srcaux->labelsize(blab_size);

  m_but_addvar->labelsize(m_mutable_text_size);
  m_but_addvar->textsize(blab_size);

  m_but_setvar->labelsize(m_mutable_text_size);
  m_but_setvar->textsize(blab_size);

  m_but_delvar->labelsize(m_mutable_text_size);
  m_but_delvar->textsize(blab_size);

  m_fld_time->textsize(m_mutable_text_size); 
  m_fld_time->labelsize(m_mutable_text_size);
}

//-------------------------------------------------------------------

Fl_Menu_Item GUI_VarScope::menu_[] = {
 {"Invisible", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Replay/Step+1",  FL_CTRL + ']', (Fl_Callback*)GUI_VarScope::cb_Step, (void*)100, 0},
 {"Replay/Step-",   FL_CTRL + '[', (Fl_Callback*)GUI_VarScope::cb_Step, (void*)-100, 0},
 {"Replay/Step+1",  ']', (Fl_Callback*)GUI_VarScope::cb_Step, (void*)1000, 0},
 {"Replay/Step-",   '[', (Fl_Callback*)GUI_VarScope::cb_Step, (void*)-1000, 0},
 {"Replay/Step+5",  '}', (Fl_Callback*)GUI_VarScope::cb_Step, (void*)5000, 0},
 {"Replay/Step-5",  '{', (Fl_Callback*)GUI_VarScope::cb_Step, (void*)-5000, 0},
 {0}
};

//----------------------------------------------------------
// Procedure: setDataBroker

void GUI_VarScope::setDataBroker(ALogDataBroker dbroker, unsigned int mix)
{
  m_dbroker = dbroker;

  m_vsmodel.setDataBroker(dbroker, mix);
  
  m_vname = dbroker.getVNameFromMix(mix);

  m_current_mix_vars.push_back(mix);

  initMenuVarButtons();
  updateMenuDelVarButton();
}

//----------------------------------------------------------
// Procedure: resize

void GUI_VarScope::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgetsShape();
  resizeWidgetsText();
}

//----------------------------------------------------------
// Procedure: handle

int GUI_VarScope::handle(int event) 
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
    else if(Fl::event_key() == FL_Up) {
      if((m_brw_past->size() == 0) || (m_brw_soon->size() == 0))
	return(1);
      else
	return(Fl_Window::handle(event));
    }
    else if(Fl::event_key() == FL_Down) {
      if((m_brw_past->size() == 0) || (m_brw_soon->size() == 0))
	return(1);
      else
	return(Fl_Window::handle(event));
    }
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

void GUI_VarScope::setCurrTime(double curr_time)
{
  // Check if there is an actual time difference. It's possible that the 
  // time update is coming from the parent gui, prompted by this gui.
  if(m_vsmodel.getCurrTime() == curr_time)
    return;
  m_vsmodel.setTime(curr_time);
  updateBrowsers();
  updateXY();
}


//----------------------------------------- BrowserPast
inline void GUI_VarScope::cb_BrowserPast_i() {
  //cout << "In cb_BrowserPast_i()" << endl;
}
void GUI_VarScope::cb_BrowserPast(Fl_Widget* o) {
  ((GUI_VarScope*)(o->parent()->user_data()))->cb_BrowserPast_i();
}

//----------------------------------------- BrowserSoon
inline void GUI_VarScope::cb_BrowserSoon_i() {
  //cout << "In cb_BrowserSoon_i()" << endl;
}
void GUI_VarScope::cb_BrowserSoon(Fl_Widget* o) {
  ((GUI_VarScope*)(o->parent()->user_data()))->cb_BrowserSoon_i();
}

//----------------------------------------- ButtonAddVar
inline void GUI_VarScope::cb_ButtonAddVar_i(int mix) {
  for(unsigned int i=0; i<m_current_mix_vars.size(); i++)
    if(m_current_mix_vars[i] == (unsigned int)(mix))
      return;
  m_vsmodel.addVarPlot(mix);
  m_current_mix_vars.push_back(mix);
  updateMenuDelVarButton();
  updateBrowsers();
}
void GUI_VarScope::cb_ButtonAddVar(Fl_Widget* o, int v) {
  ((GUI_VarScope*)(o->parent()->user_data()))->cb_ButtonAddVar_i(v);
}

//----------------------------------------- ButtonSetVar
inline void GUI_VarScope::cb_ButtonSetVar_i(int mix) {
  m_vsmodel.setVarPlot(mix);
  m_current_mix_vars.clear();
  m_current_mix_vars.push_back(mix);
  updateMenuDelVarButton();
  updateBrowsers();
}
void GUI_VarScope::cb_ButtonSetVar(Fl_Widget* o, int v) {
  ((GUI_VarScope*)(o->parent()->user_data()))->cb_ButtonSetVar_i(v);
}

//----------------------------------------- ButtonDelVar
inline void GUI_VarScope::cb_ButtonDelVar_i(int mix) {
  vector<unsigned int> new_current_mix_vars;
  for(unsigned int i=0; i<m_current_mix_vars.size(); i++) {
    if(m_current_mix_vars[i] == (unsigned int)(mix))
      m_vsmodel.delVarPlot(mix);
    else
      new_current_mix_vars.push_back(m_current_mix_vars[i]);
  }
  m_current_mix_vars = new_current_mix_vars;
  updateMenuDelVarButton();
  updateBrowsers();
}
void GUI_VarScope::cb_ButtonDelVar(Fl_Widget* o, int v) {
  ((GUI_VarScope*)(o->parent()->user_data()))->cb_ButtonDelVar_i(v);
}

//----------------------------------------- ButtonFilter
inline void GUI_VarScope::cb_ButtonFilter_i(int v) {
  if(v == 0) {
    bool show = m_but_vname->value();
    m_vsmodel.setShowVName(show);
  }
  else if(v == 1) {
    bool show = m_but_varname->value();
    m_vsmodel.setShowVarName(show);
  }
  else if(v == 2) {
    bool show = m_but_source->value();
    m_vsmodel.setShowSource(show);
  }
  else if(v == 3) {
    bool show = m_but_srcaux->value();
    m_vsmodel.setShowSrcAux(show);
  }
  else
    return;

  m_vsmodel.reformat();  
  updateBrowsers();
}
void GUI_VarScope::cb_ButtonFilter(Fl_Widget* o, int val) {
  ((GUI_VarScope*)(o->parent()->user_data()))->cb_ButtonFilter_i(val);
}

//----------------------------------------- Step
inline void GUI_VarScope::cb_Step_i(int val) {
  if(m_parent_gui)
    m_parent_gui->steptime(val);
  updateBrowsers();
  updateXY();
}

void GUI_VarScope::cb_Step(Fl_Widget* o, int val) {
  ((GUI_VarScope*)(o->parent()->user_data()))->cb_Step_i(val);
}

//----------------------------------------- UpdateXY
void GUI_VarScope::updateXY() 
{
  string title = m_vname + " " + m_replay_warp_msg;
  copy_label(title.c_str());

  double vtime_loc = m_vsmodel.getCurrTime();
  string stime_loc = doubleToString(vtime_loc, 3);
  m_fld_time->value(stime_loc.c_str());

  redraw();
}


//----------------------------------------------------------
// Procedure: initMenuVarButtons

void GUI_VarScope::initMenuVarButtons() 
{
  m_but_addvar->clear();
  m_but_setvar->clear();

  unsigned int max_mix = m_dbroker.sizeMix();

  for(unsigned int mix=0; mix<max_mix; mix++) {
    string vname   = m_dbroker.getVNameFromMix(mix);
    string varname = m_dbroker.getVarNameFromMix(mix);
    string entry = vname + "/" + varname;

    // Use special unsigned int type having same size a pointer (void*)  
    uintptr_t ix = mix;

    m_but_addvar->add(entry.c_str(), 0, 
		       (Fl_Callback*)GUI_VarScope::cb_ButtonAddVar, (void*)ix);
    m_but_setvar->add(entry.c_str(), 0, 
		       (Fl_Callback*)GUI_VarScope::cb_ButtonSetVar, (void*)ix);
  }
}

//----------------------------------------------------------
// Procedure: updateMenuDelVarButton

void GUI_VarScope::updateMenuDelVarButton() 
{
  m_but_delvar->clear();

  for(unsigned int i=0; i<m_current_mix_vars.size(); i++) {
    unsigned int mix = m_current_mix_vars[i];
    string vname   = m_dbroker.getVNameFromMix(mix);
    string varname = m_dbroker.getVarNameFromMix(mix);
    string entry = vname + ":" + varname;

    // Use special unsigned int type having same size a pointer (void*)  
    uintptr_t ix = mix;

    m_but_delvar->add(entry.c_str(), 0, 
		       (Fl_Callback*)GUI_VarScope::cb_ButtonDelVar, (void*)ix);
    m_but_delvar->add(entry.c_str(), 0, 
		       (Fl_Callback*)GUI_VarScope::cb_ButtonDelVar, (void*)ix);
  }
}

//----------------------------------------------------------
// Procedure: updateMutableTextSize()

void GUI_VarScope::updateMutableTextSize(string val) 
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
}


//----------------------------------------- UpdateBrowsers
void GUI_VarScope::updateBrowsers()
{
  m_brw_past->clear();
  vector<string> pvector = m_vsmodel.getPastEntries();
  for(unsigned i=0; i<pvector.size(); i++) {
    m_brw_past->add(pvector[i].c_str());
  }
  m_brw_past->bottomline(m_brw_past->size());

  m_brw_soon->clear();
  vector<string> svector = m_vsmodel.getSoonEntries();
  for(unsigned int i=0; i<svector.size(); i++) {
    m_brw_soon->add(svector[i].c_str());
    //m_brw_general->bottomline(m_brw_general->size()-1);
  }
}




