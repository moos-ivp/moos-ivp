/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GUI_Encounters.cpp                                   */
/*    DATE: Jan 11th, 2016                                       */
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
#include "GUI_Encounters.h"
#include "MBUtils.h"
#include "REPLAY_GUI.h"
#include "FL/fl_ask.H"

using namespace std;

//-------------------------------------------------------------------
// Constructor

GUI_Encounters::GUI_Encounters(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) 
{
  mbar = new Fl_Menu_Bar(0, 0, 0, 0);
  mbar->menu(menu_);

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();
  this->size_range(375,250, 1200,1200, 0,0, 1);

  m_replay_warp_msg = "(PAUSED)";
  m_parent_gui = 0;
  m_fullscreen = false;
  m_mutable_text_size = 10;

  this->initWidgets();
  this->resizeWidgetsShape();
  this->resizeWidgetsText();
  this->redraw();

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
// Destructor: Must carefully implement since master GUI may create
//             and destroy instances of this GUI all in same session

GUI_Encounters::~GUI_Encounters()
{
  delete(m_eviewer);

  delete(m_fld_loc_time);
  delete(m_fld_ipf_iter);
}


//---------------------------------------------------------------------------
// Procedure: initWidgets()

void GUI_Encounters::initWidgets()
{
  Fl_Color fcolor1 = fl_rgb_color(200, 90, 90);

  m_eviewer = new EncounterViewer(0, 0, 1, 1);
  m_eviewer->setClearColor("0.6,0.7,0.5");

  m_fld_loc_time = new Fl_Output(0, 0, 1, 1, "Time:"); 
  m_fld_loc_time->clear_visible_focus();

  m_fld_ipf_iter = new Fl_Output(0, 0, 1, 1, "Iter:"); 
  m_fld_ipf_iter->clear_visible_focus();
  m_fld_ipf_iter->color(fcolor1); 

}

//---------------------------------------------------------------------------
// Procedure: resizeWidgetsShape()

void GUI_Encounters::resizeWidgetsShape()
{
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

  m_eviewer->EncounterViewer::resize(ipf_x, ipf_y, ipf_wid, ipf_hgt);

  if(m_fullscreen) 
    return;

  int time_x = 40;
  int time_y = 5;
  int time_wid = (80.0/550.0)*w();
  int time_hgt = 20;
  m_fld_loc_time->resize(time_x, time_y, time_wid, time_hgt); 

  int iter_x = time_x + time_wid + 40;
  int iter_y = 5;
  int iter_wid = (50.0/550.0)*w();
  int iter_hgt = 20;
  m_fld_ipf_iter->resize(iter_x, iter_y, iter_wid, iter_hgt); 

}

//---------------------------------------------------------------------------
// Procedure: resizeWidgetsText()

void GUI_Encounters::resizeWidgetsText()
{
  if(m_fullscreen) 
    return;

  int small_wid = 440;  // Below which we start to shrink things
  int info_size = 10;
  int blab_size = 12;

  if(w() < small_wid)
    blab_size = 10;

  m_fld_loc_time->textsize(info_size); 
  m_fld_loc_time->labelsize(info_size);

  m_fld_ipf_iter->textsize(info_size); 
  m_fld_ipf_iter->labelsize(info_size);
}

//-------------------------------------------------------------------

Fl_Menu_Item GUI_Encounters::menu_[] = {
 {"Invisible", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Pan Up ",    FL_Up,  (Fl_Callback*)GUI_Encounters::cb_HandleUpDown, (void*)-100, 0},
 {"Pan Down ",  FL_Down,  (Fl_Callback*)GUI_Encounters::cb_HandleUpDown, (void*)100, 0},
 {"Pan Left ",  FL_Left,  (Fl_Callback*)GUI_Encounters::cb_HandleLeftRight, (void*)100, 0},
 {"Pan Right ", FL_Right,  (Fl_Callback*)GUI_Encounters::cb_HandleLeftRight, (void*)-100, 0},
 {"Pan U (slow) ", FL_ALT + FL_Up, (Fl_Callback*)GUI_Encounters::cb_HandleUpDown, (void*)-25, 0},
 {"Pan D (slow) ", FL_ALT + FL_Down, (Fl_Callback*)GUI_Encounters::cb_HandleUpDown, (void*)25, 0},
 {"Pan L (slow) ", FL_ALT + FL_Left, (Fl_Callback*)GUI_Encounters::cb_HandleLeftRight, (void*)25, 0},
 {"Pan R (slow)",  FL_ALT + FL_Right, (Fl_Callback*)GUI_Encounters::cb_HandleLeftRight, (void*)-25, 0},
 {"Replay/Step Ahead 0.1 secs", FL_CTRL+']', (Fl_Callback*)GUI_Encounters::cb_Step, (void*)100, 0},
 {"Replay/Step Back 0.1 secs", FL_CTRL+'[', (Fl_Callback*)GUI_Encounters::cb_Step, (void*)-100, 0},
 {"Replay/Step Ahead 1 sec",  ']', (Fl_Callback*)GUI_Encounters::cb_Step, (void*)1000, 0},
 {"Replay/Step Back  1 sec",  '[', (Fl_Callback*)GUI_Encounters::cb_Step, (void*)-1000, 0},
 {"Replay/Step Ahead 5 secs", '}', (Fl_Callback*)GUI_Encounters::cb_Step, (void*)5000, 0},
 {"Replay/Step Back  5 secs", '{', (Fl_Callback*)GUI_Encounters::cb_Step, (void*)-5000, 0},
 {0}
};


//----------------------------------------------------------
// Procedure: setDataBroker

void GUI_Encounters::setDataBroker(ALogDataBroker dbroker, string vname)
{
  m_dbroker = dbroker;
  m_vname   = vname;

  unsigned int aix = m_dbroker.getAixFromVName(vname);
  m_sources = m_dbroker.getBhvsInALog(aix);

  if(!m_eviewer)
    return;

  m_eviewer->setDataBroker(m_dbroker, vname);
}

//----------------------------------------------------------
// Procedure: setParentGUI

void GUI_Encounters::setParentGUI(REPLAY_GUI* parent_gui)
{
  m_parent_gui = parent_gui;
  this->redraw();
}

//----------------------------------------------------------
// Procedure: resize

void GUI_Encounters::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgetsShape();
  resizeWidgetsText();
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

int GUI_Encounters::handle(int event) 
{
  switch(event) {
  case FL_KEYDOWN:
    if(Fl::event_key() == 32) 
      m_parent_gui->streaming(2);
    else if(Fl::event_key() == 'f') 
      toggleFullScreen();
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

void GUI_Encounters::setCurrTime(double curr_time)
{
  if(!m_eviewer)
    return;

  m_eviewer->setTime(curr_time);
  m_eviewer->redraw();
  updateXY();
}


//----------------------------------------- HandleUpDown
inline void GUI_Encounters::cb_HandleUpDown_i(int amt) 
{
#if 0
  double damt = (double)(amt) / 100.0;
  m_ipf_viewer->setParam("mod_x_rotation", ((double)(-damt)));
  m_ipf_viewer->redraw();
  redraw();
  updateXY();
#endif
}
void GUI_Encounters::cb_HandleUpDown(Fl_Widget* o, int v) {
  ((GUI_Encounters*)(o->parent()->user_data()))->cb_HandleUpDown_i(v);
}

//----------------------------------------- HandleLeftRight
inline void GUI_Encounters::cb_HandleLeftRight_i(int amt) 
{
#if 0
  double damt = (double)(amt) / 100.0;
  m_ipf_viewer->setParam("mod_z_rotation", ((double)(damt)));
  m_ipf_viewer->redraw();
  redraw();
  updateXY();
#endif
}
void GUI_Encounters::cb_HandleLeftRight(Fl_Widget* o, int v) {
  ((GUI_Encounters*)(o->parent()->user_data()))->cb_HandleLeftRight_i(v);
}


//----------------------------------------- Zoom In
inline void GUI_Encounters::cb_Zoom_i(int val) {
#if 0
  if(val < 0)
    m_ipf_viewer->setParam("mod_zoom", 1.05); 
  else if(val > 0)
    m_ipf_viewer->setParam("mod_zoom", 0.95); 
  else
    m_ipf_viewer->setParam("set_zoom", 1.0); 
  m_ipf_viewer->redraw();
#endif
}

//----------------------------------------- Step
inline void GUI_Encounters::cb_Step_i(int val) {
  if(m_parent_gui)
    m_parent_gui->steptime(val);
  updateXY();
}

void GUI_Encounters::cb_Step(Fl_Widget* o, int val) {
  ((GUI_Encounters*)(o->parent()->user_data()))->cb_Step_i(val);
}

//----------------------------------------- FullScreen
void GUI_Encounters::toggleFullScreen() 
{
#if 0
  m_fullscreen = !m_fullscreen;
  
  if(m_fullscreen) {
    m_fld_loc_time->hide();
    m_fld_ipf_iter->hide();
    resizeWidgetsShape();
    redraw();
  }
  else {
    m_fld_loc_time->show();
    m_fld_ipf_iter->show();
    resizeWidgetsShape();
    resizeWidgetsText();
    updateXY();
    redraw();
  }
#endif
}

//----------------------------------------------------------
// Procedure: updateMutableTextSize()

void GUI_Encounters::updateMutableTextSize(string val) 
{
#if 0
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
  
  m_ipf_viewer->setMutableTextSize(m_mutable_text_size);
  m_ipf_viewer->redraw();
#endif
}

//-------------------------------------------------------
// Procedure: updateXY()

void GUI_Encounters::updateXY() 
{
#if 0
  string title = m_vname + " " + m_replay_warp_msg;
  copy_label(title.c_str());

  if(m_fullscreen)
    return;

  // IPF fields
  double time     = m_ipf_viewer->getCurrTime();
  string time_str = doubleToString(time, 3);
  m_fld_loc_time->value(time_str.c_str());

  string ipf_iter_str = m_ipf_viewer->getCurrIteration();
  m_fld_ipf_iter->value(ipf_iter_str.c_str());
#endif
}

