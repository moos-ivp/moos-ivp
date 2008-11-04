/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MarineVehiGUI.cpp                                    */
/*    DATE: November, 2004                                       */
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

#include "MarineVehiGUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

MarineVehiGUI::MarineVehiGUI(int g_w, int g_h, const char *g_l)
  : MarineGUI(g_w, g_h, g_l) 
{
  m_trail_color_ix = 0;
  m_vname_color_ix = 0;

  mbar->add("Vehicles/Vehicles-Toggle", FL_CTRL+'v', (Fl_Callback*)MarineVehiGUI::cb_ToggleVehicles, (void*)0, FL_MENU_DIVIDER);
  mbar->add("Vehicles/Cycle Focus", 'v', (Fl_Callback*)MarineVehiGUI::cb_CycleFocus,(void*)0, FL_MENU_DIVIDER);

  mbar->add("Vehicles/center_view = vehicle_average", 'c', (Fl_Callback*)MarineVehiGUI::cb_CenterView,  (void*)0, 0);
  mbar->add("Vehicles/center_view = vehicle_active",  'C', (Fl_Callback*)MarineVehiGUI::cb_CenterView,  (void*)1, FL_MENU_DIVIDER);

  mbar->add("Vehicles/VehicleSize/vehicle_shape_scale = larger",    '+', (Fl_Callback*)MarineVehiGUI::cb_AltShapeScale, (void*)+1, 0);
  mbar->add("Vehicles/VehicleSize/vehicle_shape_scale = smaller",   '-', (Fl_Callback*)MarineVehiGUI::cb_AltShapeScale, (void*)-1, 0);
  mbar->add("Vehicles/VehicleSize/vehicle_shape_scale = actual", FL_ALT+'v', (Fl_Callback*)MarineVehiGUI::cb_AltShapeScale, (void*)0, FL_MENU_DIVIDER);
  mbar->add("Vehicles/VehicleNames/vehicle_names_viewable = true", 0, (Fl_Callback*)MarineVehiGUI::cb_ToggleVName, 0, 0);
  mbar->add("Vehicles/VehicleNames/vehicle_names_viewable = false", 0, (Fl_Callback*)MarineVehiGUI::cb_ToggleVName, 0, 0);
  mbar->add("Vehicles/VehicleNames/vehicle_names_viewable = toggle", 'n', (Fl_Callback*)MarineVehiGUI::cb_ToggleVName, 0, FL_MENU_DIVIDER);
  mbar->add("Vehicles/VehicleNames/vehicle_names_color = toggle", FL_CTRL+'n', (Fl_Callback*)MarineVehiGUI::cb_ToggleVNameColor, 0, 0);

  mbar->add("Vehicles/Trails/trails_viewable = true",    0, (Fl_Callback*)MarineVehiGUI::cb_AltTrails, (void*)1, 0);
  mbar->add("Vehicles/Trails/trails_viewable = false",   0, (Fl_Callback*)MarineVehiGUI::cb_AltTrails, (void*)0, 0);
  mbar->add("Vehicles/Trails/trails_viewable = toggle",  't', (Fl_Callback*)MarineVehiGUI::cb_AltTrails, (void*)2, FL_MENU_DIVIDER);
  mbar->add("Vehicles/Trails/trails_connect_viewable = true", 0, (Fl_Callback*)MarineVehiGUI::cb_AltTrailConnect, (void*)1, 0);
  mbar->add("Vehicles/Trails/trails_connect_viewable = false", 0, (Fl_Callback*)MarineVehiGUI::cb_AltTrailConnect, (void*)0, 0);
  mbar->add("Vehicles/Trails/trails_connect_viewable = toggle", 'y', (Fl_Callback*)MarineVehiGUI::cb_AltTrailConnect, (void*)2, FL_MENU_DIVIDER);
  mbar->add("Vehicles/Trails/trails_color = toggle", FL_CTRL+'t', (Fl_Callback*)MarineVehiGUI::cb_ToggleTrailColor, 0, FL_MENU_DIVIDER);
  mbar->add("Vehicles/Trails/trails_point_size = larger", ']', (Fl_Callback*)MarineVehiGUI::cb_AltTrailSize, (void*)+1, 0);
  mbar->add("Vehicles/Trails/trails_point_size = smaller", '[', (Fl_Callback*)MarineVehiGUI::cb_AltTrailSize, (void*)-1, 0);
  mbar->add("Vehicles/Trails/trails_length = longer", ')', (Fl_Callback*)MarineVehiGUI::cb_AltTrailLength, (void*)+1, 0);
  mbar->add("Vehicles/Trails/trails_length = shorter", '(', (Fl_Callback*)MarineVehiGUI::cb_AltTrailLength, (void*)-1, 0);
  //mbar->add("Vehicles/Trails/trails_gap_size = larger",  '}', (Fl_Callback*)MarineVehiGUI::cb_AltTrailGap, (void*)1,  0);
  //mbar->add("Vehicles/Trails/trails_gap_size = smaller",  '{', (Fl_Callback*)MarineVehiGUI::cb_AltTrailGap, (void*)-1, FL_MENU_DIVIDER);

  mbar->add("Vehicles/ActiveColor/active_vcolor=red", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)700, 0);
  mbar->add("Vehicles/ActiveColor/active_vcolor=crimson", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)701, 0);
  mbar->add("Vehicles/ActiveColor/active_vcolor=white", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)702, 0);
  mbar->add("Vehicles/ActiveColor/active_vcolor=blue_violet", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)703, 0);
  mbar->add("Vehicles/ActiveColor/active_vcolor=dark_olive_green", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)704, 0);
  mbar->add("Vehicles/ActiveColor/active_vcolor=orange_red", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)705, FL_MENU_DIVIDER);

  mbar->add("Vehicles/InactiveColor/inactive_vcolor=yellow", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)800, 0);
  mbar->add("Vehicles/InactiveColor/inactive_vcolor=dark_sea_green", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)801, 0);
  mbar->add("Vehicles/InactiveColor/inactive_vcolor=khaki", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)802, 0);
  mbar->add("Vehicles/InactiveColor/inactive_vcolor=gold", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)803, 0);
  mbar->add("Vehicles/InactiveColor/inactive_vcolor=goldenrod", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)804, 0);
  mbar->add("Vehicles/InactiveColor/inactive_vcolor=green_yellow", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)805, 0);
}

//----------------------------------------- ToggleVehicles
inline void MarineVehiGUI::cb_ToggleVehicles_i() {
  cmviewer->setParam("vehicles_viewable", "toggle");
  cmviewer->redraw();
}
void MarineVehiGUI::cb_ToggleVehicles(Fl_Widget* o) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_ToggleVehicles_i();
}

//----------------------------------------- CycleFocus
inline void MarineVehiGUI::cb_CycleFocus_i() {
  cmviewer->setParam("cycle_active");
  cmviewer->redraw();
  updateXY();
}

void MarineVehiGUI::cb_CycleFocus(Fl_Widget* o) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_CycleFocus_i();
}
//----------------------------------------- AltTrails
inline void MarineVehiGUI::cb_AltTrails_i(int v) {
  if(v==1)
    cmviewer->setParam("trails_viewable", "true");
  else if(v==0)
    cmviewer->setParam("trails_viewable", "false");
  else
    cmviewer->setParam("trails_viewable", "toggle");
  cmviewer->redraw();
}
void MarineVehiGUI::cb_AltTrails(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_AltTrails_i(v);
}

//----------------------------------------- AltTrailConnect
inline void MarineVehiGUI::cb_AltTrailConnect_i(int v) {
  if(v==1)
    cmviewer->setParam("trails_connect_viewable", "true");
  else if(v==0)
    cmviewer->setParam("trails_connect_viewable", "false");
  else
    cmviewer->setParam("trails_connect_viewable", "toggle");
  cmviewer->redraw();
}
void MarineVehiGUI::cb_AltTrailConnect(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_AltTrailConnect_i(v);
}

//----------------------------------------- ToggleVName
inline void MarineVehiGUI::cb_ToggleVName_i() {
  cmviewer->setParam("vehicle_names_viewable", "toggle");
  cmviewer->redraw();
}
void MarineVehiGUI::cb_ToggleVName(Fl_Widget* o) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_ToggleVName_i();
}

//----------------------------------------- ToggleVNameColor
inline void MarineVehiGUI::cb_ToggleVNameColor_i() {
  m_vname_color_ix = (m_vname_color_ix+1) % 6;
  if(m_vname_color_ix == 0)
    cmviewer->setParam("vehicle_names_color", "blue");
  else if(m_vname_color_ix == 1)
    cmviewer->setParam("vehicle_names_color", "darkblue");
  else if(m_vname_color_ix == 2)
    cmviewer->setParam("vehicle_names_color", "yellow");
  else if(m_vname_color_ix == 3)
    cmviewer->setParam("vehicle_names_color", "red");
  else if(m_vname_color_ix == 4)
    cmviewer->setParam("vehicle_names_color", "green");
  else
    cmviewer->setParam("vehicle_names_color", "white");
  cmviewer->redraw();
}
void MarineVehiGUI::cb_ToggleVNameColor(Fl_Widget* o) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_ToggleVNameColor_i();
}

//----------------------------------------- ToggleTrailColor
inline void MarineVehiGUI::cb_ToggleTrailColor_i() {
  m_trail_color_ix = (m_trail_color_ix+1) % 3;
  if(m_trail_color_ix == 0)
    cmviewer->setParam("trails_color", "white");
  else if(m_trail_color_ix == 1)
    cmviewer->setParam("trails_color", "blue");
  else
    cmviewer->setParam("trails_color", "green");
  cmviewer->redraw();
}
void MarineVehiGUI::cb_ToggleTrailColor(Fl_Widget* o) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_ToggleTrailColor_i();
}

//----------------------------------------- Alt Trail Length
inline void MarineVehiGUI::cb_AltTrailLength_i(int delta) {
  if(delta < 0)
    cmviewer->setParam("trails_length", "shorter");
  else
    cmviewer->setParam("trails_length", "longer");
  cmviewer->redraw();
}
void MarineVehiGUI::cb_AltTrailLength(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_AltTrailLength_i(v);
}

//----------------------------------------- Alt Trail Gap
inline void MarineVehiGUI::cb_AltTrailGap_i(int delta) {
  cmviewer->setParam("trails_gap_size", (delta));
  cmviewer->redraw();
}
void MarineVehiGUI::cb_AltTrailGap(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_AltTrailGap_i(v);
}

//----------------------------------------- MVG_SetGeoAttr
inline void MarineVehiGUI::cb_MVG_SetGeoAttr_i(int v) {
  if(v==700) cmviewer->setParam("active_vcolor", "red");
  else if(v==701) cmviewer->setParam("active_vcolor", "crimson");
  else if(v==702) cmviewer->setParam("active_vcolor", "white");
  else if(v==703) cmviewer->setParam("active_vcolor", "blue_violet");
  else if(v==704) cmviewer->setParam("active_vcolor", "dark_olive_green");
  else if(v==705) cmviewer->setParam("active_vcolor", "orange_red");

  else if(v==800) cmviewer->setParam("inactive_vcolor", "1.0,0.9,0.243");
  else if(v==801) cmviewer->setParam("inactive_vcolor", "dark_sea_green");
  else if(v==802) cmviewer->setParam("inactive_vcolor", "khaki");
  else if(v==803) cmviewer->setParam("inactive_vcolor", "gold");
  else if(v==804) cmviewer->setParam("inactive_vcolor", "goldenrod");
  else if(v==805) cmviewer->setParam("inactive_vcolor", "green_yellow");

  else 
    return;

  cmviewer->redraw();
}
void MarineVehiGUI::cb_MVG_SetGeoAttr(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_MVG_SetGeoAttr_i(v);
}

//----------------------------------------- Alt Trail Size
inline void MarineVehiGUI::cb_AltTrailSize_i(int delta) {
  if(delta > 0)
    cmviewer->setParam("trails_point_size", "bigger");
  else
    cmviewer->setParam("trails_point_size", "smaller");
  cmviewer->redraw();
}
void MarineVehiGUI::cb_AltTrailSize(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_AltTrailSize_i(v);
}

//----------------------------------------- Alt Shape Scale
inline void MarineVehiGUI::cb_AltShapeScale_i(int delta) {
  if(delta > 0)
    cmviewer->setParam("vehicle_shape_scale", "bigger");
  if(delta < 0)
    cmviewer->setParam("vehicle_shape_scale", "smaller");
  cmviewer->redraw();
}
void MarineVehiGUI::cb_AltShapeScale(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_AltShapeScale_i(v);
}

//----------------------------------------- CenterView
inline void MarineVehiGUI::cb_CenterView_i(int v) {
  if(v == 0)
    cmviewer->setParam("center_view", "vehicle_average");
  if(v == 1)
    cmviewer->setParam("center_view", "vehicle_active");

  cmviewer->redraw();
  updateXY();
}

void MarineVehiGUI::cb_CenterView(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_CenterView_i(v);
}


