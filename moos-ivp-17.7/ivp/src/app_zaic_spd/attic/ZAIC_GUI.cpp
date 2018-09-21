/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_SPD_GUI.cpp                                     */
/*    DATE: June 2nd, 2015                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "ZAIC_SPD_GUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

ZAIC_SPD_GUI::ZAIC_SPD_GUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) {

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  // Min wid/hgt: 700,460 
  // Max wid/hgt: 750,500 
  this->size_range(700,460, 1250, 500); 

  mbar = new Fl_Menu_Bar(0, 0, w(), 25);
  mbar->menu(menu_);
    
  int info_size=10;

  int top_marg = 30;
  int bot_marg = 45;
  int sid_marg = 5;
  int q_height = h()-(top_marg+bot_marg);
  int q_width  = w()-(sid_marg*2);

  m_zaic_model = new ZAIC_SPD_Model();
  m_zaic_model->setDomain(501);

  m_zaic_viewer = new ZAIC_Viewer(sid_marg, top_marg, w()-(sid_marg*2), q_height);
  m_zaic_viewer->setModel(m_zaic_model);

  m_lowval = new Fl_Output(60, q_height+top_marg+10, 60, 20, "lowval:"); 
  m_lowval->textsize(info_size); 
  m_lowval->labelsize(info_size);
  m_lowval->clear_visible_focus();

  m_medval = new Fl_Output(160, q_height+top_marg+10, 60, 20, "medval:"); 
  m_medval->textsize(info_size); 
  m_medval->labelsize(info_size);
  m_medval->clear_visible_focus();

  m_hghval = new Fl_Output(260, q_height+top_marg+10, 60, 20, "hghval:"); 
  m_hghval->textsize(info_size); 
  m_hghval->labelsize(info_size);
  m_hghval->clear_visible_focus();

  m_lowval_util = new Fl_Output(450, q_height+top_marg+10, 60, 20, "lowval_util:"); 
  m_lowval_util->textsize(info_size); 
  m_lowval_util->labelsize(info_size);
  m_lowval_util->clear_visible_focus();

  m_hghval_util = new Fl_Output(600, q_height+top_marg+10, 60, 20, "hghval_util:"); 
  m_hghval_util->textsize(info_size); 
  m_hghval_util->labelsize(info_size);
  m_hghval_util->clear_visible_focus();

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------

Fl_Menu_Item ZAIC_SPD_GUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)ZAIC_SPD_GUI::cb_Quit, 0, 0},
 {0},

 {"View-Options", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Grid Size Reset ",   'g', (Fl_Callback*)ZAIC_SPD_GUI::cb_GridSize, (void*)0, 0},
 {"Grid Size Smaller ", FL_ALT+'g', (Fl_Callback*)ZAIC_SPD_GUI::cb_GridSize, (void*)-1, 0},
 {"Grid Size Larger ",  FL_CTRL+'g', (Fl_Callback*)ZAIC_SPD_GUI::cb_GridSize, (void*)1, FL_MENU_DIVIDER},
 {"Grid Shade Reset ",   's', (Fl_Callback*)ZAIC_SPD_GUI::cb_GridShade, (void*)0, 0},
 {"Grid Shade Lighter ", FL_ALT+'s', (Fl_Callback*)ZAIC_SPD_GUI::cb_GridShade, (void*)-1, 0},
 {"Grid Shade Darker ",  FL_CTRL+'s', (Fl_Callback*)ZAIC_SPD_GUI::cb_GridShade, (void*)1, FL_MENU_DIVIDER},
 {"Back Shade Reset ",   'b', (Fl_Callback*)ZAIC_SPD_GUI::cb_BackShade, (void*)0, 0},
 {"Back Shade Lighter ", FL_ALT+'b', (Fl_Callback*)ZAIC_SPD_GUI::cb_BackShade, (void*)-1, 0},
 {"Back Shade Darker ",  FL_CTRL+'b', (Fl_Callback*)ZAIC_SPD_GUI::cb_BackShade, (void*)1, FL_MENU_DIVIDER},
 {"Line Shade Reset ",   'l', (Fl_Callback*)ZAIC_SPD_GUI::cb_LineShade, (void*)0, 0},
 {"Line Shade Lighter ", FL_ALT+'l', (Fl_Callback*)ZAIC_SPD_GUI::cb_LineShade, (void*)-1, 0},
 {"Line Shade Darker ",  FL_CTRL+'l', (Fl_Callback*)ZAIC_SPD_GUI::cb_LineShade, (void*)1, FL_MENU_DIVIDER},
 {"Toggle Verbose ",     'v', (Fl_Callback*)ZAIC_SPD_GUI::cb_ToggleVerbose, (void*)1, FL_MENU_DIVIDER},
 {"Toggle Labels ", 'j', (Fl_Callback*)ZAIC_SPD_GUI::cb_ToggleLabels, (void*)1, FL_MENU_DIVIDER},
 {"Toggle ColorScheme ", 'c', (Fl_Callback*)ZAIC_SPD_GUI::cb_ToggleColorScheme, (void*)1, FL_MENU_DIVIDER},
 {0},

 {"Modify Mode", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Adjust MedVal ", '0', (Fl_Callback*)ZAIC_SPD_GUI::cb_CurrMode, (void*)0, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Adjust LowVal ", '1', (Fl_Callback*)ZAIC_SPD_GUI::cb_CurrMode, (void*)1, FL_MENU_RADIO},
 {"Adjust HighVal ",     '2', (Fl_Callback*)ZAIC_SPD_GUI::cb_CurrMode, (void*)2, FL_MENU_RADIO},
 {"Adjust LowValUtil ",  '3', (Fl_Callback*)ZAIC_SPD_GUI::cb_CurrMode, (void*)3, FL_MENU_RADIO},
 {"Adjust HighValUtil ", '4', (Fl_Callback*)ZAIC_SPD_GUI::cb_CurrMode, (void*)4, FL_MENU_RADIO},
 {0},

 {"Modify ZAIC", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Move Left  ", FL_Left,    (Fl_Callback*)ZAIC_SPD_GUI::cb_MoveX, (void*)-1, 0},
 {"Move Right ", FL_Right,   (Fl_Callback*)ZAIC_SPD_GUI::cb_MoveX, (void*)1, 0},
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


int ZAIC_SPD_GUI::handle(int event) 
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
void ZAIC_SPD_GUI::cb_Quit() {
  exit(0);
}

//----------------------------------------- MoveX
inline void ZAIC_SPD_GUI::cb_MoveX_i(int amt) {
  m_zaic_model->moveX((double)(amt));
  updateOutput();
  m_zaic_viewer->redraw();
}
void ZAIC_SPD_GUI::cb_MoveX(Fl_Widget* o, int v) {
  ((ZAIC_SPD_GUI*)(o->parent()->user_data()))->cb_MoveX_i(v);
}

//----------------------------------------- CurrMode
inline void ZAIC_SPD_GUI::cb_CurrMode_i(int index) {
  m_zaic_model->currMode(index);
  updateOutput();
  m_zaic_viewer->redraw();
}
void ZAIC_SPD_GUI::cb_CurrMode(Fl_Widget* o, int v) {
  ((ZAIC_SPD_GUI*)(o->parent()->user_data()))->cb_CurrMode_i(v);
}

//----------------------------------------- GridSize
inline void ZAIC_SPD_GUI::cb_GridSize_i(int index) {
  if(index == 0)
    m_zaic_viewer->setParam("gridsize", "reset");
  else if(index < 0)
    m_zaic_viewer->setParam("gridsize", "down");
  else
    m_zaic_viewer->setParam("gridsize", "up");

  m_zaic_viewer->redraw();
}
void ZAIC_SPD_GUI::cb_GridSize(Fl_Widget* o, int v) {
  ((ZAIC_SPD_GUI*)(o->parent()->user_data()))->cb_GridSize_i(v);
}

//----------------------------------------- GridShade
inline void ZAIC_SPD_GUI::cb_GridShade_i(int index) {
  if(index == 0)
    m_zaic_viewer->setParam("gridshade", "reset");
  else if(index < 0)
    m_zaic_viewer->setParam("gridshade", "down");
  else
    m_zaic_viewer->setParam("gridshade", "up");

  m_zaic_viewer->redraw();
}
void ZAIC_SPD_GUI::cb_GridShade(Fl_Widget* o, int v) {
  ((ZAIC_SPD_GUI*)(o->parent()->user_data()))->cb_GridShade_i(v);
}

//----------------------------------------- LineShade
inline void ZAIC_SPD_GUI::cb_LineShade_i(int index) {
  if(index == 0)
    m_zaic_viewer->setParam("lineshade", "reset");
  else if(index < 0)
    m_zaic_viewer->setParam("lineshade", "down");
  else
    m_zaic_viewer->setParam("lineshade", "up");

  m_zaic_viewer->redraw();
}
void ZAIC_SPD_GUI::cb_LineShade(Fl_Widget* o, int v) {
  ((ZAIC_SPD_GUI*)(o->parent()->user_data()))->cb_LineShade_i(v);
}

//----------------------------------------- BackShade
inline void ZAIC_SPD_GUI::cb_BackShade_i(int index) {
  if(index == 0)
    m_zaic_viewer->setParam("backshade", "reset");
  else if(index < 0)
    m_zaic_viewer->setParam("backshade", "down");
  else
    m_zaic_viewer->setParam("backshade", "up");

  m_zaic_viewer->redraw();
}
void ZAIC_SPD_GUI::cb_BackShade(Fl_Widget* o, int v) {
  ((ZAIC_SPD_GUI*)(o->parent()->user_data()))->cb_BackShade_i(v);
}

//----------------------------------------- ToggleVerbose
inline void ZAIC_SPD_GUI::cb_ToggleVerbose_i() {
  m_zaic_viewer->toggleVerbose();
  m_zaic_viewer->redraw();
}
void ZAIC_SPD_GUI::cb_ToggleVerbose(Fl_Widget* o) {
  ((ZAIC_SPD_GUI*)(o->parent()->user_data()))->cb_ToggleVerbose_i();
}

//----------------------------------------- ToggleLabels
inline void ZAIC_SPD_GUI::cb_ToggleLabels_i() {
  m_zaic_viewer->setParam("labels", "toggle");
  m_zaic_viewer->redraw();
}
void ZAIC_SPD_GUI::cb_ToggleLabels(Fl_Widget* o) {
  ((ZAIC_SPD_GUI*)(o->parent()->user_data()))->cb_ToggleLabels_i();
}


//----------------------------------------- ToggleColorScheme
inline void ZAIC_SPD_GUI::cb_ToggleColorScheme_i() {
  m_zaic_viewer->setParam("color_scheme", "toggle");
  m_zaic_viewer->redraw();
}
void ZAIC_SPD_GUI::cb_ToggleColorScheme(Fl_Widget* o) {
  ((ZAIC_SPD_GUI*)(o->parent()->user_data()))->cb_ToggleColorScheme_i();
}


//----------------------------------------- UpdateOutput
void ZAIC_SPD_GUI::updateOutput() 
{
  string str;

  str = doubleToString(m_zaic_model->getMedVal(),2);
  m_medval->value(str.c_str());

  str = doubleToString(m_zaic_model->getLowVal(),2);
  m_lowval->value(str.c_str());

  str = doubleToString(m_zaic_model->getHghVal(),2);
  m_hghval->value(str.c_str());

  str = doubleToString(m_zaic_model->getLowValUtil(),2);
  m_lowval_util->value(str.c_str());

  str = doubleToString(m_zaic_model->getHghValUtil(),2);
  m_hghval_util->value(str.c_str());
}
