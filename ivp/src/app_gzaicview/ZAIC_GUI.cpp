/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_GUI.cpp                                         */
/*    DATE: June 17th, 2006                                      */
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
  //int q_width  = w()-(sid_marg*2);

  zaic_viewer = new ZAICViewer(sid_marg, top_marg, w()-(sid_marg*2), q_height);

  summit = new MY_Output(85, q_height+top_marg+10, 60, 20, "summit:"); 
  summit->textsize(info_size); 
  summit->labelsize(info_size);

  p_width = new MY_Output(85, q_height+top_marg+40, 60, 20, "peak_width:"); 
  p_width->textsize(info_size); 
  p_width->labelsize(info_size);

  s_delta = new MY_Output(235, q_height+top_marg+10, 60, 20, "summit_delta:"); 
  s_delta->textsize(info_size); 
  s_delta->labelsize(info_size);

  b_width = new MY_Output(235, q_height+top_marg+40, 60, 20, "base_width:"); 
  b_width->textsize(info_size); 
  b_width->labelsize(info_size);

  s_maxutil = new MY_Output(380, q_height+top_marg+10, 60, 20, "max_util:"); 
  s_maxutil->textsize(info_size); 
  s_maxutil->labelsize(info_size);

  s_minutil = new MY_Output(380, q_height+top_marg+40, 60, 20, "min_util:"); 
  s_minutil->textsize(info_size); 
  s_minutil->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();
}

Fl_Menu_Item ZAIC_GUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)ZAIC_GUI::cb_Quit, 0, 0},
 {0},

 {"View-Options", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Grid Size Reset ",   'g', (Fl_Callback*)ZAIC_GUI::cb_GridSize, (void*)0, 0},
 {"Grid Size Smaller ", FL_ALT+'g', (Fl_Callback*)ZAIC_GUI::cb_GridSize, (void*)-1, 0},
 {"Grid Size Larger ",  FL_CTRL+'g', (Fl_Callback*)ZAIC_GUI::cb_GridSize, (void*)1, FL_MENU_DIVIDER},
 {"Grid Shade Reset ",   's', (Fl_Callback*)ZAIC_GUI::cb_GridShade, (void*)0, 0},
 {"Grid Shade Lighter ", FL_ALT+'s', (Fl_Callback*)ZAIC_GUI::cb_GridShade, (void*)-1, 0},
 {"Grid Shade Darker ",  FL_CTRL+'s', (Fl_Callback*)ZAIC_GUI::cb_GridShade, (void*)1, FL_MENU_DIVIDER},
 {"Back Shade Reset ",   'b', (Fl_Callback*)ZAIC_GUI::cb_BackShade, (void*)0, 0},
 {"Back Shade Lighter ", FL_ALT+'b', (Fl_Callback*)ZAIC_GUI::cb_BackShade, (void*)-1, 0},
 {"Back Shade Darker ",  FL_CTRL+'b', (Fl_Callback*)ZAIC_GUI::cb_BackShade, (void*)1, FL_MENU_DIVIDER},
 {"Line Shade Reset ",   'l', (Fl_Callback*)ZAIC_GUI::cb_LineShade, (void*)0, 0},
 {"Line Shade Lighter ", FL_ALT+'l', (Fl_Callback*)ZAIC_GUI::cb_LineShade, (void*)-1, 0},
 {"Line Shade Darker ",  FL_CTRL+'l', (Fl_Callback*)ZAIC_GUI::cb_LineShade, (void*)1, FL_MENU_DIVIDER},
 {0},

 {"ZAIC-Elements-View", 0,  0, 0, 64, 0, 0, 14, 0},
 {"ZAIC Element  One  ", 0,    (Fl_Callback*)ZAIC_GUI::cb_DrawMode, (void*)1, 0},
 {"ZAIC Element  Two  ", 0,    (Fl_Callback*)ZAIC_GUI::cb_DrawMode, (void*)2, 0},
 {"Both Elements Separate ", 0,    (Fl_Callback*)ZAIC_GUI::cb_DrawMode, (void*)3, 0},
 {"Both Elements Summed   ", 0,    (Fl_Callback*)ZAIC_GUI::cb_DrawMode, (void*)4, 0},
 {"Both Elements Maxed    ", 0,    (Fl_Callback*)ZAIC_GUI::cb_DrawMode, (void*)5, 0},
 {"Elements View Toggled  ", 'v',  (Fl_Callback*)ZAIC_GUI::cb_DrawMode, (void*)6, 0},
 {0},

 {"Modify Mode", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Adjust Summit Position ", '0', (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)0, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Adjust PeakWidth ",       '1', (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)1, FL_MENU_RADIO},
 {"Adjust BaseWidth ",       '2', (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)2, FL_MENU_RADIO},
 {"Adjust SummitDelta ",     '3', (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)3, FL_MENU_RADIO},
 {"Adjust MaxUtil ",         '4', (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)4, FL_MENU_RADIO},
 {"Adjust MinUtil ",         '5', (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)5, FL_MENU_RADIO},
 {0},

 {"Modify ZAIC", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Move Left  ", FL_Left,    (Fl_Callback*)ZAIC_GUI::cb_MoveX, (void*)-1, 0},
 {"Move Right ", FL_Right,   (Fl_Callback*)ZAIC_GUI::cb_MoveX, (void*)1, 0},
 {"Toggle WRAP ", 'w',       (Fl_Callback*)ZAIC_GUI::cb_ToggleWRAP, (void*)0, 0},
 {"Toggle ZAIC ",   'z', (Fl_Callback*)ZAIC_GUI::cb_CurrZAIC, 0, 0},
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
  //int step = 1;
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


//----------------------------------------- GridSize
inline void ZAIC_GUI::cb_GridSize_i(int index) {
  if(index == 0)
    zaic_viewer->setParam("gridsize", "reset");
  else if(index < 0)
    zaic_viewer->setParam("gridsize", "down");
  else
    zaic_viewer->setParam("gridsize", "up");

  zaic_viewer->redraw();
}
void ZAIC_GUI::cb_GridSize(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_GridSize_i(v);
}

//----------------------------------------- GridShade
inline void ZAIC_GUI::cb_GridShade_i(int index) {
  if(index == 0)
    zaic_viewer->setParam("gridshade", "reset");
  else if(index < 0)
    zaic_viewer->setParam("gridshade", "down");
  else
    zaic_viewer->setParam("gridshade", "up");

  zaic_viewer->redraw();
}
void ZAIC_GUI::cb_GridShade(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_GridShade_i(v);
}

//----------------------------------------- LineShade
inline void ZAIC_GUI::cb_LineShade_i(int index) {
  if(index == 0)
    zaic_viewer->setParam("lineshade", "reset");
  else if(index < 0)
    zaic_viewer->setParam("lineshade", "down");
  else
    zaic_viewer->setParam("lineshade", "up");

  zaic_viewer->redraw();
}
void ZAIC_GUI::cb_LineShade(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_LineShade_i(v);
}

//----------------------------------------- BackShade
inline void ZAIC_GUI::cb_BackShade_i(int index) {
  if(index == 0)
    zaic_viewer->setParam("backshade", "reset");
  else if(index < 0)
    zaic_viewer->setParam("backshade", "down");
  else
    zaic_viewer->setParam("backshade", "up");

  zaic_viewer->redraw();
}
void ZAIC_GUI::cb_BackShade(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_BackShade_i(v);
}


//----------------------------------------- CurrZAIC
inline void ZAIC_GUI::cb_CurrZAIC_i() {
  zaic_viewer->toggleIndex();
  updateOutput();
  zaic_viewer->redraw();
}
void ZAIC_GUI::cb_CurrZAIC(Fl_Widget* o) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_CurrZAIC_i();
}


//----------------------------------------- ToggleWRAP
inline void ZAIC_GUI::cb_ToggleWRAP_i() {
  zaic_viewer->setParam("wrap_mode", "toggle");
  updateOutput();
  zaic_viewer->redraw();
}
void ZAIC_GUI::cb_ToggleWRAP(Fl_Widget* o) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_ToggleWRAP_i();
}


//----------------------------------------- DrawMode
inline void ZAIC_GUI::cb_DrawMode_i(int v) {
  if(v==1)
    zaic_viewer->setParam("draw_mode", "one");
  else if(v==2)
    zaic_viewer->setParam("draw_mode", "two");
  else if(v==3)
    zaic_viewer->setParam("draw_mode", "both");
  else if(v==4)
    zaic_viewer->setParam("draw_mode", "sum");
  else if(v==5)
    zaic_viewer->setParam("draw_mode", "max");
  else if(v==6)
    zaic_viewer->setParam("draw_mode", "toggle");
  zaic_viewer->redraw();
}
void ZAIC_GUI::cb_DrawMode(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_DrawMode_i(v);
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

  str = doubleToString(zaic_viewer->getMaxUtil(),2);
  s_maxutil->value(str.c_str());

  str = doubleToString(zaic_viewer->getMinUtil(),2);
  s_minutil->value(str.c_str());
}





