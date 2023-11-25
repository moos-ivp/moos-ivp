/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MarineVehiGUI.cpp                                    */
/*    DATE: November, 2004                                       */
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
#include "MarineVehiGUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

MarineVehiGUI::MarineVehiGUI(int g_w, int g_h, const char *g_l)
  : MarineGUI(g_w, g_h, g_l) 
{
  addGeoAttrMenu();
  augmentMenu();
}

//-------------------------------------------------------------------
// Procedure: augmentMenu()

void MarineVehiGUI::augmentMenu()
{
  m_menubar->add("Vehicles/vehicles_viewable=true",  0,                (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)10, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_viewable=false", 0,                (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)11, FL_MENU_RADIO);
  m_menubar->add("Vehicles/    Toggle vehicles_viewable", FL_CTRL+'v', (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)12, FL_MENU_DIVIDER);

  m_menubar->add("Vehicles/    Toggle Vehicle Name Mode",      'n', (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)29, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_name_mode=names",            0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)20, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_name_mode=names+mode",       0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)21, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_name_mode=names+shortmode",  0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)22, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_name_mode=names+depth",      0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)23, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_name_mode=names+auxmode",    0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)24, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_name_mode=off",   FL_SHIFT+'n', (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)25, FL_MENU_RADIO|FL_MENU_DIVIDER);


  m_menubar->add("Vehicles/vehicles_active_color=red",    0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)40, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_active_color=yellow", 0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)41, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_active_color=white",  0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)42, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_active_color=blue",   0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)43, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_active_color=green",  0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)44, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("Vehicles/vehicles_inactive_color=yellow", 0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)50, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_inactive_color=white",  0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)51, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_inactive_color=blue",   0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)52, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_inactive_color=green",  0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)53, FL_MENU_RADIO);
  m_menubar->add("Vehicles/vehicles_inactive_color=red",    0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)54, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("Vehicles/trails_viewable=true",          0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)60, FL_MENU_RADIO);
  m_menubar->add("Vehicles/trails_viewable=false",         0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)61, FL_MENU_RADIO);
  m_menubar->add("Vehicles/    Toggle trails_viewable",  't', (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)62, FL_MENU_DIVIDER);

  m_menubar->add("Vehicles/trails_color=white",  0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)65, FL_MENU_RADIO);
  m_menubar->add("Vehicles/trails_color=yellow", 0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)66, FL_MENU_RADIO);
  m_menubar->add("Vehicles/trails_color=green",  0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)67, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("Vehicles/trails_connect_viewable=true",         0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)70, FL_MENU_RADIO);
  m_menubar->add("Vehicles/trails_connect_viewable=false",        0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)71, FL_MENU_RADIO);
  m_menubar->add("Vehicles/    Toggle trails_connect_viewable", 'y', (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)72, FL_MENU_DIVIDER);

  m_menubar->add("Vehicles/bearing_lines_viewable=true",         0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)80, FL_MENU_RADIO);
  m_menubar->add("Vehicles/bearing_lines_viewable=false",        0, (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)81, FL_MENU_RADIO);
  m_menubar->add("Vehicles/    Toggle bearing_lines_viewable", '$', (Fl_Callback*)MarineVehiGUI::cb_SetRadioVehiAttrib, (void*)82, FL_MENU_DIVIDER);

  m_menubar->add("Vehicles/Cycle Vehicle Focus", 'v', (Fl_Callback*)MarineVehiGUI::cb_CycleFocus,(void*)0, FL_MENU_DIVIDER);

  m_menubar->add("Vehicles/center_view = active", FL_CTRL+'c', (Fl_Callback*)MarineVehiGUI::cb_CenterView,  (void*)1, FL_MENU_RADIO);
  m_menubar->add("Vehicles/center_view = average", 'c', (Fl_Callback*)MarineVehiGUI::cb_CenterView,  (void*)0, FL_MENU_DIVIDER);

  m_menubar->add("Vehicles/VehicleSize/vehicles_shape_scale = larger",    '+', (Fl_Callback*)MarineVehiGUI::cb_AltShapeScale, (void*)+1, 0);
  m_menubar->add("Vehicles/VehicleSize/vehicles_shape_scale = smaller",   '-', (Fl_Callback*)MarineVehiGUI::cb_AltShapeScale, (void*)-1, 0);
  m_menubar->add("Vehicles/VehicleSize/vehicles_shape_scale = actual",    '\\', (Fl_Callback*)MarineVehiGUI::cb_AltShapeScale, (void*)0, FL_MENU_DIVIDER);

  m_menubar->add("Vehicles/Trails/trails_point_size = larger", '>', (Fl_Callback*)MarineVehiGUI::cb_AltTrailSize, (void*)+1, 0);
  m_menubar->add("Vehicles/Trails/trails_point_size = smaller", '<', (Fl_Callback*)MarineVehiGUI::cb_AltTrailSize, (void*)-1, 0);
  m_menubar->add("Vehicles/Trails/trails_length = longer", ')', (Fl_Callback*)MarineVehiGUI::cb_AltTrailLength, (void*)+1, 0);
  m_menubar->add("Vehicles/Trails/trails_length = shorter", '(', (Fl_Callback*)MarineVehiGUI::cb_AltTrailLength, (void*)-1, FL_MENU_DIVIDER);
}

//----------------------------------------------------- 
// Procedure: updateRadios

void MarineVehiGUI::updateRadios()
{
  MarineGUI::updateRadios();
  setRadioVehiAttrib("vehicles_viewable");
  setRadioVehiAttrib("vehicles_name_mode");
  setRadioVehiAttrib("vehicles_active_color");
  setRadioVehiAttrib("vehicles_inactive_color");
  setRadioVehiAttrib("trails_viewable");
  setRadioVehiAttrib("trails_color");
  setRadioVehiAttrib("trails_connect_viewable");
  setRadioVehiAttrib("bearing_lines_viewable");
}

//----------------------------------------------------- 
// Procedure: setMenuItemColors

void MarineVehiGUI::setMenuItemColors()
{
  MarineGUI::setMenuItemColors();
  setMenuItemColor("Vehicles/vehicles_viewable=true");
  setMenuItemColor("Vehicles/vehicles_viewable=false");

  setMenuItemColor("Vehicles/vehicles_name_mode=names");
  setMenuItemColor("Vehicles/vehicles_name_mode=names+mode");
  setMenuItemColor("Vehicles/vehicles_name_mode=names+shortmode");
  setMenuItemColor("Vehicles/vehicles_name_mode=names+depth");
  setMenuItemColor("Vehicles/vehicles_name_mode=off");

  setMenuItemColor("Vehicles/vehicles_active_color=red");
  setMenuItemColor("Vehicles/vehicles_active_color=yellow");
  setMenuItemColor("Vehicles/vehicles_active_color=white");
  setMenuItemColor("Vehicles/vehicles_active_color=blue");
  setMenuItemColor("Vehicles/vehicles_active_color=green");

  setMenuItemColor("Vehicles/vehicles_inactive_color=yellow");
  setMenuItemColor("Vehicles/vehicles_inactive_color=white");
  setMenuItemColor("Vehicles/vehicles_inactive_color=blue");
  setMenuItemColor("Vehicles/vehicles_inactive_color=green");
  setMenuItemColor("Vehicles/vehicles_inactive_color=red");

  setMenuItemColor("Vehicles/trails_viewable=true");
  setMenuItemColor("Vehicles/trails_viewable=false");

  setMenuItemColor("Vehicles/trails_color=white");
  setMenuItemColor("Vehicles/trails_color=yellow");
  setMenuItemColor("Vehicles/trails_color=green");

  setMenuItemColor("Vehicles/trails_connect_viewable=true");
  setMenuItemColor("Vehicles/trails_connect_viewable=false");

  setMenuItemColor("Vehicles/bearing_lines_viewable=true");
  setMenuItemColor("Vehicles/bearing_lines_viewable=false");
}


//----------------------------------------------------- 
// Procedure: setRadioVehiAttrib
//   Purpose: A way of setting the vehicle attributes that assures that 
//            the GUI radio buttons are properly updated.
//  
//  Examples: "vehicles_viewable", true
//            "vehicles_active_color", red
//            "vehicles_inactive_color", yellow

bool MarineVehiGUI::setRadioVehiAttrib(string attr, string value) 
{
  if(!m_mviewer || !m_menubar)
    return(false);

  // If empty value, then no update attempted, just updated the menu
  // radio button settings if appropriate.
  if(value != "") {
    bool handled = m_mviewer->setParam(attr, value);
    if(!handled)
      return(false);
  }

  // Get latest setting of this attr (esp if this value is "toggle" etc)
  value = m_mviewer->vehisetting(attr);

  // Form for example, "Vehicle/vehicles_viewable=true"
  string item_str = "Vehicles/" + attr + "=" + value;
  const Fl_Menu_Item *item = m_menubar->find_item(item_str.c_str());
  if(item)
    ((Fl_Menu_Item *)item)->setonly();

  m_mviewer->redraw();
  return(true);
}

//--------------------------------------------------------------------
// procedure: cb_SetRadioVehiAttrib_i()

inline void MarineVehiGUI::cb_SetRadioVehiAttrib_i(int v) 
{
  if(v==10)      setRadioVehiAttrib("vehicles_viewable", "true");
  else if(v==11) setRadioVehiAttrib("vehicles_viewable", "false");
  else if(v==12) {
    setRadioVehiAttrib("vehicles_viewable", "toggle");
  }

  else if(v==20) setRadioVehiAttrib("vehicles_name_mode", "names");
  else if(v==21) setRadioVehiAttrib("vehicles_name_mode", "names+mode");
  else if(v==22) setRadioVehiAttrib("vehicles_name_mode", "names+shortmode");
  else if(v==23) setRadioVehiAttrib("vehicles_name_mode", "names+depth");
  else if(v==24) setRadioVehiAttrib("vehicles_name_mode", "names+auxmode");
  else if(v==25) setRadioVehiAttrib("vehicles_name_mode", "off");
  else if(v==29) setRadioVehiAttrib("vehicles_name_mode", "toggle");

  else if(v==40) setRadioVehiAttrib("vehicles_active_color", "red");
  else if(v==41) setRadioVehiAttrib("vehicles_active_color", "yellow");
  else if(v==42) setRadioVehiAttrib("vehicles_active_color", "white");
  else if(v==43) setRadioVehiAttrib("vehicles_active_color", "blue");
  else if(v==44) setRadioVehiAttrib("vehicles_active_color", "green");

  else if(v==50) setRadioVehiAttrib("vehicles_inactive_color", "yellow");
  else if(v==51) setRadioVehiAttrib("vehicles_inactive_color", "white");
  else if(v==52) setRadioVehiAttrib("vehicles_inactive_color", "blue");
  else if(v==53) setRadioVehiAttrib("vehicles_inactive_color", "green");
  else if(v==54) setRadioVehiAttrib("vehicles_inactive_color", "red");

  else if(v==60) setRadioVehiAttrib("trails_viewable", "true");
  else if(v==61) setRadioVehiAttrib("trails_viewable", "false");
  else if(v==62) setRadioVehiAttrib("trails_viewable", "toggle");

  else if(v==65) setRadioVehiAttrib("trails_color", "white");
  else if(v==66) setRadioVehiAttrib("trails_color", "yellow");
  else if(v==67) setRadioVehiAttrib("trails_color", "green");

  else if(v==70) setRadioVehiAttrib("trails_connect_viewable", "true");
  else if(v==71) setRadioVehiAttrib("trails_connect_viewable", "false");
  else if(v==72) setRadioVehiAttrib("trails_connect_viewable", "toggle");

  else if(v==80) setRadioVehiAttrib("bearing_lines_viewable", "true");
  else if(v==81) setRadioVehiAttrib("bearing_lines_viewable", "false");
  else if(v==82) setRadioVehiAttrib("bearing_lines_viewable", "toggle");
}

void MarineVehiGUI::cb_SetRadioVehiAttrib(Fl_Widget* o, int v) 
{
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_SetRadioVehiAttrib_i(v);
}

//----------------------------------------- CycleFocus
inline void MarineVehiGUI::cb_CycleFocus_i() {
  m_mviewer->setParam("cycle_active");
  m_mviewer->redraw();
  syncNodesAtoB();
  updateXY();
}

void MarineVehiGUI::cb_CycleFocus(Fl_Widget* o) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_CycleFocus_i();
}

//----------------------------------------- Alt Trail Size
inline void MarineVehiGUI::cb_AltTrailSize_i(int delta) {
  if(delta > 0)
    m_mviewer->setParam("trails_point_size", "bigger");
  else
    m_mviewer->setParam("trails_point_size", "smaller");
  m_mviewer->redraw();
}
void MarineVehiGUI::cb_AltTrailSize(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_AltTrailSize_i(v);
}

//----------------------------------------- ToggleTrailColor
inline void MarineVehiGUI::cb_ToggleTrailColor_i() {
  m_mviewer->setParam("trails_color", "toggle");
  m_mviewer->redraw();
}
void MarineVehiGUI::cb_ToggleTrailColor(Fl_Widget* o) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_ToggleTrailColor_i();
}

//----------------------------------------- Alt Trail Length
inline void MarineVehiGUI::cb_AltTrailLength_i(int delta) {
  if(delta < 0)
    m_mviewer->setParam("trails_length", "shorter");
  else
    m_mviewer->setParam("trails_length", "longer");
  m_mviewer->redraw();
}
void MarineVehiGUI::cb_AltTrailLength(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_AltTrailLength_i(v);
}

//----------------------------------------- Alt Shape Scale
inline void MarineVehiGUI::cb_AltShapeScale_i(int delta) {
  if(delta > 0)
    m_mviewer->setParam("vehicle_shape_scale", "bigger");
  else if(delta < 0)
    m_mviewer->setParam("vehicle_shape_scale", "smaller");
  else 
    m_mviewer->setParam("vehicle_shape_scale", "reset");
  m_mviewer->redraw();
}
void MarineVehiGUI::cb_AltShapeScale(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_AltShapeScale_i(v);
}

//----------------------------------------- CenterView
inline void MarineVehiGUI::cb_CenterView_i(int v) {
  if(v == 0)
    m_mviewer->setParam("center_view", "average");
  if(v == 1)
    m_mviewer->setParam("center_view", "active");

  m_mviewer->redraw();
  updateXY();
}

void MarineVehiGUI::cb_CenterView(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_CenterView_i(v);
}












