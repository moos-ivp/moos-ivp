/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ULI_GUI.cpp                                          */
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
#include "ULI_GUI.h"
#include "MBUtils.h"
#include "FL/fl_ask.H"

using namespace std;

//-------------------------------------------------------------------
// Constructor

ULI_GUI::ULI_GUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) 
{
  mbar = new Fl_Menu_Bar(0, 0, 0, 0);
  mbar->menu(menu_);

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  // Init window with bogus extents. Actual extents
  // are set in the setWindowLayout() call below.
  ipf_viewer = new IvPFuncViewerX(1, 1, 1, 1);

  m_fld_utc_time = 0;
  m_fld_ipf_iter = 0;
  m_fld_ipf_pcs  = 0;
  m_fld_ipf_pwt  = 0;
  m_fld_ipf_dom  = 0;
  m_but_ipf_set  = 0;
  m_but_ipf_pin  = 0;

  m_but_ipf_source = 0;
  m_but_time_utc   = 0;

  setWindowLayout("normal");

  ipf_viewer->setClearColor("0.6,0.7,0.5");

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------

Fl_Menu_Item ULI_GUI::menu_[] = {
 {"Invisible", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Pan Up ",         FL_Up,  (Fl_Callback*)ULI_GUI::cb_HandleUpDown, (void*)-100, 0},
 {"Pan Down ",       FL_Down,  (Fl_Callback*)ULI_GUI::cb_HandleUpDown, (void*)100, 0},
 {"Pan Left ",       FL_Left,  (Fl_Callback*)ULI_GUI::cb_HandleLeftRight, (void*)100, 0},
 {"Pan Right ",      FL_Right,  (Fl_Callback*)ULI_GUI::cb_HandleLeftRight, (void*)-100, 0},
 {"Pan U (slow) ",   FL_ALT + FL_Up, (Fl_Callback*)ULI_GUI::cb_HandleUpDown, (void*)-25, 0},
 {"Pan D (slow) ",   FL_ALT + FL_Down, (Fl_Callback*)ULI_GUI::cb_HandleUpDown, (void*)25, 0},
 {"Pan L (slow) ",   FL_ALT + FL_Left, (Fl_Callback*)ULI_GUI::cb_HandleLeftRight, (void*)25, 0},
 {"Pan R (slow)",    FL_ALT + FL_Right, (Fl_Callback*)ULI_GUI::cb_HandleLeftRight, (void*)-25, 0},
 {"Replay/Step+1",  ']', (Fl_Callback*)ULI_GUI::cb_Step, (void*)1, 0},
 {"Replay/Step-",   '[', (Fl_Callback*)ULI_GUI::cb_Step, (void*)-1, 0},
 {"Replay/Step+1",  '>', (Fl_Callback*)ULI_GUI::cb_Step, (void*)5, 0},
 {"Replay/Step-",   '<', (Fl_Callback*)ULI_GUI::cb_Step, (void*)-5, 0},
 {0}
};


//-------------------------------------------------------------------
// Destructor

ULI_GUI::~ULI_GUI()
{
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

int ULI_GUI::handle(int event) 
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

void ULI_GUI::setWindowLayout(string layout)
{
  if(ipf_viewer)
    ipf_viewer->resize(0, 60, w(), h()-60);

  int info_size = 10;

  Fl_Color fcolor1 = fl_rgb_color(200, 90, 90);
  if(!m_but_ipf_source) {
    m_but_ipf_source = new MY_Menu_Button(10, 30, 150, 20, "Behavior");
    m_but_ipf_source->labelsize(12);
    m_but_ipf_source->callback((Fl_Callback*)ULI_GUI::cb_SelectSource,(void*)-1);
  }

  if(!m_fld_utc_time) {
    m_fld_utc_time = new MY_Output(40, 5, 95, 20, "Time:"); 
    m_fld_utc_time->textsize(info_size); 
    m_fld_utc_time->labelsize(info_size);
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
    m_but_ipf_set->callback((Fl_Callback*)ULI_GUI::cb_ToggleSet);
  }

  if(!m_but_ipf_pin) {
    m_but_ipf_pin = new MY_Button(370, 30, 33, 20, "pin");
    m_but_ipf_pin->labelsize(12);
    m_but_ipf_pin->callback((Fl_Callback*)ULI_GUI::cb_TogglePin);
  }

  if(!m_but_time_utc) {
    m_but_time_utc = new Fl_Check_Button(140, 5, 20, 20, "");
    m_but_time_utc->labelsize(16);
    m_but_time_utc->callback((Fl_Callback*)ULI_GUI::cb_ToggleUTC);
  }

  if(!m_fld_ipf_dom) {
    m_fld_ipf_dom = new MY_Output(195, 30, 125, 20, "Dom:"); 
    m_fld_ipf_dom->textsize(info_size); 
    m_fld_ipf_dom->labelsize(info_size);
  }
}

//----------------------------------------------------------
// Procedure: setCurrTime

void ULI_GUI::setCurrTime(double curr_time)
{
  if(!ipf_viewer)
    return;

  ipf_viewer->setTime(curr_time);
  ipf_viewer->redraw();
  updateXY();
}


//----------------------------------------------------------
// Procedure: initChoicesIPF

void ULI_GUI::initChoicesIPF()
{
  if(!ipf_viewer)
    return;

  cb_SelectSource_i(900);
}

//----------------------------------------- HandleUpDown
inline void ULI_GUI::cb_HandleUpDown_i(int amt) 
{
  double damt = (double)(amt) / 100.0;
  ipf_viewer->setParam("mod_x_rotation", ((double)(-damt)));
  ipf_viewer->redraw();
  updateXY();
}
void ULI_GUI::cb_HandleUpDown(Fl_Widget* o, int v) {
  ((ULI_GUI*)(o->parent()->user_data()))->cb_HandleUpDown_i(v);
}

//----------------------------------------- HandleLeftRight
inline void ULI_GUI::cb_HandleLeftRight_i(int amt) 
{
  double damt = (double)(amt) / 100.0;
  ipf_viewer->setParam("mod_z_rotation", ((double)(damt)));
  ipf_viewer->redraw();
  updateXY();
}
void ULI_GUI::cb_HandleLeftRight(Fl_Widget* o, int v) {
  ((ULI_GUI*)(o->parent()->user_data()))->cb_HandleLeftRight_i(v);
}


//----------------------------------------- Zoom In
inline void ULI_GUI::cb_Zoom_i(int val) {
  if(val < 0)
    ipf_viewer->setParam("mod_zoom", 1.05); 
  else if(val > 0)
    ipf_viewer->setParam("mod_zoom", 0.95); 
  else
    ipf_viewer->setParam("set_zoom", 1.0); 
  ipf_viewer->redraw();
}


//----------------------------------------- SelectSource
inline void ULI_GUI::cb_SelectSource_i(int index) {
  if(!ipf_viewer)
      return;
  if(index == 900) {
    ipf_viewer->setSource("collective-hdgspd");
    m_but_ipf_source->label("collective");
  }
  else if(index == 901) {
    ipf_viewer->setSource("collective-depth");
    m_but_ipf_source->label("collective (dep)");
  }

  if((index >= 0) && ((unsigned int)(index) < m_sources.size())) {
    m_source = m_sources[index];
    m_but_ipf_source_str = truncString(m_source, 18, "middle");
    m_but_ipf_source->label(m_but_ipf_source_str.c_str());
    ipf_viewer->setSource(m_source);
  }
  updateXY();

}
void ULI_GUI::cb_SelectSource(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((ULI_GUI*)(o->parent()->user_data()))->cb_SelectSource_i(val);
}

//----------------------------------------- ToggleUTC
inline void ULI_GUI::cb_ToggleUTC_i() {
  ipf_viewer->setTimeFormat("toggle");
  updateXY();
}
void ULI_GUI::cb_ToggleUTC(Fl_Widget* o) {
  ((ULI_GUI*)(o->parent()->user_data()))->cb_ToggleUTC_i();
}

//----------------------------------------- ToggleSet
inline void ULI_GUI::cb_ToggleSet_i() {
  ipf_viewer->setParam("reset_view", "2");
}
void ULI_GUI::cb_ToggleSet(Fl_Widget* o) {
  ((ULI_GUI*)(o->parent()->user_data()))->cb_ToggleSet_i();
}

//----------------------------------------- TogglePin
inline void ULI_GUI::cb_TogglePin_i() {
  ipf_viewer->setParam("draw_pin", "toggle");
}

void ULI_GUI::cb_TogglePin(Fl_Widget* o) {
  ((ULI_GUI*)(o->parent()->user_data()))->cb_TogglePin_i();
}

//----------------------------------------- Step
inline void ULI_GUI::cb_Step_i(int val) {
  ipf_viewer->incrementIter(val);
  updateXY();
}

void ULI_GUI::cb_Step(Fl_Widget* o, int val) {
  ((ULI_GUI*)(o->parent()->user_data()))->cb_Step_i(val);
}

//----------------------------------------- UpdateXY
void ULI_GUI::updateXY() 
{
  // IPF fields
  double time     = ipf_viewer->getCurrTime();
  string time_str = doubleToString(time, 2);
  m_fld_utc_time->value(time_str.c_str());

  string ipf_iter_str = ipf_viewer->getCurrIteration();
  m_fld_ipf_iter->value(ipf_iter_str.c_str());
  string ipf_pcs_str = ipf_viewer->getCurrPieces();
  m_fld_ipf_pcs->value(ipf_pcs_str.c_str());
  string ipf_pwt_str = ipf_viewer->getCurrPriority();
  m_fld_ipf_pwt->value(ipf_pwt_str.c_str());
  string ipf_dom_str = ipf_viewer->getCurrDomain();
  m_fld_ipf_dom->value(ipf_dom_str.c_str());
}

//----------------------------------------------------------
// Procedure: addIPF_Plot

void ULI_GUI::addIPF_Plot(const IPF_TPlot& ipf_plot)
{
  if(!ipf_viewer)
    return;

  string vname  = ipf_plot.getVName();

  if(m_vname == "")
    m_vname = vname;

  if(m_vname != vname)
    return;

  string source = tolower(ipf_plot.getSource());
  string tag    = tolower(vname) + " : " + source;

  if(!vectorContains(m_sources, source))
    m_sources.push_back(source);

  ipf_viewer->addIPF_Plot(ipf_plot);    
  updateSourceMenuButton();
}


//----------------------------------------------------------
// Procedure: updateSourceMenuButton
//   Purpose: Each time the vehicle is reset for the ipf viewer, the 
//            list of available sources is recalculated and the source
//            pull-down menu is reset with the new values.

void ULI_GUI::updateSourceMenuButton() 
{
  m_but_ipf_source->clear();
  m_but_ipf_source->label("<none>");

  if(m_sources.size() > 1) {
    m_but_ipf_source->add("collective", 0, 
			  (Fl_Callback*)ULI_GUI::cb_SelectSource, 
			  (void*)900);
    m_but_ipf_source->add("collective (dep)", 0, 
			  (Fl_Callback*)ULI_GUI::cb_SelectSource, 
			  (void*)901);
  }

  // Use special unsigned int type having same size a pointer (void*)  
  uintptr_t i, vsize = m_sources.size();
  for(i=0; i<vsize; i++) {
    if(i==0) 
      m_but_ipf_source->label(m_sources[i].c_str());

    m_but_ipf_source->add(m_sources[i].c_str(), 0, 
			  (Fl_Callback*)ULI_GUI::cb_SelectSource, 
			  (void*)i);
  }
}





