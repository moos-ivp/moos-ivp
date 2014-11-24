/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FV_GUI.cpp                                           */
/*    DATE: May 13th 2006                                        */
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
#include <cstdlib>
#include "FV_GUI.h"
#include "MBUtils.h"

using namespace std;

//--------------------------------------------------------------
// Constructor

FV_GUI::FV_GUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) 
{
  m_model = 0;

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  m_menu_bar = new Fl_Menu_Bar(0, 0, w(), 25);
  m_menu_bar->menu(menu_);
    
  int info_size=10;

  m_viewer = new FV_Viewer(0, 30, w(), h()-60);

  m_curr_plat = new MY_Output(60, h()-25, 65, 20, "Platform:"); 
  m_curr_plat->textsize(info_size); 
  m_curr_plat->labelsize(info_size);

  m_curr_src = new MY_Output(180, h()-25, 130, 20, "Behavior:"); 
  m_curr_src->textsize(info_size); 
  m_curr_src->labelsize(info_size);

  curr_iteration = new MY_Output(345, h()-25, 50, 20, "Iter:"); 
  curr_iteration->textsize(info_size); 
  curr_iteration->labelsize(info_size);

  m_curr_pcs = new MY_Output(440, h()-25, 50, 20, "Pieces:"); 
  m_curr_pcs->textsize(info_size); 
  m_curr_pcs->labelsize(info_size);

  m_curr_pwt = new MY_Output(520, h()-25, 50, 20, "Pwt:"); 
  m_curr_pwt->textsize(info_size); 
  m_curr_pwt->labelsize(info_size);

  m_curr_domain = new MY_Output(620, h()-25, 155, 20, "Domain:"); 
  m_curr_domain->textsize(info_size); 
  m_curr_domain->labelsize(info_size);

  m_but_ipf_set = new MY_Button(w()-105, h()-25, 34, 20, "set");
  m_but_ipf_set->labelsize(12);
  m_but_ipf_set->shortcut('s');
  m_but_ipf_set->callback((Fl_Callback*)FV_GUI::cb_ToggleSet,(void*)1);

  m_but_ipf_pin = new MY_Button(w()-65, h()-25, 34, 20, "pin");
  m_but_ipf_pin->labelsize(12);
  m_but_ipf_pin->callback((Fl_Callback*)FV_GUI::cb_TogglePin,(void*)1);

  this->end();
  this->resizable(this);
  this->show();
  this->updateFields();

  string label = "Behaviors/Collective";
  m_menu_bar->add(label.c_str(), 0, (Fl_Callback*)FV_GUI::cb_BehaviorSelect, 
		  (void*)900);

  label = "Behaviors/Collective (dep)";
  m_menu_bar->add(label.c_str(), 0, (Fl_Callback*)FV_GUI::cb_BehaviorSelect, 
		  (void*)901, FL_MENU_DIVIDER);

  m_menu_bar->redraw();
}

Fl_Menu_Item FV_GUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)FV_GUI::cb_Quit, 0, 0},
 {0},

 {"Rotate/Zoom", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Rotate X- ", FL_Down,  (Fl_Callback*)FV_GUI::cb_RotateX, (void*)-1, 0},
 {"Rotate X+ ", FL_Up,  (Fl_Callback*)FV_GUI::cb_RotateX, (void*)1, 0},
 {"Rotate Z- ", FL_Left,  (Fl_Callback*)FV_GUI::cb_RotateZ, (void*)-1, 0},
 {"Rotate Z+ ", FL_Right,  (Fl_Callback*)FV_GUI::cb_RotateZ, (void*)1, 0},
 {"Toggle Frame ",   'f',  (Fl_Callback*)FV_GUI::cb_ToggleFrame, (void*)-1, FL_MENU_DIVIDER},
 {"Expand Radius ",  '}',  (Fl_Callback*)FV_GUI::cb_StretchRad, (void*)1, 0},
 {"Shrink Radius ",  '{',  (Fl_Callback*)FV_GUI::cb_StretchRad, (void*)-1, 0},
 {"Zoom In",         'i', (Fl_Callback*)FV_GUI::cb_Zoom, (void*)-1, 0},
 {"Zoom Out",        'o', (Fl_Callback*)FV_GUI::cb_Zoom, (void*)1, 0},
 {"Zoom Reset", FL_ALT+'Z', (Fl_Callback*)FV_GUI::cb_Zoom, (void*)0, 0},
 {"Lock/UnLock", 'l', (Fl_Callback*)FV_GUI::cb_ToggleLockIPF, (void*)2, 0},
 {0},

 {"Color-Map", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Default",   0, (Fl_Callback*)FV_GUI::cb_ColorMap, (void*)1, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Copper",    0, (Fl_Callback*)FV_GUI::cb_ColorMap, (void*)2, FL_MENU_RADIO},
 {"Bone",      0, (Fl_Callback*)FV_GUI::cb_ColorMap, (void*)3, FL_MENU_RADIO|FL_MENU_DIVIDER},
 {0},

 {0}
};

//----------------------------------------------------------
// Procedure: addBehaviorSource

void FV_GUI::addBehaviorSource(string bhv_source)
{
  bool found = false;
  // Use special unsigned int type having same size a pointer (void*)
  uintptr_t index = 0;
  unsigned int i, vsize = m_bhv_sources.size();
  for(i=0; i<vsize; i++) {
    if(m_bhv_sources[i] == bhv_source) {
      found = true;
      index = i;
    }
  }

  if(!found) {
    m_bhv_sources.push_back(bhv_source);
    index = m_bhv_sources.size() - 1;
  }
  
  string label = "Behaviors/" + bhv_source;
  m_menu_bar->add(label.c_str(), 0, (Fl_Callback*)FV_GUI::cb_BehaviorSelect, (void*)index);
}


//----------------------------------------------------------
// Procedure: handle
//     Notes: We want the various "Output" widgets to ignore keyboard
//            events (as they should, right?!), so we wrote a MY_Output
//            subclass to do just that. However the keyboard arrow keys
//            still seem to be grabbed by Fl_Window to change focuse
//            between sub-widgets. We over-ride that here to do the 
//            panning on the image by invoking the pan callbacks. By
//            then returning (1), we've indicated that the event has
//            been handled.

int FV_GUI::handle(int event) 
{
  switch(event) {
  case FL_KEYBOARD:
    if(Fl::event_key()==FL_Down) {
      m_but_ipf_set->activate();
      cb_RotateX_i(-1);
      return(1); 
    }
    if(Fl::event_key()==FL_Up) {
      m_but_ipf_set->activate();
      cb_RotateX_i(+1);
      return(1); 
    }
    if(Fl::event_key()==FL_Right) {
      m_but_ipf_set->activate();
      cb_RotateZ_i(+1);
      return(1); 
    }
    if(Fl::event_key()==FL_Left) {
      m_but_ipf_set->activate();
      cb_RotateZ_i(-1);
      return(1); 
    }
    if(Fl::event_key()==FL_CTRL+FL_Left) {
      return(1); 
    }
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------- BehaviorSelect
inline void FV_GUI::cb_BehaviorSelect_i(int index) {
  if((!m_model) || (index < 0))
    return;

  if((unsigned int)(index) < m_bhv_sources.size()) {
    cout << "Chosen behavior:" << m_bhv_sources[index] << endl;
    m_model->modSource(m_bhv_sources[index]);
    m_model->setCollective();
  }
  else if(index == 900) {
    cout << "Chosen behavior: collective" << endl;
    m_model->setCollective("collective-hdgspd");
  }
  else if(index == 901) {
    cout << "Chosen behavior: collective (dep)" << endl;
    m_model->setCollective("collective-depth");
  }
  updateFields();
}
void FV_GUI::cb_BehaviorSelect(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_BehaviorSelect_i(v);
}

//----------------------------------------- Zoom In
inline void FV_GUI::cb_Zoom_i(int val) {
  if(val < 0) m_viewer->setParam("mod_zoom", 1.25);
  if(val > 0) m_viewer->setParam("mod_zoom", 0.80);
}
void FV_GUI::cb_Zoom(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_Zoom_i(v);
}

//----------------------------------------- Rotate  X
inline void FV_GUI::cb_RotateX_i(int amt) {
  m_viewer->setParam("mod_x_rotation", (double)(amt));
}
void FV_GUI::cb_RotateX(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_RotateX_i(v);
}

//----------------------------------------- Rotate  Z
inline void FV_GUI::cb_RotateZ_i(int amt) {
  m_viewer->setParam("mod_z_rotation", (double)(amt));
}
void FV_GUI::cb_RotateZ(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_RotateZ_i(v);
}

//----------------------------------------- Stretch Radius
inline void FV_GUI::cb_StretchRad_i(int amt) {
  if(amt > 0) m_viewer->setParam("mod_radius", 1.05);
  if(amt < 0) m_viewer->setParam("mod_radius", 0.95);
}
void FV_GUI::cb_StretchRad(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_StretchRad_i(v);
}

//----------------------------------------- Toggle Frame
inline void FV_GUI::cb_ToggleFrame_i() {
  m_viewer->setParam("draw_frame", "toggle");
}
void FV_GUI::cb_ToggleFrame(Fl_Widget* o) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ToggleFrame_i();
}

//----------------------------------------- ToggleLockIPF
inline void FV_GUI::cb_ToggleLockIPF_i() {
  m_model->toggleLockIPF();
  if(m_model->isLocked()) {
    Fl_Color fcolor = fl_rgb_color(200, 90, 90);
    curr_iteration->color(fcolor);
  }
  else {
    Fl_Color fcolor = fl_rgb_color(255, 255, 255);
    curr_iteration->color(fcolor);
  }    
}
void FV_GUI::cb_ToggleLockIPF(Fl_Widget* o) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ToggleLockIPF_i();
}

//----------------------------------------- ColorMap
inline void FV_GUI::cb_ColorMap_i(int index) {
  string str = "default";
  if(index ==2)
    str = "copper";
  else if(index == 3)
    str = "bone";
  m_model->modColorMap(str);
  m_viewer->redraw();
}
void FV_GUI::cb_ColorMap(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ColorMap_i(v);
}

//----------------------------------------- ToggleSet
inline void FV_GUI::cb_ToggleSet_i() {
  m_but_ipf_set->deactivate();
  m_viewer->setParam("reset_view", "2");
}
void FV_GUI::cb_ToggleSet(Fl_Widget* o) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ToggleSet_i();
}

//----------------------------------------- TogglePin
inline void FV_GUI::cb_TogglePin_i() {
  m_viewer->setParam("draw_pin", "toggle");
}

void FV_GUI::cb_TogglePin(Fl_Widget* o) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_TogglePin_i();
}


//----------------------------------------- UpdateFields
void FV_GUI::updateFields() 
{
  if(!m_model)
    return;

  string source;
  if(m_model)
    source = m_model->getCurrSource();
  if(source == "")
    source = " - no function - ";
  m_curr_src->value(source.c_str());
  
  string pcs = "n/a";
  if(m_model)
    pcs = m_model->getCurrPieces();
  m_curr_pcs->value(pcs.c_str());

  string pwt = "n/a";
  if(m_model)
    pwt = m_model->getCurrPriority();
  m_curr_pwt->value(pwt.c_str());

  string domain = "n/a";
  if(m_model)
    domain = m_model->getCurrDomain();
  m_curr_domain->value(domain.c_str());

  string platform = "";
  if(m_model)
    platform = m_model->getCurrPlatform();
  m_curr_plat->value(platform.c_str());

  string iteration = intToString(m_model->getCurrIteration());
  if(m_model->isLocked())
    iteration = "<" + iteration + ">";
  curr_iteration->value(iteration.c_str());

  m_viewer->redraw();
}

//----------------------------------------- Quit
void FV_GUI::cb_Quit() {
  exit(0);
}






