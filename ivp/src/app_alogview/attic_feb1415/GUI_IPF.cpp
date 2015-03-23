/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GUI_IPF.cpp                                          */
/*    DATE: Sep 25th, 2011                                       */
/*    DATE: Feb 13th, 2015  Merge with alogview                  */
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

GUI_IPF::GUI_IPF(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) 
{
  mbar = new Fl_Menu_Bar(0, 0, 0, 0);
  mbar->menu(menu_);

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  // Init window with bogus extents. Actual extents
  // are set in the setWindowLayout() call below.
  m_ipf_viewer = new IvPFuncViewerX(1, 1, 1, 1);
  m_ipf_viewer->setClearColor("0.6,0.7,0.5");

  m_fld_loc_time = 0;
  m_fld_ipf_iter = 0;
  m_fld_ipf_pcs  = 0;
  m_fld_ipf_pwt  = 0;
  m_fld_ipf_dom  = 0;
  m_but_ipf_set  = 0;
  m_but_ipf_pin  = 0;

  m_but_ipf_source = 0;

  setWindowLayout("normal");

  m_parent_gui = 0;

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
Fl_Menu_Item GUI_IPF::menu_[] = {
 {"Invisible", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Pan Up ",         FL_Up,  (Fl_Callback*)GUI_IPF::cb_HandleUpDown, (void*)-100, 0},
 {"Pan Down ",       FL_Down,  (Fl_Callback*)GUI_IPF::cb_HandleUpDown, (void*)100, 0},
 {"Pan Left ",       FL_Left,  (Fl_Callback*)GUI_IPF::cb_HandleLeftRight, (void*)100, 0},
 {"Pan Right ",      FL_Right,  (Fl_Callback*)GUI_IPF::cb_HandleLeftRight, (void*)-100, 0},
 {"Pan U (slow) ",   FL_ALT + FL_Up, (Fl_Callback*)GUI_IPF::cb_HandleUpDown, (void*)-25, 0},
 {"Pan D (slow) ",   FL_ALT + FL_Down, (Fl_Callback*)GUI_IPF::cb_HandleUpDown, (void*)25, 0},
 {"Pan L (slow) ",   FL_ALT + FL_Left, (Fl_Callback*)GUI_IPF::cb_HandleLeftRight, (void*)25, 0},
 {"Pan R (slow)",    FL_ALT + FL_Right, (Fl_Callback*)GUI_IPF::cb_HandleLeftRight, (void*)-25, 0},
 {"Replay/Step+1",  ']', (Fl_Callback*)GUI_IPF::cb_Step, (void*)1, 0},
 {"Replay/Step-",   '[', (Fl_Callback*)GUI_IPF::cb_Step, (void*)-1, 0},
 {"Replay/Step+1",  '>', (Fl_Callback*)GUI_IPF::cb_Step, (void*)5, 0},
 {"Replay/Step-",   '<', (Fl_Callback*)GUI_IPF::cb_Step, (void*)-5, 0},
 {0}
};


//----------------------------------------------------------
// Procedure: setDataBroker

void GUI_IPF::setDataBroker(ALogDataBroker dbroker, string vname)
{
  m_dbroker = dbroker;
  m_vname   = vname;

  unsigned int aix = m_dbroker.getAixFromVName(vname);
  m_sources = m_dbroker.getBhvsInALog(aix);
  initMenuButton();

  if(m_ipf_viewer) {
    m_ipf_viewer->setDataBroker(m_dbroker, vname);
    m_ipf_viewer->setIPF_Plots(m_sources);
    m_ipf_viewer->setHelmIterPlot();
  }
}

//----------------------------------------------------------
// Procedure: setParentGUI

void GUI_IPF::setParentGUI(REPLAY_GUI* parent_gui)
{
  m_parent_gui = parent_gui;
}

//----------------------------------------------------------
// Procedure: handle
//     Notes: We want the various "Output" widgets to ignore keyboard
//            events (as they should, right?!), so we wrote a MY_Output
//            subclass to do just that. However the keyboard arrow keys
//            still seem to be grabbed by Fl_Window to change focus
//            between sub-widgets. We over-ride that here to do the 
//            panning on the image by invoking the pan callbacks. By
//            then returning (1), we've indicated that the event has
//            been handled.

int GUI_IPF::handle(int event) 
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

void GUI_IPF::setWindowLayout(string layout)
{
  if(m_ipf_viewer)
    m_ipf_viewer->resize(0, 60, w(), h()-60);

  int info_size = 10;

  Fl_Color fcolor1 = fl_rgb_color(200, 90, 90);
  if(!m_but_ipf_source) {
    m_but_ipf_source = new MY_Menu_Button(10, 30, 150, 20, "Behavior");
    m_but_ipf_source->labelsize(12);
    m_but_ipf_source->callback((Fl_Callback*)GUI_IPF::cb_SelectSource,(void*)-1);
  }

  if(!m_fld_loc_time) {
    m_fld_loc_time = new MY_Output(40, 5, 115, 20, "Time:"); 
    m_fld_loc_time->textsize(info_size); 
    m_fld_loc_time->labelsize(info_size);
  }

  if(!m_fld_ipf_iter) {
    m_fld_ipf_iter = new MY_Output(195, 5, 50, 20, "Iter:"); 
    m_fld_ipf_iter->textsize(info_size); 
    m_fld_ipf_iter->color(fcolor1); 
    m_fld_ipf_iter->labelsize(info_size);
  }

  if(!m_fld_ipf_pcs) {
    m_fld_ipf_pcs = new MY_Output(275, 5, 45, 20, "Pcs:"); 
    m_fld_ipf_pcs->textsize(info_size); 
    m_fld_ipf_pcs->labelsize(info_size);
  }

  if(!m_fld_ipf_pwt) {
    m_fld_ipf_pwt = new MY_Output(353, 5, 50, 20, "Pwt:"); 
    m_fld_ipf_pwt->textsize(info_size); 
    m_fld_ipf_pwt->labelsize(info_size);
  }

  if(!m_but_ipf_set) {
    m_but_ipf_set = new MY_Button(330, 30, 33, 20, "set");
    m_but_ipf_set->labelsize(12);
    m_but_ipf_set->callback((Fl_Callback*)GUI_IPF::cb_ToggleSet);
  }

  if(!m_but_ipf_pin) {
    m_but_ipf_pin = new MY_Button(370, 30, 33, 20, "pin");
    m_but_ipf_pin->labelsize(12);
    m_but_ipf_pin->callback((Fl_Callback*)GUI_IPF::cb_TogglePin);
  }

  if(!m_fld_ipf_dom) {
    m_fld_ipf_dom = new MY_Output(195, 30, 125, 20, "Dom:"); 
    m_fld_ipf_dom->textsize(info_size); 
    m_fld_ipf_dom->labelsize(info_size);
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
}


//----------------------------------------------------------
// Procedure: initChoicesIPF

void GUI_IPF::initChoicesIPF()
{
  if(!m_ipf_viewer)
    return;

  cb_SelectSource_i(900);
}

//----------------------------------------- HandleUpDown
inline void GUI_IPF::cb_HandleUpDown_i(int amt) 
{
  double damt = (double)(amt) / 100.0;
  m_ipf_viewer->setParam("mod_x_rotation", ((double)(-damt)));
  m_ipf_viewer->redraw();
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


//----------------------------------------- SelectSource
inline void GUI_IPF::cb_SelectSource_i(int index) {
  if(!m_ipf_viewer)
      return;
  if(index == 4900) {
    m_ipf_viewer->setSource("collective-hdgspd");
    m_but_ipf_source->label("collective");
  }
  else if(index == 4901) {
    m_ipf_viewer->setSource("collective-depth");
    m_but_ipf_source->label("collective (dep)");
  }

  if((index >= 0) && ((unsigned int)(index) < m_sources.size())) {
    m_source = m_sources[index];
    m_but_ipf_source_str = truncString(m_source, 18, "middle");
    m_but_ipf_source->label(m_but_ipf_source_str.c_str());
    m_ipf_viewer->setSource(m_source);
  }
  updateXY();

}
void GUI_IPF::cb_SelectSource(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((GUI_IPF*)(o->parent()->user_data()))->cb_SelectSource_i(val);
}

//----------------------------------------- ToggleUTC
inline void GUI_IPF::cb_ToggleUTC_i() {
  //  m_ipf_viewer->setTimeFormat("toggle");
  updateXY();
}
void GUI_IPF::cb_ToggleUTC(Fl_Widget* o) {
  ((GUI_IPF*)(o->parent()->user_data()))->cb_ToggleUTC_i();
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
  cout << "GUI_IPF::In cb_Step_i: " << val << endl;
  if(m_parent_gui) {
    cout << "sending to parent" << endl;
    m_parent_gui->steptime(val);
  }
  updateXY();
}

void GUI_IPF::cb_Step(Fl_Widget* o, int val) {
  ((GUI_IPF*)(o->parent()->user_data()))->cb_Step_i(val);
}

//----------------------------------------- UpdateXY
void GUI_IPF::updateXY() 
{
  // IPF fields
  double time     = m_ipf_viewer->getCurrTime();
  string time_str = doubleToString(time, 3);
  m_fld_loc_time->value(time_str.c_str());

  string ipf_iter_str = m_ipf_viewer->getCurrIteration();
  m_fld_ipf_iter->value(ipf_iter_str.c_str());
  string ipf_pcs_str = m_ipf_viewer->getCurrPieces();
  m_fld_ipf_pcs->value(ipf_pcs_str.c_str());
  string ipf_pwt_str = m_ipf_viewer->getCurrPriority();
  m_fld_ipf_pwt->value(ipf_pwt_str.c_str());
  string ipf_dom_str = m_ipf_viewer->getCurrDomain();
  m_fld_ipf_dom->value(ipf_dom_str.c_str());
}

//----------------------------------------------------------
// Procedure: initMenuButton

void GUI_IPF::initMenuButton() 
{
  m_but_ipf_source->clear();
  m_but_ipf_source->label("<none>");

  if(m_sources.size() > 1) {
    m_but_ipf_source->add("collective", 0, 
			  (Fl_Callback*)GUI_IPF::cb_SelectSource, 
			  (void*)4900);
    m_but_ipf_source->add("collective (dep)", 0, 
			  (Fl_Callback*)GUI_IPF::cb_SelectSource, 
			  (void*)4901);
  }

  for(unsigned int i=0; i<m_sources.size(); i++) {
    unsigned int bix = m_dbroker.getBixFromVNameBName(m_vname, m_sources[i]);
    // Use special unsigned int type having same size a pointer (void*)  
    uintptr_t ix = bix;
    if(i==0) 
      m_but_ipf_source->label(m_sources[i].c_str());

    m_but_ipf_source->add(m_sources[i].c_str(), 0, 
			  (Fl_Callback*)GUI_IPF::cb_SelectSource, (void*)ix);
  }
}
