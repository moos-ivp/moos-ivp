/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
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

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "REPLAY_GUI.h"
#include "MBUtils.h"
#include "FL/fl_ask.H"

using namespace std;

// slight diff

//-------------------------------------------------------------------
// Constructor

REPLAY_GUI::REPLAY_GUI(int g_w, int g_h, const char *g_l)
  : MarineVehiGUI(g_w, g_h, g_l) 
{
  stream       = false;
  collect      = "Off";
  step_amt     = 5;
  step_time    = 1.0;
  step_time_ix = 3;
  save_file_ix = 0;

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  augmentMenu();

  int info_size=10;

#if 0
  np_viewer = new NavPlotViewer(0, 30, w(), h()-350);
  cmviewer = np_viewer;

  int lp_height = 290;
  int margin = 2;
#endif
#if 1
  np_viewer = new NavPlotViewer(0, 30, w(), h()-200);
  cmviewer = np_viewer;

  int lp_height = 140;
  int margin = 2;
#endif
  
#if 0
  lp_viewer = new LogPlotViewer(55, (h()-(lp_height-margin)), 
  				w()-110, (lp_height-(2*margin)));
#endif
#if 1
  lp_viewer = new LogPlotViewer(55, (h()-(lp_height-margin)), 
  				w()-110, (lp_height-(2*margin)));
#endif


  disp_time = new MY_Output((w()/2)-140, h()-(lp_height+25), 50, 20, "time:"); 
  disp_time->textsize(info_size); 
  disp_time->labelsize(info_size);

  x_mtr = new MY_Output((w()/2)-55, h()-(lp_height+25), 50, 20, "x:"); 
  x_mtr->textsize(info_size); 
  x_mtr->labelsize(info_size);

  y_mtr = new MY_Output((w()/2)+20, h()-(lp_height+25), 50, 20, "y:"); 
  y_mtr->textsize(info_size); 
  y_mtr->labelsize(info_size);

  play_rate = new MY_Output((w()/2)+140, h()-(lp_height+25), 70, 20, "play-rate:"); 
  play_rate->textsize(info_size); 
  play_rate->labelsize(info_size);

  collect_state = new MY_Output((w()/2)+260, h()-(lp_height+25), 70, 20, "collect:"); 
  collect_state->textsize(info_size); 
  collect_state->labelsize(info_size);

  // Handle LogPlot 1 -------------
  label1 = new MY_Output(60, h()-(lp_height+25), 120, 20, "Data:"); 
  label1->textsize(info_size); 
  label1->labelsize(info_size);

  curr1 = new MY_Output(240, h()-(lp_height+25), 60, 20, "CurrVal:"); 
  curr1->textsize(info_size); 
  curr1->labelsize(info_size);

  low1 = new MY_Output(2, h()-20, 51, 20, ""); 
  low1->textsize(info_size); 
  low1->labelsize(info_size);

  high1 = new MY_Output(2, h()-(lp_height-2), 51, 20, ""); 
  high1->textsize(info_size); 
  high1->labelsize(info_size);

  // Handle LogPlot 2 -------------
  label2 = new MY_Output(w()-120, h()-(lp_height+25), 120, 20, "Data:"); 
  label2->textsize(info_size); 
  label2->labelsize(info_size);

  curr2 = new MY_Output(w()-265, h()-(lp_height+25), 60, 20, "CurrVal:"); 
  curr2->textsize(info_size); 
  curr2->labelsize(info_size);

  low2 = new MY_Output(w()-110+57, h()-20, 51, 20, ""); 
  low2->textsize(info_size); 
  low2->labelsize(info_size);

  high2 = new MY_Output(w()-110+57, h()-(lp_height-2), 51, 20, ""); 
  high2->textsize(info_size); 
  high2->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
// Procedure: augmentMenu()

void REPLAY_GUI::augmentMenu()
{
  mbar->add("File/Delete ", 0, (Fl_Callback*)REPLAY_GUI::cb_Delete, 0, 0);

  mbar->add("Replay/Collecting Toggle",  'w', (Fl_Callback*)REPLAY_GUI::cb_CollectToggle,(void*)0, FL_MENU_DIVIDER);
  mbar->add("Replay/Streaming Toggle",  ' ', (Fl_Callback*)REPLAY_GUI::cb_StreamToggle,(void*)0, 0);
  mbar->add("Replay/Streaming Faster", 'a', (Fl_Callback*)REPLAY_GUI::cb_StreamSpeed, (void*)0, 0);
  mbar->add("Replay/Streaming Slower", 'z', (Fl_Callback*)REPLAY_GUI::cb_StreamSpeed, (void*)1, 0);
  mbar->add("Replay/Streaming Step 1",  0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)1, FL_MENU_RADIO);
  mbar->add("Replay/Streaming Step 3",  0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)3, FL_MENU_RADIO);
  mbar->add("Replay/Streaming Step 5",  0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)5, FL_MENU_RADIO|FL_MENU_VALUE);
  mbar->add("Replay/Streaming Step 10", 0, (Fl_Callback*)REPLAY_GUI::cb_StreamStep, (void*)10, FL_MENU_RADIO|FL_MENU_DIVIDER);
  mbar->add("Replay/Toggle Show All Trail", FL_ALT+'t', (Fl_Callback*)REPLAY_GUI::cb_AllTrail, (void*)0, 134);
  mbar->add("Replay/To Start" ,     's', (Fl_Callback*)REPLAY_GUI::cb_Jump,(void*)0, 0);
  mbar->add("Replay/To End" ,       'e', (Fl_Callback*)REPLAY_GUI::cb_Jump,(void*)1, 0);
  mbar->add("Replay/To Middle" ,    'm', (Fl_Callback*)REPLAY_GUI::cb_Jump,(void*)2, FL_MENU_DIVIDER);
  mbar->add("Replay/Step Ahead 1",  FL_ALT+']', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)1, 0);
  mbar->add("Replay/Step Back  1",  FL_ALT+'[', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)-1, 0);
  mbar->add("Replay/Step Ahead 10", FL_ALT+'}', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)10, 0);
  mbar->add("Replay/Step Back  10", FL_ALT+'{', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)-10, 0);
  mbar->add("Replay/Step Ahead 50", '>', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)50, 0);
  mbar->add("Replay/Step Back  50", '<', (Fl_Callback*)REPLAY_GUI::cb_Step, (void*)-50, FL_MENU_DIVIDER);
};

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

int REPLAY_GUI::handle(int event) 
{
  switch(event) {
  case FL_PUSH:
    Fl_Window::handle(event);
    updateXY();
    return(1);
    break;
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------- Step
inline bool REPLAY_GUI::cb_Step_i(int val) {
  bool changed = np_viewer->incCurrIndex(val);
  if(changed) { 
    updateXY();
    double curr_time = np_viewer->getCurrTime();
    np_viewer->redraw();
    lp_viewer->set_curr_time(curr_time);
    lp_viewer->redraw();
  }
  return(changed);
}
void REPLAY_GUI::cb_Step(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_Step_i(val);
}

//----------------------------------------- Jump
inline void REPLAY_GUI::cb_Jump_i(int val) {
  bool changed = np_viewer->jumpCurrIndex(val);
  if(changed) { 
    updateXY();
    double curr_time = np_viewer->getCurrTime();
    np_viewer->redraw();
    lp_viewer->set_curr_time(curr_time);
    lp_viewer->redraw();
  }
}
void REPLAY_GUI::cb_Jump(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_Jump_i(val);
}

//----------------------------------------- LeftLogPlot
inline void REPLAY_GUI::cb_LeftLogPlot_i(int index) {
  lp_viewer->set_left_plot(index);
  lp_viewer->redraw();
  updateXY();
}
void REPLAY_GUI::cb_LeftLogPlot(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_LeftLogPlot_i(val);
}

//----------------------------------------- SelectGridPlot
inline void REPLAY_GUI::cb_SelectGridPlot_i(int index) {
  np_viewer->setGridPlotIndex(index);
  np_viewer->redraw();
  updateXY();
}
void REPLAY_GUI::cb_SelectGridPlot(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_SelectGridPlot_i(val);
}

//----------------------------------------- RightLogPlot
inline void REPLAY_GUI::cb_RightLogPlot_i(int index) {
  lp_viewer->set_right_plot(index);
  lp_viewer->redraw();
  updateXY();
}
void REPLAY_GUI::cb_RightLogPlot(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_RightLogPlot_i(val);
}

//----------------------------------------- CollectToggle
inline void REPLAY_GUI::cb_CollectToggle_i() {
  if(collect == "Off")
    collect = "1024x768";
  else if(collect == "1024x768")
    collect = "800x600";
  else if(collect == "800x600")
    collect = "640x480";
  else if(collect == "640x480")
    collect = "480x360";
  else if(collect == "480x360")
    collect = "Off";

  if(collect != "Off")
    np_viewer->setFrame(collect + "+10+10");
  else
    np_viewer->setFrame("");

  np_viewer->redraw();
  updateXY();
}
void REPLAY_GUI::cb_CollectToggle(Fl_Widget* o) {
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_CollectToggle_i();
}

//----------------------------------------- StreamToggle
inline void REPLAY_GUI::cb_StreamToggle_i() {
  stream = !stream;
  if(stream)
    timer.start();
  else
    timer.stop();
  updateXY();
}
void REPLAY_GUI::cb_StreamToggle(Fl_Widget* o) {
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_StreamToggle_i();
}

//----------------------------------------- StreamStep
inline void REPLAY_GUI::cb_StreamStep_i(int val) {
  step_amt = val;
}

void REPLAY_GUI::cb_StreamStep(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_StreamStep_i(val);
}

//----------------------------------------- StreamSpeed
inline void REPLAY_GUI::cb_StreamSpeed_i(bool faster) {
  if(faster)
    step_time_ix++;
  else
    step_time_ix--;

  if(step_time_ix > 4)
    step_time_ix = 4;
  if(step_time_ix < 0)
    step_time_ix = 0;

  if(step_time_ix == 0)
    step_time = 0;
  else if(step_time_ix == 1)
    step_time = np_viewer->getAvgStepTime() / 4.0;
  else if(step_time_ix == 2)
    step_time = np_viewer->getAvgStepTime() / 2.0;
  else if(step_time_ix == 3)
    step_time = np_viewer->getAvgStepTime() / 1.0;
  else
    step_time = np_viewer->getAvgStepTime() / 0.5;
  updateXY();
}


void REPLAY_GUI::cb_StreamSpeed(Fl_Widget* o, bool v) {
  bool val = (bool)(v);
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_StreamSpeed_i(val);
}

//----------------------------------------- AllTrail
inline void REPLAY_GUI::cb_AllTrail_i() {
  np_viewer->toggleAllTrail();
  np_viewer->redraw();
}

void REPLAY_GUI::cb_AllTrail(Fl_Widget* o) {
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_AllTrail_i();
}

//----------------------------------------- Delete
inline void REPLAY_GUI::cb_Delete_i() {
  string alog_file = findReplace(log_file, ".slog", ".alog");
  string ylog_file = findReplace(log_file, ".slog", ".ylog");
  string moos_file = findReplace(log_file, ".slog", "._moos");
  string question = "Delete " + log_file + "  and  " + alog_file + "? ";
  int res = fl_choice(question.c_str(), "Yes", "No", 0);
  if(res==1) 
    return;
  
  string command = "rm -f " + log_file + " " + alog_file + " " +
    ylog_file + " " + moos_file; 
  system(command.c_str());
  exit(0);
}
void REPLAY_GUI::cb_Delete(Fl_Widget* o) {
  ((REPLAY_GUI*)(o->parent()->user_data()))->cb_Delete_i();
}

//----------------------------------------- UpdateXY
void REPLAY_GUI::updateXY() {
  
  // DISPLAY TIME
  double ctime = np_viewer->getCurrTime();
  string dtime = doubleToString(ctime, 1);
  disp_time->value(dtime.c_str());
  
  // Meters X
  double dxmeters = np_viewer->getMetersX();
  string sxmeters = doubleToString(dxmeters, 2);
  x_mtr->value(sxmeters.c_str());

  // Meters X
  double dymeters = np_viewer->getMetersY();
  string symeters = doubleToString(dymeters, 2);
  y_mtr->value(symeters.c_str());

  // Play Rate
  if(stream) {
    if(step_time_ix == 0) play_rate->value("FAP");
    if(step_time_ix == 1) play_rate->value("4xReal");
    if(step_time_ix == 2) play_rate->value("2xReal");
    if(step_time_ix == 3) play_rate->value("Real");
    if(step_time_ix == 4) play_rate->value("Real/2");
  }
  else
    play_rate->value("Paused");

  collect_state->value(collect.c_str());
  
  //------------------------------
  // Label1
  string label_str1 = lp_viewer->get_variable1();
  label1->value(label_str1.c_str());

  // Low1
  string min_str1 = lp_viewer->get_min_val1();
  min_str1 = dstringCompact(min_str1);
  low1->value(min_str1.c_str());

  // High1
  string max_str1 = lp_viewer->get_max_val1();
  max_str1 = dstringCompact(max_str1);
  high1->value(max_str1.c_str());

  // Curr1
  double curr_val1 = lp_viewer->get_curr_val1(ctime);
  string curr_str1 = doubleToString(curr_val1, 3);
  curr_str1 = dstringCompact(curr_str1);
  curr1->value(curr_str1.c_str());


  //------------------------------
  // Label2
  string label_str2 = lp_viewer->get_variable2();
  label2->value(label_str2.c_str());

  // Low2
  string min_str2 = lp_viewer->get_min_val2();
  min_str2 = dstringCompact(min_str2);
  low2->value(min_str2.c_str());

  // High2
  string max_str2 = lp_viewer->get_max_val2();
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

  string vname = logplot.get_vehicle();

  int ix = lp_viewer->add_logplot(logplot) - 1;

  string labelA, labelB;
  if(vname != "") {
    labelA = "LogPlots/"      + vname + "/" + logplot.get_varname();
    labelB = "(2nd)LogPlots/" + vname + "/" + logplot.get_varname();
  }
  else {
    labelA = "LogPlots/"      + logplot.get_varname();
    labelB = "(2nd)LogPlots/" + logplot.get_varname();
  }

  mbar->add(labelA.c_str(), 0, 
	    (Fl_Callback*)REPLAY_GUI::cb_LeftLogPlot,  (void*)ix);
  mbar->add(labelB.c_str(), 0, 
	    (Fl_Callback*)REPLAY_GUI::cb_RightLogPlot, (void*)ix);
}

//----------------------------------------------------------
// Procedure: addGridPlot

void REPLAY_GUI::addGridPlot(const GridPlot& gridplot)
{
  if(!np_viewer)
    return;
  
  string label = gridplot.getGridLabel();
  
  cout << "Add GridPlot label: " << label << endl;

  int ix = np_viewer->addGridPlot(gridplot)-1;
  
  string menu_tag = "GridPlots/" + label;
  mbar->add(menu_tag.c_str(), 0, 
	    (Fl_Callback*)REPLAY_GUI::cb_SelectGridPlot,  (void*)ix);
}

//----------------------------------------------------------
// Procedure: conditional_step

void REPLAY_GUI::conditional_step() 
{
  if(stream) {
    bool reached_the_end = false;
    timer.stop();
    if(timer.get_float_wall_time() > step_time) {
      bool incremented = this->cb_Step_i(step_amt);
      if(!incremented)
	reached_the_end = true;
      timer.reset();
      if(collect != "Off")
	capture_to_file();
    }
    timer.start();
    if(reached_the_end)
      cb_StreamToggle_i();
  }
}

//----------------------------------------------------------
// Procedure: capture_to_file

void REPLAY_GUI::capture_to_file() 
{
  string command;
  command += "import -quality 90 -window OpRegion-Viewer ";
  command += "-crop " + collect + "+10+40 save_file_";
  if(save_file_ix < 10)   command += "0";
  if(save_file_ix < 100)  command += "0";
  if(save_file_ix < 1000) command += "0";
  command += intToString(save_file_ix) + ".png";
  system(command.c_str());
  cout << "command: " << command << endl;
  save_file_ix++;
}


