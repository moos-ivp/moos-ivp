/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GUI_IPF.cpp                                          */
/*    DATE: Sep 25th, 2011                                       */
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
#include "GUI_IPF.h"
#include "MBUtils.h"
#include "REPLAY_GUI.h"
#include "FL/fl_ask.H"

using namespace std;

//-------------------------------------------------------------------
// Constructor

GUI_IPF::GUI_IPF(int wid, int hgt, const char *label)
  : Common_IPF_GUI(wid, hgt, label) 
{
  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();
  this->size_range(375,250, 1200,1200, 0,0, 1);

  augmentMenu();

  m_replay_warp_msg = "(PAUSED)";
  m_parent_gui = 0;
  m_fullscreen = false;
  m_fullscreen_tmp = false;
  m_mutable_text_size = 10;

  initWidgets();
  m_viewer = (Common_IPFViewer*)(m_ipf_viewer);
  
  resizeWidgetsShape();
  resizeWidgetsText();
  redraw();

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
// Destructor: Must carefully implement since master GUI may create
//             and destroy instances of this GUI all in same session

GUI_IPF::~GUI_IPF()
{
  delete(m_ipf_viewer);

  delete(m_fld_loc_time);
  delete(m_fld_ipf_iter);
  delete(m_fld_ipf_pcs);
  delete(m_fld_ipf_dom);
  delete(m_but_ipf_set);
  delete(m_but_ipf_pin);

  delete(m_but_collective);
  delete(m_but_collective_dep);
  delete(m_but_show_pcs);
}


//---------------------------------------------------------------------------
// Procedure: initWidgets()

void GUI_IPF::initWidgets()
{
  Fl_Color fcolor1 = fl_rgb_color(200, 90, 90);

  m_ipf_viewer = new IvPFuncViewerX(0, 0, 1, 1);
  m_ipf_viewer->setClearColor("0.6,0.7,0.5");

  m_but_ipf_set = new Fl_Button(0, 0, 1, 1, "set");
  m_but_ipf_set->clear_visible_focus();
  m_but_ipf_set->callback((Fl_Callback*)GUI_IPF::cb_ToggleSet);

  m_but_ipf_pin = new Fl_Button(0, 0, 1, 1, "pin");
  m_but_ipf_pin->clear_visible_focus();
  m_but_ipf_pin->callback((Fl_Callback*)GUI_IPF::cb_TogglePin);

  m_fld_loc_time = new Fl_Output(0, 0, 1, 1, "Time:"); 
  m_fld_loc_time->clear_visible_focus();

  m_fld_ipf_iter = new Fl_Output(0, 0, 1, 1, "Iter:"); 
  m_fld_ipf_iter->clear_visible_focus();
  m_fld_ipf_iter->color(fcolor1); 

  m_fld_ipf_pcs = new Fl_Output(0, 0, 1, 1, "Pcs:"); 
  m_fld_ipf_pcs->clear_visible_focus();

  m_fld_ipf_dom = new Fl_Output(0, 0, 1, 1, "Dom:"); 
  m_fld_ipf_dom->clear_visible_focus();

  m_but_collective = new Fl_Check_Button(0, 0, 1, 1, "Collective");
  m_but_collective->clear_visible_focus();
  m_but_collective->callback((Fl_Callback*)GUI_IPF::cb_SelectCollective, (void*)0);

  m_but_collective_dep = new Fl_Check_Button(0, 0, 1, 1, "CollectiveD");
  m_but_collective_dep->clear_visible_focus();
  m_but_collective_dep->callback((Fl_Callback*)GUI_IPF::cb_SelectCollective, (void*)1);

  m_but_show_pcs = new Fl_Check_Button(0, 0, 1, 1, "pcs");
  m_but_show_pcs->clear_visible_focus();
  m_but_show_pcs->callback((Fl_Callback*)Common_IPF_GUI::cb_ToggleDrawPcs, (void*)1);

  m_brw_bhvs = new Fl_Hold_Browser(0, 0, 0, 0);
  m_brw_bhvs->clear_visible_focus();
  m_brw_bhvs->callback((Fl_Callback*)GUI_IPF::cb_SelectSource);
  m_brw_bhvs->textfont(FL_COURIER);

  m_but_addvar_a = new Fl_Menu_Button(0, 0, 1, 1, "Scope Var A");
  m_but_addvar_a->clear_visible_focus();
  m_but_addvar_a->callback((Fl_Callback*)GUI_IPF::cb_ButtonAddVarA,(void*)-1);

  m_but_addvar_b = new Fl_Menu_Button(0, 0, 1, 1, "Scope Var B");
  m_but_addvar_b->clear_visible_focus();
  m_but_addvar_b->callback((Fl_Callback*)GUI_IPF::cb_ButtonAddVarB,(void*)-1);
}

//---------------------------------------------------------------------------
// Procedure: resizeWidgetsShape()

void GUI_IPF::resizeWidgetsShape()
{
  if(w() < 400) {
    m_fullscreen=true;
    m_fullscreen_tmp=true;
  }
  else {
    if(m_fullscreen_tmp) {
      m_fullscreen = false;
      m_fullscreen_tmp = false;
    }
  }
  
  // We want the bhv browser to grow with the overall window width but not
  // quite linearly.
  int lmarg = 160 + (w() * 0.1);
  int tmarg = 60;

  int ipf_x = lmarg; 
  int ipf_y = tmarg; 
  int ipf_wid = w()-lmarg;
  int ipf_hgt = h()-tmarg;

  if(m_fullscreen) {
    ipf_x = 0; 
    ipf_y = 0;
    ipf_wid = w();
    ipf_hgt = h();
  }

  m_menubar->resize(0,0,0,0);
  m_ipf_viewer->Common_IPFViewer::resize(ipf_x, ipf_y, ipf_wid, ipf_hgt);

  if(m_fullscreen) 
    return;

  int fld_hgt = 20;

  //----------------------------------------------------------
  // Browser (LEFT)
  //----------------------------------------------------------
  int bhvs_x = 5; 
  int bhvs_y = tmarg;
  int bhvs_wid = lmarg-10;
  m_brw_bhvs->resize(bhvs_x, bhvs_y, bhvs_wid, ipf_hgt);

  //----------------------------------------------------------
  // Row 1
  //----------------------------------------------------------
  int time_x = 40;
  int time_y = 5;
  int time_wid = (80.0/550.0)*w();
  m_fld_loc_time->resize(time_x, time_y, time_wid, fld_hgt); 

  int iter_x = time_x + time_wid + 40;
  int iter_y = 5;
  int iter_wid = (50.0/550.0)*w();
  m_fld_ipf_iter->resize(iter_x, iter_y, iter_wid, fld_hgt); 

  int pcs_x = iter_x + iter_wid + 40;
  int pcs_y = 5;
  int pcs_wid = (45.0/550.0)*w();
  m_fld_ipf_pcs->resize(pcs_x, pcs_y, pcs_wid, fld_hgt); 

  int set_x = pcs_x + pcs_wid + 10;
  int set_y = 5;
  int set_wid = (33.0/550.0)*w();
  m_but_ipf_set->resize(set_x, set_y, set_wid, fld_hgt);

  int pin_x = set_x + set_wid + 10;
  int pin_y = 5;
  int pin_wid = (33.0/550.0)*w();
  m_but_ipf_pin->resize(pin_x, pin_y, pin_wid, fld_hgt);

  int vara_x = pin_x + pin_wid + 12;
  int vara_y = 5;
  int vara_wid = w()-vara_x-10;
  m_but_addvar_a->resize(vara_x, vara_y, vara_wid, fld_hgt);

  //----------------------------------------------------------
  // Row 2
  //----------------------------------------------------------
  int coll_x = 10; 
  int coll_y = 30;
  int coll_wid = 20; 
  m_but_collective->resize(coll_x, coll_y, coll_wid, fld_hgt);

  int cold_x = 90; 
  int cold_y = 30;
  int cold_wid = 20; 
  m_but_collective_dep->resize(cold_x, cold_y, cold_wid, fld_hgt);

  int cpcs_x = 180; 
  int cpcs_y = 30;
  int cpcs_wid = 20; 
  m_but_show_pcs->resize(cpcs_x, cpcs_y, cpcs_wid, fld_hgt);

  int dom_x = pcs_x;
  int dom_y = 30;
  int dom_wid = (110.0/550.0)*w();
  m_fld_ipf_dom->resize(dom_x, dom_y, dom_wid, fld_hgt); 

  int varb_x = pin_x + pin_wid + 12;
  int varb_y = 30;
  int varb_wid = w()-varb_x-10;
  m_but_addvar_b->resize(varb_x, varb_y, varb_wid, fld_hgt);
}

//---------------------------------------------------------------------------
// Procedure: resizeWidgetsText()

void GUI_IPF::resizeWidgetsText()
{
  if(m_fullscreen) 
    return;

  int small_wid = 440;  // Below which we start to shrink things
  int info_size = 12;
  int blab_size = 12;

  if(w() < small_wid)
    blab_size = 10;

  m_but_ipf_set->labelsize(blab_size);
  m_but_ipf_pin->labelsize(blab_size);

  m_but_collective->labelsize(blab_size);
  m_but_collective_dep->labelsize(blab_size);

  m_but_show_pcs->labelsize(blab_size);
  m_but_show_pcs->labelsize(blab_size);

  m_fld_loc_time->textsize(info_size); 
  m_fld_loc_time->labelsize(info_size);

  m_fld_ipf_iter->textsize(info_size); 
  m_fld_ipf_iter->labelsize(info_size);

  m_fld_ipf_pcs->textsize(info_size); 
  m_fld_ipf_pcs->labelsize(info_size);

  m_fld_ipf_dom->textsize(info_size); 
  m_fld_ipf_dom->labelsize(info_size);

  m_brw_bhvs->textsize(m_mutable_text_size);

  string labela = m_but_addvar_a->label();
  if(strContains(labela, "Var A")) {
    if(w() < small_wid) 
      labela = "Var A";
    else
      labela = "Scope Var A";
    m_but_addvar_a->copy_label(labela.c_str());
  }
  m_but_addvar_a->textsize(12);
  if(labela.length() < 12) 
    m_but_addvar_a->labelsize(12);
  else if(labela.length() < 17) 
    m_but_addvar_a->labelsize(10);
  else 
    m_but_addvar_a->labelsize(8);
  

  string labelb = m_but_addvar_b->label();
  if(strContains(labelb, "Var B")) {
    if(w() < small_wid) 
      labelb = "Var B";
    else
      labelb = "Scope Var B";
    m_but_addvar_b->copy_label(labelb.c_str());
  }
  m_but_addvar_b->textsize(info_size);
  if(labelb.length() < 12) 
    m_but_addvar_b->labelsize(12);
  else if(labelb.length() < 17) 
    m_but_addvar_b->labelsize(10);
  else 
    m_but_addvar_b->labelsize(8);
}

//-------------------------------------------------------------------
// Procedure: augmentMenu()

void GUI_IPF::augmentMenu()
{
  m_menubar->add("RotateZoom/Pan Up ",    FL_Up,
	      (Fl_Callback*)GUI_IPF::cb_HandleUpDown, (void*)-100, 0);
  m_menubar->add("RotateZoom/Pan Down ",  FL_Down,
	      (Fl_Callback*)GUI_IPF::cb_HandleUpDown, (void*)100, 0);
  m_menubar->add("RotateZoom/Pan Left ",  FL_Left,
	      (Fl_Callback*)GUI_IPF::cb_HandleLeftRight, (void*)100, 0);
  m_menubar->add("RotateZoom/Pan Right ", FL_Right,
	      (Fl_Callback*)GUI_IPF::cb_HandleLeftRight, (void*)-100, 0);
  m_menubar->add("RotateZoom/Pan U (slow) ", FL_ALT + FL_Up,
	      (Fl_Callback*)GUI_IPF::cb_HandleUpDown, (void*)-25, 0);
  m_menubar->add("RotateZoom/Pan D (slow) ", FL_ALT + FL_Down,
	      (Fl_Callback*)GUI_IPF::cb_HandleUpDown, (void*)25, 0);
  m_menubar->add("RotateZoom/Pan L (slow) ", FL_ALT + FL_Left,
	      (Fl_Callback*)GUI_IPF::cb_HandleLeftRight, (void*)25, 0);
  m_menubar->add("RotateZoom/Pan R (slow)",  FL_ALT + FL_Right,
	      (Fl_Callback*)GUI_IPF::cb_HandleLeftRight, (void*)-25, 0);

  m_menubar->add("Replay/Step Ahead 0.1 secs", FL_CTRL+']',
	      (Fl_Callback*)GUI_IPF::cb_Step, (void*)100, 0);
  m_menubar->add("Replay/Step Back 0.1 secs", FL_CTRL+'[',
	      (Fl_Callback*)GUI_IPF::cb_Step, (void*)-100, 0);
  m_menubar->add("Replay/Step Ahead 1 sec",  ']',
	      (Fl_Callback*)GUI_IPF::cb_Step, (void*)1000, 0);
  m_menubar->add("Replay/Step Back  1 sec",  '[',
	      (Fl_Callback*)GUI_IPF::cb_Step, (void*)-1000, 0);
  m_menubar->add("Replay/Step Ahead 5 secs", '}',
	      (Fl_Callback*)GUI_IPF::cb_Step, (void*)5000, 0);
  m_menubar->add("Replay/Step Back  5 secs", '{',
	      (Fl_Callback*)GUI_IPF::cb_Step, (void*)-5000, 0);
}


//----------------------------------------------------------
// Procedure: setDataBroker ()

void GUI_IPF::setDataBroker(ALogDataBroker dbroker, string vname)
{
  m_dbroker = dbroker;
  m_vname   = vname;

  unsigned int aix = m_dbroker.getAixFromVName(vname);
  m_sources = m_dbroker.getBhvsInALog(aix);

  if(!m_ipf_viewer)
    return;

  m_ipf_viewer->setDataBroker(m_dbroker, vname);
  m_ipf_viewer->setIPF_Plots(m_sources);
  m_ipf_viewer->setHelmIterPlot();

  initSourceSelection();
  initMenuVarButtons();

  updateBrowser();
}

//----------------------------------------------------------
// Procedure: setParentGUI()

void GUI_IPF::setParentGUI(REPLAY_GUI* parent_gui)
{
  m_parent_gui = parent_gui;
  this->redraw();
}

//----------------------------------------------------------
// Procedure: resize()

void GUI_IPF::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgetsShape();
  resizeWidgetsText();
}

//----------------------------------------------------------
// Procedure: handle()
//     Notes: Return of 1 indicates event has been handled.

int GUI_IPF::handle(int event) 
{
  switch(event) {
  case FL_KEYDOWN:
    if(Fl::event_key() == 32) 
      m_parent_gui->streaming(2);
    else if(Fl::event_key() == '\\') 
      toggleFullScreen();
    else if(Fl::event_key() == 'a') 
      m_parent_gui->streamspeed(true);
    else if(Fl::event_key() == 'z') 
      m_parent_gui->streamspeed(false);
    else if(Fl::event_key() == '+') 
      updateMutableTextSize("bigger");
    else if(Fl::event_key() == '-') 
      updateMutableTextSize("smaller");
    else if((Fl::event_key() == 'c')) {
      if(m_but_collective->value()) {
	m_but_collective->value(0);
	m_but_collective_dep->value(0);
      }
      else
	m_but_collective->value(1);
      cb_SelectCollective_i(0);
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

void GUI_IPF::setCurrTime(double curr_time)
{
  if(!m_ipf_viewer)
    return;

  m_ipf_viewer->setTime(curr_time);
  m_ipf_viewer->redraw();
  updateXY();
  updateBrowser();
}


//----------------------------------------------------------
// Procedure: initSourceSelection()

void GUI_IPF::initSourceSelection()
{
  for(unsigned int i=0; i<m_sources.size(); i++) {
    string pwt = m_ipf_viewer->getCurrPriority(m_sources[i]);
    if(pwt != "0") {
      m_source = m_sources[i];
      m_ipf_viewer->setSource(m_source);
    }
  }
}

//----------------------------------------- HandleUpDown
inline void GUI_IPF::cb_HandleUpDown_i(int amt) 
{
  double damt = (double)(amt) / 100.0;
  m_ipf_viewer->setParam("mod_x_rotation", ((double)(-damt)));
  m_ipf_viewer->redraw();
  redraw();
  updateXY();
}
void GUI_IPF::cb_HandleUpDown(Fl_Widget* o, int v) {
  ((GUI_IPF*)(o->parent()->user_data()))->cb_HandleUpDown_i(v);
}

//----------------------------------------- HandleLeftRight
inline void GUI_IPF::cb_HandleLeftRight_i(int amt) 
{
  double damt = (double)(amt) / 100.0;
  m_ipf_viewer->setParam("mod_z_rotation", ((double)(damt)));
  m_ipf_viewer->redraw();
  redraw();
  updateXY();
}
void GUI_IPF::cb_HandleLeftRight(Fl_Widget* o, int v) {
  ((GUI_IPF*)(o->parent()->user_data()))->cb_HandleLeftRight_i(v);
}


//----------------------------------------- Zoom In
inline void GUI_IPF::cb_Zoom_i(int val) {
  if(val < 0)
    m_ipf_viewer->setParam("mod_zoom", 1.05); 
  else if(val > 0)
    m_ipf_viewer->setParam("mod_zoom", 0.95); 
  else
    m_ipf_viewer->setParam("set_zoom", 1.0); 
  m_ipf_viewer->redraw();
}

//----------------------------------------- ToggleSet
inline void GUI_IPF::cb_ToggleSet_i() {
  m_ipf_viewer->setParam("reset_view", "2");
}
void GUI_IPF::cb_ToggleSet(Fl_Widget* o) {
  ((GUI_IPF*)(o->parent()->user_data()))->cb_ToggleSet_i();
}

//----------------------------------------- TogglePin
inline void GUI_IPF::cb_TogglePin_i() {
  m_ipf_viewer->setParam("draw_pin", "toggle");
}

void GUI_IPF::cb_TogglePin(Fl_Widget* o) {
  ((GUI_IPF*)(o->parent()->user_data()))->cb_TogglePin_i();
}

//----------------------------------------- Step
inline void GUI_IPF::cb_Step_i(int val) {
  if(m_parent_gui)
    m_parent_gui->steptime(val);
  updateXY();
}

void GUI_IPF::cb_Step(Fl_Widget* o, int val) {
  ((GUI_IPF*)(o->parent()->user_data()))->cb_Step_i(val);
}

//----------------------------------------- SelectSource
inline void GUI_IPF::cb_SelectSource_i() {

  int ix = ((Fl_Browser *)m_brw_bhvs)->value();
  if(ix == 0)
    return;

  string txt = ((Fl_Browser *)m_brw_bhvs)->text(ix);

  txt = stripBlankEnds(txt);
  biteStringX(txt, ' ');
  m_source = txt;
  m_ipf_viewer->setSource(m_source);

  // If we're selecting a menu item, it must mean we're no longer 
  // interested in any collective. Make sure button box is unchecked.
  m_but_collective->value(0);
  m_but_collective_dep->value(0);

  updateXY();
  resizeWidgetsText();
}
void GUI_IPF::cb_SelectSource(Fl_Widget* o) {
  ((GUI_IPF*)(o->parent()->user_data()))->cb_SelectSource_i();
}

//----------------------------------------- SelectCollective
inline void GUI_IPF::cb_SelectCollective_i(int index) {
  if(index == 0) {
    if(m_but_collective->value()) {
      m_ipf_viewer->setSource("collective-hdgspd");
      m_but_collective_dep->value(0);
    }
    else
      m_ipf_viewer->setSource(m_source);
  }
  else if(index == 1) {
    if(m_but_collective_dep->value()) {
      m_ipf_viewer->setSource("collective-depth"); 
      m_but_collective->value(0);
    }
    else
      m_ipf_viewer->setSource(m_source);
  }
  updateBrowser();
  updateXY();
}
void GUI_IPF::cb_SelectCollective(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((GUI_IPF*)(o->parent()->user_data()))->cb_SelectCollective_i(val);
}


//----------------------------------------- ButtonAddVarA
inline void GUI_IPF::cb_ButtonAddVarA_i(int mix) {
  if(mix != 29999) {
    m_ipf_viewer->setVarPlotA(mix);
    string varname = m_dbroker.getVarNameFromMix(mix);
    varname = truncString(varname, 22, "middle");
    m_but_addvar_a->copy_label(varname.c_str());
  }
  else {
    m_but_addvar_a->copy_label("Scope Var A");
    m_ipf_viewer->clearVarPlotA();    
  }    
  resizeWidgetsText();
  m_ipf_viewer->redraw();
}
void GUI_IPF::cb_ButtonAddVarA(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((GUI_IPF*)(o->parent()->user_data()))->cb_ButtonAddVarA_i(val);
}

//----------------------------------------- ButtonAddVarB
inline void GUI_IPF::cb_ButtonAddVarB_i(int mix) {
  if(mix != 29999) {
    m_ipf_viewer->setVarPlotB(mix);
    string varname = m_dbroker.getVarNameFromMix(mix);
    varname = truncString(varname, 22, "middle");
    m_but_addvar_b->copy_label(varname.c_str());
  }
  else {
    m_but_addvar_b->copy_label("Scope Var B");
    m_ipf_viewer->clearVarPlotB();    
  }
  resizeWidgetsText();
  m_ipf_viewer->redraw();
}
void GUI_IPF::cb_ButtonAddVarB(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((GUI_IPF*)(o->parent()->user_data()))->cb_ButtonAddVarB_i(val);
}

//----------------------------------------------------------
// Procedure: initMenuVarButtons

void GUI_IPF::initMenuVarButtons() 
{
  m_but_addvar_a->clear();
  m_but_addvar_b->clear();

  vector<string> non_helm_entries;
  vector<unsigned int> non_helm_indices;

  // Use special unsigned int type having same size a pointer (void*)  
  uintptr_t ix = 29999;      
  m_but_addvar_a->add(" -- CLEAR --", 0, 
		      (Fl_Callback*)GUI_IPF::cb_ButtonAddVarA, (void*)ix);
  m_but_addvar_b->add(" -- CLEAR --", 0, 
		      (Fl_Callback*)GUI_IPF::cb_ButtonAddVarB, (void*)ix);

  // Part 1: Add menu items for all variable where pHelmIvP is a source
  unsigned int max_mix = m_dbroker.sizeMix();
  for(unsigned int mix=0; mix<max_mix; mix++) {
    string vname  = m_dbroker.getVNameFromMix(mix);
    string source = m_dbroker.getVarSourceFromMix(mix);
    if(vname == m_vname) {
      string varname = m_dbroker.getVarNameFromMix(mix);
      if(!strBegins(varname, "BHV_IPF")) {
	string entry = varname;
	if(!strContains(source, "pHelmIvP")) {
	  entry = "AllOtherVars/" + varname;
	  non_helm_entries.push_back(entry);
	  non_helm_indices.push_back(mix);
	}
	else {
	  // Use special unsigned int type having same size a pointer (void*)  
	  uintptr_t iix = mix;      
	  m_but_addvar_a->add(entry.c_str(), 0, 
			      (Fl_Callback*)GUI_IPF::cb_ButtonAddVarA, (void*)iix);
	  m_but_addvar_b->add(entry.c_str(), 0, 
			      (Fl_Callback*)GUI_IPF::cb_ButtonAddVarB, (void*)iix);
	}
      }
    }
  }

  // Part 2: Add menu items for all variable where pHelmIvP is NOT a source
  for(unsigned int i=0; i<non_helm_entries.size(); i++) {
    string entry = non_helm_entries[i];
    unsigned int mix = non_helm_indices[i];
    // Use special unsigned int type having same size a pointer (void*)  
    uintptr_t iix = mix;      
	  m_but_addvar_a->add(entry.c_str(), 0, 
			      (Fl_Callback*)GUI_IPF::cb_ButtonAddVarA, (void*)iix);
	  m_but_addvar_b->add(entry.c_str(), 0, 
			      (Fl_Callback*)GUI_IPF::cb_ButtonAddVarB, (void*)iix);
  }

}

//----------------------------------------- UpdateBrowser
void GUI_IPF::updateBrowser() 
{
  if(m_fullscreen)
    return;

  m_brw_bhvs->clear();

  int curr_src_ix = -1;
  for(unsigned int i=0; i<m_sources.size(); i++) {
    string pwt = m_ipf_viewer->getCurrPriority(m_sources[i]);
    pwt = dstringCompact(pwt);
    pwt = padString(pwt, 4);
    string entry = pwt + "  " + m_sources[i];

    if(m_source == m_sources[i])
      curr_src_ix = (int)(i+1);
    m_brw_bhvs->add(entry.c_str());
  }

  bool coll_active_a = m_but_collective->value();
  bool coll_active_b = m_but_collective_dep->value();

  if((curr_src_ix >= 0) && !coll_active_a && !coll_active_b)
    m_brw_bhvs->select(curr_src_ix);
}

//----------------------------------------- FullScreen
void GUI_IPF::toggleFullScreen() 
{
  m_fullscreen = !m_fullscreen;
  
  if(m_fullscreen) {
    m_brw_bhvs->hide();
    m_fld_loc_time->hide();
    m_fld_ipf_iter->hide();
    m_fld_ipf_pcs->hide();
    m_fld_ipf_dom->hide();
    m_but_ipf_set->hide();
    m_but_ipf_pin->hide();
    m_but_addvar_a->hide();
    m_but_addvar_b->hide();
    m_but_collective->hide();
    m_but_collective_dep->hide();
    resizeWidgetsShape();
    redraw();
  }
  else {
    m_brw_bhvs->show();
    m_fld_loc_time->show();
    m_fld_ipf_iter->show();
    m_fld_ipf_pcs->show();
    m_fld_ipf_dom->show();
    m_but_ipf_set->show();
    m_but_ipf_pin->show();
    m_but_addvar_a->show();
    m_but_addvar_b->show();
    m_but_collective->show();
    m_but_collective_dep->show();
    resizeWidgetsShape();
    resizeWidgetsText();
    updateXY();
    updateBrowser();
    redraw();
  }
}

//----------------------------------------------------------
// Procedure: updateMutableTextSize()

void GUI_IPF::updateMutableTextSize(string val) 
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
  
  m_ipf_viewer->setMutableTextSize(m_mutable_text_size);
  m_ipf_viewer->redraw();
}

//---------------------------------------------------------
// Procedure: applyBehaviorVarMap()

void GUI_IPF::applyBehaviorVarMap(map<string, string> map_bhv_vars) 
{
  map<string,string>::iterator p;
  for(p=map_bhv_vars.begin(); p!=map_bhv_vars.end(); p++) {
    string bhv_name = p->first;
    if(!vectorContains(m_sources, bhv_name))
      continue;

    string vars = p->second;
    string var1, var2;
    if(!strContains(vars, ':')) {
      var1 = vars;
      var2 = "";
    }
    else {
      var1 = biteStringX(vars, ':');
      var2 = vars;
    }

    if(var1 != "") {
      int mix = m_dbroker.getMixFromVNameVarName(m_vname,var1);
      m_ipf_viewer->setVarPlotA(bhv_name, mix);
      //cb_ButtonAddVarA_i(mix);
    }
    if(var2 != "") {
      int mix = m_dbroker.getMixFromVNameVarName(m_vname,var2);
      m_ipf_viewer->setVarPlotB(bhv_name, mix);
      //cb_ButtonAddVarB_i(mix);
    }
  }
  resizeWidgetsText();
}


//-------------------------------------------------------
// Procedure: updateXY()

void GUI_IPF::updateXY() 
{
  string title = m_vname + " " + m_replay_warp_msg;
  copy_label(title.c_str());

  if(m_fullscreen)
    return;

  if(m_viewer->getShowPieces())
    m_but_show_pcs->value(1);
  else
    m_but_show_pcs->value(0);

  
  // IPF fields
  double time     = m_ipf_viewer->getCurrTime();
  string time_str = doubleToString(time, 3);
  m_fld_loc_time->value(time_str.c_str());

  string ipf_iter_str = m_ipf_viewer->getCurrIteration();
  m_fld_ipf_iter->value(ipf_iter_str.c_str());
  string ipf_pcs_str = m_ipf_viewer->getCurrPieces();
  m_fld_ipf_pcs->value(ipf_pcs_str.c_str());
  string ipf_dom_str = m_ipf_viewer->getCurrDomain();
  m_fld_ipf_dom->value(ipf_dom_str.c_str());

  string labela = m_ipf_viewer->getCurrScopeVarA();
  if(labela == "") {
    m_but_addvar_a->labelsize(12);
    labela = "Scope Var A";
  }
  m_but_addvar_a->copy_label(labela.c_str());

  string labelb = m_ipf_viewer->getCurrScopeVarB();
  if(labelb == "") {
    m_but_addvar_b->labelsize(12);
    labelb = "Scope Var B";
  }
  m_but_addvar_b->copy_label(labelb.c_str());
}





