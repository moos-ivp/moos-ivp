/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_SPD_GUI.cpp                                     */
/*    DATE: June 2nd, 2015                                       */
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

#include <iostream>
#include <cstdlib>
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

  // Min wid/hgt: 700,460 
  // Max wid/hgt: 750,500 
  this->size_range(700,460, 1250, 500); 

  mbar = new Fl_Menu_Bar(0, 0, w(), 25);
  mbar->menu(menu_);
}

//-------------------------------------------------------------------

Fl_Menu_Item ZAIC_GUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)ZAIC_GUI::cb_Quit, 0, 0},
 {0},

 {"View-Options", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Grid Size Reset ",   'g', (Fl_Callback*)ZAIC_GUI::cb_GridSize, (void*)0, 0},
 {"Grid Size Smaller ", FL_ALT+'g', (Fl_Callback*)ZAIC_GUI::cb_GridSize,
  (void*)-1, 0},
 {"Grid Size Larger ",  FL_CTRL+'g', (Fl_Callback*)ZAIC_GUI::cb_GridSize,
  (void*)1, FL_MENU_DIVIDER},
 {"Grid Shade Reset ",   's', (Fl_Callback*)ZAIC_GUI::cb_GridShade, (void*)0, 0},
 {"Grid Shade Lighter ", FL_ALT+'s', (Fl_Callback*)ZAIC_GUI::cb_GridShade,
  (void*)-1, 0},
 {"Grid Shade Darker ",  FL_CTRL+'s', (Fl_Callback*)ZAIC_GUI::cb_GridShade,
  (void*)1, FL_MENU_DIVIDER},
 {"Back Shade Reset ",   'b', (Fl_Callback*)ZAIC_GUI::cb_BackShade,
  (void*)0, 0},
 {"Back Shade Lighter ", FL_ALT+'b', (Fl_Callback*)ZAIC_GUI::cb_BackShade,
  (void*)-1, 0},
 {"Back Shade Darker ",  FL_CTRL+'b', (Fl_Callback*)ZAIC_GUI::cb_BackShade,
  (void*)1, FL_MENU_DIVIDER},
 {"Line Shade Reset ",   'l', (Fl_Callback*)ZAIC_GUI::cb_LineShade,
  (void*)0, 0},
 {"Line Shade Lighter ", FL_ALT+'l', (Fl_Callback*)ZAIC_GUI::cb_LineShade,
  (void*)-1, 0},
 {"Line Shade Darker ",  FL_CTRL+'l', (Fl_Callback*)ZAIC_GUI::cb_LineShade,
  (void*)1, FL_MENU_DIVIDER},
 {"Toggle Verbose ",     'v', (Fl_Callback*)ZAIC_GUI::cb_ToggleVerbose,
  (void*)1, FL_MENU_DIVIDER},
 {"Toggle Labels ", 'j', (Fl_Callback*)ZAIC_GUI::cb_ToggleLabels,
  (void*)1, FL_MENU_DIVIDER},
 {"Toggle ColorScheme ", 'c', (Fl_Callback*)ZAIC_GUI::cb_ToggleColorScheme,
  (void*)1, FL_MENU_DIVIDER},
 {0},

 {"Modify ZAIC", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Move Left  ", FL_Left,    (Fl_Callback*)ZAIC_GUI::cb_MoveX, (void*)-1, 0},
 {"Move Right ", FL_Right,   (Fl_Callback*)ZAIC_GUI::cb_MoveX, (void*)1, 0},
 {0},

 {0}
};

//----------------------------------------------------------
// Procedure: handle()

int ZAIC_GUI::handle(int event) 
{
  switch(event) {
  case FL_PUSH:
    cout << "In ZAIC_GUI::handle()" << endl;
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
  m_zaic_model->moveX((double)(amt));
  updateOutput();
  m_zaic_viewer->redraw();
}
void ZAIC_GUI::cb_MoveX(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_MoveX_i(v);
}

//----------------------------------------- CurrMode
inline void ZAIC_GUI::cb_CurrMode_i(int index) {
  m_zaic_model->currMode(index);
  updateOutput();
  m_zaic_viewer->redraw();
}
void ZAIC_GUI::cb_CurrMode(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_CurrMode_i(v);
}

//----------------------------------------- GridSize
inline void ZAIC_GUI::cb_GridSize_i(int index) {
  if(index == 0)
    m_zaic_viewer->setParam("gridsize", "reset");
  else if(index < 0)
    m_zaic_viewer->setParam("gridsize", "down");
  else
    m_zaic_viewer->setParam("gridsize", "up");

  m_zaic_viewer->redraw();
}
void ZAIC_GUI::cb_GridSize(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_GridSize_i(v);
}

//----------------------------------------- GridShade
inline void ZAIC_GUI::cb_GridShade_i(int index) {
  if(index == 0)
    m_zaic_viewer->setParam("gridshade", "reset");
  else if(index < 0)
    m_zaic_viewer->setParam("gridshade", "down");
  else
    m_zaic_viewer->setParam("gridshade", "up");

  m_zaic_viewer->redraw();
}
void ZAIC_GUI::cb_GridShade(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_GridShade_i(v);
}

//----------------------------------------- LineShade
inline void ZAIC_GUI::cb_LineShade_i(int index) {
  if(index == 0)
    m_zaic_viewer->setParam("lineshade", "reset");
  else if(index < 0)
    m_zaic_viewer->setParam("lineshade", "down");
  else
    m_zaic_viewer->setParam("lineshade", "up");

  m_zaic_viewer->redraw();
}
void ZAIC_GUI::cb_LineShade(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_LineShade_i(v);
}

//----------------------------------------- BackShade
inline void ZAIC_GUI::cb_BackShade_i(int index) {
  if(index == 0)
    m_zaic_viewer->setParam("backshade", "reset");
  else if(index < 0)
    m_zaic_viewer->setParam("backshade", "down");
  else
    m_zaic_viewer->setParam("backshade", "up");

  m_zaic_viewer->redraw();
}
void ZAIC_GUI::cb_BackShade(Fl_Widget* o, int v) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_BackShade_i(v);
}

//----------------------------------------- ToggleVerbose
inline void ZAIC_GUI::cb_ToggleVerbose_i() {
  m_zaic_viewer->toggleVerbose();
  m_zaic_viewer->redraw();
}
void ZAIC_GUI::cb_ToggleVerbose(Fl_Widget* o) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_ToggleVerbose_i();
}

//----------------------------------------- ToggleLabels
inline void ZAIC_GUI::cb_ToggleLabels_i() {
  m_zaic_viewer->setParam("labels", "toggle");
  m_zaic_viewer->redraw();
}
void ZAIC_GUI::cb_ToggleLabels(Fl_Widget* o) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_ToggleLabels_i();
}


//----------------------------------------- ToggleColorScheme
inline void ZAIC_GUI::cb_ToggleColorScheme_i() {
  m_zaic_viewer->setParam("color_scheme", "toggle");
  m_zaic_viewer->redraw();
}
void ZAIC_GUI::cb_ToggleColorScheme(Fl_Widget* o) {
  ((ZAIC_GUI*)(o->parent()->user_data()))->cb_ToggleColorScheme_i();
}






