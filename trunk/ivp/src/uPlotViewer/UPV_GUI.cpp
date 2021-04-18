/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UPV_GUI.cpp                                          */
/*    DATE: May 18th, 2012                                       */
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
#include "UPV_GUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

UPV_GUI::UPV_GUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) {

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();
  this->size_range(650,400);

  m_menubar = new Fl_Menu_Bar(0, 0, w(), 25);
  m_menubar->menu(menu_);
    
  int info_size=10;

  int top_marg = 30;
  int bot_marg = 50;
  int sid_marg = 5;
  int q_height = h()-(top_marg+bot_marg);
  //int q_width  = w()-(sid_marg*2);

  m_plot_viewer = new UPV_Viewer(sid_marg, top_marg, w()-(sid_marg*2), q_height);

  m_varname = new MY_Output(85, q_height+top_marg+10, 120, 20, "Variable:"); 
  m_varname->textsize(info_size); 
  m_varname->labelsize(info_size);

  m_varavg = new MY_Output(285, q_height+top_marg+10, 60, 20, "Average:"); 
  m_varavg->textsize(info_size); 
  m_varavg->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();
}

Fl_Menu_Item UPV_GUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)UPV_GUI::cb_Quit, 0, 0},
 {0},

 {"View-Options", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Grid Size Reset ",   'g', (Fl_Callback*)UPV_GUI::cb_GridSize, (void*)0, 0},
 {"Grid Size Smaller ", FL_ALT+'g', (Fl_Callback*)UPV_GUI::cb_GridSize, (void*)-1, 0},
 {"Grid Size Larger ",  FL_CTRL+'g', (Fl_Callback*)UPV_GUI::cb_GridSize, (void*)1, FL_MENU_DIVIDER},
 {"Grid Shade Reset ",   's', (Fl_Callback*)UPV_GUI::cb_GridShade, (void*)0, 0},
 {"Grid Shade Lighter ", FL_ALT+'s', (Fl_Callback*)UPV_GUI::cb_GridShade, (void*)-1, 0},
 {"Grid Shade Darker ",  FL_CTRL+'s', (Fl_Callback*)UPV_GUI::cb_GridShade, (void*)1, FL_MENU_DIVIDER},
 {"Back Shade Reset ",   'b', (Fl_Callback*)UPV_GUI::cb_BackShade, (void*)0, 0},
 {"Back Shade Lighter ", FL_ALT+'b', (Fl_Callback*)UPV_GUI::cb_BackShade, (void*)-1, 0},
 {"Back Shade Darker ",  FL_CTRL+'b', (Fl_Callback*)UPV_GUI::cb_BackShade, (void*)1, FL_MENU_DIVIDER},
 {"Line Shade Reset ",   'l', (Fl_Callback*)UPV_GUI::cb_LineShade, (void*)0, 0},
 {"Line Shade Lighter ", FL_ALT+'l', (Fl_Callback*)UPV_GUI::cb_LineShade, (void*)-1, 0},
 {"Line Shade Darker ",  FL_CTRL+'l', (Fl_Callback*)UPV_GUI::cb_LineShade, (void*)1, FL_MENU_DIVIDER},
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

int UPV_GUI::handle(int event) 
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

//----------------------------------------------------------
// Procedure: addScopeVar

void UPV_GUI::addScopeVar(string varname)
{
  if(vectorContains(m_scope_vars, varname))
    return;
  
  long int index = (int)(m_scope_vars.size());
  m_scope_vars.push_back(varname);

  string entry = "HotVar/" + varname;
  m_menubar->add(entry.c_str(), 0, (Fl_Callback*)UPV_GUI::cb_ChangeScopeVar,(void*)index, FL_MENU_RADIO);
  Fl_Menu_Item *item = (Fl_Menu_Item*)m_menubar->find_item(entry.c_str());
  if(item)
    item->setonly();

  string entry2 = "Render/" + varname;
  m_menubar->add(entry2.c_str(), 0, (Fl_Callback*)UPV_GUI::cb_ToggleScopeVar,(void*)index, FL_MENU_TOGGLE);
  Fl_Menu_Item *item2 = (Fl_Menu_Item*)m_menubar->find_item(entry2.c_str());
  if(item2)
    item2->set();

  m_plot_viewer->setHotVar(varname);
  m_menubar->redraw();
}


//----------------------------------------- Quit
void UPV_GUI::cb_Quit() {
  exit(0);
}

//----------------------------------------- ToggleScopeVar
inline void UPV_GUI::cb_ToggleScopeVar_i(int index) {
  if((index < 0) || (index >= (int)(m_scope_vars.size())))
    return;

  string varname = m_scope_vars[index];

  string fullpath = "Render/" + varname;
  Fl_Menu_Item *item = (Fl_Menu_Item*)m_menubar->find_item(fullpath.c_str());

  if(item) {
    if(item->value() == 0) {
      // Viewer will not allow all vars to be hidden, only toggle if done
      bool done = m_plot_viewer->hideVar(varname);
      if(!done)
	item->set();
    }
    else
      m_plot_viewer->showVar(varname);
    m_plot_viewer->redraw();
  }
}
void UPV_GUI::cb_ToggleScopeVar(Fl_Widget* o, int v) {
  ((UPV_GUI*)(o->parent()->user_data()))->cb_ToggleScopeVar_i(v);
}

//----------------------------------------- ChangeScopeVar
inline void UPV_GUI::cb_ChangeScopeVar_i(int index) {
  if((index < 0) || (index >= (int)(m_scope_vars.size())))
    return;

  string varname = m_scope_vars[index];
  m_plot_viewer->setHotVar(varname);

  string fullpath = "Render/" + varname;
  Fl_Menu_Item *item = (Fl_Menu_Item*)m_menubar->find_item(fullpath.c_str());
  if(item) {
    m_plot_viewer->showVar(varname);
    item->set();
  }

  m_plot_viewer->redraw();
}
void UPV_GUI::cb_ChangeScopeVar(Fl_Widget* o, int v) {
  ((UPV_GUI*)(o->parent()->user_data()))->cb_ChangeScopeVar_i(v);
}

//----------------------------------------- GridSize
inline void UPV_GUI::cb_GridSize_i(int index) {
  if(index == 0)
    m_plot_viewer->setParam("gridsize", "reset");
  else if(index < 0)
    m_plot_viewer->setParam("gridsize", "down");
  else
    m_plot_viewer->setParam("gridsize", "up");

  m_plot_viewer->redraw();
}
void UPV_GUI::cb_GridSize(Fl_Widget* o, int v) {
  ((UPV_GUI*)(o->parent()->user_data()))->cb_GridSize_i(v);
}

//----------------------------------------- GridShade
inline void UPV_GUI::cb_GridShade_i(int index) {
  if(index == 0)
    m_plot_viewer->setParam("gridshade", "reset");
  else if(index < 0)
    m_plot_viewer->setParam("gridshade", "down");
  else
    m_plot_viewer->setParam("gridshade", "up");

  m_plot_viewer->redraw();
}
void UPV_GUI::cb_GridShade(Fl_Widget* o, int v) {
  ((UPV_GUI*)(o->parent()->user_data()))->cb_GridShade_i(v);
}

//----------------------------------------- LineShade
inline void UPV_GUI::cb_LineShade_i(int index) {
  if(index == 0)
    m_plot_viewer->setParam("lineshade", "reset");
  else if(index < 0)
    m_plot_viewer->setParam("lineshade", "down");
  else
    m_plot_viewer->setParam("lineshade", "up");

  m_plot_viewer->redraw();
}
void UPV_GUI::cb_LineShade(Fl_Widget* o, int v) {
  ((UPV_GUI*)(o->parent()->user_data()))->cb_LineShade_i(v);
}

//----------------------------------------- BackShade
inline void UPV_GUI::cb_BackShade_i(int index) {
  if(index == 0)
    m_plot_viewer->setParam("backshade", "reset");
  else if(index < 0)
    m_plot_viewer->setParam("backshade", "down");
  else
    m_plot_viewer->setParam("backshade", "up");

  m_plot_viewer->redraw();
}
void UPV_GUI::cb_BackShade(Fl_Widget* o, int v) {
  ((UPV_GUI*)(o->parent()->user_data()))->cb_BackShade_i(v);
}

//----------------------------------------- UpdateOutput
void UPV_GUI::updateOutput() 
{
  string str = m_plot_viewer->getHotVar();
  m_varname->value(str.c_str());

  double val  = m_plot_viewer->getHotVarAvg();
  string sval = doubleToStringX(val, 4);
  m_varavg->value(sval.c_str());
}









