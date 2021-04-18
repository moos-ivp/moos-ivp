/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
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

FV_GUI::FV_GUI(int wid, int hgt, const char *label)
  : Common_IPF_GUI(wid, hgt, label) 
{
  m_model = 0;

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();
  this->size_range(850,550, 1400,1000, 0,0, 1);
  
  augmentMenu();
  
  initWidgets();
  resizeWidgetsShape();
  resizeWidgetsText();

  m_viewer = (Common_IPFViewer*)(m_fv_viewer);
  
  this->end();
  this->resizable(this);
  this->show();
  this->updateFields();

  m_menubar->redraw();
}

//----------------------------------------------------------
// Procedure: initWidgets()

void FV_GUI::initWidgets()
{
  m_fv_viewer = new FV_Viewer(0, 0, 1, 1);

  m_fld_curr_plat = new Fl_Output(0, 0, 1, 1, "Platform:"); 
  m_fld_curr_plat->clear_visible_focus();

  m_fld_curr_src = new Fl_Output(0, 0, 1, 1, "Behavior:"); 
  m_fld_curr_src->clear_visible_focus();

  m_fld_curr_iter = new Fl_Output(0, 0, 1, 1, "Iter:"); 
  m_fld_curr_iter->clear_visible_focus();

  m_fld_curr_pcs = new Fl_Output(0, 0, 1, 1, "Pieces:"); 
  m_fld_curr_pcs->clear_visible_focus();

  m_fld_curr_pwt = new Fl_Output(0, 0, 1, 1, "Pwt:"); 
  m_fld_curr_pwt->clear_visible_focus();

  m_fld_curr_domain = new Fl_Output(0, 0, 1, 1, "Domain:"); 
  m_fld_curr_domain->clear_visible_focus();

  m_but_ipf_lock = new Fl_Button(0, 0, 1, 1, "lock");
  m_but_ipf_lock->shortcut(FL_ALT+'l');
  m_but_ipf_lock->clear_visible_focus();
  m_but_ipf_lock->callback((Fl_Callback*)FV_GUI::cb_ToggleLockIPF,(void*)1);

  m_but_ipf_set = new Fl_Button(0, 0, 1, 1, "set");
  m_but_ipf_set->shortcut('s');
  m_but_ipf_set->clear_visible_focus();
  m_but_ipf_set->callback((Fl_Callback*)FV_GUI::cb_ToggleSet,(void*)1);

  m_but_ipf_pin = new Fl_Button(0, 0, 1, 1, "pin");
  m_but_ipf_pin->clear_visible_focus();
  m_but_ipf_pin->callback((Fl_Callback*)FV_GUI::cb_TogglePin,(void*)1);

}

//---------------------------------------------------------------------------
// Procedure: resizeWidgetsShape()

void FV_GUI::resizeWidgetsShape()
{
  m_fv_viewer->resize(0, 30, w(), h()-60);

  int extra_wid = w() - m_start_wid;
  int field_hgt = 20;
  int row1      = h()-25;
  
  int plat_x = 50;
  int plat_y = row1;
  int plat_wid = 65;
  m_fld_curr_plat->resize(plat_x, plat_y, plat_wid, field_hgt);
  
  int src_x = plat_x + plat_wid + 55;
  int src_y = row1;
  int src_wid = 120 + (extra_wid/2);
  m_fld_curr_src->resize(src_x, src_y, src_wid, field_hgt);
  
  int iter_x = src_x + src_wid + 30;
  int iter_y = row1;
  int iter_wid = 40;
  m_fld_curr_iter->resize(iter_x, iter_y, iter_wid, field_hgt);
  
  int pcs_x = iter_x + iter_wid + 45;
  int pcs_y = row1;
  int pcs_wid = 40;
  m_fld_curr_pcs->resize(pcs_x, pcs_y, pcs_wid, field_hgt);
  
  int pwt_x = pcs_x + pcs_wid + 35;
  int pwt_y = row1;
  int pwt_wid = 40;
  m_fld_curr_pwt->resize(pwt_x, pwt_y, pwt_wid, field_hgt);
  
  int dom_x = pwt_x + pwt_wid + 55;
  int dom_y = row1;
  int dom_wid = 100 + (extra_wid/2);
  m_fld_curr_domain->resize(dom_x, dom_y, dom_wid, field_hgt);
  
  int lock_x = dom_x + dom_wid + 10;
  int lock_y = row1;
  int lock_wid = 40;
  m_but_ipf_lock->resize(lock_x, lock_y, lock_wid, field_hgt);

  int set_x = lock_x + lock_wid + 10;
  int set_y = row1;
  int set_wid = 40;
  m_but_ipf_set->resize(set_x, set_y, set_wid, field_hgt);
  
  int pin_x = set_x + set_wid + 10;
  int pin_y = row1;
  int pin_wid = 40;
  m_but_ipf_pin->resize(pin_x, pin_y, pin_wid, field_hgt);
  
}

//---------------------------------------------------------------------------
// Procedure: resizeWidgetsText()

void FV_GUI::resizeWidgetsText()
{
  int info_size=10;

  m_fld_curr_plat->textsize(info_size); 
  m_fld_curr_plat->labelsize(info_size);

  m_fld_curr_src->textsize(info_size); 
  m_fld_curr_src->labelsize(info_size);

  m_fld_curr_iter->textsize(info_size); 
  m_fld_curr_iter->labelsize(info_size);

  m_fld_curr_pcs->textsize(info_size); 
  m_fld_curr_pcs->labelsize(info_size);

  m_fld_curr_pwt->textsize(info_size); 
  m_fld_curr_pwt->labelsize(info_size);

  m_fld_curr_domain->textsize(info_size); 
  m_fld_curr_domain->labelsize(info_size);

  m_but_ipf_lock->labelsize(12);
  m_but_ipf_set->labelsize(12);
  m_but_ipf_pin->labelsize(12);
}


//---------------------------------------------------------- 
// Procedure: resize   

void FV_GUI::resize(int x, int y, int wid, int hgt)
{
  Fl_Window::resize(x, y, wid, hgt);
  resizeWidgetsShape();
  resizeWidgetsText();
}


//----------------------------------------------------------
// Procedure: augmentMenu()

void FV_GUI::augmentMenu()
{
  m_menubar->add("RotateZoom/Expand Radius ",  '}',
		 (Fl_Callback*)FV_GUI::cb_StretchRad, (void*)1, 0);
  m_menubar->add("RotateZoom/Shrink Radius ",  '{',
		 (Fl_Callback*)FV_GUI::cb_StretchRad, (void*)-1, 0);
  m_menubar->add("RotateZoom/Lock/UnLock", FL_ALT+'l',
		 (Fl_Callback*)FV_GUI::cb_ToggleLockIPF, (void*)2, 0);
  
  m_menubar->add("Color-Map/Default", 0,
		 (Fl_Callback*)FV_GUI::cb_ColorMap, (void*)1,
		 FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("Color-Map/Copper", 0,
		 (Fl_Callback*)FV_GUI::cb_ColorMap, (void*)2,
		 FL_MENU_RADIO);
  m_menubar->add("Color-Map/Bone", 0,
		 (Fl_Callback*)FV_GUI::cb_ColorMap, (void*)3,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("Behaviors/Collective", 0,
		 (Fl_Callback*)FV_GUI::cb_BehaviorSelect, (void*)900);
  m_menubar->add("Behaviors/Collective (dep)", 0,
		 (Fl_Callback*)FV_GUI::cb_BehaviorSelect, (void*)901,
		 FL_MENU_DIVIDER);
}

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
  m_menubar->add(label.c_str(), 0,
		 (Fl_Callback*)FV_GUI::cb_BehaviorSelect, (void*)index);
}


//----------------------------------------------------------
// Procedure: handle
//     Notes: We want the various "Output" widgets to ignore keyboard
//            events (as they should, right?!), so we wrote a Fl_Output
//            subclass to do just that. However the keyboard arrow keys
//            still seem to be grabbed by Fl_Window to change focuse
//            between sub-widgets. We over-ride that here to do the 
//            panning on the image by invoking the pan callbacks. By
//            then returning (1), we've indicated that the event has
//            been handled.

int FV_GUI::handle(int event) 
{
#if 0
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
#endif
  return(Fl_Window::handle(event));
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

//----------------------------------------- Stretch Radius
inline void FV_GUI::cb_StretchRad_i(int amt) {
  if(amt > 0)
    m_fv_viewer->setParam("mod_radius", 1.05);
  if(amt < 0)
    m_fv_viewer->setParam("mod_radius", 0.95);
}
void FV_GUI::cb_StretchRad(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_StretchRad_i(v);
}

//----------------------------------------- ToggleLockIPF
inline void FV_GUI::cb_ToggleLockIPF_i() {
  m_model->toggleLockIPF();
  if(m_model->isLocked()) {
    Fl_Color fcolor = fl_rgb_color(200, 90, 90);
    m_fld_curr_iter->color(fcolor);
  }
  else {
    Fl_Color fcolor = fl_rgb_color(255, 255, 255);
    m_fld_curr_iter->color(fcolor);
  }    
}
void FV_GUI::cb_ToggleLockIPF(Fl_Widget* o) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ToggleLockIPF_i();
}

//----------------------------------------- ToggleSet
inline void FV_GUI::cb_ToggleSet_i() {
  m_but_ipf_set->deactivate();
  m_fv_viewer->setParam("reset_view", "2");
}
void FV_GUI::cb_ToggleSet(Fl_Widget* o) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ToggleSet_i();
}

//----------------------------------------- TogglePin
inline void FV_GUI::cb_TogglePin_i() {
  m_fv_viewer->setParam("draw_pin", "toggle");
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
  m_fld_curr_src->value(source.c_str());
  
  string pcs = "n/a";
  if(m_model)
    pcs = m_model->getCurrPieces();
  m_fld_curr_pcs->value(pcs.c_str());

  string pwt = "n/a";
  if(m_model)
    pwt = m_model->getCurrPriority();
  m_fld_curr_pwt->value(pwt.c_str());

  string domain = "n/a";
  if(m_model)
    domain = m_model->getCurrDomain();
  m_fld_curr_domain->value(domain.c_str());

  string platform = "";
  if(m_model)
    platform = m_model->getCurrPlatform();
  m_fld_curr_plat->value(platform.c_str());

  string iteration = intToString(m_model->getCurrIteration());
  if(m_model->isLocked())
    iteration = "<" + iteration + ">";
  m_fld_curr_iter->value(iteration.c_str());

  m_fv_viewer->redraw();
}





