/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: REPLAY_GUI.cpp                                       */
/*    DATE: May 31st, 2005                                       */
/*    DATE: Feb 2nd, 2015 Major overhaul/simplification          */
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
// Constructor

REPLAY_GUI::REPLAY_GUI(int g_w, int g_h, const char *g_l)
  : MarineVehiGUI(g_w, g_h, g_l) 
{
  m_stream       = false;
  m_step_amt     = 1;
  m_step_time    = 1.0;
  m_step_time_ix = 3;
  m_left_mix     = -1;
  m_right_mix    = -1;

  m_clip_time_min = 0;
  m_clip_time_max = 0;

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();
  this->size_range(800,600);

  augmentMenu();

  int info_size = 10;
  // Init window with bogus extents. Actual extents
  // are set in the setWindowLayout() call below.
  np_viewer    = new NavPlotViewer(1, 1, 1, 1);
  lp_viewer    = new LogPlotViewer(1, 1, 1, 1);

  setWindowLayout("normal");

  m_mviewer = np_viewer;

  // It's only polite to center the view on the center of the 
  // bounding box of the total vehicle trails.
  m_mviewer->setParam("center_view", "average");

  // Initialize Time fields ------------------------------------------
  double time_pos = (w()/2)-160;
  m_disp_time = new MY_Output(time_pos, h()-(m_lp_viewer_hgt+26), 
			    110, 22, "Time:"); 
  m_disp_time->color(FL_RED); 
  m_disp_time->textcolor(FL_WHITE); 
  m_disp_time->textsize(12); 
  m_disp_time->labelsize(12);

  m_but_zoom_in_time = new MY_Repeat_Button(time_pos+210, h()-(m_lp_viewer_hgt+26), 
					    30, 21, "IN");
  m_but_zoom_out_time = new MY_Repeat_Button(time_pos+240, h()-(m_lp_viewer_hgt+26), 
					     35, 21, "OUT");
  m_but_zoom_reset_time = new MY_Button(time_pos+290, h()-(m_lp_viewer_hgt+26), 
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

  // Handle LogPlot 1 -------------
  m_label1 = new MY_Output(35, h()-(m_lp_viewer_hgt+25), 150, 20, "Var:"); 
  m_label1->textsize(info_size-1); 
  m_label1->color(FL_DARK_GREEN); 
  m_label1->textcolor(FL_WHITE); 
  m_label1->labelsize(info_size);

  m_curr1 = new MY_Output(255, h()-(m_lp_viewer_hgt+25), 100, 20, "CurrVal:"); 
  m_curr1->textsize(info_size); 
  m_curr1->color(FL_DARK_GREEN); 
  m_curr1->textcolor(FL_WHITE); 
  m_curr1->labelsize(info_size);

  m_low1 = new MY_Output(2, h()-20, 51, 20, ""); 
  m_low1->textsize(info_size); 
  m_low1->color(FL_DARK_GREEN); 
  m_low1->textcolor(FL_WHITE); 
  m_low1->labelsize(info_size);

  m_high1 = new MY_Output(2, h()-(m_lp_viewer_hgt-2), 51, 20, ""); 
  m_high1->textsize(info_size); 
  m_high1->color(FL_DARK_GREEN); 
  m_high1->textcolor(FL_WHITE); 
  m_high1->labelsize(info_size);

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
  m_label2 = new MY_Output(w()-154, h()-(m_lp_viewer_hgt+25), 150, 20, "Var:"); 
  m_label2->textsize(info_size-1); 
  m_label2->labelsize(info_size);
  m_label2->textcolor(FL_WHITE); 
  m_label2->color(FL_DARK_BLUE); 

  m_curr2 = new MY_Output(w()-285, h()-(m_lp_viewer_hgt+25), 100, 20, "CurrVal:"); 
  m_curr2->textsize(info_size); 
  m_curr2->labelsize(info_size);
  m_curr2->textcolor(FL_WHITE); 
  m_curr2->color(FL_DARK_BLUE); 

  m_low2 = new MY_Output(w()-110+57, h()-20, 51, 20, ""); 
  m_low2->textsize(info_size); 
  m_low2->color(FL_DARK_BLUE); 
  m_low2->textcolor(FL_WHITE); 
  m_low2->labelsize(info_size);

  m_high2 = new MY_Output(w()-110+57, h()-(m_lp_viewer_hgt-2), 51, 20, ""); 
  m_high2->textsize(info_size); 
  m_high2->color(FL_DARK_BLUE); 
  m_high2->textcolor(FL_WHITE); 
  m_high2->labelsize(info_size);

  copy_label("alogview (PAUSED)");

  resizeWidgets();
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
  delete(m_low1);
  delete(m_high1);
  delete(m_curr1);
  delete(m_fld_time_low);
  delete(m_fld_time_high);

  delete(m_label2);
  delete(m_low2);
  delete(m_high2);
  delete(m_curr2);
  delete(m_but_zoom_in_time);
  delete(m_but_zoom_out_time);
  delete(m_but_zoom_reset_time);
}

//----------------------------------------------------------
// Procedure: resize

void REPLAY_GUI::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgets();
}

//-------------------------------------------------------------------
// Procedure: setWindowLayout()

void REPLAY_GUI::setWindowLayout(string layout)
{
  int npw, nph, npx, npy;
  int lpw, lph, lpx, lpy;

  m_lp_viewer_hgt = 140;
  m_np_viewer_hgt = h()-440;

  if(layout == "normal") {
    m_np_viewer_hgt = h()-240;
  }
  else if(layout == "fullview") {
    m_np_viewer_hgt = h()-200;
  }

  // Set the extents of the NavPlotViewer
  npx = 0;
  npy = 30;
  nph = m_np_viewer_hgt;
  npw = w();
  
  // Set the extents of the LogPlotViewer
  int margin = 2;
  lpx = 90;
  lph = m_lp_viewer_hgt;
  lpy = h() - (lph - margin);
  lpw = w() - (2 * lpx);
  lph = lph - (2 * margin);
  
  if(np_viewer)
    np_viewer->resize(npx, npy, npw, nph);
  if(lp_viewer)
    lp_viewer->resize(lpx, lpy, lpw, lph);
  
  m_np_viewer_hgt = nph;
  m_lp_viewer_hgt = lph;
}


//-------------------------------------------------------------------
// Procedure: augmentMenu()

void REPLAY_GUI::augmentMenu()
{
  m_menubar->add("Replay/Streaming Toggle",  '=', (Fl_Callback*)REPLAY_GUI::cb_Streaming,(void*)2, 0);
  m_menubar->add("Replay/Streaming Faster", 'a', (Fl_Callback*)REPLAY_GUI::cb_StreamSpeed, (void*)1, 0);
  m_menubar->add("Replay/Streaming Slower", 'z', (Fl_Callback*)REPLAY_GUI::cb_StreamSpeed, (void*)0, FL_MENU_DIVIDER);
  m_menubar->add("Replay/Streaming Step 1",  0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)1, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("Replay/Streaming Step 3",  0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)3, FL_MENU_RADIO);
  m_menubar->add("Replay/Streaming Step 5",  0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)5, FL_MENU_RADIO);
  m_menubar->add("Replay/Streaming Step 10", 0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)10, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("Replay/Step by Seconds",  0, (Fl_Callback*)REPLAY_GUI::cb_StepType, (void*)0, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("Replay/Step by Helm Iterations",  0, (Fl_Callback*)REPLAY_GUI::cb_StepType, (void*)1, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("Replay/Step Ahead 1",  ']', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)1, 0);
  m_menubar->add("Replay/Step Back  1",  '[', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)-1, 0);
  m_menubar->add("Replay/Step Ahead 5", '>', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)5, 0);
  m_menubar->add("Replay/Step Back  5", '<', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)-5, FL_MENU_DIVIDER);
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

int REPLAY_GUI::handle(int event) 
{
  double lpv_curr_time = 0;
  switch(event) {
  case FL_KEYDOWN:
    // Handle the Space Bar Key for pausing playback
    if(Fl::event_key() == 32) 
      cb_Streaming_i(0);
    // Handle the Zoom Keys
    else if((Fl::event_key() == 105) && (Fl::event_state() == FL_SHIFT))
      zoom(-1);
    else if((Fl::event_key() == 111) && (Fl::event_state() == FL_SHIFT))
      zoom(1);
    else if(Fl::event_key() == 105) 
      zoom(-10);
    else if(Fl::event_key() == 111)
      zoom(10);
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
      lpv_curr_time = lp_viewer->getCurrTime();
      np_viewer->setCurrTime(lpv_curr_time);
      updateTimeSubGUI();
      cout << "In REPLAY_GUI::push" << endl;
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
  updateXY();
}

//----------------------------------------------------------
// Procedure: setClipTimeMin

void REPLAY_GUI::setClipTimeMin(double clip_time)
{
  m_clip_time_min = clip_time;

  // Ensure min_clip time is always greater than zero.
  if(m_clip_time_min < 0)
    m_clip_time_min = 0;

  // Ensure max_clip time is always greater and that
  // the clip interval is always at least one second
  if(m_clip_time_min > (m_clip_time_max-1))
    m_clip_time_max = m_clip_time_min + 1;

  lp_viewer->setClipTimeMinMax(m_clip_time_min, m_clip_time_max);
  np_viewer->setClipTimeMinMax(m_clip_time_min, m_clip_time_max);
  updateXY();
}

//----------------------------------------------------------
// Procedure: setClipTimeMax

void REPLAY_GUI::setClipTimeMax(double clip_time)
{
  m_clip_time_max = clip_time;

  // Ensure max_clip time is always greater than one.
  if(m_clip_time_max < 0)
    m_clip_time_max = 1;

  // Ensure that the max_clip time is always greater and that
  // the clip interval is always at least one second
  if(m_clip_time_max < (m_clip_time_min+1))
    m_clip_time_min = m_clip_time_max -1;

  lp_viewer->setClipTimeMinMax(m_clip_time_min, m_clip_time_max);
  np_viewer->setClipTimeMinMax(m_clip_time_min, m_clip_time_max);
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
    np_viewer->redraw();
    updateXY();
  }
}

//----------------------------------------- handleLeftRight
inline void REPLAY_GUI::handleLeftRight(int amt) 
{
  if(inLogPlotViewer()) {
    if(amt < 0)
      cb_Step_i(1);
    else if(amt > 0)
      cb_Step_i(-1);
  }
  //else if(inNavPlotViewer()) {
  else {
    np_viewer->setParam("pan_x", ((double)(amt)/10)); 
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
void REPLAY_GUI::steptime(int val) 
{
  cb_Step_i(val);
}

//----------------------------------------- Step
inline bool REPLAY_GUI::cb_Step_i(int val) {
  np_viewer->stepTime(val);
  np_viewer->redraw();

  double curr_time = np_viewer->getCurrTime();
  lp_viewer->setCurrTime(curr_time);
  lp_viewer->redraw();

  updateTimeSubGUI();
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
  lp_viewer->setLeftPlot((unsigned int)(index));
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
  lp_viewer->redraw();
  updateXY();
}
void REPLAY_GUI::cb_RightLogPlot(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_RightLogPlot_i(val);
}

//----------------------------------------- IPF_GUI
inline void REPLAY_GUI::cb_IPF_GUI_i(int aix) {
  string vname = m_dbroker.getVNameFromAix(aix);
  if(vname == "")
    return;

  double curr_time = lp_viewer->getCurrTime();
  GUI_IPF *ipfgui = new GUI_IPF(450,300, vname.c_str());   
  ipfgui->setDataBroker(m_dbroker, vname);
  ipfgui->setParentGUI(this);
  ipfgui->setCurrTime(curr_time);

  m_sub_guis.push_front(ipfgui);
  
  updateXY();
}
void REPLAY_GUI::cb_IPF_GUI(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_IPF_GUI_i(val);
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
  
  updateXY();
}
void REPLAY_GUI::cb_Streaming(Fl_Widget* o, int v) {
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_Streaming_i(v);
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
  // First handle case where we start accelerating from a paused state
  if(faster && !m_stream) {
    m_step_time_ix = 3;
    m_step_time = 1;
    cb_Streaming_i(1);
    updateXY();
    return;
  }
      
  if(faster)
    m_step_time_ix++;
  else
    m_step_time_ix--;

  if(m_step_time_ix > 8)
    m_step_time_ix = 8;
  else if(m_step_time_ix < 0)
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

//----------------------------------------- UpdateXY
void REPLAY_GUI::updateXY() 
{
  // DISPLAY TIME
  double ctime = np_viewer->getCurrTime();

  string dtime = doubleToString(ctime, 3);
  m_disp_time->value(dtime.c_str());
  
   // Play Rate
  string play_rate = "PAUSED";
  if(m_stream) {
    if(m_step_time_ix == 0) play_rate = "0.125Hz";
    else if(m_step_time_ix == 1) play_rate = "0.25Hz";
    else if(m_step_time_ix == 2) play_rate = "0.5Hz";
    else if(m_step_time_ix == 3) play_rate = "1Hz";
    else if(m_step_time_ix == 4) play_rate = "2Hz";
    else if(m_step_time_ix == 5) play_rate = "4Hz";
    else if(m_step_time_ix == 6) play_rate = "8Hz";
    else if(m_step_time_ix == 7) play_rate = "16Hz";
    else if(m_step_time_ix == 8) play_rate = "32Hz";
  }
  string title = "alogview (" + play_rate + ")";
  copy_label(title.c_str());

  // Time_Low/High
  double tlow     = lp_viewer->getTimeLow();
  string tlow_str = doubleToString(tlow, 3);
  m_fld_time_low->value(tlow_str.c_str());
 
  double thigh     = lp_viewer->getTimeHigh();
  string thigh_str = doubleToString(thigh, 3);
  m_fld_time_high->value(thigh_str.c_str());

  //------------------------------
  // Label1
  string label_str1 = lp_viewer->getFullVar1();
  m_label1->value(label_str1.c_str());

  // Low1
  string min_str1 = lp_viewer->getMinVal1();
  min_str1 = dstringCompact(min_str1);
  m_low1->value(min_str1.c_str());

  // High1
  string max_str1 = lp_viewer->getMaxVal1();
  max_str1 = dstringCompact(max_str1);
  m_high1->value(max_str1.c_str());

  // Curr1
  double curr_val1 = lp_viewer->get_curr_val1(ctime);
  string curr_str1 = doubleToString(curr_val1, 3);
  curr_str1 = dstringCompact(curr_str1);
  m_curr1->value(curr_str1.c_str());


  //------------------------------
  // Label2
  string label_str2 = lp_viewer->getFullVar2();
  m_label2->value(label_str2.c_str());

  // Low2
  string min_str2 = lp_viewer->getMinVal2();
  min_str2 = dstringCompact(min_str2);
  m_low2->value(min_str2.c_str());

  // High2
  string max_str2 = lp_viewer->getMaxVal2();
  max_str2 = dstringCompact(max_str2);
  m_high2->value(max_str2.c_str());

  // Curr2
  double curr_val2 = lp_viewer->get_curr_val2(ctime);
  string curr_str2 = doubleToString(curr_val2, 3);
  curr_str2 = dstringCompact(curr_str2);
  m_curr2->value(curr_str2.c_str());
}

//----------------------------------------------------------
// Procedure: setDataBroker

void REPLAY_GUI::setDataBroker(ALogDataBroker broker)
{
  m_dbroker = broker;
  setLogPlotMenus();
  setIPFPlotMenus();

  if(np_viewer) {
    np_viewer->setDataBroker(broker);
    np_viewer->initPlots();
  }
  if(lp_viewer) {
    lp_viewer->setDataBroker(broker);
  }
}

//----------------------------------------------------------
// Procedure: setLogPlotMenus
//      Note: MIX short for MasterIndex

void REPLAY_GUI::setLogPlotMenus()
{
  unsigned int mix_count = m_dbroker.sizeMix();

  for(unsigned int mix=0; mix<mix_count; mix++) {
    string vname   = m_dbroker.getVNameFromMix(mix);
    string varname = m_dbroker.getVarNameFromMix(mix);

    // Use special unsigned int type having same size a pointer (void*)
    uintptr_t ix = mix;

    string labelA, labelB;
    if(vname != "") {
      labelA = "LogPlots(L)/" + vname + "/" + varname;
      labelB = "LogPlots(R)/" + vname + "/" + varname;
    }
    else {
      labelA = "LogPlots(L)/" + varname;
      labelB = "LogPlots(R)/" + varname;
    }
    
    m_menubar->add(labelA.c_str(), 0, 
		   (Fl_Callback*)REPLAY_GUI::cb_LeftLogPlot,  (void*)ix);
    m_menubar->add(labelB.c_str(), 0, 
		   (Fl_Callback*)REPLAY_GUI::cb_RightLogPlot, (void*)ix);
    m_menubar->redraw();
  }
}

//----------------------------------------------------------
// Procedure: setIPFPlotMenus
//      Note: BIX short for BehaviorIndex

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
// Procedure: initLogPlotChoiceA()

void REPLAY_GUI::initLogPlotChoiceA(string vname, string varname)
{
  unsigned int mix = m_dbroker.getMixFromVNameVarName(vname, varname);

  if(mix >= m_dbroker.sizeMix())
    return;
  cb_LeftLogPlot_i((int)(mix));
}

//----------------------------------------------------------
// Procedure: initLogPlotChoiceB()

void REPLAY_GUI::initLogPlotChoiceB(string vname, string varname)
{
  unsigned int mix = m_dbroker.getMixFromVNameVarName(vname, varname);

  if(mix >= m_dbroker.sizeMix())
    return;
  cb_RightLogPlot_i((int)(mix));
}


//----------------------------------------------------------
// Procedure: conditionalStep

void REPLAY_GUI::conditionalStep() 
{
  if(m_stream) {
    bool reached_the_end = false;
    m_timer.stop();
    if(m_timer.get_float_wall_time() > m_step_time) {
      bool incremented = this->cb_Step_i(m_step_amt);
      if(!incremented)
	reached_the_end = true;
      m_timer.reset();
    }
    m_timer.start();
    if(reached_the_end)
      cb_Streaming_i(0);
  }
}

//----------------------------------------------------------
// Procedure: updateTimeSubGUI

void REPLAY_GUI::updateTimeSubGUI()
{
  // Update the times to all the sub_guis
  double curr_time = lp_viewer->getCurrTime();
  list<GUI_IPF*>::iterator p;
  for(p=m_sub_guis.begin(); p!=m_sub_guis.end(); p++) {
    GUI_IPF *sub_gui = *p;
    sub_gui->setCurrTime(curr_time);
  }
}


//----------------------------------------------------------
// Procedure: resizeDataText

void REPLAY_GUI::resizeDataText(int wsize) 
{
  // LogPlot1
  m_label1->labelsize(wsize);
  m_curr1->labelsize(wsize);
  m_low1->labelsize(wsize);
  m_high1->labelsize(wsize);

  m_label1->textsize(wsize);
  m_curr1->textsize(wsize);
  m_low1->textsize(wsize);
  m_high1->textsize(wsize);

  // LogPlot2
  m_label2->labelsize(wsize);
  m_curr2->labelsize(wsize);
  m_low2->labelsize(wsize);
  m_high2->labelsize(wsize);

  m_label2->textsize(wsize);
  m_curr2->textsize(wsize);
  m_low2->textsize(wsize);
  m_high2->textsize(wsize);

  // Time Min/Max
  m_fld_time_low->labelsize(wsize);
  m_fld_time_low->textsize(wsize);
  m_fld_time_high->labelsize(wsize);
  m_fld_time_high->textsize(wsize);

  // TopCenter Fields
  m_disp_time->labelsize(wsize);
  m_disp_time->textsize(wsize);
  
  m_but_zoom_in_time->labelsize(wsize);
  m_but_zoom_out_time->labelsize(wsize);
  m_but_zoom_reset_time->labelsize(wsize);
}



//----------------------------------------------------------
// Procedure: resizeWidgets()

void REPLAY_GUI::resizeWidgets() 
{
  double now_hgt = h();
  //double now_wid = w();
 
  // Part 1: Figure out the basic pane extents.
  // ================================================================
  // This is the default extent configuration so long as any minimal
  // constraints are not violated.

  double pct_view_hgt = 0.715;
  double pct_menu_hgt = 0.035;
  double pct_data_hgt = 0.250; // Above three add up to 1.0

  // Make sure the menu bar is at least 15 and at most 25
  if((pct_menu_hgt * now_hgt) < 15) 
    pct_menu_hgt = 15 / now_hgt;
  if((pct_menu_hgt * now_hgt) > 25) 
    pct_menu_hgt = 25 / now_hgt;

  pct_view_hgt = (1 - (pct_menu_hgt + pct_data_hgt));

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

  double fld_hgt = dh * 0.09;

  if((dw < 850) || (dw < 650))
    resizeDataText(8);
  else
    resizeDataText(10);

  // height = 150 is the "baseline" design. All adjustments from there.
  double row5 = dy + 5 * (dh/150);
  double row4 = dy + 25 * (dh/150);
  //  double row3 = dy + 50 * (dh/150);
  double row2 = dy + 95 * (dh/150);
  //double row1 = dy + 135 * (dh/150);

  // LogPlot1
  m_label1->resize(dw*0.13, row5, dw*0.15, fld_hgt);
  m_curr1->resize(dw*0.13, row4, dw*0.15, fld_hgt);
  m_low1->resize(2, row4, dw*0.08, fld_hgt);
  m_high1->resize(2, row5, dw*0.08, fld_hgt);
  // LogPlot2
  m_label2->resize(dw*0.74, row5, dw*0.16, fld_hgt);
  m_curr2->resize(dw*0.74, row4, dw*0.16, fld_hgt);
  m_low2->resize(dw*0.93, row4, dw*0.07, fld_hgt);
  m_high2->resize(dw*0.93, row5, dw*0.07, fld_hgt);
  // Time Min/Max
  m_fld_time_low->resize(1, row2, dw*0.07, fld_hgt);
  m_fld_time_high->resize(dw*0.93, row2, dw*0.07, fld_hgt);
  // TopCenter Fields
  m_disp_time->resize(dw*0.33, row4, dw*0.06, fld_hgt);
  m_but_zoom_in_time->resize(dw*0.42, row4, dw*0.03, fld_hgt);
  m_but_zoom_out_time->resize(dw*0.46, row4, dw*0.03, fld_hgt);
  m_but_zoom_reset_time->resize(dw*0.50, row4, dw*0.05, fld_hgt);
}
