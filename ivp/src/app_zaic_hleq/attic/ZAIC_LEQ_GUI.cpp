/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_LEQ_GUI.cpp                                     */
/*    DATE: Apr 6th, 2008                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "MBUtils.h"
#include "ZAIC_LEQ_GUI.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

ZAIC_LEQ_GUI::ZAIC_LEQ_GUI(int g_w, int g_h, const char *g_l)
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

  zaic_viewer = new ZAIC_LEQ_Viewer(sid_marg, top_marg, w()-(sid_marg*2), q_height);

  m_fld_summit = new Fl_Output(85, q_height+top_marg+10, 60, 20, "summit:"); 
  m_fld_summit->textsize(info_size); 
  m_fld_summit->labelsize(info_size);
  m_fld_summit->clear_visible_focus();
  

  m_fld_bwidth = new Fl_Output(85, q_height+top_marg+40, 60, 20, "base_width:"); 
  m_fld_bwidth->textsize(info_size); 
  m_fld_bwidth->labelsize(info_size);
  m_fld_bwidth->clear_visible_focus();

  m_fld_minutil = new Fl_Output(205, q_height+top_marg+10, 60, 20, "minutil:"); 
  m_fld_minutil->textsize(info_size); 
  m_fld_minutil->labelsize(info_size);
  m_fld_minutil->clear_visible_focus();

  m_fld_maxutil = new Fl_Output(205, q_height+top_marg+40, 60, 20, "maxutil:"); 
  m_fld_maxutil->textsize(info_size); 
  m_fld_maxutil->labelsize(info_size);
  m_fld_maxutil->clear_visible_focus();

  m_fld_sumdelta = new Fl_Output(380, q_height+top_marg+10, 60, 20, "summit_delta:"); 
  m_fld_sumdelta->textsize(info_size); 
  m_fld_sumdelta->labelsize(info_size);
  m_fld_sumdelta->clear_visible_focus();

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
Fl_Menu_Item ZAIC_LEQ_GUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)ZAIC_LEQ_GUI::cb_Quit, 0, 0},
 {0},

 {"View-Options", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Grid Size Reset ",   'g', (Fl_Callback*)ZAIC_LEQ_GUI::cb_GridSize, (void*)0, 0},
 {"Grid Size Smaller ", FL_ALT+'g', (Fl_Callback*)ZAIC_LEQ_GUI::cb_GridSize, (void*)-1, 0},
 {"Grid Size Larger ",  FL_CTRL+'g', (Fl_Callback*)ZAIC_LEQ_GUI::cb_GridSize, (void*)1, FL_MENU_DIVIDER},
 {"Grid Shade Reset ",   's', (Fl_Callback*)ZAIC_LEQ_GUI::cb_GridShade, (void*)0, 0},
 {"Grid Shade Lighter ", FL_ALT+'s', (Fl_Callback*)ZAIC_LEQ_GUI::cb_GridShade, (void*)-1, 0},
 {"Grid Shade Darker ",  FL_CTRL+'s', (Fl_Callback*)ZAIC_LEQ_GUI::cb_GridShade, (void*)1, FL_MENU_DIVIDER},
 {"Back Shade Reset ",   'b', (Fl_Callback*)ZAIC_LEQ_GUI::cb_BackShade, (void*)0, 0},
 {"Back Shade Lighter ", FL_ALT+'b', (Fl_Callback*)ZAIC_LEQ_GUI::cb_BackShade, (void*)-1, 0},
 {"Back Shade Darker ",  FL_CTRL+'b', (Fl_Callback*)ZAIC_LEQ_GUI::cb_BackShade, (void*)1, FL_MENU_DIVIDER},
 {"Line Shade Reset ",   'l', (Fl_Callback*)ZAIC_LEQ_GUI::cb_LineShade, (void*)0, 0},
 {"Line Shade Lighter ", FL_ALT+'l', (Fl_Callback*)ZAIC_LEQ_GUI::cb_LineShade, (void*)-1, 0},
 {"Line Shade Darker ",  FL_CTRL+'l', (Fl_Callback*)ZAIC_LEQ_GUI::cb_LineShade, (void*)1, FL_MENU_DIVIDER},
 {0},

 {"Modify Mode", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Adjust Summit Position ", '0', (Fl_Callback*)ZAIC_LEQ_GUI::cb_CurrMode, (void*)0, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Adjust BaseWidth ",       '1', (Fl_Callback*)ZAIC_LEQ_GUI::cb_CurrMode, (void*)1, FL_MENU_RADIO},
 {"Adjust MinUtil ",         '2', (Fl_Callback*)ZAIC_LEQ_GUI::cb_CurrMode, (void*)2, FL_MENU_RADIO},
 {"Adjust MaxUtil ",         '3', (Fl_Callback*)ZAIC_LEQ_GUI::cb_CurrMode, (void*)3, FL_MENU_RADIO},
 {"Adjust SummitDelta ",     '4', (Fl_Callback*)ZAIC_LEQ_GUI::cb_CurrMode, (void*)4, FL_MENU_RADIO},
 {"Toggle HLEQ ",            'z', (Fl_Callback*)ZAIC_LEQ_GUI::cb_ToggleHLEQ, 0, },
 {0},

 {"Modify ZAIC", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Move Left  ", FL_Left,    (Fl_Callback*)ZAIC_LEQ_GUI::cb_MoveX, (void*)-1, 0},
 {"Move Right ", FL_Right,   (Fl_Callback*)ZAIC_LEQ_GUI::cb_MoveX, (void*)1, 0},
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

int ZAIC_LEQ_GUI::handle(int event) 
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
void ZAIC_LEQ_GUI::cb_Quit() {
  exit(0);
}

//----------------------------------------- MoveX
inline void ZAIC_LEQ_GUI::cb_MoveX_i(int amt) {
  zaic_viewer->moveX((double)(amt));
  updateOutput();
  zaic_viewer->redraw();
}
void ZAIC_LEQ_GUI::cb_MoveX(Fl_Widget* o, int v) {
  ((ZAIC_LEQ_GUI*)(o->parent()->user_data()))->cb_MoveX_i(v);
}

//----------------------------------------- CurrMode
inline void ZAIC_LEQ_GUI::cb_CurrMode_i(int index) {
  zaic_viewer->currMode(index);
  updateOutput();
  zaic_viewer->redraw();
}
void ZAIC_LEQ_GUI::cb_CurrMode(Fl_Widget* o, int v) {
  ((ZAIC_LEQ_GUI*)(o->parent()->user_data()))->cb_CurrMode_i(v);
}


//----------------------------------------- ToggleHLEQ
inline void ZAIC_LEQ_GUI::cb_ToggleHLEQ_i() {
  zaic_viewer->toggleHLEQ();
  updateOutput();
  zaic_viewer->redraw();
}
void ZAIC_LEQ_GUI::cb_ToggleHLEQ(Fl_Widget* o) {
  ((ZAIC_LEQ_GUI*)(o->parent()->user_data()))->cb_ToggleHLEQ_i();
}


//----------------------------------------- GridSize
inline void ZAIC_LEQ_GUI::cb_GridSize_i(int index) {
  if(index == 0)
    zaic_viewer->setParam("gridsize", "reset");
  else if(index < 0)
    zaic_viewer->setParam("gridsize", "down");
  else
    zaic_viewer->setParam("gridsize", "up");

  zaic_viewer->redraw();
}
void ZAIC_LEQ_GUI::cb_GridSize(Fl_Widget* o, int v) {
  ((ZAIC_LEQ_GUI*)(o->parent()->user_data()))->cb_GridSize_i(v);
}

//----------------------------------------- GridShade
inline void ZAIC_LEQ_GUI::cb_GridShade_i(int index) {
  if(index == 0)
    zaic_viewer->setParam("gridshade", "reset");
  else if(index < 0)
    zaic_viewer->setParam("gridshade", "down");
  else
    zaic_viewer->setParam("gridshade", "up");

  zaic_viewer->redraw();
}
void ZAIC_LEQ_GUI::cb_GridShade(Fl_Widget* o, int v) {
  ((ZAIC_LEQ_GUI*)(o->parent()->user_data()))->cb_GridShade_i(v);
}

//----------------------------------------- LineShade
inline void ZAIC_LEQ_GUI::cb_LineShade_i(int index) {
  if(index == 0)
    zaic_viewer->setParam("lineshade", "reset");
  else if(index < 0)
    zaic_viewer->setParam("lineshade", "down");
  else
    zaic_viewer->setParam("lineshade", "up");

  zaic_viewer->redraw();
}
void ZAIC_LEQ_GUI::cb_LineShade(Fl_Widget* o, int v) {
  ((ZAIC_LEQ_GUI*)(o->parent()->user_data()))->cb_LineShade_i(v);
}

//----------------------------------------- BackShade
inline void ZAIC_LEQ_GUI::cb_BackShade_i(int index) {
  if(index == 0)
    zaic_viewer->setParam("backshade", "reset");
  else if(index < 0)
    zaic_viewer->setParam("backshade", "down");
  else
    zaic_viewer->setParam("backshade", "up");

  zaic_viewer->redraw();
}
void ZAIC_LEQ_GUI::cb_BackShade(Fl_Widget* o, int v) {
  ((ZAIC_LEQ_GUI*)(o->parent()->user_data()))->cb_BackShade_i(v);
}


//----------------------------------------- UpdateOutput
void ZAIC_LEQ_GUI::updateOutput() 
{
  string str;

  str = doubleToString(zaic_viewer->getSummit(),2);
  m_fld_summit->value(str.c_str());

  str = doubleToString(zaic_viewer->getBaseWidth(),2);
  m_fld_bwidth->value(str.c_str());

  str = doubleToString(zaic_viewer->getMinUtil(),2);
  m_fld_minutil->value(str.c_str());

  str = doubleToString(zaic_viewer->getMaxUtil(),2);
  m_fld_maxutil->value(str.c_str());

  str = doubleToString(zaic_viewer->getSummitDelta(),2);
  m_fld_sumdelta->value(str.c_str());
}
