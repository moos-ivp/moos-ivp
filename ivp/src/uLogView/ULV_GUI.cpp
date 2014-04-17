/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ULV_GUI.cpp                                          */
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
#include "ULV_GUI.h"
#include "MBUtils.h"
#include "FL/fl_ask.H"
#include <FL/Fl.H>

using namespace std;

//-------------------------------------------------------------------
// Constructor

ULV_GUI::ULV_GUI(int g_w, int g_h, const char *g_l)
  : MarineVehiGUI(g_w, g_h, g_l) 
{
  m_stream       = false;
  m_step_amt     = 1;
  m_step_time    = 1.0;
  m_step_time_ix = 3;

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  augmentMenu();

  int info_size = 10;
  // Init window with bogus extents. Actual extents
  // are set in the setWindowLayout() call below.
  np_viewer    = new NavPlotViewer(1, 1, 1, 1);
  lp_viewer    = new LogPlotViewer(1, 1, 1, 1);

  setWindowLayout("normal");

  m_mviewer    = np_viewer;

  // Initialize Time fields ------------------------------------------
  double time_pos = (w()/2)-250;
  disp_time = new MY_Output(time_pos, h()-(m_lp_viewer_hgt+26), 
			    115, 22, "Time:"); 
  disp_time->color(FL_YELLOW); 
  disp_time->textcolor(FL_BLUE); 
  disp_time->textsize(12); 
  disp_time->labelsize(12);

  m_but_time_utc = new Fl_Check_Button(time_pos+120, 
				       h()-(m_lp_viewer_hgt+26), 
				       20, 20, "");
  m_but_time_utc->labelsize(16);
  m_but_time_utc->callback((Fl_Callback*)ULV_GUI::cb_ToggleUTC);



  m_but_zoom_in_time = new MY_Repeat_Button(time_pos+160, 
					    h()-(m_lp_viewer_hgt+26), 
					    30, 21, "IN");
  m_but_zoom_out_time = new MY_Repeat_Button(time_pos+200, 
					     h()-(m_lp_viewer_hgt+26), 
					     35, 21, "OUT");
  m_but_zoom_reset_time = new MY_Button(time_pos+240, 
					h()-(m_lp_viewer_hgt+26), 
					45, 21, "RESET");
  
  m_but_zoom_in_time->callback((Fl_Callback*)ULV_GUI::cb_TimeZoom,
			       (void*)1);
  m_but_zoom_out_time->callback((Fl_Callback*)ULV_GUI::cb_TimeZoom,
				(void*)-1);
  m_but_zoom_reset_time->callback((Fl_Callback*)ULV_GUI::cb_TimeZoom,
				  (void*)0);
  
  m_but_zoom_in_time->color(FL_DARK_RED);
  m_but_zoom_out_time->color(FL_DARK_RED);
  m_but_zoom_reset_time->color(FL_DARK_RED);
  m_but_zoom_in_time->labelcolor(FL_WHITE);
  m_but_zoom_out_time->labelcolor(FL_WHITE);
  m_but_zoom_reset_time->labelcolor(FL_WHITE);

  m_but_zoom_in_time->labelsize(10);
  m_but_zoom_out_time->labelsize(10);
  m_but_zoom_reset_time->labelsize(10);

  play_rate = new MY_Output((w()/2)+140, h()-(m_lp_viewer_hgt+25), 
			    70, 20, "play-rate:"); 
  play_rate->textsize(info_size); 
  play_rate->labelsize(info_size);

  // Handle LogPlot 1 -------------
  m_fld_var_left = new MY_Output(35, h()-(m_lp_viewer_hgt+25), 
				 150, 20, "Var:"); 
  m_fld_var_left->textsize(info_size-1); 
  m_fld_var_left->color(FL_DARK_GREEN); 
  m_fld_var_left->textcolor(FL_WHITE); 
  m_fld_var_left->labelsize(info_size);

  m_fld_cur_left = new MY_Output(235, h()-(m_lp_viewer_hgt+25), 
				 90, 20, "CurrVal:"); 
  m_fld_cur_left->textsize(info_size); 
  m_fld_cur_left->color(FL_DARK_GREEN); 
  m_fld_cur_left->textcolor(FL_WHITE); 
  m_fld_cur_left->labelsize(info_size);

  m_fld_low_left = new MY_Output(2, h()-20, 51, 20, ""); 
  m_fld_low_left->textsize(info_size); 
  m_fld_low_left->color(FL_DARK_GREEN); 
  m_fld_low_left->textcolor(FL_WHITE); 
  m_fld_low_left->labelsize(info_size);

  m_fld_hgh_left = new MY_Output(2, h()-(m_lp_viewer_hgt-2), 51, 20, ""); 
  m_fld_hgh_left->textsize(info_size); 
  m_fld_hgh_left->color(FL_DARK_GREEN); 
  m_fld_hgh_left->textcolor(FL_WHITE); 
  m_fld_hgh_left->labelsize(info_size);

  // Handle LogPlot 2 -------------
  m_fld_var_right = new MY_Output(w()-154, h()-(m_lp_viewer_hgt+25), 
				  150, 20, "Var:"); 
  m_fld_var_right->textsize(info_size-1); 
  m_fld_var_right->labelsize(info_size);
  m_fld_var_right->textcolor(FL_WHITE); 
  m_fld_var_right->color(FL_DARK_BLUE); 

  m_fld_cur_right = new MY_Output(w()-275, h()-(m_lp_viewer_hgt+25), 
				  90, 20, "CurrVal:"); 
  m_fld_cur_right->textsize(info_size); 
  m_fld_cur_right->labelsize(info_size);
  m_fld_cur_right->textcolor(FL_WHITE); 
  m_fld_cur_right->color(FL_DARK_BLUE); 

  m_fld_low_right = new MY_Output(w()-110+57, h()-20, 51, 20, ""); 
  m_fld_low_right->textsize(info_size); 
  m_fld_low_right->color(FL_DARK_BLUE); 
  m_fld_low_right->textcolor(FL_WHITE); 
  m_fld_low_right->labelsize(info_size);

  m_fld_hgh_right = new MY_Output(w()-110+57, h()-(m_lp_viewer_hgt-2), 
				  51, 20, ""); 
  m_fld_hgh_right->textsize(info_size); 
  m_fld_hgh_right->color(FL_DARK_BLUE); 
  m_fld_hgh_right->textcolor(FL_WHITE); 
  m_fld_hgh_right->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
// Destructor

ULV_GUI::~ULV_GUI()
{
  delete(lp_viewer);
  delete(np_viewer);
  delete(disp_time);
  delete(play_rate);

  delete(m_fld_var_left);
  delete(m_fld_low_left);
  delete(m_fld_hgh_left);
  delete(m_fld_cur_left);

  delete(m_fld_var_right);
  delete(m_fld_low_right);
  delete(m_fld_hgh_right);
  delete(m_fld_cur_right);
  delete(m_but_zoom_in_time);
  delete(m_but_zoom_out_time);
  delete(m_but_zoom_reset_time);
}

//-------------------------------------------------------------------
// Procedure: setWindowLayout()

void ULV_GUI::setWindowLayout(string layout)
{
  int npw, nph, npx, npy;
  int lpw, lph, lpx, lpy;

  m_lp_viewer_hgt = 140;
  m_np_viewer_hgt = h()-200;

  // Set the extents of the NavPlotViewer
  npx = 0;
  npy = 30;
  nph = m_np_viewer_hgt;
  npw = w();
  
  // Set the extents of the LogPlotViewer
  int margin = 2;
  lpx = 55;
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

void ULV_GUI::augmentMenu()
{
  m_menubar->add("File/Delete ", 0, (Fl_Callback*)ULV_GUI::cb_Delete, 0, 0);

  m_menubar->add("Replay/Streaming Toggle",  '=', (Fl_Callback*)ULV_GUI::cb_StreamToggle,(void*)0, 0);
  m_menubar->add("Replay/Streaming Faster", 'a',  (Fl_Callback*)ULV_GUI::cb_StreamSpeed, (void*)1, 0);
  m_menubar->add("Replay/Streaming Slower", 'z',  (Fl_Callback*)ULV_GUI::cb_StreamSpeed, (void*)0, FL_MENU_DIVIDER);
  m_menubar->add("Replay/Streaming Step 1",  0, (Fl_Callback*)ULV_GUI::cb_StreamStep, (void*)1, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("Replay/Streaming Step 3",  0, (Fl_Callback*)ULV_GUI::cb_StreamStep, (void*)3, FL_MENU_RADIO);
  m_menubar->add("Replay/Streaming Step 5",  0, (Fl_Callback*)ULV_GUI::cb_StreamStep, (void*)5, FL_MENU_RADIO);
  m_menubar->add("Replay/Streaming Step 10", 0, (Fl_Callback*)ULV_GUI::cb_StreamStep, (void*)10, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("Replay/Step Ahead 1", ']', (Fl_Callback*)ULV_GUI::cb_Step, (void*)1, 0);
  m_menubar->add("Replay/Step Back  1", '[', (Fl_Callback*)ULV_GUI::cb_Step, (void*)-1, 0);
  m_menubar->add("Replay/Step Ahead 10", '}',  (Fl_Callback*)ULV_GUI::cb_Step, (void*)10, 0);
  m_menubar->add("Replay/Step Back  10", '{',  (Fl_Callback*)ULV_GUI::cb_Step, (void*)-10, 0);
  m_menubar->add("Replay/Step Ahead 50", FL_CTRL+']',  (Fl_Callback*)ULV_GUI::cb_Step, (void*)50, 0);
  m_menubar->add("Replay/Step Back  50", FL_CTRL+'[',  (Fl_Callback*)ULV_GUI::cb_Step, (void*)-50, FL_MENU_DIVIDER);
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

int ULV_GUI::handle(int event) 
{
  double lpv_curr_time = 0;
  switch(event) {
  case FL_PUSH:
    Fl_Window::handle(event);
    lpv_curr_time = lp_viewer->getCurrTime();
    np_viewer->setCurrTime(lpv_curr_time);
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

void ULV_GUI::setCurrTime(double curr_time)
{
  lp_viewer->setCurrTime(curr_time);
  np_viewer->setCurrTime(curr_time);
  updateXY();
}


//----------------------------------------- HandleUpDown
inline void ULV_GUI::cb_HandleUpDown_i(int amt) 
{
  if(inNavPlotViewer()) {
    np_viewer->setParam("pan_y", ((double)(amt)/10)); 
    np_viewer->redraw();
    updateXY();
  }
  else { // Assumed to be in logplotviewer
    if(amt < 0)
      lp_viewer->adjustZoom("out");
    else if(amt > 0)
      lp_viewer->adjustZoom("in");
    lp_viewer->redraw();
    updateXY();
  }
}

//----------------------------------------- HandleLeftRight
inline void ULV_GUI::cb_HandleLeftRight_i(int amt) 
{
  if(inNavPlotViewer()) {
    np_viewer->setParam("pan_x", ((double)(amt)/10)); 
    np_viewer->redraw();
    updateXY();
  }
  else { // Assumed to be log plot viewer
    if(amt < 0)
      cb_Step_i(1);
    else if(amt > 0)
      cb_Step_i(-1);
  }
}

//----------------------------------------- Zoom In
inline void ULV_GUI::cb_Zoom_i(int val) {
  if(inNavPlotViewer()) {
    if(val < 0)
      np_viewer->setParam("zoom", 1.05);
    else if(val > 0)
      np_viewer->setParam("zoom", 0.8);
    else
      np_viewer->setParam("zoom", "reset");
    np_viewer->redraw();
  }
  else { // Assumed to be in LogPlotViewer
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
bool ULV_GUI::inNavPlotViewer()
{
  if(!np_viewer)
    return(false);
  
  int vx = (Fl::event_x() + Fl::event_x_root()) / 2;
  int vy = (Fl::event_y() + Fl::event_y_root()) / 2;
  int x  = np_viewer->x();
  int y  = np_viewer->y();
  int w  = np_viewer->w();
  int h  = np_viewer->h();

  return((vx>=x)&&(vx<=x+w)&&(vy>=y)&&(vy<=(y+h)));
}

//----------------------------------------- Step
inline bool ULV_GUI::cb_Step_i(int val) {
  double dval = (double)(val) / 10;
  bool changed = np_viewer->stepTime(dval);
  if(changed) {
    np_viewer->redraw();
    double curr_time = np_viewer->getCurrTime();
    lp_viewer->setCurrTime(curr_time);
    lp_viewer->redraw();
  }
  else
    m_stream = false;

  updateXY();
  return(changed);
}
void ULV_GUI::cb_Step(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((ULV_GUI*)(o->parent()->user_data()))->cb_Step_i(val);
}

//----------------------------------------- LeftLogPlot
inline void ULV_GUI::cb_LeftLogPlot_i(int index) {
  lp_viewer->setLeftPlot(index);
  lp_viewer->redraw();
  updateXY();
}
void ULV_GUI::cb_LeftLogPlot(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((ULV_GUI*)(o->parent()->user_data()))->cb_LeftLogPlot_i(val);
}

//----------------------------------------- RightLogPlot
inline void ULV_GUI::cb_RightLogPlot_i(int index) {
  lp_viewer->setRightPlot(index);
  lp_viewer->redraw();
  updateXY();
}
void ULV_GUI::cb_RightLogPlot(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((ULV_GUI*)(o->parent()->user_data()))->cb_RightLogPlot_i(val);
}

//----------------------------------------- TimeZoom
inline void ULV_GUI::cb_TimeZoom_i(int val) {
  if(val > 0)
    lp_viewer->adjustZoom("in");
  else if(val < 0)
    lp_viewer->adjustZoom("out");
  else
    lp_viewer->adjustZoom("reset");
  updateXY();
}

void ULV_GUI::cb_TimeZoom(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((ULV_GUI*)(o->parent()->user_data()))->cb_TimeZoom_i(val);
}

//----------------------------------------- StreamToggle
inline void ULV_GUI::cb_StreamToggle_i() {
  m_stream = !m_stream;
  if(m_stream)
    m_timer.start();
  else
    m_timer.stop();
  updateXY();
}
void ULV_GUI::cb_StreamToggle(Fl_Widget* o) {
  ((ULV_GUI*)(o->parent()->user_data()))->cb_StreamToggle_i();
}

//----------------------------------------- ToggleUTC
inline void ULV_GUI::cb_ToggleUTC_i() {
  np_viewer->setParam("time_format", "toggle");
  updateXY();
}
void ULV_GUI::cb_ToggleUTC(Fl_Widget* o) {
  ((ULV_GUI*)(o->parent()->user_data()))->cb_ToggleUTC_i();
}

//----------------------------------------- StreamStep

inline void ULV_GUI::cb_StreamStep_i(int val) {
  m_step_amt = val;
}

void ULV_GUI::cb_StreamStep(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((ULV_GUI*)(o->parent()->user_data()))->cb_StreamStep_i(val);
}

//----------------------------------------- StreamSpeed
inline void ULV_GUI::cb_StreamSpeed_i(bool faster) {
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


void ULV_GUI::cb_StreamSpeed(Fl_Widget* o, bool v) {
  bool val = (bool)(v);
  ((ULV_GUI*)(o->parent()->user_data()))->cb_StreamSpeed_i(val);
}

//----------------------------------------- Delete
inline void ULV_GUI::cb_Delete_i() {
  string slog_file = findReplace(m_log_file, ".alog", ".slog");
  string ylog_file = findReplace(m_log_file, ".alog", ".ylog");
  string moos_file = findReplace(m_log_file, ".alog", "._moos");
  int res = fl_choice("Delete Log files?", "Yes", "No", 0);
  if(res==1) 
    return;
  
  string command = "rm -f " + m_log_file + " " + slog_file + " " +
    ylog_file + " " + moos_file; 

  // Pretend to care about the system result to avoid compiler warning
  // Declare the result variable to be unused to avoid a compiler warning
  int result __attribute__((unused));
  result = system(command.c_str());
  exit(0);
}
void ULV_GUI::cb_Delete(Fl_Widget* o) {
  ((ULV_GUI*)(o->parent()->user_data()))->cb_Delete_i();
}

//----------------------------------------- UpdateXY
void ULV_GUI::updateXY() 
{
  // DISPLAY TIME
  double ctime = np_viewer->getCurrTime();
  string stime = np_viewer->getCurrTimeStr();
  disp_time->value(stime.c_str());
  
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

  //------------------------------
  // m_fld_var_left
  string label_str1 = lp_viewer->getVariable1();
  m_fld_var_left->value(label_str1.c_str());

  // m_fld_low_left
  string min_str1 = lp_viewer->getMinVal1();
  min_str1 = dstringCompact(min_str1);
  m_fld_low_left->value(min_str1.c_str());

  // m_fld_hgh_left
  string max_str1 = lp_viewer->getMaxVal1();
  max_str1 = dstringCompact(max_str1);
  m_fld_hgh_left->value(max_str1.c_str());

  // m_fld_cur_left
  double curr_val1 = lp_viewer->getCurrVal1(ctime);
  string curr_str1 = doubleToString(curr_val1, 3);
  curr_str1 = dstringCompact(curr_str1);
  m_fld_cur_left->value(curr_str1.c_str());


  //------------------------------
  // m_fld_var_right
  string label_str2 = lp_viewer->getVariable2();
  m_fld_var_right->value(label_str2.c_str());

  // m_fld_low_right
  string min_str2 = lp_viewer->getMinVal2();
  min_str2 = dstringCompact(min_str2);
  m_fld_low_right->value(min_str2.c_str());

  // m_fld_hgh_right
  string max_str2 = lp_viewer->getMaxVal2();
  max_str2 = dstringCompact(max_str2);
  m_fld_hgh_right->value(max_str2.c_str());

  // m_fld_cur_right
  double curr_val2 = lp_viewer->getCurrVal2(ctime);
  string curr_str2 = doubleToString(curr_val2, 3);
  curr_str2 = dstringCompact(curr_str2);
  m_fld_cur_right->value(curr_str2.c_str());
}

//----------------------------------------------------------
// Procedure: addLogPlot

void ULV_GUI::addLogPlot(const LogTPlot& logplot)
{
  if(!lp_viewer)
    return;

  string vname   = logplot.getVName();
  string varname = logplot.getVarName();
  intptr_t ix = lp_viewer->addLogPlot(logplot) - 1;

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
	    (Fl_Callback*)ULV_GUI::cb_LeftLogPlot,  (void*)ix);
  m_menubar->add(labelB.c_str(), 0, 
	    (Fl_Callback*)ULV_GUI::cb_RightLogPlot, (void*)ix);
  m_menubar->redraw();
}

//----------------------------------------------------------
// Procedure: conditional_step

void ULV_GUI::conditional_step() 
{
  if(m_stream) {
    m_timer.stop();
    if(m_timer.get_float_wall_time() > m_step_time) {
      bool incremented = this->cb_Step_i(m_step_amt);
      if(!incremented)
	m_stream = false;
      m_timer.reset();
    }
    m_timer.start();
  }
}

//-------------------------------------------------------------------
// Procedure: getPostingBuffer

PostingBuffer ULV_GUI::getPostingBuffer(bool clear)
{
  PostingBuffer pbuffer = m_pbuffer;
  pbuffer.merge(np_viewer->m_pbuffer);

  if(clear) {
    m_pbuffer.clear();
    np_viewer->m_pbuffer.clear();
  }

  return(pbuffer);
}




