/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: REPLAY_GUI.cpp                                       */
/*    DATE: May 31st, 2005                                       */
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
#include "REPLAY_GUI.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Constructor()

REPLAY_GUI::REPLAY_GUI(int g_w, int g_h, const char *g_l)
  : MarineVehiGUI(g_w, g_h, g_l) 
{
  m_stream        = false;
  m_left_mix      = -1;
  m_right_mix     = -1;

  m_replay_warp_msg = "PAUSED";
  m_replay_warp     = 1.0;  // multiplier
  m_replay_warp_ix  = 3;    // integer index
  m_replay_disp_gap = 500;  // milliseconds

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();
  this->size_range(800,600);

  augmentMenu();
  initWidgets();
  resizeWidgetsShape();

  this->end();
  this->resizable(this);
  this->show();
}

//-----------------------------------------------------------------
// Procedure: initWidgets()

void REPLAY_GUI::initWidgets()
{
  np_viewer = new NavPlotViewer(1, 1, 1, 1);
  lp_viewer = new LogPlotViewer(1, 1, 1, 1);

  m_mviewer = np_viewer;
  m_mviewer->setParam("center_view", "objects");

  // Initialize Time fields ------------------------------------------
  m_disp_time = new Fl_Output(0, 0, 1, 1, "Time:"); 
  m_disp_time->clear_visible_focus();
  m_disp_time->color(FL_RED); 
  m_disp_time->textcolor(FL_WHITE); 

  m_but_zoom_in_time = new Fl_Repeat_Button(0, 0, 1, 1, "IN");
  m_but_zoom_in_time->clear_visible_focus();
  m_but_zoom_in_time->callback((Fl_Callback*)REPLAY_GUI::cb_TimeZoom,(void*)1);
  m_but_zoom_in_time->color(FL_DARK_RED);
  m_but_zoom_in_time->labelcolor(FL_WHITE);

  m_but_zoom_out_time = new Fl_Repeat_Button(0, 0, 1, 1, "OUT");
  m_but_zoom_out_time->clear_visible_focus();
  m_but_zoom_out_time->callback((Fl_Callback*)REPLAY_GUI::cb_TimeZoom,(void*)-1);
  m_but_zoom_out_time->color(FL_DARK_RED);
  m_but_zoom_out_time->labelcolor(FL_WHITE);

  m_but_zoom_reset_time = new Fl_Button(0, 0, 1, 1, "RESET");
  m_but_zoom_reset_time->clear_visible_focus();
  m_but_zoom_reset_time->callback((Fl_Callback*)REPLAY_GUI::cb_TimeZoom,(void*)0);
  m_but_zoom_reset_time->color(FL_DARK_RED);
  m_but_zoom_reset_time->labelcolor(FL_WHITE);

  m_but_sync_scales = new Fl_Button(0, 0, 1, 1, "SYNC");
  m_but_sync_scales->clear_visible_focus();
  m_but_sync_scales->callback((Fl_Callback*)REPLAY_GUI::cb_ToggleSyncScales);
  m_but_sync_scales->color(FL_DARK_RED);
  m_but_sync_scales->labelcolor(FL_WHITE);

  m_but_hide_lp = new Fl_Check_Button(0, 0, 0, 0);
  m_but_hide_lp->callback((Fl_Callback*)REPLAY_GUI::cb_ButtonHideLogPlot, (void*)0);
  m_but_hide_lp->clear_visible_focus();
  m_but_hide_lp->value(1);

  m_but_hide_rp = new Fl_Check_Button(0, 0, 0, 0);
  m_but_hide_rp->callback((Fl_Callback*)REPLAY_GUI::cb_ButtonHideLogPlot, (void*)1);
  m_but_hide_rp->clear_visible_focus();
  m_but_hide_rp->value(1);
    
  // Handle LogPlot 1 -------------
  m_label1 = new Fl_Output(0, 0, 1, 1, "Var:"); 
  m_label1->color(FL_DARK_GREEN); 
  m_label1->clear_visible_focus();
  m_label1->textcolor(FL_WHITE); 

  m_curr1 = new Fl_Output(0, 0, 1, 1, "CurrVal:"); 
  m_curr1->clear_visible_focus();
  m_curr1->color(FL_DARK_GREEN); 
  m_curr1->textcolor(FL_WHITE); 

  // Handle LogPlot 2 -------------
  m_label2 = new Fl_Output(0, 0, 1, 1, "Var:"); 
  m_label2->clear_visible_focus();
  m_label2->textcolor(FL_WHITE); 
  m_label2->color(FL_DARK_BLUE); 

  m_curr2 = new Fl_Output(0, 0, 1, 1, "CurrVal:"); 
  m_curr2->clear_visible_focus();
  m_curr2->textcolor(FL_WHITE); 
  m_curr2->color(FL_DARK_BLUE); 

  copy_label("alogview (MIT) (PAUSED)");

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
  delete(m_disp_time);

  delete(m_label1);
  delete(m_curr1);

  delete(m_label2);
  delete(m_curr2);

  delete(m_but_zoom_in_time);
  delete(m_but_zoom_out_time);
  delete(m_but_zoom_reset_time);
  delete(m_but_sync_scales);

  delete(m_but_hide_lp);
  delete(m_but_hide_rp);

#if 0
  list<GUI_IPF*>::iterator p;
  for(p=m_sub_guis.begin(); p!=m_sub_guis.end(); p++)
    delete(*p);

  list<GUI_HelmScope*>::iterator q;
  for(q=m_sub_guis_h.begin(); q!=m_sub_guis_h.end(); q++)
    delete(*q);
#endif
}

//----------------------------------------------------------
// Procedure: resize()

void REPLAY_GUI::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgetsShape();
}

//-------------------------------------------------------------------
// Procedure: augmentMenu()

void REPLAY_GUI::augmentMenu()
{
  m_menubar->add("Replay/Streaming Toggle",  ' ', (Fl_Callback*)REPLAY_GUI::cb_Streaming,(void*)2, 0);
  m_menubar->add("Replay/Streaming Warp Faster", 'a', (Fl_Callback*)REPLAY_GUI::cb_StreamSpeed, (void*)1, 0);
  m_menubar->add("Replay/Streaming Warp Slower", 'z', (Fl_Callback*)REPLAY_GUI::cb_StreamSpeed, (void*)0, FL_MENU_DIVIDER);
  m_menubar->add("Replay/Streaming Redraw 10x Per Logged Second", 0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)100, FL_MENU_RADIO|FL_MENU_RADIO);
  m_menubar->add("Replay/Streaming Redraw 4x Per Logged Second", 0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)250, FL_MENU_RADIO|FL_MENU_RADIO);
  m_menubar->add("Replay/Streaming Redraw 2x Per Logged Second", 0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)500, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("Replay/Streaming Redraw Once Per 1 Logged Second",  0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)1000, FL_MENU_RADIO|FL_MENU_RADIO);
  m_menubar->add("Replay/Streaming Redraw Once Per 2 Logged Seconds",  0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)2000, FL_MENU_RADIO|FL_MENU_RADIO);
  m_menubar->add("Replay/Streaming Redraw Once Per 5 Logged Seconds",  0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)5000, FL_MENU_RADIO|FL_MENU_RADIO);
  m_menubar->add("Replay/Streaming Redraw Once Per 10 Logged Seconds", 0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)10000, FL_MENU_RADIO|FL_MENU_RADIO);
  m_menubar->add("Replay/Streaming Redraw Once Per 30 Logged Seconds", 0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)30000, FL_MENU_RADIO|FL_MENU_RADIO);
  m_menubar->add("Replay/Streaming Redraw Once Per 60 Logged Seconds", 0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)60000, FL_MENU_RADIO|FL_MENU_DIVIDER);

  //m_menubar->add("Replay/Step by Seconds",  0, (Fl_Callback*)REPLAY_GUI::cb_StepType, (void*)0, FL_MENU_RADIO|FL_MENU_VALUE);
  //m_menubar->add("Replay/Step by Helm Iterations",  0, (Fl_Callback*)REPLAY_GUI::cb_StepType, (void*)1, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("Replay/Jump To Start",  FL_ALT + '[', (Fl_Callback*)REPLAY_GUI::cb_JumpTime, (void*)0, 0);
  m_menubar->add("Replay/Jump To End",  FL_ALT + ']', (Fl_Callback*)REPLAY_GUI::cb_JumpTime, (void*)1, FL_MENU_DIVIDER);

  m_menubar->add("Replay/Step Ahead 0.1 secs",  FL_CTRL + ']', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)100, 0);
  m_menubar->add("Replay/Step Back  0.1 secs",  FL_CTRL + '[', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)-100, 0);
  m_menubar->add("Replay/Step Ahead 1 sec",  ']', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)1000, 0);
  m_menubar->add("Replay/Step Back  1 sec",  '[', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)-1000, 0);
  m_menubar->add("Replay/Step Ahead 5 secs", '}', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)5000, 0);
  m_menubar->add("Replay/Step Back  5 secs", '{', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)-5000, FL_MENU_DIVIDER);

  int ix1 = m_menubar->find_index("Vehicles/    Toggle Vehicle Name Mode");
  if(ix1 != -1) m_menubar->remove(ix1);
  int ix2 = m_menubar->find_index("Vehicles/    Toggle trails_connect_viewable");
  if(ix2 != -1) m_menubar->remove(ix2);
  int ix3 = m_menubar->find_index("Vehicles/Cycle Vehicle Focus");
  if(ix3 != -1) m_menubar->remove(ix3);


  // Remove some Menu items inherited from the parent class not implemented in alogview
  
  Fl_Menu_Item *item;
  
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/bearing_lines_viewable=true");
  if(item) item->hide();
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/bearing_lines_viewable=false");
  if(item) item->hide();
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/    Toggle bearing_lines_viewable");
  if(item) item->hide();

  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/vehicles_name_mode=names+mode");
  if(item) item->hide();
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/vehicles_name_mode=names+shortmode");
  if(item) item->hide();
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/vehicles_name_mode=names+auxmode");
  if(item) item->hide();
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/vehicles_name_mode=names+depth");
  if(item) item->hide();

  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/vehicles_inactive_color=yellow");
  if(item) item->hide();
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/vehicles_inactive_color=white");
  if(item) item->hide();
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/vehicles_inactive_color=blue");
  if(item) item->hide();
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/vehicles_inactive_color=green");
  if(item) item->hide();
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/vehicles_inactive_color=red");
  if(item) item->hide();


  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/trails_connect_viewable=true");
  if(item) item->hide();
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/trails_connect_viewable=false");
  if(item) item->hide();

  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/center_view = vehicle_active");
  if(item) item->hide();

  item = (Fl_Menu_Item*)m_menubar->find_item("BackView/tiff_type toggle");
  if(item) item->hide();

  item = (Fl_Menu_Item*)m_menubar->find_item("GeoAttr/DropPoints");
  if(item) item->hide();
  item = (Fl_Menu_Item*)m_menubar->find_item("GeoAttr/Datum");
  if(item) item->hide();
  //item = (Fl_Menu_Item*)m_menubar->find_item("GeoAttr/CommsPulses");
  //if(item) item->hide();
  item = (Fl_Menu_Item*)m_menubar->find_item("GeoAttr/OpArea");
  if(item) item->hide();


  // Turn on the right radio buttons
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/vehicles_viewable=true");
  if(item) item->setonly();
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/vehicles_name_mode=names");
  if(item) item->setonly();
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/vehicles_active_color=red");
  if(item) item->setonly();
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/trails_viewable=true");
  if(item) item->setonly();
  item = (Fl_Menu_Item*)m_menubar->find_item("Vehicles/trails_color=white");
  if(item) item->setonly();

  item = (Fl_Menu_Item*)m_menubar->find_item("BackView/hash_delta=100");
  if(item) item->setonly();
  item = (Fl_Menu_Item*)m_menubar->find_item("BackView/hash_viewable=true");
  if(item) item->setonly();
}

//----------------------------------------------------------
// Procedure: handle()
//     Notes: Return of 1 indicates event has been handled.

int REPLAY_GUI::handle(int event) 
{
  double lp_curr_time = 0;
  switch(event) {
  case FL_KEYDOWN:
    // Handle the Space Bar Key for pausing playback
    if(Fl::event_key() == 32) 
      cb_Streaming_i(2);
    // Handle the Zoom Keys
    else if((Fl::event_key() == 105) && (Fl::event_state() == FL_SHIFT))
      zoom(-1);
    else if((Fl::event_key() == 111) && (Fl::event_state() == FL_SHIFT))
      zoom(1);
    else if(Fl::event_key() == 105) 
      zoom(-10);
    else if(Fl::event_key() == 111)
      zoom(10);
    else if(Fl::event_key() == 108) // 'l' key
      toggleLeftLogPlot();
    else if(Fl::event_key() == 114) // 'r' key
      toggleRightLogPlot();
    // Handle the Left-Right Key Combinations
    else if((Fl::event_key() == FL_Left) && (Fl::event_state() == FL_ALT))
      handleLeftRight(10);
    else if((Fl::event_key() == FL_Right) && (Fl::event_state() == FL_ALT))
      handleLeftRight(-10);
    else if((Fl::event_key() == FL_Left) && (Fl::event_state() == FL_CTRL))
      handleLeftRight(1);
    else if((Fl::event_key() == FL_Right) && (Fl::event_state() == FL_CTRL))
      handleLeftRight(-1);
    else if(Fl::event_key() == FL_Left)
      handleLeftRight(100);
    else if(Fl::event_key() == FL_Right)
      handleLeftRight(-100);
    // Handle Up-Down Key Combinations
    else if((Fl::event_key() == FL_Up) && (Fl::event_state() == FL_ALT))
      handleUpDown(-10);
    else if((Fl::event_key() == FL_Down) && (Fl::event_state() == FL_ALT))
      handleUpDown(10);
    else if((Fl::event_key() == FL_Up) && (Fl::event_state() == FL_CTRL))
      handleUpDown(-1);
    else if((Fl::event_key() == FL_Down) && (Fl::event_state() == FL_CTRL))
      handleUpDown(1);
    else if(Fl::event_key() == FL_Up)
      handleUpDown(-100);
    else if(Fl::event_key() == FL_Down)
      handleUpDown(100);
    else
      return(Fl_Window::handle(event));
    return(1);
  case FL_PUSH:
    Fl_Window::handle(event);
    if(inLogPlotViewer()) {
      lp_curr_time = lp_viewer->getCurrTime();
      np_viewer->setCurrTime(lp_curr_time);
      np_viewer->redraw();
      updateTimeSubGUI();
      //cout << "In REPLAY_GUI::push. Curr lp time:" << lp_curr_time << endl;
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
// Procedure: setCurrTime()

void REPLAY_GUI::setCurrTime(double curr_time)
{
  if(curr_time == -1)
    curr_time = np_viewer->getStartTimeHint();

  lp_viewer->setCurrTime(curr_time);
  lp_viewer->redraw();
  np_viewer->setCurrTime(curr_time);
  np_viewer->redraw();
  updateXY();
}

//----------------------------------------- HandleUpDown
inline void REPLAY_GUI::handleUpDown(int amt) 
{
  if(inLogPlotViewer()) {
    if(amt < 0)
      lp_viewer->adjustZoom("out");
    else if(amt > 0)
      lp_viewer->adjustZoom("in");
    lp_viewer->redraw();
    updateXY();
  }
  //else if(inNavPlotViewer()) {
  else {
    np_viewer->setParam("pan_y", ((double)(amt)/10)); 
    np_viewer->setParam("center_view", "none");
    np_viewer->redraw();
    updateXY();
  }
}

//----------------------------------------- handleLeftRight
inline void REPLAY_GUI::handleLeftRight(int amt) 
{
  if(inLogPlotViewer()) {
    if(amt < 0)
      cb_Step_i(1000);
    else if(amt > 0)
      cb_Step_i(-1000);
  }
  //else if(inNavPlotViewer()) {
  else {
    np_viewer->setParam("pan_x", ((double)(amt)/10)); 
    np_viewer->setParam("center_view", "none");
    np_viewer->redraw();
    updateXY();
  }
}

//----------------------------------------- Zoom In
inline void REPLAY_GUI::zoom(int val) 
{
  if(inLogPlotViewer()) {
    if(val < 0)
      lp_viewer->adjustZoom("in");
    else if(val > 0)
      lp_viewer->adjustZoom("out");
    else
      lp_viewer->adjustZoom("reset");
    lp_viewer->redraw();
    updateXY();
  }
  //else if(inNavPlotViewer()) {
  else {
    if(val == -10)
      np_viewer->setParam("zoom", 1.03);
    else if(val == -1)
      np_viewer->setParam("zoom", 1.006);
    else if(val == 10)
      np_viewer->setParam("zoom", 0.9);
    else if(val == 1)
      np_viewer->setParam("zoom", 0.985);
    else
      np_viewer->setParam("zoom", "reset");
    np_viewer->redraw();
  }
}

//----------------------------------------- toggleLeftLogPlot
void REPLAY_GUI::toggleLeftLogPlot() 
{
  //if(!inLogPlotViewer()) 
  //  return;

  lp_viewer->toggleLeftLogPlot();
  lp_viewer->redraw();
  updateXY();
}

//----------------------------------------- toggleRightLogPlot
void REPLAY_GUI::toggleRightLogPlot() 
{
  //if(!inLogPlotViewer()) 
  //  return;
  
  lp_viewer->toggleRightLogPlot();
  lp_viewer->redraw();
  updateXY();
}

//----------------------------------------- inNavPlotViewer
bool REPLAY_GUI::inNavPlotViewer() const
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
bool REPLAY_GUI::inLogPlotViewer() const
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

//----------------------------------------- steptime
void REPLAY_GUI::steptime(int millisecs) 
{
  cb_Step_i(millisecs);
}

//----------------------------------------- streaming
void REPLAY_GUI::streaming(int val) 
{
  cb_Streaming_i(val);
}

//----------------------------------------- streaming
void REPLAY_GUI::streamspeed(bool faster) 
{
  cb_StreamSpeed_i(faster);
}

//----------------------------------------- JumpTime
inline void REPLAY_GUI::cb_JumpTime_i(int val) {
  if(val == 0)
    np_viewer->setCurrTimeStart();
  else
    np_viewer->setCurrTimeEnd();      
  np_viewer->redraw();

  double curr_time = np_viewer->getCurrTime();
  lp_viewer->setCurrTime(curr_time);
  lp_viewer->redraw();

  updateTimeSubGUI();
  updateXY();
}
void REPLAY_GUI::cb_JumpTime(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_JumpTime_i(val);
}

//----------------------------------------- Step
inline void REPLAY_GUI::cb_Step_i(int millisecs) {
  double dbl_val = (double)(millisecs) / 1000.0;
  bool time_in_bounds = np_viewer->stepTime(dbl_val);
  if(!time_in_bounds) {
    m_stream = false;
    np_viewer->setStreaming(false);
  }
  np_viewer->redraw();

  double curr_time = np_viewer->getCurrTime();
  lp_viewer->setCurrTime(curr_time);
  lp_viewer->redraw();

  updateTimeSubGUI();
  updateXY();
}
void REPLAY_GUI::cb_Step(Fl_Widget* o, int msecs) {
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_Step_i(msecs);
}

//----------------------------------------- StepType
inline void REPLAY_GUI::cb_StepType_i(int val) {
  if(val == 0)
    np_viewer->setStepType("seconds");
  if(val == 1)
    np_viewer->setStepType("helm_iterations");
}
void REPLAY_GUI::cb_StepType(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_StepType_i(val);
}

//----------------------------------------- LeftLogPlot
inline void REPLAY_GUI::cb_LeftLogPlot_i(int index) {
  lp_viewer->setLeftPlot((unsigned int)(index));
  lp_viewer->showLeftLogPlot(true);
  lp_viewer->redraw();
  updateXY();
}
void REPLAY_GUI::cb_LeftLogPlot(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_LeftLogPlot_i(val);
}

//----------------------------------------- RightLogPlot
inline void REPLAY_GUI::cb_RightLogPlot_i(int index) {
  lp_viewer->setRightPlot((unsigned int)(index));
  lp_viewer->showRightLogPlot(true);
  lp_viewer->redraw();
  updateXY();
}
void REPLAY_GUI::cb_RightLogPlot(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_RightLogPlot_i(val);
}

//----------------------------------------- cb_Encounter
inline void REPLAY_GUI::cb_Encounter_i(int aix) {
  string vname = m_dbroker.getVNameFromAix(aix);
  if(vname == "")
    return;

  double curr_time = np_viewer->getCurrTime();
  GUI_Encounters *engui = new GUI_Encounters(500,300, vname.c_str());   
  engui->setDataBroker(m_dbroker, vname);
  engui->setParentGUI(this);
  engui->setCurrTime(curr_time);

  m_sub_guis_e.push_front(engui);
  
  updateXY();
}
void REPLAY_GUI::cb_Encounter(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_Encounter_i(val);
}

//----------------------------------------- cb_VarHist
inline void REPLAY_GUI::cb_VarHist_i(int mix) {
  string vname = m_dbroker.getVNameFromMix(mix);
  string varname = m_dbroker.getVarNameFromMix(mix);
  if((vname == "") || (varname == ""))
    return;

  double curr_time = np_viewer->getCurrTime();
  string title = vname;
  GUI_VarScope *vpgui = new GUI_VarScope(500,500, title.c_str());   
  vpgui->setDataBroker(m_dbroker, mix);
  vpgui->setParentGUI(this);
  vpgui->setCurrTime(curr_time);

  m_sub_guis_v.push_front(vpgui);
  updateXY();
}
void REPLAY_GUI::cb_VarHist(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_VarHist_i(val);
}

//----------------------------------------- cb_AppLog
inline void REPLAY_GUI::cb_AppLog_i(int alix) {
  string vname    = m_dbroker.getVNameFromALix(alix);
  string app_name = m_dbroker.getAppNameFromALix(alix);
  if((vname == "") || (app_name == ""))
    return;

  double curr_time = np_viewer->getCurrTime();
  string title = vname + ":" + app_name;
  GUI_AppLogScope *algui = new GUI_AppLogScope(500,500, title.c_str());   
  algui->setDataBroker(m_dbroker, alix);
  algui->setParentGUI(this);
  algui->setGrepStr1(m_grep1);
  algui->setGrepStr2(m_grep2);
  algui->setCurrTime(curr_time);

  m_sub_guis_a.push_front(algui);
  updateXY();
}
void REPLAY_GUI::cb_AppLog(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_AppLog_i(val);
}

//----------------------------------------- cb_TaskDiary
inline void REPLAY_GUI::cb_TaskDiary_i() {

  double curr_time = np_viewer->getCurrTime();
  string title = "Task Diary (All Vehicles)";
  GUI_TaskDiary *tdgui = new GUI_TaskDiary(500,500, title.c_str());   
  tdgui->setDataBroker(m_dbroker);
  tdgui->setParentGUI(this);
  tdgui->setCurrTime(curr_time);

  m_sub_guis_t.push_front(tdgui);
  updateXY();
}
void REPLAY_GUI::cb_TaskDiary(Fl_Widget* o) {
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_TaskDiary_i();
}

//----------------------------------------- cb_IPF_GUI
inline void REPLAY_GUI::cb_IPF_GUI_i(int aix) {
  string vname = m_dbroker.getVNameFromAix(aix);
  if(vname == "")
    return;

  double curr_time = np_viewer->getCurrTime();
  GUI_IPF *ipfgui = new GUI_IPF(550,300, vname.c_str());   
  ipfgui->setDataBroker(m_dbroker, vname);
  ipfgui->setParentGUI(this);
  ipfgui->setCurrTime(curr_time);
  ipfgui->applyBehaviorVarMap(m_map_bhv_vars);

  m_sub_guis.push_front(ipfgui);
  
  updateXY();
}
void REPLAY_GUI::cb_IPF_GUI(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_IPF_GUI_i(val);
}

//----------------------------------------- cb_Helm_GUI
inline void REPLAY_GUI::cb_Helm_GUI_i(int aix) {
  string vname = m_dbroker.getVNameFromAix(aix);
  if(vname == "")
    return;

  double curr_time = np_viewer->getCurrTime();
  GUI_HelmScope *helm_gui = new GUI_HelmScope(640,520, vname.c_str());   

  helm_gui->setDataBroker(m_dbroker, vname);
  helm_gui->setParentGUI(this);
  helm_gui->setCurrTime(curr_time);

  m_sub_guis_h.push_front(helm_gui);
  
  updateXY();
}
void REPLAY_GUI::cb_Helm_GUI(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_Helm_GUI_i(val);
}

//----------------------------------------- TimeZoom
inline void REPLAY_GUI::cb_TimeZoom_i(int val) {
  if(val > 0)
    lp_viewer->adjustZoom("in");
  else if(val < 0)
    lp_viewer->adjustZoom("out");
  else {
    lp_viewer->adjustZoom("reset");
  }
  updateXY();
}

void REPLAY_GUI::cb_TimeZoom(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_TimeZoom_i(val);
}

//----------------------------------------- ToggleSyncScales
inline void REPLAY_GUI::cb_ToggleSyncScales_i(int val) {
  lp_viewer->setSyncScales("toggle");
  updateXY();
}

void REPLAY_GUI::cb_ToggleSyncScales(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_ToggleSyncScales_i(val);
}

//----------------------------------------- ButtonHideLogPlot
inline void REPLAY_GUI::cb_ButtonHideLogPlot_i(int val) {
  if(val == 0) {
    if(m_but_hide_lp->value())
      lp_viewer->showLeftLogPlot(true);
    else
      lp_viewer->showLeftLogPlot(false);
  }
  else if(val == 1) {
    if(m_but_hide_rp->value())
      lp_viewer->showRightLogPlot(true);
    else
      lp_viewer->showRightLogPlot(false);
    lp_viewer->redraw();
  }
  else
    return;

  lp_viewer->redraw();
  updateXY();
}

void REPLAY_GUI::cb_ButtonHideLogPlot(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_ButtonHideLogPlot_i(val);
}

//----------------------------------------- Streaming
inline void REPLAY_GUI::cb_Streaming_i(int val) {
  bool prev_stream = m_stream;
  if(val == 0) 
    m_stream = false;
  else if(val == 1) 
    m_stream = true;
  else 
    m_stream = !m_stream;

  if(m_stream && !prev_stream)
    m_timer.start();
  else if(!m_stream && prev_stream)
    m_timer.stop();
  
  np_viewer->setStreaming(m_stream);

  updateXY();
  updateTimeSubGUI();
}
void REPLAY_GUI::cb_Streaming(Fl_Widget* o, int v) {
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_Streaming_i(v);
}

//----------------------------------------- StreamStep

inline void REPLAY_GUI::cb_StreamStep_i(int val) {
  m_replay_disp_gap = val;
}

void REPLAY_GUI::cb_StreamStep(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_StreamStep_i(val);
}

//----------------------------------------- StreamSpeed
inline void REPLAY_GUI::cb_StreamSpeed_i(bool faster) {
  // First handle case where we start accelerating from a paused state
  if(faster && !m_stream) {
    m_replay_warp_ix = 3;
    m_replay_warp = 1;
    m_warp_gaps.clear();
    cb_Streaming_i(1);
    updateXY();
    return;
  }

  int prev_replay_warp_ix = m_replay_warp_ix;

  // Adjust the rate index
  if(faster)
    m_replay_warp_ix++;
  else
    m_replay_warp_ix--;

  // clip the rate index to the range [0,9]
  if(m_replay_warp_ix > 9)
    m_replay_warp_ix = 9;
  else if(m_replay_warp_ix < 0)
    m_replay_warp_ix = 0;

  // Apply the rate index to other needed values
  if(m_replay_warp_ix == 0) 
    m_replay_warp = 8.0;
  else if(m_replay_warp_ix == 1) 
    m_replay_warp = 4.0;
  else if(m_replay_warp_ix == 2) 
    m_replay_warp = 2.0;
  else if(m_replay_warp_ix == 3) 
    m_replay_warp = 1.0;
  else if(m_replay_warp_ix == 4) 
    m_replay_warp = 0.5;
  else if(m_replay_warp_ix == 5) 
    m_replay_warp = 0.25;
  else if(m_replay_warp_ix == 6) 
    m_replay_warp = 0.125;
  else if(m_replay_warp_ix == 7) 
    m_replay_warp = 0.0625;
  else if(m_replay_warp_ix == 8) 
    m_replay_warp = 0.03125;
  else if(m_replay_warp_ix == 9) 
    m_replay_warp = 0.015625;

  if(m_replay_warp_ix != prev_replay_warp_ix)
    m_warp_gaps.clear();
  
  updateXY();
}
void REPLAY_GUI::cb_StreamSpeed(Fl_Widget* o, bool v) {
  bool val = (bool)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_StreamSpeed_i(val);
}

//----------------------------------------- updatePlayRateMsg
void REPLAY_GUI::updatePlayRateMsg() 
{
  m_replay_warp_msg = "(PAUSED)";
  if(m_stream) {
    if(m_replay_warp_ix == 0) m_replay_warp_msg = "(Time Warp = 0.125)";
    else if(m_replay_warp_ix == 1) m_replay_warp_msg = "(Time Warp = 0.25)";
    else if(m_replay_warp_ix == 2) m_replay_warp_msg = "(Time Warp = 0.5)";
    else if(m_replay_warp_ix == 3) m_replay_warp_msg = "(Time Warp = 1)";
    else if(m_replay_warp_ix == 4) m_replay_warp_msg = "(Time Warp = 2)";
    else if(m_replay_warp_ix == 5) m_replay_warp_msg = "(Time Warp = 4)";
    else if(m_replay_warp_ix == 6) m_replay_warp_msg = "(Time Warp = 8)";
    else if(m_replay_warp_ix == 7) m_replay_warp_msg = "(Time Warp = 16)";
    else if(m_replay_warp_ix == 8) m_replay_warp_msg = "(Time Warp = 32)";
    else if(m_replay_warp_ix == 9) m_replay_warp_msg = "(Time Warp = 64)";

    if(m_warp_gaps.size() > 0) {
      double gaps_total = 0;
      list<double>::iterator p;
      for(p=m_warp_gaps.begin(); p!=m_warp_gaps.end(); p++)
	gaps_total += *p;     
      double gaps_avg = gaps_total / ((double)(m_warp_gaps.size()));
      double actual_warp  = ((double)(m_replay_disp_gap) / 1000.0) / gaps_avg;
      
      double pct_lagging = actual_warp / (1.0/m_replay_warp);
      if(pct_lagging < 0.85)
	m_replay_warp_msg += " (Actual:" + doubleToString(actual_warp,2) + ")";
      //m_replay_warp_msg += " (Actual:" + doubleToString(actual_warp,2) + "/" + doubleToString(m_replay_warp) + ")";
    }
    
  }
}

//----------------------------------------- updateXY
void REPLAY_GUI::updateXY() 
{
  // DISPLAY TIME
  double ctime = np_viewer->getCurrTime();

  string dtime = doubleToString(ctime, 3);
  m_disp_time->value(dtime.c_str());
  
  updatePlayRateMsg();
  string title = "alogview (MIT) " + m_replay_warp_msg;
  copy_label(title.c_str());

  //------------------------------
  // Label1
  string label_str1 = lp_viewer->getFullVar1();
  m_label1->value(label_str1.c_str());

  // Curr1
  double curr_val1 = lp_viewer->get_curr_val1(ctime);
  string curr_str1 = doubleToString(curr_val1, 3);
  curr_str1 = dstringCompact(curr_str1);
  m_curr1->value(curr_str1.c_str());

  //------------------------------
  // Label2
  string label_str2 = lp_viewer->getFullVar2();
  m_label2->value(label_str2.c_str());

  // Curr2
  double curr_val2 = lp_viewer->get_curr_val2(ctime);
  string curr_str2 = doubleToString(curr_val2, 3);
  curr_str2 = dstringCompact(curr_str2);
  m_curr2->value(curr_str2.c_str());
}

//----------------------------------------------------------
// Procedure: setDataBroker()

void REPLAY_GUI::setDataBroker(const ALogDataBroker& broker)
{
  cout << "  1: Copying the broker..." << endl;
  m_dbroker = broker;
  cout << "  2: Setting LogPlot Menus..." << endl;
  setLogPlotMenus();
  cout << "  3: Setting IPF Plot Menus..." << endl;
  setIPFPlotMenus();
  cout << "  4: Setting Encounter Plot Menus..." << endl;
  setEncounterPlotMenus();
  cout << "  5: Setting Helm Plot Menus..." << endl;
  setHelmPlotMenus();
  cout << "  6: Setting Var Hist Menus..." << endl;
  setVarHistMenus();
  cout << "  7: Setting AppLog Menus..." << endl;
  setAppLogMenus();
  cout << "  8: Setting TaskDiary Menus..." << endl;
  setTaskDiaryMenus();

  cout << "  9: Setting the DataBroker for the NavPlot Viewer..." << endl;
  if(np_viewer) {
    np_viewer->setDataBroker(broker);
    np_viewer->initPlots();
  }
  cout << " 10: Setting the DataBroker for the LogPlot Viewer..." << endl;
  if(lp_viewer) {
    lp_viewer->setDataBroker(broker);
  }
}

//----------------------------------------------------------
// Procedure: setLogPlotMenus()
//      Note: MIX short for MasterIndex

void REPLAY_GUI::setLogPlotMenus()
{
  unsigned int mix_count = m_dbroker.sizeMix();

  for(unsigned int mix=0; mix<mix_count; mix++) {
    if(m_dbroker.getVarTypeFromMix(mix) == "double") {
      string vname   = m_dbroker.getVNameFromMix(mix);
      string varname = m_dbroker.getVarNameFromMix(mix);
      
      // Use special unsigned int type having same size a pointer (void*)
      uintptr_t ix = mix;
      
      string labelA = "LogPlots(L)/" + varname;
      string labelB = "LogPlots(R)/" + varname;
      if(vname != "") {
	labelA = "LogPlots(L)/" + vname + "/" + varname;
	labelB = "LogPlots(R)/" + vname + "/" + varname;
      }

      m_menubar->add(labelA.c_str(), 0, 
		     (Fl_Callback*)REPLAY_GUI::cb_LeftLogPlot,  (void*)ix);
      m_menubar->add(labelB.c_str(), 0, 
		     (Fl_Callback*)REPLAY_GUI::cb_RightLogPlot, (void*)ix);
      m_menubar->redraw();
    }
  }
}

//----------------------------------------------------------
// Procedure: setBehaviorVarMap()
//      Note: The map associates a given behavior instance with one or more
//            MOOS variables. Simply for convenience so the GUI_IPF when
//            launched can come up with the desired scope variable(s) already
//            chosen.

void REPLAY_GUI::setBehaviorVarMap(map<string,string> map_bhv_vars)
{
  m_map_bhv_vars = map_bhv_vars;
}

//----------------------------------------------------------
// Procedure: setVarHistMenus()
//      Note: MIX short for MasterIndex

void REPLAY_GUI::setVarHistMenus()
{
  unsigned int mix_count = m_dbroker.sizeMix();
  
  for(unsigned int mix=0; mix<mix_count; mix++) {
    string vname   = m_dbroker.getVNameFromMix(mix);
    string varname = m_dbroker.getVarNameFromMix(mix);
    string vartype = m_dbroker.getVarTypeFromMix(mix);
    
    // Use special unsigned int type having same size a pointer (void*)
    uintptr_t ix = mix;

    string title = "VarHist(D)";
    if(vartype == "string")
      title = "VarHist(S)";
    
    string label = title + "/" + varname;
    if(vname != "") 
      label = title + "/" + vname + "/" + varname;

    m_menubar->add(label.c_str(), 0, 
		   (Fl_Callback*)REPLAY_GUI::cb_VarHist,  (void*)ix);

    if(vartype == "string")
      setMenuItemColor(label);
    
    m_menubar->redraw();
  }
}

//----------------------------------------------------------
// Procedure: setAppLogMenus()
//      Note: MIX short for MasterIndex

void REPLAY_GUI::setAppLogMenus()
{
  unsigned int alix_count = m_dbroker.sizeALix();

  for(unsigned int alix=0; alix<alix_count; alix++) {
    string vname    = m_dbroker.getVNameFromALix(alix);
    string app_name = m_dbroker.getAppNameFromALix(alix);

    // Use special unsigned int type having same size a pointer (void*)
    uintptr_t ix = alix;

    string label = "AppLogs/" + vname + "/" + app_name;
    
    m_menubar->add(label.c_str(), 0, 
		   (Fl_Callback*)REPLAY_GUI::cb_AppLog, (void*)ix);
    m_menubar->redraw();
  }
}

//----------------------------------------------------------
// Procedure: setTaskDiaryMenus()

void REPLAY_GUI::setTaskDiaryMenus()
{
  TaskDiary task_diary = m_dbroker.getTaskDiary();
  if(task_diary.size() == 0)
    return;
  
  string label = "TaskDiary";
  
  m_menubar->add(label.c_str(), 0, 
		 (Fl_Callback*)REPLAY_GUI::cb_TaskDiary);
  m_menubar->redraw();
}

//----------------------------------------------------------
// Procedure: setIPFPlotMenus()

void REPLAY_GUI::setIPFPlotMenus()
{
  unsigned int aix_count = m_dbroker.sizeALogs();

  for(unsigned int aix=0; aix<aix_count; aix++) {
    string vname = m_dbroker.getVNameFromAix(aix);

    // Use special unsigned int type having same size a pointer (void*)
    uintptr_t ix = aix;

    string label = "IPFPlots/" + vname;
    
    m_menubar->add(label.c_str(), 0, 
		   (Fl_Callback*)REPLAY_GUI::cb_IPF_GUI, (void*)ix);
    m_menubar->redraw();
  }
}

//----------------------------------------------------------
// Procedure: setEncounterPlotMenus()

void REPLAY_GUI::setEncounterPlotMenus()
{
  unsigned int aix_count = m_dbroker.sizeALogs();

  for(unsigned int aix=0; aix<aix_count; aix++) {
    string vname = m_dbroker.getVNameFromAix(aix);

    // Use special unsigned int type having same size a pointer (void*)
    uintptr_t ix = aix;

    string label = "Encounters/" + vname;
    
    m_menubar->add(label.c_str(), 0, 
		   (Fl_Callback*)REPLAY_GUI::cb_Encounter, (void*)ix);
    m_menubar->redraw();
  }
}

//----------------------------------------------------------
// Procedure: setHelmPlotMenus()
//   Purpose: Add the pull-down menu for selecting a vehicle and opening
//            a Helm Report sub-gui

void REPLAY_GUI::setHelmPlotMenus()
{
  unsigned int aix_count = m_dbroker.sizeALogs();

  for(unsigned int aix=0; aix<aix_count; aix++) {
    string vname = m_dbroker.getVNameFromAix(aix);

    // Use special unsigned int type having same size a pointer (void*)
    uintptr_t ix = aix;

    string label = "HelmPlots/" + vname;
    
    m_menubar->add(label.c_str(), 0, 
		   (Fl_Callback*)REPLAY_GUI::cb_Helm_GUI, (void*)ix);
    m_menubar->redraw();
  }
}

//----------------------------------------------------------
// Procedure: initLogPlotChoiceA()

void REPLAY_GUI::initLogPlotChoiceA(string vname, string varname)
{
  if(vname == "")
    vname = m_dbroker.getVNameFromAix(0);

  unsigned int mix = m_dbroker.getMixFromVNameVarName(vname, varname);

  if(mix >= m_dbroker.sizeMix())
    return;
  
  //cout << "*****************************************" << endl;
  //cout << "In REPLAY_GUI::cb_LeftLogPlot_i: mix:" << mix << endl;
  //cout << "*****************************************" << endl;
  cb_LeftLogPlot_i((int)(mix));
}

//----------------------------------------------------------
// Procedure: initLogPlotChoiceB()

void REPLAY_GUI::initLogPlotChoiceB(string vname, string varname)
{
  if(vname == "")
    vname = m_dbroker.getVNameFromAix(0);

  unsigned int mix = m_dbroker.getMixFromVNameVarName(vname, varname);

  if(mix >= m_dbroker.sizeMix())
    return;
  cb_RightLogPlot_i((int)(mix));
}


//----------------------------------------------------------
// Procedure: conditionalStep()

void REPLAY_GUI::conditionalStep() 
{
  if(m_stream) {
    m_timer.stop();
    double gap_thresh = (double)(m_replay_disp_gap) / 1000.0;
    double warped_thresh = m_replay_warp * gap_thresh;
    
    double observed_gap = m_timer.get_float_wall_time();
    if(observed_gap > warped_thresh) {
      cb_Step_i(m_replay_disp_gap);
      m_timer.reset();

      m_warp_gaps.push_front(observed_gap);
      if(m_warp_gaps.size() > 10)
	m_warp_gaps.pop_back();
    }
    m_timer.start();
  }
}

//----------------------------------------------------------
// Procedure: updateTimeSubGUI()

void REPLAY_GUI::updateTimeSubGUI()
{
  // Update the times to all the sub_guis
  double curr_time = np_viewer->getCurrTime();
  list<GUI_IPF*>::iterator p;
  for(p=m_sub_guis.begin(); p!=m_sub_guis.end(); p++) {
    GUI_IPF *sub_gui = *p;
    sub_gui->setCurrTime(curr_time);
    sub_gui->setReplayWarpMsg(m_replay_warp_msg);
  }
  list<GUI_HelmScope*>::iterator p2;
  for(p2=m_sub_guis_h.begin(); p2!=m_sub_guis_h.end(); p2++) {
    GUI_HelmScope *sub_gui = *p2;
    sub_gui->setCurrTime(curr_time);
    sub_gui->setReplayWarpMsg(m_replay_warp_msg);
  }
  list<GUI_VarScope*>::iterator p3;
  for(p3=m_sub_guis_v.begin(); p3!=m_sub_guis_v.end(); p3++) {
    GUI_VarScope *sub_gui = *p3;
    sub_gui->setCurrTime(curr_time);
    sub_gui->setReplayWarpMsg(m_replay_warp_msg);
  }
  list<GUI_Encounters*>::iterator p4;
  for(p4=m_sub_guis_e.begin(); p4!=m_sub_guis_e.end(); p4++) {
    GUI_Encounters *sub_gui = *p4;
    sub_gui->setCurrTime(curr_time);
    sub_gui->setReplayWarpMsg(m_replay_warp_msg);
  }
  list<GUI_AppLogScope*>::iterator p5;
  for(p5=m_sub_guis_a.begin(); p5!=m_sub_guis_a.end(); p5++) {
    GUI_AppLogScope *sub_gui = *p5;
    sub_gui->setCurrTime(curr_time);
    sub_gui->setReplayWarpMsg(m_replay_warp_msg);
  }
  list<GUI_TaskDiary*>::iterator p6;
  for(p6=m_sub_guis_t.begin(); p6!=m_sub_guis_t.end(); p6++) {
    GUI_TaskDiary *sub_gui = *p6;
    sub_gui->setCurrTime(curr_time);
    sub_gui->setReplayWarpMsg(m_replay_warp_msg);
  }
}


//----------------------------------------------------------
// Procedure: resizeWidgetsText()

void REPLAY_GUI::resizeWidgetsText(int wsize) 
{
  // LogPlot1
  m_label1->labelsize(wsize);
  m_curr1->labelsize(wsize);

  m_label1->textsize(wsize);
  m_curr1->textsize(wsize);

  // LogPlot2
  m_label2->labelsize(wsize);
  m_curr2->labelsize(wsize);

  m_label2->textsize(wsize);
  m_curr2->textsize(wsize);

  // TopCenter Fields
  m_disp_time->labelsize(wsize);
  m_disp_time->textsize(wsize);
  
  m_but_zoom_in_time->labelsize(wsize);
  m_but_zoom_out_time->labelsize(wsize);
  m_but_zoom_reset_time->labelsize(wsize);
  m_but_sync_scales->labelsize(wsize);
}



//----------------------------------------------------------
// Procedure: resizeWidgetsShape()

void REPLAY_GUI::resizeWidgetsShape() 
{
  double now_hgt = h();
  //double now_wid = w();
 
  // Part 1: Figure out the basic pane extents.
  // ================================================================
  // This is the default extent configuration so long as any minimal
  // constraints are not violated.

  //double pct_view_hgt = 0.755;
  double pct_menu_hgt = 0.035;
  double pct_data_hgt = 0.210; // Above three add up to 1.0

  // Make sure the menu bar is at least 15 and at most 25
  if((pct_menu_hgt * now_hgt) < 15) 
    pct_menu_hgt = 15 / now_hgt;
  if((pct_menu_hgt * now_hgt) > 25) 
    pct_menu_hgt = 25 / now_hgt;

  //pct_view_hgt = (1 - (pct_menu_hgt + pct_data_hgt));

  // Part 2: Adjust the extents of the MENU Bar
  // ================================================================  
  double menu_hgt = h() * pct_menu_hgt;

  m_menubar->resize(0, 0, w(), menu_hgt);
  
  int menu_font_size = 14;
  if(now_hgt < 550) 
    menu_font_size = 10;
  else if(now_hgt < 800) 
    menu_font_size = 12;
  
  int  msize = m_menubar->size();

  const Fl_Menu_Item* itemsx = m_menubar->menu();
  Fl_Menu_Item* items = (Fl_Menu_Item*)(itemsx);

  //Fl_Color blue = fl_rgb_color(71, 71, 205);    
  for(int i=0; i<msize; i++) {
    //items[i].labelcolor(blue);
    items[i].labelsize(menu_font_size);
  }

  // Part 3: Adjust the extents of the VIEWER window
  // ================================================================
  double xpos = 0;
  double ypos = 0 + menu_hgt;
  double xwid = w();
  double yhgt = h() - (int)(menu_hgt);

  int datafld_hgt = h() * pct_data_hgt;
  yhgt -= datafld_hgt;

  m_mviewer->resize(xpos, ypos, xwid, yhgt);

  // Part 6: Adjust the extents of the DATA block of widgets
  // ------------------------------------------------------------
  double dh = h() * pct_data_hgt;
  double dy = h()-dh;
  double dw = w();

  double wh = 25; // widgetheight

  lp_viewer->resize(0, dy+wh, w(), dh-wh);

  double fld_hgt = 18;

  if((dw < 850) || (dw < 680))
    resizeWidgetsText(8);
  else
    resizeWidgetsText(10);

  // height = 150 is the "baseline" design. All adjustments from there.
  double row = dy + 4;

  // position of time button. Use as anchor for resizing fields to left.
  double timex = dw * 0.40;  
    
  // LogPlot1
  int lab1_x = 30;
  int lab1_y = row;
  int lab1_w = dw * 0.18;
  int lab1_h = fld_hgt;
  m_label1->resize(lab1_x, lab1_y, lab1_w, lab1_h);
  int chk1_x = lab1_x + lab1_w + 5;
  int chk1_y = row+2;
  int chk1_w = 16;
  int chk1_h = 16;
  m_but_hide_lp->resize(chk1_x, chk1_y, chk1_w, chk1_h);
  int cur1_x = chk1_x + chk1_w + 45;
  int cur1_y = row;
  int cur1_w = timex - cur1_x - 35;
  int cur1_h = fld_hgt;
  m_curr1->resize(cur1_x, cur1_y, cur1_w, cur1_h);

  // LogPlot2
  int lab2_x = dw*0.66;
  int lab2_y = row;
  int lab2_w = dw * 0.18;
  int lab2_h = fld_hgt;
  m_label2->resize(lab2_x, lab2_y, lab2_w, lab2_h);
  int chk2_x = lab2_x + lab2_w + 5;
  int chk2_y = row+2;
  int chk2_w = 16;
  int chk2_h = 16;
  m_but_hide_rp->resize(chk2_x, chk2_y, chk2_w, chk2_h);
  int cur2_x = chk2_x + chk2_w + 50;
  int cur2_y = row;
  int cur2_w = w() - cur2_x - 2;
  int cur2_h = fld_hgt;
  m_curr2->resize(cur2_x, cur2_y, cur2_w, cur2_h);

  // TopCenter Fields
  m_disp_time->resize(dw*0.40, row, dw*0.05, fld_hgt);
  m_but_zoom_in_time->resize(dw*0.46, row, dw*0.03, fld_hgt);
  m_but_zoom_out_time->resize(dw*0.50, row, dw*0.03, fld_hgt);
  m_but_zoom_reset_time->resize(dw*0.54, row, dw*0.04, fld_hgt);
  m_but_sync_scales->resize(dw*0.59, row, dw*0.035, fld_hgt);

  // Invoke redraw() on all widgets
  m_disp_time->redraw();
  m_label1->redraw();
  m_curr1->redraw();
  m_label2->redraw();
  m_curr2->redraw();
  m_disp_time->redraw();
  m_but_zoom_in_time->redraw();
  m_but_zoom_out_time->redraw();
  m_but_zoom_reset_time->redraw();
  m_but_sync_scales->redraw();
}






