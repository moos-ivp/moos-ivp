/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: REPLAY_GUI.cpp                                       */
/*    DATE: May 31st, 2005                                       */
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
#include "REPLAY_GUI.h"
#include "MBUtils.h"
#include "FL/fl_ask.H"

using namespace std;

//-------------------------------------------------------------------
// Constructor

REPLAY_GUI::REPLAY_GUI(int g_w, int g_h, const char *g_l)
  : MarineVehiGUI(g_w, g_h, g_l) 
{
  m_stream       = false;
  m_collect      = "Off";
  m_step_amt     = 1;

  m_step_time    = 1.0;
  m_step_time_ix = 3;

  m_save_file_ix   = 0;
  m_num_ipfplots = 0;

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  augmentMenu();

  int info_size = 10;
  // Init window with bogus extents. Actual extents
  // are set in the setWindowLayout() call below.
  np_viewer    = new NavPlotViewer(1, 1, 1, 1);
  lp_viewer    = new LogPlotViewer(1, 1, 1, 1);
  ipf_viewer_a = new IvPFuncViewer(1, 1, 1, 1);
  ipf_viewer_b = new IvPFuncViewer(1, 1, 1, 1);

  ipf_pcs_a = 0;
  ipf_pcs_b = 0;
  ipf_pwt_a = 0;
  ipf_pwt_b = 0;
  ipf_iter_a = 0;
  ipf_iter_b = 0;
  ipf_domain_a = 0;
  ipf_domain_b = 0;
  m_but_ipf_set_a = 0;
  m_but_ipf_set_b = 0;
  m_but_ipf_pin_a = 0;
  m_but_ipf_pin_b = 0;

  m_but_ipf_vname_a = 0;
  m_but_ipf_vname_b = 0;
  m_but_ipf_source_a = 0;
  m_but_ipf_source_b = 0;

  setWindowLayout("normal");

  cmviewer     = np_viewer;
  ipf_viewer_a->setClearColor("0.6,0.7,0.5");
  ipf_viewer_b->setClearColor("0.7,0.6,0.5");
  //ipf_viewer_b->setClearColor("macbeige");

  // Initialize Time fields ------------------------------------------
  double time_pos = (w()/2)-250;
  disp_time = new MY_Output(time_pos, h()-(m_lp_viewer_hgt+26), 
			    110, 22, "Timex:"); 
  disp_time->color(FL_RED); 
  disp_time->textcolor(FL_WHITE); 
  disp_time->textsize(12); 
  disp_time->labelsize(12);

  m_but_zoom_in_time = new MY_Repeat_Button(time_pos+120, h()-(m_lp_viewer_hgt+26), 
					    30, 21, "IN");
  m_but_zoom_out_time = new MY_Repeat_Button(time_pos+160, h()-(m_lp_viewer_hgt+26), 
					     35, 21, "OUT");
  m_but_zoom_reset_time = new MY_Button(time_pos+200, h()-(m_lp_viewer_hgt+26), 
					45, 21, "RESET");

  m_but_zoom_in_time->callback((Fl_Callback*)REPLAY_GUI::cb_TimeZoom,(void*)1);
  m_but_zoom_out_time->callback((Fl_Callback*)REPLAY_GUI::cb_TimeZoom,(void*)-1);
  m_but_zoom_reset_time->callback((Fl_Callback*)REPLAY_GUI::cb_TimeZoom,(void*)0);
  
  m_but_zoom_in_time->color(FL_DARK_RED);
  m_but_zoom_out_time->color(FL_DARK_RED);
  m_but_zoom_reset_time->color(FL_DARK_RED);
  m_but_zoom_in_time->labelcolor(FL_WHITE);
  m_but_zoom_out_time->labelcolor(FL_WHITE);
  m_but_zoom_reset_time->labelcolor(FL_WHITE);

  m_but_zoom_in_time->labelsize(10);
  m_but_zoom_out_time->labelsize(10);
  m_but_zoom_reset_time->labelsize(10);

  play_rate = new MY_Output((w()/2)+140, h()-(m_lp_viewer_hgt+25), 70, 20, "play-rate:"); 
  play_rate->textsize(info_size); 
  play_rate->labelsize(info_size);

  collect_state = new MY_Output((w()/2)+260, h()-(m_lp_viewer_hgt+25), 70, 20, "collect:"); 
  collect_state->textsize(info_size); 
  collect_state->labelsize(info_size);

  // Handle LogPlot 1 -------------
  label1 = new MY_Output(35, h()-(m_lp_viewer_hgt+25), 150, 20, "Var:"); 
  label1->textsize(info_size-1); 
  label1->color(FL_DARK_GREEN); 
  label1->textcolor(FL_WHITE); 
  label1->labelsize(info_size);

  curr1 = new MY_Output(255, h()-(m_lp_viewer_hgt+25), 70, 20, "CurrVal:"); 
  curr1->textsize(info_size); 
  curr1->color(FL_DARK_GREEN); 
  curr1->textcolor(FL_WHITE); 
  curr1->labelsize(info_size);

  low1 = new MY_Output(2, h()-20, 51, 20, ""); 
  low1->textsize(info_size); 
  low1->color(FL_DARK_GREEN); 
  low1->textcolor(FL_WHITE); 
  low1->labelsize(info_size);

  high1 = new MY_Output(2, h()-(m_lp_viewer_hgt-2), 51, 20, ""); 
  high1->textsize(info_size); 
  high1->color(FL_DARK_GREEN); 
  high1->textcolor(FL_WHITE); 
  high1->labelsize(info_size);

  // Handle Time Min/Max -------------
  m_fld_time_low = new MY_Output(2, h()-(m_lp_viewer_hgt/2)-10, 51, 20, ""); 
  m_fld_time_low->textsize(info_size); 
  m_fld_time_low->color(FL_YELLOW); 
  m_fld_time_low->textcolor(FL_BLUE); 
  m_fld_time_low->labelsize(info_size);
  
  m_fld_time_high = new MY_Output(w()-110+57, h()-(m_lp_viewer_hgt/2)-10, 51, 20, ""); 
  m_fld_time_high->textsize(info_size); 
  m_fld_time_high->color(FL_YELLOW); 
  m_fld_time_high->textcolor(FL_BLUE); 
  m_fld_time_high->labelsize(info_size);

  // Handle LogPlot 2 -------------
  label2 = new MY_Output(w()-154, h()-(m_lp_viewer_hgt+25), 150, 20, "Var:"); 
  label2->textsize(info_size-1); 
  label2->labelsize(info_size);
  label2->textcolor(FL_WHITE); 
  label2->color(FL_DARK_BLUE); 

  curr2 = new MY_Output(w()-255, h()-(m_lp_viewer_hgt+25), 70, 20, "CurrVal:"); 
  curr2->textsize(info_size); 
  curr2->labelsize(info_size);
  curr2->textcolor(FL_WHITE); 
  curr2->color(FL_DARK_BLUE); 

  low2 = new MY_Output(w()-110+57, h()-20, 51, 20, ""); 
  low2->textsize(info_size); 
  low2->color(FL_DARK_BLUE); 
  low2->textcolor(FL_WHITE); 
  low2->labelsize(info_size);

  high2 = new MY_Output(w()-110+57, h()-(m_lp_viewer_hgt-2), 51, 20, ""); 
  high2->textsize(info_size); 
  high2->color(FL_DARK_BLUE); 
  high2->textcolor(FL_WHITE); 
  high2->labelsize(info_size);

  // Handle Helm1 -------------
  int fld_wid = (w()/2) - 55 - 4;
  Fl_Color fcolor1 = fl_rgb_color(200, 255, 200);
  Fl_Color fcolor2 = fl_rgb_color(200, 200, 255);
  m_fld_bhvs_vname_1 = new MY_Output(55, m_np_viewer_hgt+35, 65, 20, "Vehicle:"); 
  m_fld_bhvs_vname_1->textsize(info_size); 
  m_fld_bhvs_vname_1->color(fcolor1); 
  m_fld_bhvs_vname_1->labelsize(info_size);

  m_fld_bhvs_mode_1 = new MY_Output(165, m_np_viewer_hgt+35, 120, 20, "Mode:"); 
  m_fld_bhvs_mode_1->textsize(info_size-1); 
  m_fld_bhvs_mode_1->color(fcolor1); 
  m_fld_bhvs_mode_1->labelsize(info_size);

  m_fld_bhvs_dec_1 = new MY_Output(340, m_np_viewer_hgt+35, (w()/2)-344, 20, "Decision:"); 
  m_fld_bhvs_dec_1->textsize(info_size); 
  m_fld_bhvs_dec_1->color(fcolor1); 
  m_fld_bhvs_dec_1->labelsize(info_size);

  m_fld_bhvs_act_1 = new MY_Output(55, m_np_viewer_hgt+60, fld_wid, 20, "Active:"); 
  m_fld_bhvs_act_1->textsize(info_size); 
  m_fld_bhvs_act_1->color(fcolor1); 
  m_fld_bhvs_act_1->labelsize(info_size);

  m_fld_bhvs_run_1 = new MY_Output(55, m_np_viewer_hgt+85, fld_wid, 20, "Running:"); 
  m_fld_bhvs_run_1->textsize(info_size); 
  m_fld_bhvs_run_1->color(fcolor1); 
  m_fld_bhvs_run_1->labelsize(info_size);

  m_fld_bhvs_idle_1 = new MY_Output(55, m_np_viewer_hgt+110, fld_wid, 20, "Idle:"); 
  m_fld_bhvs_idle_1->textsize(info_size); 
  m_fld_bhvs_idle_1->color(fcolor1); 
  m_fld_bhvs_idle_1->labelsize(info_size);

  m_fld_bhvs_cplt_1 = new MY_Output(55, m_np_viewer_hgt+135, fld_wid, 20, "Complete:"); 
  m_fld_bhvs_cplt_1->textsize(info_size); 
  m_fld_bhvs_cplt_1->color(fcolor1); 
  m_fld_bhvs_cplt_1->labelsize(info_size);

  // Handle Helm2 -------------
  int lc = (w()/2);
  m_fld_bhvs_vname_2 = new MY_Output(lc+55, m_np_viewer_hgt+35, 65, 20, "Vehicle:"); 
  m_fld_bhvs_vname_2->textsize(info_size); 
  m_fld_bhvs_vname_2->color(fcolor2); 
  m_fld_bhvs_vname_2->labelsize(info_size);

  m_fld_bhvs_mode_2 = new MY_Output(lc+165, m_np_viewer_hgt+35, 120, 20, "Mode:"); 
  m_fld_bhvs_mode_2->textsize(info_size-1); 
  m_fld_bhvs_mode_2->color(fcolor2); 
  m_fld_bhvs_mode_2->labelsize(info_size);

  m_fld_bhvs_dec_2 = new MY_Output(lc+340, m_np_viewer_hgt+35, (w()/2)-344, 20, "Decision:"); 
  m_fld_bhvs_dec_2->textsize(info_size); 
  m_fld_bhvs_dec_2->color(fcolor2); 
  m_fld_bhvs_dec_2->labelsize(info_size);

  m_fld_bhvs_act_2 = new MY_Output(lc+55, m_np_viewer_hgt+60, fld_wid, 20, "Active:"); 
  m_fld_bhvs_act_2->textsize(info_size); 
  m_fld_bhvs_act_2->color(fcolor2); 
  m_fld_bhvs_act_2->labelsize(info_size);

  m_fld_bhvs_run_2 = new MY_Output(lc+55, m_np_viewer_hgt+85, fld_wid, 20, "Running:"); 
  m_fld_bhvs_run_2->textsize(info_size); 
  m_fld_bhvs_run_2->color(fcolor2); 
  m_fld_bhvs_run_2->labelsize(info_size);

  m_fld_bhvs_idle_2 = new MY_Output(lc+55, m_np_viewer_hgt+110, fld_wid, 20, "Idle:"); 
  m_fld_bhvs_idle_2->textsize(info_size); 
  m_fld_bhvs_idle_2->color(fcolor2); 
  m_fld_bhvs_idle_2->labelsize(info_size);

  m_fld_bhvs_cplt_2 = new MY_Output(lc+55, m_np_viewer_hgt+135, fld_wid, 20, "Complete:"); 
  m_fld_bhvs_cplt_2->textsize(info_size); 
  m_fld_bhvs_cplt_2->color(fcolor2); 
  m_fld_bhvs_cplt_2->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
// Destructor

REPLAY_GUI::~REPLAY_GUI()
{
  delete(lp_viewer);
  delete(np_viewer);
  delete(disp_time);
  delete(play_rate);
  delete(collect_state);

  delete(vname1);
  delete(label1);
  delete(low1);
  delete(high1);
  delete(curr1);
  delete(m_fld_time_low);
  delete(m_fld_time_high);

  delete(vname2);
  delete(label2);
  delete(low2);
  delete(high2);
  delete(curr2);
  delete(m_but_zoom_in_time);
  delete(m_but_zoom_out_time);
  delete(m_but_zoom_reset_time);
}

//-------------------------------------------------------------------
// Procedure: setWindowLayout()

void REPLAY_GUI::setWindowLayout(string layout)
{
  int npw, nph, npx, npy;
  int lpw, lph, lpx, lpy;
  int ipaw, ipah, ipax, ipay;
  int ipbw, ipbh, ipbx, ipby;

  m_lp_viewer_hgt = 140;
  m_np_viewer_hgt = h()-340;

  double ipf_pct = 0.3;
  if(layout == "normal") {
    ipf_pct = 0.295;
    m_np_viewer_hgt = h()-340;
  }
  else if(layout == "noipfs") {
    ipf_pct = 0.0;
    m_np_viewer_hgt = h()-340;
  }
  else if(layout == "fullview") {
    ipf_pct = 0.0;
    m_np_viewer_hgt = h()-200;
  }

  // Set the extents of the NavPlotViewer
  npx = 0;
  npy = 30;
  nph = m_np_viewer_hgt;
  npw = w() * (1-ipf_pct);
  
  // Set the extents of the LogPlotViewer
  int margin = 2;
  lpx = 55;
  lph = m_lp_viewer_hgt;
  lpy = h() - (lph - margin);
  lpw = w() - (2 * lpx);
  lph = lph - (2 * margin);
  
  // Set the extents of the IvPFuncViewers
  int ipf_hgt = (nph / 2);
  ipax = w() * (1-ipf_pct);
  ipay = 30;
  ipaw = w() * (ipf_pct);
  ipah = ipf_hgt-65;
  
  ipbx = w() * (1-ipf_pct);
  ipby = ipf_hgt + 30;
  ipbw = w() * (ipf_pct);
  ipbh = ipf_hgt-65;
  
  if(np_viewer)
    np_viewer->resize(npx, npy, npw, nph);
  if(lp_viewer)
    lp_viewer->resize(lpx, lpy, lpw, lph);
  
  if(ipf_viewer_a)
    ipf_viewer_a->resize(ipax, ipay+65, ipaw, ipah);
  if(ipf_viewer_b)
    ipf_viewer_b->resize(ipbx, ipby+65, ipbw, ipbh);

  
  m_np_viewer_hgt = nph;
  m_lp_viewer_hgt = lph;

  int info_size = 10;

  Fl_Color fcolor1 = fl_rgb_color(200, 90, 90);
  if(!m_but_ipf_vname_a) {
    m_but_ipf_vname_a = new MY_Menu_Button(ipax+10, ipay+5, 150, 20, "Vehicle");
    m_but_ipf_vname_a->labelsize(12);
    m_but_ipf_vname_a->callback((Fl_Callback*)REPLAY_GUI::cb_TopSelectVName,(void*)-1);
  }
  if(!m_but_ipf_source_a) {
    m_but_ipf_source_a = new MY_Menu_Button(ipax+10, ipay+30, 150, 20, "Behavior");
    m_but_ipf_source_a->labelsize(12);
    m_but_ipf_source_a->callback((Fl_Callback*)REPLAY_GUI::cb_TopSelectSource,(void*)-1);
  }

  if(!ipf_iter_a) {
    ipf_iter_a = new MY_Output(ipax+195, ipay+5, 50, 20, "Iter:"); 
    ipf_iter_a->textsize(info_size); 
    ipf_iter_a->color(fcolor1); 
    ipf_iter_a->labelsize(info_size);
  }

  if(!ipf_pcs_a) {
    ipf_pcs_a = new MY_Output(ipax+275, ipay+5, 45, 20, "Pcs:"); 
    ipf_pcs_a->textsize(info_size); 
    ipf_pcs_a->labelsize(info_size);
  }

  if(!ipf_pwt_a) {
    ipf_pwt_a = new MY_Output(ipax+353, ipay+5, 50, 20, "Pwt:"); 
    ipf_pwt_a->textsize(info_size); 
    ipf_pwt_a->labelsize(info_size);
  }

  if(!m_but_ipf_set_a) {
    m_but_ipf_set_a = new MY_Button(ipax+330, ipay+30, 33, 20, "set");
    m_but_ipf_set_a->labelsize(12);
    m_but_ipf_set_a->callback((Fl_Callback*)REPLAY_GUI::cb_ToggleSet,(void*)1);
  }
  

  if(!m_but_ipf_pin_a) {
    m_but_ipf_pin_a = new MY_Button(ipax+370, ipay+30, 33, 20, "pin");
    m_but_ipf_pin_a->labelsize(12);
    m_but_ipf_pin_a->callback((Fl_Callback*)REPLAY_GUI::cb_TogglePin,(void*)1);
  }

  if(!ipf_domain_a) {
    ipf_domain_a = new MY_Output(ipax+195, ipay+30, 125, 20, "Dom:"); 
    ipf_domain_a->textsize(info_size); 
    ipf_domain_a->labelsize(info_size);
  }

  //----------------------------------------------------------------------------
  if(!m_but_ipf_vname_b) {
    m_but_ipf_vname_b = new MY_Menu_Button(ipbx+10, ipby+8, 150, 20, "Vehicle");
    m_but_ipf_vname_b->labelsize(12);
    m_but_ipf_vname_b->callback((Fl_Callback*)REPLAY_GUI::cb_BotSelectVName,(void*)-1);
  }
  if(!m_but_ipf_source_b) {
    m_but_ipf_source_b = new MY_Menu_Button(ipbx+10, ipby+33, 150, 20, "Behavior");
    m_but_ipf_source_b->labelsize(12);
    m_but_ipf_source_b->callback((Fl_Callback*)REPLAY_GUI::cb_BotSelectSource,(void*)-1);
  }

  if(!ipf_iter_b) {
    ipf_iter_b = new MY_Output(ipbx+195, ipby+8, 50, 20, "Iter:"); 
    ipf_iter_b->textsize(info_size); 
    ipf_iter_b->color(fcolor1); 
    ipf_iter_b->labelsize(info_size);
  }

  if(!ipf_pcs_b) {
    ipf_pcs_b = new MY_Output(ipbx+275, ipby+8, 45, 20, "Pcs:"); 
    ipf_pcs_b->textsize(info_size); 
    ipf_pcs_b->labelsize(info_size);
  }

  if(!ipf_pwt_b) {
    ipf_pwt_b = new MY_Output(ipbx+353, ipby+8, 50, 20, "Pwt:"); 
    ipf_pwt_b->textsize(info_size); 
    ipf_pwt_b->labelsize(info_size);
  }

  if(!m_but_ipf_set_b) {
    m_but_ipf_set_b = new MY_Button(ipbx+330, ipby+33, 33, 20, "set");
    m_but_ipf_set_b->labelsize(12);
    m_but_ipf_set_b->callback((Fl_Callback*)REPLAY_GUI::cb_ToggleSet,(void*)2);
  }
  

  if(!m_but_ipf_pin_b) {
    m_but_ipf_pin_b = new MY_Button(ipbx+370, ipby+33, 33, 20, "pin");
    m_but_ipf_pin_b->labelsize(12);
    m_but_ipf_pin_b->callback((Fl_Callback*)REPLAY_GUI::cb_TogglePin,(void*)2);
  }

  if(!ipf_domain_b) {
    ipf_domain_b = new MY_Output(ipbx+195, ipby+33, 125, 20, "Dom:"); 
    ipf_domain_b->textsize(info_size); 
    ipf_domain_b->labelsize(info_size);
  }

}


//-------------------------------------------------------------------
// Procedure: augmentMenu()

void REPLAY_GUI::augmentMenu()
{
  mbar->add("File/Delete ", 0, (Fl_Callback*)REPLAY_GUI::cb_Delete, 0, 0);

  mbar->add("Replay/Collecting Toggle",  'w', (Fl_Callback*)REPLAY_GUI::cb_CollectToggle,(void*)0, FL_MENU_DIVIDER);
  mbar->add("Replay/Streaming Toggle",  '=', (Fl_Callback*)REPLAY_GUI::cb_StreamToggle,(void*)0, 0);
  mbar->add("Replay/Streaming Faster", 'a', (Fl_Callback*)REPLAY_GUI::cb_StreamSpeed, (void*)1, 0);
  mbar->add("Replay/Streaming Slower", 'z', (Fl_Callback*)REPLAY_GUI::cb_StreamSpeed, (void*)0, FL_MENU_DIVIDER);
  mbar->add("Replay/Streaming Step 1",  0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)1, FL_MENU_RADIO|FL_MENU_VALUE);
  mbar->add("Replay/Streaming Step 3",  0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)3, FL_MENU_RADIO);
  mbar->add("Replay/Streaming Step 5",  0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)5, FL_MENU_RADIO);
  mbar->add("Replay/Streaming Step 10", 0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)10, FL_MENU_RADIO|FL_MENU_DIVIDER);

  mbar->add("Replay/Step by Seconds",  0, (Fl_Callback*)REPLAY_GUI::cb_StepType, (void*)0, FL_MENU_RADIO|FL_MENU_VALUE);
  mbar->add("Replay/Step by Helm Iterations",  0, (Fl_Callback*)REPLAY_GUI::cb_StepType, (void*)1, FL_MENU_RADIO|FL_MENU_DIVIDER);

  mbar->add("Replay/Step Ahead 1",  ']', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)1, 0);
  mbar->add("Replay/Step Back  1",  '[', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)-1, 0);
  mbar->add("Replay/Step Ahead 5", '>', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)5, 0);
  mbar->add("Replay/Step Back  5", '<', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)-5, FL_MENU_DIVIDER);
};

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

int REPLAY_GUI::handle(int event) 
{
  double lpv_curr_time = 0;
  switch(event) {
  case FL_PUSH:
    Fl_Window::handle(event);
    lpv_curr_time = lp_viewer->getCurrTime();
    np_viewer->setCurrTime(lpv_curr_time);
    if(ipf_viewer_a) {
      map<string, unsigned int> viter_map = np_viewer->getVIterMap();
      ipf_viewer_a->setVIterMap(viter_map);
      ipf_viewer_a->resetPlotIndex();
      ipf_viewer_a->redraw();
    }
    if(ipf_viewer_b) {
      map<string, unsigned int> viter_map = np_viewer->getVIterMap();
      ipf_viewer_b->setVIterMap(viter_map);
      ipf_viewer_b->resetPlotIndex();
      ipf_viewer_b->redraw();
    }
    return(1);
    break;
  case FL_RELEASE:
    updateXY();
    return(1);
    break;
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------------------------
// Procedure: setCurrTime

void REPLAY_GUI::setCurrTime(double curr_time)
{
  if(curr_time == -1)
    curr_time = np_viewer->getStartTimeHint();

  lp_viewer->setCurrTime(curr_time);
  np_viewer->setCurrTime(curr_time);
  if(ipf_viewer_a) {
    map<string, unsigned int> viter_map = np_viewer->getVIterMap();
    ipf_viewer_a->setVIterMap(viter_map);
    ipf_viewer_a->resetPlotIndex();
    ipf_viewer_a->redraw();
  }
  if(ipf_viewer_b) {
    map<string, unsigned int> viter_map = np_viewer->getVIterMap();
    ipf_viewer_b->setVIterMap(viter_map);
    ipf_viewer_b->resetPlotIndex();
    ipf_viewer_b->redraw();
  }
  updateXY();
}


//----------------------------------------------------------
// Procedure: initChoicesIPF

void REPLAY_GUI::initChoicesIPF()
{
  if(!ipf_viewer_a || !ipf_viewer_b || !np_viewer)
    return;

  if((m_vnames.size() == 0) || (m_sources.size() == 0))
     return;

  map<string, unsigned int> viter_map = np_viewer->getVIterMap();
  ipf_viewer_a->setVIterMap(viter_map);
  ipf_viewer_b->setVIterMap(viter_map);

  cb_TopSelectVName_i(0);
  cb_BotSelectVName_i(0);
  cb_TopSelectSource_i(900);
  cb_BotSelectSource_i(0);
}

//----------------------------------------- HandleUpDown
inline void REPLAY_GUI::cb_HandleUpDown_i(int amt) 
{
  if(inNavPlotViewer()) {
    np_viewer->setParam("pan_y", ((double)(amt)/10)); 
    np_viewer->redraw();
    updateXY();
  }
  else if(inIPFViewerA()) {
    if(amt < 0) amt = -1;
    if(amt > 0) amt = 1;
    ipf_viewer_a->setParam("mod_x_rotation", ((double)(-amt)));
    ipf_viewer_a->redraw();
    if(Fl::event_state() == FL_CTRL) {
      ipf_viewer_b->setParam("mod_x_rotation", ((double)(-amt)));
      ipf_viewer_b->redraw();
    }
    updateXY();
  }
  else if(inIPFViewerB()) {
    if(amt < 0) amt = -1;
    if(amt > 0) amt = 1;
    ipf_viewer_b->setParam("mod_x_rotation", ((double)(-amt))); 
    ipf_viewer_b->redraw();
    if(Fl::event_state() == FL_CTRL) {
      ipf_viewer_a->setParam("mod_x_rotation", ((double)(-amt)));
      ipf_viewer_a->redraw();
    }
    updateXY();
  }
  else if(inLogPlotViewer()) {
    if(amt < 0)
      lp_viewer->adjustZoom("out");
    else if(amt > 0)
      lp_viewer->adjustZoom("in");
    lp_viewer->redraw();
    updateXY();
  }
}

//----------------------------------------- HandleLeftRight
inline void REPLAY_GUI::cb_HandleLeftRight_i(int amt) 
{
  if(inNavPlotViewer()) {
    np_viewer->setParam("pan_x", ((double)(amt)/10)); 
    np_viewer->redraw();
    updateXY();
  }
  else if(inIPFViewerA()) {
    if(amt < 0) amt = -1;
    if(amt > 0) amt = 1;
    ipf_viewer_a->setParam("mod_z_rotation", ((double)(amt)));
    ipf_viewer_a->redraw();
    if(Fl::event_state() == FL_CTRL) {
      ipf_viewer_b->setParam("mod_z_rotation", ((double)(amt)));
      ipf_viewer_b->redraw();
    }
    updateXY();
  }
  else if(inIPFViewerB()) {
    if(amt < 0) amt = -1;
    if(amt > 0) amt = 1;
    ipf_viewer_b->setParam("mod_z_rotation", ((double)(amt))); 
    ipf_viewer_b->redraw();
    if(Fl::event_state() == FL_CTRL) {
      ipf_viewer_a->setParam("mod_z_rotation", ((double)(amt)));
      ipf_viewer_a->redraw();
    }
    updateXY();
  }
  else if(inLogPlotViewer()) {
    if(amt < 0)
      cb_Step_i(1);
    else if(amt > 0)
      cb_Step_i(-1);
  }
}

//----------------------------------------- Zoom In
inline void REPLAY_GUI::cb_Zoom_i(int val) {
  if(inNavPlotViewer()) {
    if(val < 0)
      np_viewer->setParam("zoom", 1.05);
    else if(val > 0)
      np_viewer->setParam("zoom", 0.8);
    else
      np_viewer->setParam("zoom", "reset");
    np_viewer->redraw();
  }
  else if(inIPFViewerA()) {
    if(val < 0)
      ipf_viewer_a->setParam("mod_zoom", 1.05); 
    else if(val > 0)
      ipf_viewer_a->setParam("mod_zoom", 0.95); 
    else
      ipf_viewer_a->setParam("set_zoom", 1.0); 
    ipf_viewer_a->redraw();
  }
  else if(inIPFViewerB()) {
    if(val < 0)
      ipf_viewer_b->setParam("mod_zoom", 1.05); 
    else if(val > 0)
      ipf_viewer_b->setParam("mod_zoom", 0.95); 
    else
      ipf_viewer_b->setParam("set_zoom", 1.0); 
    ipf_viewer_b->redraw();
  }
  else if(inLogPlotViewer()) {
    if(val < 0)
      lp_viewer->adjustZoom("in");
    else if(val > 0)
      lp_viewer->adjustZoom("out");
    else
      lp_viewer->adjustZoom("reset");
    lp_viewer->redraw();
    updateXY();
  }
}

//----------------------------------------- inNavPlotViewer
bool REPLAY_GUI::inNavPlotViewer()
{
  if(!np_viewer)
    return(false);
  int vx = Fl::event_x();
  int vy = Fl::event_y();
  int x  = np_viewer->x();
  int y  = np_viewer->y();
  int w  = np_viewer->w();
  int h  = np_viewer->h();
  return((vx>=x)&&(vx<=x+w)&&(vy>=y)&&(vy<=(y+h)));
}

//----------------------------------------- inLogPlotViewer
bool REPLAY_GUI::inLogPlotViewer()
{
  if(!lp_viewer)
    return(false);
  int vx = Fl::event_x();
  int vy = Fl::event_y();
  int x  = lp_viewer->x();
  int y  = lp_viewer->y();
  int w  = lp_viewer->w();
  int h  = lp_viewer->h();
  return((vx>=x)&&(vx<=x+w)&&(vy>=y)&&(vy<=(y+h)));
}

//----------------------------------------- inIPFViewerA
bool REPLAY_GUI::inIPFViewerA()
{
  if(!ipf_viewer_a)
    return(false);

  int vx = Fl::event_x();
  int vy = Fl::event_y();
  int x  = ipf_viewer_a->x();
  int y  = ipf_viewer_a->y();
  int w  = ipf_viewer_a->w();
  int h  = ipf_viewer_a->h();
  return((vx>=x)&&(vx<=x+w)&&(vy>=y)&&(vy<=(y+h)));
}

//----------------------------------------- inIPFViewerB
bool REPLAY_GUI::inIPFViewerB()
{
  if(!ipf_viewer_b)
    return(false);
  int vx = Fl::event_x();
  int vy = Fl::event_y();
  int x  = ipf_viewer_b->x();
  int y  = ipf_viewer_b->y();
  int w  = ipf_viewer_b->w();
  int h  = ipf_viewer_b->h();
  return((vx>=x)&&(vx<=x+w)&&(vy>=y)&&(vy<=(y+h)));
}

//----------------------------------------- Step
inline bool REPLAY_GUI::cb_Step_i(int val) {
  np_viewer->stepTime(val);
  np_viewer->redraw();

  double curr_time = np_viewer->getCurrTime();
  lp_viewer->setCurrTime(curr_time);
  lp_viewer->redraw();


  map<string, unsigned int> viter_map = np_viewer->getVIterMap();
  if(ipf_viewer_a) {
    ipf_viewer_a->setVIterMap(viter_map);
    ipf_viewer_a->resetPlotIndex();
    ipf_viewer_a->redraw();
  }
  if(ipf_viewer_b) {
    ipf_viewer_b->setVIterMap(viter_map);
    ipf_viewer_b->resetPlotIndex();
    ipf_viewer_b->redraw();
  }

  updateXY();
  return(true);
}
void REPLAY_GUI::cb_Step(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_Step_i(val);
}

//----------------------------------------- StepType
inline bool REPLAY_GUI::cb_StepType_i(int val) {
  if(val == 0)
    np_viewer->setStepType("seconds");
  if(val == 1)
    np_viewer->setStepType("helm_iterations");
  return(true);
}
void REPLAY_GUI::cb_StepType(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_StepType_i(val);
}

//----------------------------------------- LeftLogPlot
inline void REPLAY_GUI::cb_LeftLogPlot_i(int index) {
  lp_viewer->setLeftPlot(index);
  lp_viewer->redraw();
  updateXY();
}
void REPLAY_GUI::cb_LeftLogPlot(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_LeftLogPlot_i(val);
}

//----------------------------------------- RightLogPlot
inline void REPLAY_GUI::cb_RightLogPlot_i(int index) {
  lp_viewer->setRightPlot(index);
  lp_viewer->redraw();
  updateXY();
}
void REPLAY_GUI::cb_RightLogPlot(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_RightLogPlot_i(val);
}

//----------------------------------------- TopSelectVName
inline void REPLAY_GUI::cb_TopSelectVName_i(int index) {
  if((index < 0) || ((unsigned int)(index) >= m_vnames.size()))
    return;
  m_vname_a = m_vnames[index];
  setVNameMenuButtonA(m_vname_a);

  if(ipf_viewer_a)
    ipf_viewer_a->setPlotIndex(m_vname_a, "reset");
  updateXY();
}
void REPLAY_GUI::cb_TopSelectVName(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_TopSelectVName_i(val);
}

//----------------------------------------- TopSelectSource
inline void REPLAY_GUI::cb_TopSelectSource_i(int index) {
  if(!ipf_viewer_a)
      return;
  if(index == 900) {
    ipf_viewer_a->setPlotIndex(m_vname_a, "collective-hdgspd");
    m_but_ipf_source_a->label("collective");
  }
  else if(index == 901) {
    ipf_viewer_a->setPlotIndex(m_vname_a, "collective-depth");
    m_but_ipf_source_a->label("collective (dep)");
  }

  if((index >= 0) && ((unsigned int)(index) < m_sources.size())) {
    m_source_a = m_sources[index];
    m_but_ipf_source_a_str = truncString(m_source_a, 18, "middle");
    m_but_ipf_source_a->label(m_but_ipf_source_a_str.c_str());
    ipf_viewer_a->setPlotIndex(m_vname_a, m_source_a);
  }

  updateXY();

}
void REPLAY_GUI::cb_TopSelectSource(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_TopSelectSource_i(val);
}

//----------------------------------------- BotSelectVName
inline void REPLAY_GUI::cb_BotSelectVName_i(int index) {
  if((index < 0) || ((unsigned int)(index) >= m_vnames.size()))
    return;
  m_vname_b = m_vnames[index];
  setVNameMenuButtonB(m_vname_b);

  if(ipf_viewer_b) 
    ipf_viewer_b->setPlotIndex(m_vname_b, "reset");

  updateXY();
}
void REPLAY_GUI::cb_BotSelectVName(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_BotSelectVName_i(val);
}

//----------------------------------------- BotSelectSource
inline void REPLAY_GUI::cb_BotSelectSource_i(int index) {
  if(!ipf_viewer_b)
      return;

  if(index == 900) {
    ipf_viewer_b->setPlotIndex(m_vname_b, "collective-hdgspd");
    m_but_ipf_source_b->label("collective");
  }
  else if(index == 901) {
    ipf_viewer_b->setPlotIndex(m_vname_b, "collective-depth");
    m_but_ipf_source_b->label("collective (dep)");
  }

  if((index >= 0) && ((unsigned int)(index) < m_sources.size())) {
    m_source_b = m_sources[index];
    m_but_ipf_source_b_str = truncString(m_source_b, 18, "middle");
    m_but_ipf_source_b->label(m_but_ipf_source_b_str.c_str());
    ipf_viewer_b->setPlotIndex(m_vname_b, m_source_b);
  }
  updateXY();
}
void REPLAY_GUI::cb_BotSelectSource(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_BotSelectSource_i(val);
}

//----------------------------------------- LeftHelmPlot
inline void REPLAY_GUI::cb_LeftHelmPlot_i(int index) {
  np_viewer->setHPlotLeftIndex(index);
  np_viewer->redraw();
  updateXY();
}
void REPLAY_GUI::cb_LeftHelmPlot(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_LeftHelmPlot_i(val);
}

//----------------------------------------- RightHelmPlot
inline void REPLAY_GUI::cb_RightHelmPlot_i(int index) {
  np_viewer->setHPlotRightIndex(index);
  np_viewer->redraw();
  updateXY();
}
void REPLAY_GUI::cb_RightHelmPlot(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_RightHelmPlot_i(val);
}

//----------------------------------------- CollectToggle
inline void REPLAY_GUI::cb_CollectToggle_i() {
  if(m_collect == "Off")
    m_collect = "1024x768";
  else if(m_collect == "1024x768")
    m_collect = "800x600";
  else if(m_collect == "800x600")
    m_collect = "640x480";
  else if(m_collect == "640x480")
    m_collect = "480x360";
  else if(m_collect == "480x360")
    m_collect = "Off";

  if(m_collect != "Off")
    np_viewer->setFrame(m_collect + "+10+10");
  else
    np_viewer->setFrame("");

  np_viewer->redraw();
  updateXY();
}
void REPLAY_GUI::cb_CollectToggle(Fl_Widget* o) {
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_CollectToggle_i();
}

//----------------------------------------- TimeZoom
inline void REPLAY_GUI::cb_TimeZoom_i(int val) {
  if(val > 0)
    lp_viewer->adjustZoom("in");
  else if(val < 0)
    lp_viewer->adjustZoom("out");
  else
    lp_viewer->adjustZoom("reset");
  updateXY();
}

void REPLAY_GUI::cb_TimeZoom(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_TimeZoom_i(val);
}

//----------------------------------------- StreamToggle
inline void REPLAY_GUI::cb_StreamToggle_i() {
  cout << "Toggling the Stream" << endl;
  m_stream = !m_stream;
  if(m_stream)
    m_timer.start();
  else
    m_timer.stop();
  updateXY();
}
void REPLAY_GUI::cb_StreamToggle(Fl_Widget* o) {
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_StreamToggle_i();
}

//----------------------------------------- ToggleSet
inline void REPLAY_GUI::cb_ToggleSet_i(int val) {
  if(val == 1)
    ipf_viewer_a->setParam("reset_view", "2");
  if(val == 2)
    ipf_viewer_b->setParam("reset_view", "2");
}
void REPLAY_GUI::cb_ToggleSet(Fl_Widget* o, int v) {
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_ToggleSet_i(v);
}

//----------------------------------------- TogglePin
inline void REPLAY_GUI::cb_TogglePin_i(int val) {
  if(val == 1)
    ipf_viewer_a->setParam("draw_pin", "toggle");
  if(val == 2)
    ipf_viewer_b->setParam("draw_pin", "toggle");
}

void REPLAY_GUI::cb_TogglePin(Fl_Widget* o, int v) {
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_TogglePin_i(v);
}

//----------------------------------------- ToggleCollective
inline void REPLAY_GUI::cb_ToggleCollective_i(int val) {
  if(val == 1)
    ipf_viewer_a->setParam("collective", "toggle");
  if(val == 2)
    ipf_viewer_b->setParam("collective", "toggle");
}

void REPLAY_GUI::cb_ToggleCollective(Fl_Widget* o, int v) {
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_ToggleCollective_i(v);
}

//----------------------------------------- StreamStep

inline void REPLAY_GUI::cb_StreamStep_i(int val) {
  m_step_amt = val;
}

void REPLAY_GUI::cb_StreamStep(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_StreamStep_i(val);
}

//----------------------------------------- StreamSpeed
inline void REPLAY_GUI::cb_StreamSpeed_i(bool faster) {
  if(faster)
    m_step_time_ix++;
  else
    m_step_time_ix--;

  if(m_step_time_ix > 8)
    m_step_time_ix = 8;
  if(m_step_time_ix < 0)
    m_step_time_ix = 0;

  else if(m_step_time_ix == 0)
    m_step_time = 8.0;
  else if(m_step_time_ix == 1)
    m_step_time = 4.0;
  else if(m_step_time_ix == 2)
    m_step_time = 2.0;
  else if(m_step_time_ix == 3)
    m_step_time = 1.0;
  else if(m_step_time_ix == 4)
    m_step_time = 0.5;
  else if(m_step_time_ix == 5)
    m_step_time = 0.25;
  else if(m_step_time_ix == 6)
    m_step_time = 0.125;
  else if(m_step_time_ix == 7)
    m_step_time = 0.0625;
  else if(m_step_time_ix == 8)
    m_step_time = 0.03125;

  updateXY();
}


void REPLAY_GUI::cb_StreamSpeed(Fl_Widget* o, bool v) {
  bool val = (bool)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_StreamSpeed_i(val);
}

//----------------------------------------- Delete
inline void REPLAY_GUI::cb_Delete_i() {
  string slog_file = findReplace(m_log_file, ".alog", ".slog");
  string ylog_file = findReplace(m_log_file, ".alog", ".ylog");
  string moos_file = findReplace(m_log_file, ".alog", "._moos");
  int res = fl_choice("Delete Log files?", "Yes", "No", 0);
  if(res==1) 
    return;
  
  string command = "rm -f " + m_log_file + " " + slog_file + " " +
    ylog_file + " " + moos_file; 
  system(command.c_str());
  exit(0);
}
void REPLAY_GUI::cb_Delete(Fl_Widget* o) {
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_Delete_i();
}

//----------------------------------------- UpdateXY
void REPLAY_GUI::updateXY() 
{
  // DISPLAY TIME
  double ctime = np_viewer->getCurrTime();
  string dtime = doubleToString(ctime, 1);
  disp_time->value(dtime.c_str());
  
  //fl_color(FL_DARK_BLUE);
  //fl_line(0, h()-172, w(), h()-172);
  //fl_line(0, h()-285, w(), h()-285);
  //fl_line(w()/2, h()-285, w()/2, h()-172);
  //fl_color(FL_BLACK);

  // Play Rate
  if(m_stream) {
    if(m_step_time_ix == 0) play_rate->value("0.125Hz");
    else if(m_step_time_ix == 1) play_rate->value("0.25Hz");
    else if(m_step_time_ix == 2) play_rate->value("0.5Hz");
    else if(m_step_time_ix == 3) play_rate->value("1Hz");
    else if(m_step_time_ix == 4) play_rate->value("2Hz");
    else if(m_step_time_ix == 5) play_rate->value("4Hz");
    else if(m_step_time_ix == 6) play_rate->value("8Hz");
    else if(m_step_time_ix == 7) play_rate->value("16Hz");
    else if(m_step_time_ix == 8) play_rate->value("32Hz");
  }
  else
    play_rate->value("Paused");

  collect_state->value(m_collect.c_str());
  
  // Helm1
  string v1name = np_viewer->getHPlotVName("left");
  m_fld_bhvs_vname_1->value(v1name.c_str());
  string v1mode = np_viewer->getHPlotMode("left");
  m_fld_bhvs_mode_1->value(v1mode.c_str());
  string v1active = np_viewer->getHPlotBehaviors("left", "active");
  m_fld_bhvs_act_1->value(v1active.c_str());
  string v1running = np_viewer->getHPlotBehaviors("left", "running");
  m_fld_bhvs_run_1->value(v1running.c_str());
  string v1idle = np_viewer->getHPlotBehaviors("left", "idle");
  m_fld_bhvs_idle_1->value(v1idle.c_str());
  string v1completed = np_viewer->getHPlotBehaviors("left", "completed");
  m_fld_bhvs_cplt_1->value(v1completed.c_str());
  string v1decision = np_viewer->getHPlotDecision("left");
  m_fld_bhvs_dec_1->value(v1decision.c_str());

  // Helm2
  string v2name = np_viewer->getHPlotVName("right");
  m_fld_bhvs_vname_2->value(v2name.c_str());
  string v2mode = np_viewer->getHPlotMode("right");
  m_fld_bhvs_mode_2->value(v2mode.c_str());
  string v2active = np_viewer->getHPlotBehaviors("right", "active");
  m_fld_bhvs_act_2->value(v2active.c_str());
  string v2running = np_viewer->getHPlotBehaviors("right", "running");
  m_fld_bhvs_run_2->value(v2running.c_str());
  string v2idle = np_viewer->getHPlotBehaviors("right", "idle");
  m_fld_bhvs_idle_2->value(v2idle.c_str());
  string v2completed = np_viewer->getHPlotBehaviors("right", "completed");
  m_fld_bhvs_cplt_2->value(v2completed.c_str());
  string v2decision = np_viewer->getHPlotDecision("right");
  m_fld_bhvs_dec_2->value(v2decision.c_str());


  // IPF fields (A)
  string ipf_iter_a_str = ipf_viewer_a->getCurrIteration();
  ipf_iter_a->value(ipf_iter_a_str.c_str());
  string ipf_pcs_a_str = ipf_viewer_a->getCurrPieces();
  ipf_pcs_a->value(ipf_pcs_a_str.c_str());
  string ipf_pwt_a_str = ipf_viewer_a->getCurrPriority();
  ipf_pwt_a->value(ipf_pwt_a_str.c_str());
  string ipf_domain_a_str = ipf_viewer_a->getCurrDomain();
  ipf_domain_a->value(ipf_domain_a_str.c_str());

  // IPF fields (B)
  string ipf_iter_b_str = ipf_viewer_b->getCurrIteration();
  ipf_iter_b->value(ipf_iter_b_str.c_str());
  string ipf_pcs_b_str = ipf_viewer_b->getCurrPieces();
  ipf_pcs_b->value(ipf_pcs_b_str.c_str());
  string ipf_pwt_b_str = ipf_viewer_b->getCurrPriority();
  ipf_pwt_b->value(ipf_pwt_b_str.c_str());
  string ipf_domain_b_str = ipf_viewer_b->getCurrDomain();
  ipf_domain_b->value(ipf_domain_b_str.c_str());

  // Time_Low/High
  double tlow     = lp_viewer->getTimeLow();
  string tlow_str = doubleToString(tlow, 3);
  m_fld_time_low->value(tlow_str.c_str());
 
  double thigh     = lp_viewer->getTimeHigh();
  string thigh_str = doubleToString(thigh, 3);
  m_fld_time_high->value(thigh_str.c_str());

  //------------------------------
  // Label1
  string label_str1 = lp_viewer->getVariable1();
  label1->value(label_str1.c_str());

  // Low1
  string min_str1 = lp_viewer->getMinVal1();
  min_str1 = dstringCompact(min_str1);
  low1->value(min_str1.c_str());

  // High1
  string max_str1 = lp_viewer->getMaxVal1();
  max_str1 = dstringCompact(max_str1);
  high1->value(max_str1.c_str());

  // Curr1
  double curr_val1 = lp_viewer->get_curr_val1(ctime);
  string curr_str1 = doubleToString(curr_val1, 3);
  curr_str1 = dstringCompact(curr_str1);
  curr1->value(curr_str1.c_str());


  //------------------------------
  // Label2
  string label_str2 = lp_viewer->getVariable2();
  label2->value(label_str2.c_str());

  // Low2
  string min_str2 = lp_viewer->getMinVal2();
  min_str2 = dstringCompact(min_str2);
  low2->value(min_str2.c_str());

  // High2
  string max_str2 = lp_viewer->getMaxVal2();
  max_str2 = dstringCompact(max_str2);
  high2->value(max_str2.c_str());

  // Curr2
  double curr_val2 = lp_viewer->get_curr_val2(ctime);
  string curr_str2 = doubleToString(curr_val2, 3);
  curr_str2 = dstringCompact(curr_str2);
  curr2->value(curr_str2.c_str());
}

//----------------------------------------------------------
// Procedure: addLogPlot

void REPLAY_GUI::addLogPlot(const LogPlot& logplot)
{
  if(!lp_viewer)
    return;

  string vname = logplot.get_vehi_name();
  int ix = lp_viewer->addLogPlot(logplot) - 1;

  string labelA, labelB;
  if(vname != "") {
    labelA = "LogPlots(L)/" + vname + "/" + logplot.get_varname();
    labelB = "LogPlots(R)/" + vname + "/" + logplot.get_varname();
  }
  else {
    labelA = "LogPlots(L)/" + logplot.get_varname();
    labelB = "LogPlots(R)/" + logplot.get_varname();
  }

  mbar->add(labelA.c_str(), 0, 
	    (Fl_Callback*)REPLAY_GUI::cb_LeftLogPlot,  (void*)ix);
  mbar->add(labelB.c_str(), 0, 
	    (Fl_Callback*)REPLAY_GUI::cb_RightLogPlot, (void*)ix);
}

//----------------------------------------------------------
// Procedure: addHelmPlot

void REPLAY_GUI::addHelmPlot(const HelmPlot& helm_plot)
{
  if(!np_viewer) {
    cout << "NULL npviewer" << endl;
    return;
  }

  string vname = helm_plot.get_vehi_name();
  if(vname == "") 
    vname = "uknown";

  unsigned int count = np_viewer->addHelmPlot(helm_plot);
  if(count < 1)
    return;
  
  string label_a = "HelmPlots/LeftPane/" + toupper(vname);
  string label_b = "HelmPlots/RightPane/" + toupper(vname);
  
  mbar->add(label_a.c_str(), 0, 
	    (Fl_Callback*)REPLAY_GUI::cb_LeftHelmPlot,  (void*)(count-1));
  mbar->add(label_b.c_str(), 0, 
	    (Fl_Callback*)REPLAY_GUI::cb_RightHelmPlot, (void*)(count-1));  
}

//----------------------------------------------------------
// Procedure: addIPF_Plot

void REPLAY_GUI::addIPF_Plot(const IPF_Plot& ipf_plot)
{
  string vname  = ipf_plot.getVName();
  string source = tolower(ipf_plot.getSource());
  string tag    = tolower(vname) + " : " + source;

  if(!vectorContains(m_vnames, vname))
    m_vnames.push_back(vname);
  if(!vectorContains(m_sources, source))
    m_sources.push_back(source);

  m_catalog[vname].insert(source);


  if(ipf_viewer_a) {
    ipf_viewer_a->addIPF_Plot(ipf_plot);

    if((m_vname_a == "") && (m_source_a == "")) {
      m_vname_a = vname;
      m_source_a = source;
    }
    setVNameMenuButtonA(vname);
  }
  
  if(ipf_viewer_b) {
    ipf_viewer_b->addIPF_Plot(ipf_plot);

    if((m_vname_b == "") && (m_source_b == "")) {
      m_vname_b = vname;
      m_source_b = source;
    }
    setVNameMenuButtonB(vname);
 }
  
  if(ipf_viewer_a || ipf_viewer_b)
    m_num_ipfplots++;
}

//----------------------------------------------------------
// Procedure: setVNameMenuButtonA

void REPLAY_GUI::setVNameMenuButtonA(string vname) 
{
  m_vname_a = vname;
  if(!vectorContains(m_vnames, vname))
    return;

  // First get the index in the list of known vehicle names. This index is 
  // needed to configure the callback for the menu item.
  unsigned int i, vindex, vsize = m_vnames.size();
  for(i=0; i<vsize; i++)
    if(m_vnames[i] == vname)
      vindex = i;

  m_but_ipf_vname_a->add(vname.c_str(), 0, 
			 (Fl_Callback*)REPLAY_GUI::cb_TopSelectVName, (void*)vindex);
  m_but_ipf_vname_a->label(vname.c_str());

  updateSourceMenuButtonA();
}

//----------------------------------------------------------
// Procedure: updateSourceMenuButtonA
//   Purpose: Each time the vehicle is reset for the ipf viewer, the 
//            list of available sources is recalculated and the source
//            pull-down menu is reset with the new values.

void REPLAY_GUI::updateSourceMenuButtonA() 
{
  m_but_ipf_source_a->clear();
  m_but_ipf_source_a->label("<none>");

  set<string> sources = m_catalog[m_vname_a];

  if(sources.size() > 1) {
    m_but_ipf_source_a->add("collective", 0, 
			    (Fl_Callback*)REPLAY_GUI::cb_TopSelectSource, 
			    (void*)900);
    m_but_ipf_source_a->add("collective (dep)", 0, 
			    (Fl_Callback*)REPLAY_GUI::cb_TopSelectSource, 
			    (void*)901);
  }

  set<string>::const_iterator p;
  for(p=sources.begin(); p!=sources.end(); p++) {
    string source = *p;
    if(p==sources.begin()) 
      m_but_ipf_source_a->label(source.c_str());

    // First get the index in the list of known sources. This index is 
    // needed to configure the callback for the menu item.
    unsigned int i, vindex, vsize = m_sources.size();
    for(i=0; i<vsize; i++)
      if(m_sources[i] == source)
	vindex = i;
    
    m_but_ipf_source_a->add(source.c_str(), 0, 
			    (Fl_Callback*)REPLAY_GUI::cb_TopSelectSource, (void*)vindex);
  }
}

//----------------------------------------------------------
// Procedure: setVNameMenuButtonB

void REPLAY_GUI::setVNameMenuButtonB(string vname) 
{
  m_vname_b = vname;
  if(!vectorContains(m_vnames, vname))
    return;

  // First get the index in the list of known vehicle names. This index is 
  // needed to configure the callback for the menu item.
  unsigned int i, vindex, vsize = m_vnames.size();
  for(i=0; i<vsize; i++)
    if(m_vnames[i] == vname)
      vindex = i;

  m_but_ipf_vname_b->add(vname.c_str(), 0, 
			 (Fl_Callback*)REPLAY_GUI::cb_BotSelectVName, (void*)vindex);
  m_but_ipf_vname_b->label(vname.c_str());

  updateSourceMenuButtonB();
}

//----------------------------------------------------------
// Procedure: updateSourceMenuButtonB
//   Purpose: Each time the vehicle is reset for the ipf viewer, the 
//            list of available sources is recalculated and the source
//            pull-down menu is reset with the new values.

void REPLAY_GUI::updateSourceMenuButtonB() 
{
  m_but_ipf_source_b->clear();
  m_but_ipf_source_b->label("<none>");

  set<string> sources = m_catalog[m_vname_b];

  if(sources.size() > 1) {
    m_but_ipf_source_b->add("collective", 0, 
			    (Fl_Callback*)REPLAY_GUI::cb_BotSelectSource, 
			    (void*)900);
    m_but_ipf_source_b->add("collective (dep)", 0, 
			    (Fl_Callback*)REPLAY_GUI::cb_BotSelectSource, 
			    (void*)901);
  }

  set<string>::const_iterator p;
  for(p=sources.begin(); p!=sources.end(); p++) {
    string source = *p;
    if(p==sources.begin())
      m_but_ipf_source_b->label(source.c_str());

    // First get the index in the list of known sources. This index is 
    // needed to configure the callback for the menu item.
    unsigned int i, vindex, vsize = m_sources.size();
    for(i=0; i<vsize; i++)
      if(m_sources[i] == source)
	vindex = i;
    
    m_but_ipf_source_b->add(source.c_str(), 0, 
			    (Fl_Callback*)REPLAY_GUI::cb_BotSelectSource, (void*)vindex);
  }
}

//----------------------------------------------------------
// Procedure: conditional_step

void REPLAY_GUI::conditional_step() 
{
  if(m_stream) {
    bool reached_the_end = false;
    m_timer.stop();
    if(m_timer.get_float_wall_time() > m_step_time) {
      bool incremented = this->cb_Step_i(m_step_amt);
      if(!incremented)
	reached_the_end = true;
      m_timer.reset();
      if(m_collect != "Off")
	capture_to_file();
    }
    m_timer.start();
    if(reached_the_end)
      cb_StreamToggle_i();
  }
}

//----------------------------------------------------------
// Procedure: capture_to_file

void REPLAY_GUI::capture_to_file() 
{
  string command;
  command += "import -quality 90 -window alogview ";
  command += "-crop " + m_collect + "+10+40 save_file_";
  if(m_save_file_ix < 10)   command += "0";
  if(m_save_file_ix < 100)  command += "0";
  if(m_save_file_ix < 1000) command += "0";
  command += intToString(m_save_file_ix) + ".png";
  system(command.c_str());
  cout << "command: " << command << endl;
  m_save_file_ix++;
}

