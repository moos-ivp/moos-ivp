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

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "MarineVehiGUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

MarineVehiGUI::MarineVehiGUI(int g_w, int g_h, const char *g_l)
  : MarineGUI(g_w, g_h, g_l) 
{
  mbar->add("ForeView/Focus Vehicle 0", '0', (Fl_Callback*)MarineVehiGUI::cb_DataView, (void*)0, 0);
  mbar->add("ForeView/Focus Vehicle 1", '1', (Fl_Callback*)MarineVehiGUI::cb_DataView, (void*)1, 0);
  mbar->add("ForeView/Focus Vehicle 2", '2', (Fl_Callback*)MarineVehiGUI::cb_DataView, (void*)2, 0);
  mbar->add("ForeView/Focus Vehicle 3", '3', (Fl_Callback*)MarineVehiGUI::cb_DataView, (void*)3, 0);
  mbar->add("ForeView/Focus Vehicle 4", '4', (Fl_Callback*)MarineVehiGUI::cb_DataView, (void*)4, 0);
  mbar->add("ForeView/Focus Vehicle 5", '5', (Fl_Callback*)MarineVehiGUI::cb_DataView, (void*)5, 0);
  mbar->add("ForeView/Focus Vehicle 6", '6', (Fl_Callback*)MarineVehiGUI::cb_DataView, (void*)6, 0);
  mbar->add("ForeView/Focus Vehicle 7", '7', (Fl_Callback*)MarineVehiGUI::cb_DataView, (void*)7, 0);
  mbar->add("ForeView/Focus Vehicle 8", '8', (Fl_Callback*)MarineVehiGUI::cb_DataView, (void*)8, 0);
  mbar->add("ForeView/Focus Vehicle 9", '9', (Fl_Callback*)MarineVehiGUI::cb_DataView, (void*)9, FL_MENU_DIVIDER);
  mbar->add("ForeView/Shape Larger",    '+', (Fl_Callback*)MarineVehiGUI::cb_AltShapeScale, (void*)+1, 0);
  mbar->add("ForeView/Shape Smaller",   '-', (Fl_Callback*)MarineVehiGUI::cb_AltShapeScale, (void*)-1, FL_MENU_DIVIDER);
  mbar->add("ForeView/Toggle Trails",   't', (Fl_Callback*)MarineVehiGUI::cb_ToggleTrails, 0, 134);
  mbar->add("ForeView/Toggle TrailColor", FL_CTRL+'t', (Fl_Callback*)MarineVehiGUI::cb_ToggleTrailColor, 0, 0);
  mbar->add("ForeView/Toggle TrailConnect", 'y', (Fl_Callback*)MarineVehiGUI::cb_ToggleTrailConnect, 0, 0);
  mbar->add("ForeView/Toggle Names", 'n', (Fl_Callback*)MarineVehiGUI::cb_ToggleVName, 0, 0);
  mbar->add("ForeView/Toggle NameColor", FL_CTRL+'n', (Fl_Callback*)MarineVehiGUI::cb_ToggleVNameColor, 0, 0);
  mbar->add("ForeView/More Trail Gap",  '}', (Fl_Callback*)MarineVehiGUI::cb_AltTrailGap, (void*)1,  0);
  mbar->add("ForeView/Less Trail Gap",  '{', (Fl_Callback*)MarineVehiGUI::cb_AltTrailGap, (void*)-1, 0);
  mbar->add("ForeView/More Trail Size", ']', (Fl_Callback*)MarineVehiGUI::cb_AltTrailSize, (void*)+1, 0);
  mbar->add("ForeView/Less Trail Size", '[', (Fl_Callback*)MarineVehiGUI::cb_AltTrailSize, (void*)-1, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/Vehicles/active_vcolor=red", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)700, 0);
  mbar->add("GeoAttr/Vehicles/active_vcolor=crimson", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)701, 0);
  mbar->add("GeoAttr/Vehicles/active_vcolor=white", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)702, 0);
  mbar->add("GeoAttr/Vehicles/active_vcolor=blue_violet", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)703, 0);
  mbar->add("GeoAttr/Vehicles/active_vcolor=dark_olive_green", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)704, 0);
  mbar->add("GeoAttr/Vehicles/active_vcolor=orange_red", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)705, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/Vehicles/inactive_vcolor=yellow", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)800, 0);
  mbar->add("GeoAttr/Vehicles/inactive_vcolor=dark_sea_green", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)801, 0);
  mbar->add("GeoAttr/Vehicles/inactive_vcolor=khaki", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)802, 0);
  mbar->add("GeoAttr/Vehicles/inactive_vcolor=gold", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)803, 0);
  mbar->add("GeoAttr/Vehicles/inactive_vcolor=goldenrod", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)804, 0);
  mbar->add("GeoAttr/Vehicles/inactive_vcolor=green_yellow", 0, (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)805, 0);

  mbar->add("GeoAttr/Vehicles - Toggle", FL_CTRL+'v', (Fl_Callback*)MarineVehiGUI::cb_MVG_SetGeoAttr, (void*)0, FL_MENU_DIVIDER);
}

//----------------------------------------- ToggleTrails
inline void MarineVehiGUI::cb_ToggleTrails_i() {
  cmviewer->setParam("trail_view", "toggle");
}
void MarineVehiGUI::cb_ToggleTrails(Fl_Widget* o) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_ToggleTrails_i();
}

//----------------------------------------- ToggleTrailConnect
inline void MarineVehiGUI::cb_ToggleTrailConnect_i() {
  cmviewer->setParam("trail_connect", "toggle");
}
void MarineVehiGUI::cb_ToggleTrailConnect(Fl_Widget* o) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_ToggleTrailConnect_i();
}

//----------------------------------------- ToggleVName
inline void MarineVehiGUI::cb_ToggleVName_i() {
  cmviewer->setParam("display_vname", "toggle");
}
void MarineVehiGUI::cb_ToggleVName(Fl_Widget* o) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_ToggleVName_i();
}

//----------------------------------------- ToggleVNameColor
inline void MarineVehiGUI::cb_ToggleVNameColor_i() {
  cmviewer->setParam("vname_color", "toggle");
}
void MarineVehiGUI::cb_ToggleVNameColor(Fl_Widget* o) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_ToggleVNameColor_i();
}

//----------------------------------------- ToggleTrailColor
inline void MarineVehiGUI::cb_ToggleTrailColor_i() {
  cmviewer->setParam("trail_color", "toggle");
}
void MarineVehiGUI::cb_ToggleTrailColor(Fl_Widget* o) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_ToggleTrailColor_i();
}

//----------------------------------------- Alt Trail Gap
inline void MarineVehiGUI::cb_AltTrailGap_i(int delta) {
  cmviewer->setParam("trail_gap", (delta));
}
void MarineVehiGUI::cb_AltTrailGap(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_AltTrailGap_i(v);
}

//----------------------------------------- MVG_SetGeoAttr
inline void MarineVehiGUI::cb_MVG_SetGeoAttr_i(int v) {
  if(v==700) cmviewer->setCommonParam("active_vcolor", "red");
  else if(v==701) cmviewer->setCommonParam("active_vcolor", "crimson");
  else if(v==702) cmviewer->setCommonParam("active_vcolor", "white");
  else if(v==703) cmviewer->setCommonParam("active_vcolor", "blue_violet");
  else if(v==704) cmviewer->setCommonParam("active_vcolor", "dark_olive_green");
  else if(v==705) cmviewer->setCommonParam("active_vcolor", "orange_red");

  else if(v==800) cmviewer->setCommonParam("inactive_vcolor", "1.0,0.9,0.243");
  else if(v==801) cmviewer->setCommonParam("inactive_vcolor", "dark_sea_green");
  else if(v==802) cmviewer->setCommonParam("inactive_vcolor", "khaki");
  else if(v==803) cmviewer->setCommonParam("inactive_vcolor", "gold");
  else if(v==804) cmviewer->setCommonParam("inactive_vcolor", "goldenrod");
  else if(v==805) cmviewer->setCommonParam("inactive_vcolor", "green_yellow");

  else 
    return;

  cmviewer->redraw();
}
void MarineVehiGUI::cb_MVG_SetGeoAttr(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_MVG_SetGeoAttr_i(v);
}

//----------------------------------------- Alt Trail Size
inline void MarineVehiGUI::cb_AltTrailSize_i(int delta) {
  cmviewer->setParam("trail_size", (float)(delta/20.0));
}
void MarineVehiGUI::cb_AltTrailSize(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_AltTrailSize_i(v);
}

//----------------------------------------- Alt Shape Scale
inline void MarineVehiGUI::cb_AltShapeScale_i(int delta) {
  if(delta > 0)
    cmviewer->setParam("shape_scale", 1.25);
  if(delta < 0)
    cmviewer->setParam("shape_scale", 0.80);
}
void MarineVehiGUI::cb_AltShapeScale(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_AltShapeScale_i(v);
}

//----------------------------------------- DataView
inline void MarineVehiGUI::cb_DataView_i(int val) {
  cmviewer->setGlobalIndex(val);
  cmviewer->redraw();
  this->updateXY();
}
void MarineVehiGUI::cb_DataView(Fl_Widget* o, int v) {
  ((MarineVehiGUI*)(o->parent()->user_data()))->cb_DataView_i(v);
}


