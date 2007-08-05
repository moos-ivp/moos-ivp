/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_GUI.cpp                                         */
/*    DATE: June 17th, 2006                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "ZAIC_GUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

ZAIC_GUI::ZAIC_GUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) {

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  mbar = new Fl_Menu_Bar(0, 0, w(), 25);
  mbar->menu(menu_);
    
  int info_size=10;

  int top_marg = 30;
  int bot_marg = 80;
  int sid_marg = 5;
  int q_height = h()-(top_marg+bot_marg);
  int q_width  = w()-(sid_marg*2);

  zaic_viewer = new ZAICViewer(sid_marg, top_marg, w()-(sid_marg*2), q_height);

  summit = new MY_Output(100, q_height+top_marg+10, 80, 20, "summit:"); 
  summit->textsize(info_size); 
  summit->labelsize(info_size);

  p_width = new MY_Output(100, q_height+top_marg+40, 80, 20, "peak_width:"); 
  p_width->textsize(info_size); 
  p_width->labelsize(info_size);

  s_delta = new MY_Output(275, q_height+top_marg+10, 80, 20, "summit_delta:"); 
  s_delta->textsize(info_size); 
  s_delta->labelsize(info_size);

  b_width = new MY_Output(275, q_height+top_marg+40, 80, 20, "base_width:"); 
  b_width->textsize(info_size); 
  b_width->labelsize(info_size);

  s_height = new MY_Output(450, q_height+top_marg+10, 80, 20, "summit_height:"); 
  s_height->textsize(info_size); 
  s_height->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();
}

Fl_Menu_Item ZAIC_GUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)ZAIC_GUI::cb_Quit, 0, 0},
 {0},

 {"Select Mode", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Adjust Summit Position ", '0', (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)0, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Adjust PeakWidth ",       '1', (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)1, FL_MENU_RADIO},
 {"Adjust BaseWidth ",       '2', (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)2, FL_MENU_RADIO},
 {"Adjust SummitDelta ",     '3', (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)3, FL_MENU_RADIO},
 {"Adjust Summit Height ",   '4', (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)4, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Increment Summit ",   '+', (Fl_Callback*)ZAIC_GUI::cb_CurrSumm, (void*)1, FL_MENU_RADIO},
 {"Decrement Summit ",   '-', (Fl_Callback*)ZAIC_GUI::cb_CurrSumm, (void*)-1, FL_MENU_RADIO},
 {0},

 {"Adjust", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Move Left  ", FL_Left,    (Fl_Callback*)ZAIC_GUI::cb_MoveX, (void*)-1, 0},
 {"Move Right ", FL_Right,   (Fl_Callback*)ZAIC_GUI::cb_MoveX, (void*)1, 0},
 {"Toggle WRAP ", 'w',       (Fl_Callback*)ZAIC_GUI::cb_ToggleWRAP, (void*)0, 0},
 {"Toggle Draw Total ", 't', (Fl_Callback*)ZAIC_GUI::cb_ToggleTotal, (void*)0, 0},
 {0},

 {0}
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

int ZAIC_GUI::handle(int event) 
{
  int step = 1;
  switch(event) {
  case FL_PUSH:
    Fl_Window::handle(event);
    updateOutput();
    return(1);
    break;
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------- Quit
void ZAIC_GUI::cb_Quit() {
  exit(0);
}

//----------------------------------------- MoveX
inline void ZAIC_GUI::cb_MoveX_i(int amt) {
  zaic_viewer->moveX((double)(amt));
  updateOutput();
  zaic_viewer->redraw();
}
void ZAIC_GUI::cb_MoveX(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_MoveX_i(v);
}

//----------------------------------------- CurrMode
inline void ZAIC_GUI::cb_CurrMode_i(int index) {
  zaic_viewer->currMode(index);
  updateOutput();
  zaic_viewer->redraw();
}
void ZAIC_GUI::cb_CurrMode(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_CurrMode_i(v);
}


//----------------------------------------- CurrSumm
inline void ZAIC_GUI::cb_CurrSumm_i(int index) {
  zaic_viewer->alterIndex(index);
  updateOutput();
  zaic_viewer->redraw();
}
void ZAIC_GUI::cb_CurrSumm(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_CurrSumm_i(v);
}


//----------------------------------------- ToggleWRAP
inline void ZAIC_GUI::cb_ToggleWRAP_i() {
  zaic_viewer->toggleWRAP();
  updateOutput();
  zaic_viewer->redraw();
}
void ZAIC_GUI::cb_ToggleWRAP(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_ToggleWRAP_i();
}


//----------------------------------------- ToggleTotal
inline void ZAIC_GUI::cb_ToggleTotal_i() {
  zaic_viewer->toggleDrawTotal();
  zaic_viewer->redraw();
}
void ZAIC_GUI::cb_ToggleTotal(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_ToggleTotal_i();
}


//----------------------------------------- UpdateOutput
void ZAIC_GUI::updateOutput() 
{
  string str;

  str = doubleToString(zaic_viewer->getSummit(),2);
  summit->value(str.c_str());

  str = doubleToString(zaic_viewer->getPeakWidth(),2);
  p_width->value(str.c_str());

  str = doubleToString(zaic_viewer->getBaseWidth(),2);
  b_width->value(str.c_str());

  str = doubleToString(zaic_viewer->getSummitDelta(),2);
  s_delta->value(str.c_str());

  str = doubleToString(zaic_viewer->getSummitHeight(),2);
  s_height->value(str.c_str());
}
