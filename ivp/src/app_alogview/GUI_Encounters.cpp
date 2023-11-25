/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
  this->size_range(475,240, 1000,1000, 0,0, 1);

  m_replay_warp_msg = "(PAUSED)";
  m_parent_gui = 0;
  m_fullscreen = false;

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
  delete(m_fld_encounters);

  delete(m_fld_min_cpa);
  delete(m_fld_min_eff);
  delete(m_fld_avg_cpa);
  delete(m_fld_avg_eff);

  delete(m_fld_collision_range);
  delete(m_fld_near_miss_range);
  delete(m_fld_encounter_range);

  delete(m_but_draw_mineff);
  delete(m_but_draw_avgeff);
  delete(m_but_draw_mincpa);
  delete(m_but_draw_avgcpa);
  delete(m_but_show_allpts);
}


//---------------------------------------------------------------------------
// Procedure: initWidgets()

void GUI_Encounters::initWidgets()
{
  Fl_Color fcolor1 = fl_rgb_color(200, 90, 90);

  m_eviewer = new EncounterViewer(0, 0, 1, 1);
  m_eviewer->setClearColor("0.95,0.95,0.95");
  m_eviewer->setOwningGUI(this);
  
  m_fld_loc_time = new Fl_Output(0, 0, 1, 1, "Time:"); 
  m_fld_loc_time->clear_visible_focus();

  m_fld_encounters = new Fl_Output(0, 0, 1, 1, "Encs:"); 
  m_fld_encounters->clear_visible_focus();
  m_fld_encounters->color(fcolor1); 

  m_fld_min_cpa = new Fl_Output(0, 0, 1, 1, ""); 
  m_fld_min_cpa->clear_visible_focus();
  m_fld_min_eff = new Fl_Output(0, 0, 1, 1, ""); 
  m_fld_min_eff->clear_visible_focus();

  m_fld_avg_cpa = new Fl_Output(0, 0, 1, 1, ""); 
  m_fld_avg_cpa->clear_visible_focus();
  m_fld_avg_eff = new Fl_Output(0, 0, 1, 1, ""); 
  m_fld_avg_eff->clear_visible_focus();

  m_fld_collision_range = new Fl_Output(0, 0, 1, 1, "Collision\n Range:"); 
  m_fld_collision_range->clear_visible_focus();
  m_fld_near_miss_range = new Fl_Output(0, 0, 1, 1, "NearMiss\n Range:"); 
  m_fld_near_miss_range->clear_visible_focus();
  m_fld_encounter_range = new Fl_Output(0, 0, 1, 1, "Encounter\n Range:"); 
  m_fld_encounter_range->clear_visible_focus();

  m_but_draw_mineff = new Fl_Check_Button(0, 0, 1, 1, "Min\nEFF:");
  m_but_draw_mineff->clear_visible_focus();
  m_but_draw_mineff->callback((Fl_Callback*)GUI_Encounters::cb_SelectMinEff, (void*)0);

  m_but_draw_avgeff = new Fl_Check_Button(0, 0, 1, 1, "Avg\nEFF:");
  m_but_draw_avgeff->clear_visible_focus();
  m_but_draw_avgeff->callback((Fl_Callback*)GUI_Encounters::cb_SelectAvgEff, (void*)0);

  m_but_draw_mincpa = new Fl_Check_Button(0, 0, 1, 1, "Min\nCPA:");
  m_but_draw_mincpa->clear_visible_focus();
  m_but_draw_mincpa->callback((Fl_Callback*)GUI_Encounters::cb_SelectMinCPA, (void*)0);

  m_but_draw_avgcpa = new Fl_Check_Button(0, 0, 1, 1, "Avg\nCPA:");
  m_but_draw_avgcpa->clear_visible_focus();
  m_but_draw_avgcpa->callback((Fl_Callback*)GUI_Encounters::cb_SelectAvgCPA, (void*)0);

  m_but_show_allpts = new Fl_Check_Button(0, 0, 1, 1, "AllPts");
  m_but_show_allpts->clear_visible_focus();
  //  m_but_show_allpts->shortcut('p');
  m_but_show_allpts->callback((Fl_Callback*)GUI_Encounters::cb_SelectShowPts, (void*)0);

  // The "current encounter" awidgets
  m_fld_curr_id = new Fl_Output(0, 0, 1, 1, "id="); 
  m_fld_curr_id->clear_visible_focus();

  m_fld_curr_cpa = new Fl_Output(0, 0, 1, 1, "cpa="); 
  m_fld_curr_cpa->clear_visible_focus();

  m_fld_curr_eff = new Fl_Output(0, 0, 1, 1, "eff="); 
  m_fld_curr_eff->clear_visible_focus();

  m_fld_curr_time = new Fl_Output(0, 0, 1, 1, "time="); 
  m_fld_curr_time->clear_visible_focus();

  m_fld_curr_contact = new Fl_Output(0, 0, 1, 1, "contact="); 
  m_fld_curr_contact->clear_visible_focus();

  m_but_curr_go = new Fl_Button(0, 0, 1, 1, "Go"); 
  m_but_curr_go->clear_visible_focus();
  m_but_curr_go->callback((Fl_Callback*)GUI_Encounters::cb_SelectGo, (void*)0);
}

//---------------------------------------------------------------------------
// Procedure: resizeWidgetsShape()

void GUI_Encounters::resizeWidgetsShape()
{
  if(m_fullscreen)
    m_eviewer->EncounterViewer::resize(0, 0, w(), h());
  else
    m_eviewer->EncounterViewer::resize(0, 0+100, w(), h()-100);

  if(m_fullscreen) 
    return;

  int fld_hgt = 20;
  int time_x = 40;
  int time_y = 5;
  int time_wid = (80.0/550.0)*w();
  m_fld_loc_time->resize(time_x, time_y, time_wid, fld_hgt); 

  int enc_x = time_x + time_wid + 40;
  int enc_y = 5;
  int enc_wid = (50.0/550.0)*w();
  m_fld_encounters->resize(enc_x, enc_y, enc_wid, fld_hgt); 

  int show_x = enc_x + enc_wid + 30;
  int show_y = 5;
  int show_wid = 50;
  m_but_show_allpts->resize(show_x, show_y, show_wid, fld_hgt); 

  // Collision Range Field
  int krng_x = 50;
  int krng_y = 35;
  int krng_wid = 35;
  m_fld_collision_range->resize(krng_x, krng_y, krng_wid, fld_hgt); 

  // NearMiss Range Field
  int nrng_x = krng_x + krng_wid + 55;
  int nrng_y = 35;
  int nrng_wid = 35;
  m_fld_near_miss_range->resize(nrng_x, nrng_y, nrng_wid, fld_hgt); 

  // Encounter Range Field
  int erng_x = nrng_x + nrng_wid + 55;
  int erng_y = 35;
  int erng_wid = 35;
  m_fld_encounter_range->resize(erng_x, erng_y, erng_wid, fld_hgt); 
  
  // Min CPA field and checkbox
  int cmin_x = w() - 45;
  int cmin_y = 5;
  int cmin_wid = 40;
  m_fld_min_cpa->resize(cmin_x, cmin_y, cmin_wid, fld_hgt); 
  
  int mcpa_x = cmin_x - 45;
  int mcpa_y = 5;
  int mcpa_wid = 50;
  m_but_draw_mincpa->resize(mcpa_x, mcpa_y, mcpa_wid, fld_hgt); 

  // Min Efficiency field and checkbox
  int fmin_x = w() - 45;
  int fmin_y = 35;
  int fmin_wid = 40;
  m_fld_min_eff->resize(fmin_x, fmin_y, fmin_wid, fld_hgt); 
   
  int meff_x = fmin_x - 45;
  int meff_y = 35;
  int meff_wid = 50;
  m_but_draw_mineff->resize(meff_x, meff_y, meff_wid, fld_hgt); 

  // Avg CPA field and checkbox
  int amin_x = mcpa_x - 55;
  int amin_y = 5;
  int amin_wid = 40;
  m_fld_avg_cpa->resize(amin_x, amin_y, amin_wid, fld_hgt); 
  
  int acpa_x = amin_x - 45;
  int acpa_y = 5;
  int acpa_wid = 60;
  m_but_draw_avgcpa->resize(acpa_x, acpa_y, acpa_wid, fld_hgt); 

  // Avg Efficiency field and checkbox
  int kmin_x = meff_x - 55;
  int kmin_y = 35;
  int kmin_wid = 40;
  m_fld_avg_eff->resize(kmin_x, kmin_y, kmin_wid, fld_hgt); 
   
  int aeff_x = kmin_x - 45;
  int aeff_y = 35;
  int aeff_wid = 60;
  m_but_draw_avgeff->resize(aeff_x, aeff_y, aeff_wid, fld_hgt); 

  // Current Contact Fields (Third Row)
  int jgo_x = 5;
  int jgo_y = 70;
  int jgo_wid = 30;
  m_but_curr_go->resize(jgo_x, jgo_y, jgo_wid, fld_hgt); 

  int jid_x = jgo_x + jgo_wid + 30;
  int jid_y = 70;
  int jid_wid = w()/10;
  m_fld_curr_id->resize(jid_x, jid_y, jid_wid, fld_hgt); 

  int jtim_x = jid_x + jid_wid + 40;
  int jtim_y = 70;
  int jtim_wid = w()/10;
  m_fld_curr_time->resize(jtim_x, jtim_y, jtim_wid, fld_hgt); 

  int jcpa_x = jtim_x + jtim_wid + 40;
  int jcpa_y = 70;
  int jcpa_wid = w()/10;
  m_fld_curr_cpa->resize(jcpa_x, jcpa_y, jcpa_wid, fld_hgt); 

  int jeff_x = jcpa_x + jcpa_wid + 35;
  int jeff_y = 70;
  int jeff_wid = w()/10;
  m_fld_curr_eff->resize(jeff_x, jeff_y, jeff_wid, fld_hgt); 

  int jcon_x = jeff_x + jeff_wid + 55;
  int jcon_y = 70;
  int jcon_wid = w() - (jcon_x + 5);
  m_fld_curr_contact->resize(jcon_x, jcon_y, jcon_wid, fld_hgt); 
}


//---------------------------------------------------------------------------
// Procedure: resizeWidgetsText()

void GUI_Encounters::resizeWidgetsText()
{
  if(m_fullscreen) 
    return;

  int info_size = 10;

  // int small_wid = 440;  // Below which we start to shrink things
  // int blab_size = 12;
  // if(w() < small_wid)
  //   blab_size = 10;

  m_but_show_allpts->labelsize(info_size); 
  m_but_show_allpts->labelsize(info_size); 
  
  m_but_draw_mineff->labelsize(info_size); 
  m_but_draw_avgeff->labelsize(info_size); 

  m_but_draw_mincpa->labelsize(info_size); 
  m_but_draw_avgcpa->labelsize(info_size); 
  
  m_fld_collision_range->textsize(info_size); 
  m_fld_collision_range->labelsize(info_size);
  m_fld_near_miss_range->textsize(info_size); 
  m_fld_near_miss_range->labelsize(info_size);
  m_fld_encounter_range->textsize(info_size); 
  m_fld_encounter_range->labelsize(info_size);

  m_fld_loc_time->textsize(info_size); 
  m_fld_loc_time->labelsize(info_size);

  m_fld_encounters->textsize(info_size); 
  m_fld_encounters->labelsize(info_size);

  m_fld_min_cpa->textsize(info_size); 
  m_fld_min_cpa->labelsize(info_size);

  m_fld_min_eff->textsize(info_size); 
  m_fld_min_eff->labelsize(info_size);

  m_fld_avg_cpa->textsize(info_size); 
  m_fld_avg_cpa->labelsize(info_size);

  m_fld_avg_eff->textsize(info_size); 
  m_fld_avg_eff->labelsize(info_size);
  
  // Current Contact Fields (Third Row)
  m_but_curr_go->labelsize(info_size); 

  m_fld_curr_id->textsize(info_size); 
  m_fld_curr_id->labelsize(info_size);

  m_fld_curr_time->textsize(info_size); 
  m_fld_curr_time->labelsize(info_size);

  m_fld_curr_cpa->textsize(info_size); 
  m_fld_curr_cpa->labelsize(info_size);

  m_fld_curr_eff->textsize(info_size); 
  m_fld_curr_eff->labelsize(info_size);

  m_fld_curr_contact->textsize(info_size); 
  m_fld_curr_contact->labelsize(info_size);
}

//-------------------------------------------------------------------

Fl_Menu_Item GUI_Encounters::menu_[] = {
 {"Invisible", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Replay/Step +0.1 secs", FL_CTRL+']', (Fl_Callback*)GUI_Encounters::cb_Step, (void*)100, 0}, {"Replay/Step -0.1 secs", FL_CTRL+'[', (Fl_Callback*)GUI_Encounters::cb_Step, (void*)-100, 0},
 {"Replay/Step +1 sec", ']', (Fl_Callback*)GUI_Encounters::cb_Step, (void*)1000, 0},
 {"Replay/Step -1 sec", '[', (Fl_Callback*)GUI_Encounters::cb_Step, (void*)-1000, 0},
 {"Replay/Step +5 secs", '}', (Fl_Callback*)GUI_Encounters::cb_Step, (void*)5000, 0},
 {"Replay/Step -5 secs", '{', (Fl_Callback*)GUI_Encounters::cb_Step, (void*)-5000, 0},
 {0}
};


//----------------------------------------------------------
// Procedure: setDataBroker()

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
// Procedure: setParentGUI()

void GUI_Encounters::setParentGUI(REPLAY_GUI* parent_gui)
{
  m_parent_gui = parent_gui;
  this->redraw();
}

//----------------------------------------------------------
// Procedure: resize()

void GUI_Encounters::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgetsShape();
  resizeWidgetsText();
}

//----------------------------------------------------------
// Procedure: handle()
//     Notes: Return of 1 indicates event has been handled.

int GUI_Encounters::handle(int event) 
{
  switch(event) {
  case FL_KEYDOWN:
    if(Fl::event_key() == 32) 
      m_parent_gui->streaming(2);
    else if(Fl::event_key() == 'f') 
      toggleFullScreen();
    else if(Fl::event_key() == 'p') {
      bool drawing_on = m_but_show_allpts->value();
      m_but_show_allpts->value(!drawing_on);
      m_eviewer->setShowAllPts(!drawing_on);
      m_eviewer->redraw();
    }
    else if(Fl::event_key() == 'l') {
      bool draw_mincpa = m_but_draw_mincpa->value();
      m_but_draw_mincpa->value(!draw_mincpa);
      m_eviewer->setDrawMinCPA(!draw_mincpa);
      bool draw_avgcpa = m_but_draw_avgcpa->value();
      m_but_draw_avgcpa->value(!draw_avgcpa);
      m_eviewer->setDrawAvgCPA(!draw_avgcpa);
      bool draw_mineff = m_but_draw_mineff->value();
      m_but_draw_mineff->value(!draw_mineff);
      m_eviewer->setDrawMinEff(!draw_mineff);
      bool draw_avgeff = m_but_draw_avgeff->value();
      m_but_draw_avgeff->value(!draw_avgeff);
      m_eviewer->setDrawAvgEff(!draw_avgeff);
      m_eviewer->redraw();
    }
    else if(Fl::event_key() == '<') {
      m_eviewer->setDrawPointSize("smaller");
      m_eviewer->redraw();
    }
    else if(Fl::event_key() == '>') {
      m_eviewer->setDrawPointSize("bigger");
      m_eviewer->redraw();
    }
    else if(Fl::event_key() == 'a') 
      m_parent_gui->streamspeed(true);
    else if(Fl::event_key() == 'z') 
      m_parent_gui->streamspeed(false);
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
//      Note: Used when the parent gui or self needs to reach down
//            into the eviewer to update time.

void GUI_Encounters::setCurrTime(double curr_time)
{
  if(!m_eviewer)
    return;

  m_eviewer->setTime(curr_time);
  m_eviewer->redraw();

  updateXY();
}


//----------------------------------------------------------
// Procedure: setCurrTimeX
//      Note: Used when the eviewer or self needs to reach up
//            into the parent GUI to update time.

void GUI_Encounters::setCurrTimeX(double curr_time)
{
  if(!m_parent_gui)
    return;

  m_parent_gui->setCurrTime(curr_time);
  m_parent_gui->redraw();
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


//----------------------------------------- SelectShowPts
inline void GUI_Encounters::cb_SelectShowPts_i() {
  bool drawing_on = m_but_show_allpts->value();
  m_eviewer->setShowAllPts(drawing_on);
  m_eviewer->redraw();
}
void GUI_Encounters::cb_SelectShowPts(Fl_Widget* o) {
  ((GUI_Encounters*)(o->parent()->user_data()))->cb_SelectShowPts_i();
}

//----------------------------------------- SelectMinEff
inline void GUI_Encounters::cb_SelectMinEff_i() {
  bool drawing_on = m_but_draw_mineff->value();
  m_eviewer->setDrawMinEff(drawing_on);
  m_eviewer->redraw();
}
void GUI_Encounters::cb_SelectMinEff(Fl_Widget* o) {
  ((GUI_Encounters*)(o->parent()->user_data()))->cb_SelectMinEff_i();
}

//----------------------------------------- SelectAvgEff
inline void GUI_Encounters::cb_SelectAvgEff_i() {
  bool drawing_on = m_but_draw_avgeff->value();
  m_eviewer->setDrawAvgEff(drawing_on);
  m_eviewer->redraw();
}
void GUI_Encounters::cb_SelectAvgEff(Fl_Widget* o) {
  ((GUI_Encounters*)(o->parent()->user_data()))->cb_SelectAvgEff_i();
}


//----------------------------------------- SelectMinCPA
inline void GUI_Encounters::cb_SelectMinCPA_i() {
  bool drawing_on = m_but_draw_mincpa->value();
  m_eviewer->setDrawMinCPA(drawing_on);
  m_eviewer->redraw();
}
void GUI_Encounters::cb_SelectMinCPA(Fl_Widget* o) {
  ((GUI_Encounters*)(o->parent()->user_data()))->cb_SelectMinCPA_i();
}

//----------------------------------------- SelectAvgCPA
inline void GUI_Encounters::cb_SelectAvgCPA_i() {
  bool drawing_on = m_but_draw_avgcpa->value();
  m_eviewer->setDrawAvgCPA(drawing_on);
  m_eviewer->redraw();
}
void GUI_Encounters::cb_SelectAvgCPA(Fl_Widget* o) {
  ((GUI_Encounters*)(o->parent()->user_data()))->cb_SelectAvgCPA_i();
}

//----------------------------------------- SelectGo
inline void GUI_Encounters::cb_SelectGo_i() {
  double go_time = m_eviewer->getCurrIndexTime();
  m_eviewer->setTime(go_time);
  if(m_parent_gui) {
    m_parent_gui->setCurrTime(go_time);
    m_parent_gui->redraw();
  }
  m_eviewer->redraw();
  updateXY();
}
void GUI_Encounters::cb_SelectGo(Fl_Widget* o) {
  ((GUI_Encounters*)(o->parent()->user_data()))->cb_SelectGo_i();
}


//----------------------------------------- FullScreen
void GUI_Encounters::toggleFullScreen() 
{
  m_fullscreen = !m_fullscreen;
  
  if(m_fullscreen) {
    m_fld_loc_time->hide();
    m_fld_encounters->hide();
    m_fld_min_cpa->hide();
    m_fld_min_eff->hide();
    m_fld_avg_cpa->hide();
    m_fld_avg_eff->hide();
    m_fld_collision_range->hide();
    m_fld_near_miss_range->hide();
    m_fld_encounter_range->hide();
    m_but_draw_mineff->hide();
    m_but_draw_avgeff->hide();
    m_but_draw_mincpa->hide();
    m_but_draw_avgcpa->hide();
    m_but_show_allpts->hide();

    resizeWidgetsShape();
    redraw();
  }
  else {
    m_fld_loc_time->show();
    m_fld_encounters->show();
    m_fld_min_cpa->show();
    m_fld_min_eff->show();
    m_fld_avg_cpa->show();
    m_fld_avg_eff->show();
    m_fld_collision_range->show();
    m_fld_near_miss_range->show();
    m_fld_encounter_range->show();
    m_but_draw_mineff->show();
    m_but_draw_avgeff->show();
    m_but_draw_mincpa->show();
    m_but_draw_avgcpa->show();
    m_but_show_allpts->show();
    resizeWidgetsShape();
    resizeWidgetsText();
    updateXY();
    redraw();
  }
}

//-------------------------------------------------------
// Procedure: updateXY()

void GUI_Encounters::updateXY() 
{
  string title = m_vname + " " + m_replay_warp_msg;
  copy_label(title.c_str());

  if(m_fullscreen)
    return;

  double time     = m_eviewer->getCurrTime();
  string time_str = doubleToString(time, 3);
  m_fld_loc_time->value(time_str.c_str());

  unsigned int total_encounters = m_eviewer->getTotalEncounters();
  unsigned int curr_encounter = m_eviewer->getCurrEncounter();
  string total_encounters_str = uintToString(total_encounters);
  string curr_encounter_str = uintToString(curr_encounter);
  string progress = curr_encounter_str + "/" + total_encounters_str;
  m_fld_encounters->value(progress.c_str());

  double min_cpa = m_eviewer->getMinCPA();
  string min_cpa_str = doubleToString(min_cpa,2);
  m_fld_min_cpa->value(min_cpa_str.c_str());

  double min_eff = m_eviewer->getMinEFF();
  string min_eff_str = doubleToString(min_eff,2);
  m_fld_min_eff->value(min_eff_str.c_str());

  double avg_cpa = m_eviewer->getAvgCPA();
  string avg_cpa_str = doubleToString(avg_cpa,2);
  m_fld_avg_cpa->value(avg_cpa_str.c_str());

  double avg_eff = m_eviewer->getAvgEFF();
  string avg_eff_str = doubleToString(avg_eff,2);
  m_fld_avg_eff->value(avg_eff_str.c_str());

  double collision_range = m_eviewer->getCollisionRange();
  string krange_str = doubleToStringX(collision_range,2);
  m_fld_collision_range->value(krange_str.c_str());

  double near_miss_range = m_eviewer->getNearMissRange();
  string nrange_str = doubleToStringX(near_miss_range,2);
  m_fld_near_miss_range->value(nrange_str.c_str());
				     
  double encounter_range = m_eviewer->getEncounterRange();
  string erange_str = doubleToStringX(encounter_range,2);
  m_fld_encounter_range->value(erange_str.c_str());

  // Current Encounter Information
  int curr_id = m_eviewer->getCurrIndexID();
  string curr_id_str = intToString(curr_id);
  m_fld_curr_id->value(curr_id_str.c_str());
  
  double curr_time = m_eviewer->getCurrIndexTime();
  string curr_time_str = doubleToStringX(curr_time,2);
  m_fld_curr_time->value(curr_time_str.c_str());

  double curr_cpa = m_eviewer->getCurrIndexCPA();
  string curr_cpa_str = doubleToStringX(curr_cpa,2);
  m_fld_curr_cpa->value(curr_cpa_str.c_str());

  double curr_eff = m_eviewer->getCurrIndexEFF();
  string curr_eff_str = doubleToStringX(curr_eff,2);
  m_fld_curr_eff->value(curr_eff_str.c_str());

  string curr_contact = m_eviewer->getCurrIndexContact();
  m_fld_curr_contact->value(curr_contact.c_str());

}






