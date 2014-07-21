/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VZAIC_GUI.cpp                                        */
/*    DATE: May 4th, 2010                                        */
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
#include "VZAIC_GUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

VZAIC_GUI::VZAIC_GUI(int g_w, int g_h, const char *g_l)
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

  m_zaic_viewer = new VZAIC_Viewer(sid_marg, top_marg, w()-(sid_marg*2), q_height);

  s_maxutil = new MY_Output(80, q_height+top_marg+10, 60, 20, "max_util:"); 
  s_maxutil->textsize(info_size); 
  s_maxutil->labelsize(info_size);

  s_minutil = new MY_Output(80, q_height+top_marg+40, 60, 20, "min_util:"); 
  s_minutil->textsize(info_size); 
  s_minutil->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();
}

Fl_Menu_Item VZAIC_GUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)VZAIC_GUI::cb_Quit, 0, 0},
 {0},

 {"View-Options", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Grid Size Reset ",   'g', (Fl_Callback*)VZAIC_GUI::cb_GridSize, (void*)0, 0},
 {"Grid Size Smaller ", FL_ALT+'g', (Fl_Callback*)VZAIC_GUI::cb_GridSize, (void*)-1, 0},
 {"Grid Size Larger ",  FL_CTRL+'g', (Fl_Callback*)VZAIC_GUI::cb_GridSize, (void*)1, FL_MENU_DIVIDER},
 {"Grid Shade Reset ",   's', (Fl_Callback*)VZAIC_GUI::cb_GridShade, (void*)0, 0},
 {"Grid Shade Lighter ", FL_ALT+'s', (Fl_Callback*)VZAIC_GUI::cb_GridShade, (void*)-1, 0},
 {"Grid Shade Darker ",  FL_CTRL+'s', (Fl_Callback*)VZAIC_GUI::cb_GridShade, (void*)1, FL_MENU_DIVIDER},
 {"Back Shade Reset ",   'b', (Fl_Callback*)VZAIC_GUI::cb_BackShade, (void*)0, 0},
 {"Back Shade Lighter ", FL_ALT+'b', (Fl_Callback*)VZAIC_GUI::cb_BackShade, (void*)-1, 0},
 {"Back Shade Darker ",  FL_CTRL+'b', (Fl_Callback*)VZAIC_GUI::cb_BackShade, (void*)1, FL_MENU_DIVIDER},
 {"Line Shade Reset ",   'l', (Fl_Callback*)VZAIC_GUI::cb_LineShade, (void*)0, 0},
 {"Line Shade Lighter ", FL_ALT+'l', (Fl_Callback*)VZAIC_GUI::cb_LineShade, (void*)-1, 0},
 {"Line Shade Darker ",  FL_CTRL+'l', (Fl_Callback*)VZAIC_GUI::cb_LineShade, (void*)1, FL_MENU_DIVIDER},
 {"Labels Toggle ",   FL_SHIFT+'l', (Fl_Callback*)VZAIC_GUI::cb_DrawLabels, (void*)0, 0},
 {"Labels Off ", 0, (Fl_Callback*)VZAIC_GUI::cb_DrawLabels, (void*)-1, 0},
 {"Labels On ",  0, (Fl_Callback*)VZAIC_GUI::cb_DrawLabels, (void*)1, FL_MENU_DIVIDER},
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

int VZAIC_GUI::handle(int event) 
{
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

//----------------------------------------------------------
// Procedure: setZAIC

void VZAIC_GUI::setZAIC(ZAIC_Vector *zaic)
{
  if(m_zaic_viewer)
    m_zaic_viewer->setZAIC(zaic);
}

//----------------------------------------- Quit
void VZAIC_GUI::cb_Quit() {
  exit(0);
}

//----------------------------------------- GridSize
inline void VZAIC_GUI::cb_GridSize_i(int index) {
  if(index == 0)
    m_zaic_viewer->setParam("gridsize", "reset");
  else if(index < 0)
    m_zaic_viewer->setParam("gridsize", "down");
  else
    m_zaic_viewer->setParam("gridsize", "up");

  m_zaic_viewer->redraw();
}
void VZAIC_GUI::cb_GridSize(Fl_Widget* o, int v) {
  ((VZAIC_GUI*)(o->parent()->user_data()))->cb_GridSize_i(v);
}

//----------------------------------------- GridShade
inline void VZAIC_GUI::cb_GridShade_i(int index) {
  if(index == 0)
    m_zaic_viewer->setParam("gridshade", "reset");
  else if(index < 0)
    m_zaic_viewer->setParam("gridshade", "down");
  else
    m_zaic_viewer->setParam("gridshade", "up");

  m_zaic_viewer->redraw();
}
void VZAIC_GUI::cb_GridShade(Fl_Widget* o, int v) {
  ((VZAIC_GUI*)(o->parent()->user_data()))->cb_GridShade_i(v);
}

//----------------------------------------- LineShade
inline void VZAIC_GUI::cb_LineShade_i(int index) {
  if(index == 0)
    m_zaic_viewer->setParam("lineshade", "reset");
  else if(index < 0)
    m_zaic_viewer->setParam("lineshade", "down");
  else
    m_zaic_viewer->setParam("lineshade", "up");

  m_zaic_viewer->redraw();
}
void VZAIC_GUI::cb_LineShade(Fl_Widget* o, int v) {
  ((VZAIC_GUI*)(o->parent()->user_data()))->cb_LineShade_i(v);
}

//----------------------------------------- DrawLabels
inline void VZAIC_GUI::cb_DrawLabels_i(int index) {
  if(index == 0)
    m_zaic_viewer->setParam("draw_labels", "toggle");
  else if(index < 0)
    m_zaic_viewer->setParam("draw_labels", "false");
  else
    m_zaic_viewer->setParam("draw_labels", "true");

  m_zaic_viewer->redraw();
}
void VZAIC_GUI::cb_DrawLabels(Fl_Widget* o, int v) {
  ((VZAIC_GUI*)(o->parent()->user_data()))->cb_DrawLabels_i(v);
}

//----------------------------------------- BackShade
inline void VZAIC_GUI::cb_BackShade_i(int index) {
  if(index == 0)
    m_zaic_viewer->setParam("backshade", "reset");
  else if(index < 0)
    m_zaic_viewer->setParam("backshade", "down");
  else
    m_zaic_viewer->setParam("backshade", "up");

  m_zaic_viewer->redraw();
}
void VZAIC_GUI::cb_BackShade(Fl_Widget* o, int v) {
  ((VZAIC_GUI*)(o->parent()->user_data()))->cb_BackShade_i(v);
}


//----------------------------------------- UpdateOutput
void VZAIC_GUI::updateOutput() 
{
  string str;

  str = doubleToString(m_zaic_viewer->getMaxUtil(),2);
  s_maxutil->value(str.c_str());

  str = doubleToString(m_zaic_viewer->getMinUtil(),2);
  s_minutil->value(str.c_str());
}





