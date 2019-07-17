/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MarineGUI.cpp                                        */
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
#include <cstdlib>
#include "MarineGUI.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

MarineGUI::MarineGUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) 
{
  m_menubar = new Fl_Menu_Bar(0, 0, w(), 25);
  m_menubar->menu(menu_);
    
  m_mviewer = 0;
}

//-------------------------------------------------------------------

Fl_Menu_Item MarineGUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)MarineGUI::cb_Quit, 0, 0},
 {0},

 {"BackView", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Zoom In",          'i', (Fl_Callback*)MarineGUI::cb_Zoom, (void*)-1, 0},
 {"Zoom Out",         'o', (Fl_Callback*)MarineGUI::cb_Zoom, (void*)1, 0},
 {"Zoom Reset",       FL_CTRL+'z', (Fl_Callback*)MarineGUI::cb_Zoom, (void*)0, FL_MENU_DIVIDER},
 {"Pan Up ",          FL_Up,  (Fl_Callback*)MarineGUI::cb_HandleUpDown, (void*)-200, 0},
 {"Pan Down ",        FL_Down,  (Fl_Callback*)MarineGUI::cb_HandleUpDown, (void*)200, 0},
 {"Pan Left ",        FL_Left,  (Fl_Callback*)MarineGUI::cb_HandleLeftRight, (void*)200, 0},
 {"Pan Right ",       FL_Right,  (Fl_Callback*)MarineGUI::cb_HandleLeftRight, (void*)-200, FL_MENU_DIVIDER},
 {"Pan Up (slow) ",   FL_ALT + FL_Up, (Fl_Callback*)MarineGUI::cb_HandleUpDown, (void*)-10, 0},
 {"Pan Down (slow) ", FL_ALT + FL_Down, (Fl_Callback*)MarineGUI::cb_HandleUpDown, (void*)10, 0},
 {"Pan Left (slow) ", FL_ALT + FL_Left, (Fl_Callback*)MarineGUI::cb_HandleLeftRight, (void*)10, 0},
 {"Pan Right (slow)", FL_ALT + FL_Right, (Fl_Callback*)MarineGUI::cb_HandleLeftRight, (void*)-10, FL_MENU_DIVIDER},
 {"Pan Up (v. slow) ",   FL_CTRL + FL_Up, (Fl_Callback*)MarineGUI::cb_HandleUpDown, (void*)-1, 0},
 {"Pan Down (v. slow) ", FL_CTRL + FL_Down, (Fl_Callback*)MarineGUI::cb_HandleUpDown, (void*)1, 0},
 {"Pan Left (v. slow) ", FL_CTRL + FL_Left, (Fl_Callback*)MarineGUI::cb_HandleLeftRight, (void*)1, 0},
 {"Pan Right (v. slow)", FL_CTRL + FL_Right, (Fl_Callback*)MarineGUI::cb_HandleLeftRight, (void*)-1, FL_MENU_DIVIDER},

 {"tiff_viewable=true",    0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)180, FL_MENU_RADIO | FL_MENU_VALUE},
 {"tiff_viewable=false",   0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)181, FL_MENU_RADIO},
 {"    Toggle Tiff View",  'b', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)182, FL_MENU_DIVIDER},

 {"tiff_type toggle",  '`', (Fl_Callback*)MarineGUI::cb_ToggleTiffType, (void*)-1, FL_MENU_DIVIDER},
 {"back_shade lighter", FL_CTRL+'b', (Fl_Callback*)MarineGUI::cb_BackShade,  (void*)+1, 0},
 {"back_shade darker",  FL_ALT +'b', (Fl_Callback*)MarineGUI::cb_BackShade,  (void*)-1, FL_MENU_DIVIDER},

 {"hash_viewable=true",     0, (Fl_Callback*)MarineGUI::cb_ToggleHash, (void*)-1, FL_MENU_RADIO},
 {"hash_viewable=false",    0, (Fl_Callback*)MarineGUI::cb_ToggleHash, (void*)-1, FL_MENU_RADIO|FL_MENU_VALUE},
 {"    Toggle Hash Lines", 'h', (Fl_Callback*)MarineGUI::cb_ToggleHash, (void*)-1, FL_MENU_DIVIDER},

 {"hash_shade lighter",     FL_CTRL+'h', (Fl_Callback*)MarineGUI::cb_HashShade,  (void*)+1, 0},
 {"hash_shade darker",      FL_ALT +'h', (Fl_Callback*)MarineGUI::cb_HashShade,  (void*)-1, FL_MENU_DIVIDER},

 {"hash_delta=10",   FL_ALT+'1', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)10,  FL_MENU_RADIO},
 {"hash_delta=50",   FL_ALT+'2', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)50,  FL_MENU_RADIO},
 {"hash_delta=100",  FL_ALT+'3', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)100, FL_MENU_RADIO},
 {"hash_delta=200",  FL_ALT+'4', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)200, FL_MENU_RADIO},
 {"hash_delta=500",  FL_ALT+'5', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)500, FL_MENU_RADIO},
 {"hash_delta=1000", FL_ALT+'6', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)1000, FL_MENU_RADIO|FL_MENU_DIVIDER},
 {0},

 {0}
};

//-------------------------------------------------------------------
// Procedure: addGeoAttrMenu

void MarineGUI::addGeoAttrMenu() 
{
  m_menubar->add("GeoAttr/Polygons/polygon_viewable_all=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)150, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/Polygons/polygon_viewable_all=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)151, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Polygons/    Toggle Polygons",      'p', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)152, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/Polygons/polygon_viewable_labels=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)160, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/Polygons/polygon_viewable_labels=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)161, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Polygons/    Toggle Polygon Labels",   'P', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)162, FL_MENU_DIVIDER);

  // --------------------------------- SegLists
  m_menubar->add("GeoAttr/SegLists/seglist_viewable_all=true",   0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)250, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/SegLists/seglist_viewable_all=false",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)251, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/SegLists/    Toggle SegLists",       's', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)252, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/SegLists/seglist_viewable_labels=true",   0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)260, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/SegLists/seglist_viewable_labels=false",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)261, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/SegLists/    Toggle SegList Labels",    'S', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)262, FL_MENU_DIVIDER);

  // --------------------------------- Seglrs
  m_menubar->add("GeoAttr/Seglrs/seglr_viewable_all=true",   0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)270, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/Seglrs/seglr_viewable_all=false",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)271, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Seglrs/    Toggle Seglrs",       '%', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)272, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/Seglrs/seglr_viewable_labels=true",   0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)280, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/Seglrs/seglr_viewable_labels=false",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)281, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Seglrs/    Toggle Seglr Labels", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)282, FL_MENU_DIVIDER);

  // --------------------------------- Points
  m_menubar->add("GeoAttr/Points/point_viewable_all=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)550, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/Points/point_viewable_all=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)551, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Points/    Toggle Points",      'j', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)552, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/Points/point_viewable_labels=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)560, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/Points/point_viewable_labels=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)561, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Points/    Toggle Point Labels",   'J', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)562, FL_MENU_DIVIDER);

  // --------------------------------- Vectors
  m_menubar->add("GeoAttr/Vectors/vector_viewable_all=true",    0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)750,  FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/Vectors/vector_viewable_all=false",   0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)751, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Vectors/    Toggle Vectors", FL_ALT+'v', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)752, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/Vectors/vector_viewable_labels=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)760,  FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/Vectors/vector_viewable_labels=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)761, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Vectors/    Toggle Vector Labels",   'V', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)762, FL_MENU_DIVIDER);

  // --------------------------------- Circles
  m_menubar->add("GeoAttr/Circles/circle_viewable_all=true",   0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)850, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/Circles/circle_viewable_all=false",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)851, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Circles/    Toggle Circles", FL_ALT+'c', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)852, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/Circles/circle_viewable_labels=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)860, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/Circles/circle_viewable_labels=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)861, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Circles/    Toggle Circle Labels",   'C', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)862, FL_MENU_DIVIDER);

  // -------------------------------- Grids
  m_menubar->add("GeoAttr/Grids/grid_viewable_all=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)300, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/Grids/grid_viewable_all=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)301, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Grids/    Toggle Grids",      'g', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)302, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/Grids/grid_viewable_labels=true" , 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)310, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/Grids/grid_viewable_labels=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)311, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Grids/    Toggle Grid Labels",   'G', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)312, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/Grids/grid_opaqueness=0.1", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)351, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/Grids/grid_opaqueness=0.2", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)352, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Grids/grid_opaqueness=0.3", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)353, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Grids/grid_opaqueness=0.4", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)354, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Grids/grid_opaqueness=0.5", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)355, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Grids/grid_opaqueness=0.6", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)356, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Grids/grid_opaqueness=0.7", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)357, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Grids/grid_opaqueness=0.8", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)358, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Grids/grid_opaqueness=0.9", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)359, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Grids/grid_opaqueness=1.0", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)360, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/Grids/    More Opaque", FL_CTRL+'g', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)330, 0);
  m_menubar->add("GeoAttr/Grids/    Less Opaque", FL_ALT+'g',  (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)331, 0);

  // --------------------------------- Markers
  m_menubar->add("GeoAttr/Markers/marker_viewable_all=true",  0,  (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)900, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Markers/marker_viewable_all=false", 0,  (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)901, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Markers/    Markers Toggle",       'm', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)902, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/Markers/marker_viewable_labels=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)910, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Markers/marker_viewable_labels=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)911, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Markers/    Markers Labels Toggle",   'M', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)912, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/Markers/marker_scale=1", FL_CTRL+FL_ALT+'m', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)920, 0);
  m_menubar->add("GeoAttr/Markers/marker_scale *= 0.80", FL_CTRL+'m', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)921, 0);
  m_menubar->add("GeoAttr/Markers/marker_scale *= 1.25", FL_ALT+'m', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)922, FL_MENU_DIVIDER);

  // --------------------------------- RangePulses
  m_menubar->add("GeoAttr/RangePulses/range_pulse_viewable_all=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)4150, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/RangePulses/range_pulse_viewable_all=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)4151, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/RangePulses/    Toggle Range Pulses",  '#', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)4152, FL_MENU_DIVIDER);

  // --------------------------------- CommsPulses
  m_menubar->add("GeoAttr/CommsPulses/comms_pulse_viewable_all=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)4160, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/CommsPulses/comms_pulse_viewable_all=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)4161, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/CommsPulses/    Toggle Comms Pulses",  '@', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)4162, FL_MENU_DIVIDER);

  // --------------------------------- NodePulses
  m_menubar->add("GeoAttr/NodePulses/node_pulse_viewable_all=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)4170, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/NodePulses/node_pulse_viewable_all=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)4171, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/NodePulses/    Toggle Node Pulses",  '$', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)4172, FL_MENU_DIVIDER);

  // --------------------------------- Datum
  m_menubar->add("GeoAttr/Datum/datum_viewable=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)400, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("GeoAttr/Datum/datum_viewable=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)401, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Datum/    Toggle Datum",   'd', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)402, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/Datum/datum_color=red",        0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)410, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Datum/datum_color=white",      0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)411, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Datum/datum_color=yellow",     0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)412, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Datum/datum_color=dark_blue",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)413, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Datum/datum_color=dark_green", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)414, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/Datum/datum_size=1",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)440, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Datum/datum_size=2",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)441, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Datum/datum_size=3",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)442, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Datum/datum_size=4",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)443, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Datum/datum_size=5",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)444, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Datum/datum_size=6",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)445, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Datum/datum_size=7",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)446, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Datum/datum_size=8",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)447, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Datum/datum_size=9",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)448, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/Datum/datum_size=10", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)449, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/Datum/    Datum Bigger",  FL_CTRL+'d', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)450, 0);
  m_menubar->add("GeoAttr/Datum/    Datum Smaller", FL_ALT+'d', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)451, 0);

  // --------------------------------- Op Area
  m_menubar->add("GeoAttr/OpArea/oparea_viewable_all=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)950, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/OpArea/oparea_viewable_all=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)951, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/OpArea/    Toggle OpArea",       'u', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)952, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/OpArea/oparea_viewable_labels=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)960, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/OpArea/oparea_viewable_labels=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)961, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/OpArea/    Toggle OpArea Labels",   'U', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)962, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/OpArea/op_area reset", FL_CTRL+FL_ALT+'u', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)970, 0);
  m_menubar->add("GeoAttr/OpArea/op_area lighter *= 0.80", FL_CTRL+'u', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)971, 0);
  m_menubar->add("GeoAttr/OpArea/op_area darker *= 1.25", FL_ALT+'u', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)972, FL_MENU_DIVIDER);

  // --------------------------------- DropPoints
  m_menubar->add("GeoAttr/DropPoints/drop_point_viewable_all=true",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3131, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/DropPoints/drop_point_viewable_all=false", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3132, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/DropPoints/    Toggle DropPoints",       'r', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3133, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/DropPoints/clear all", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3100, 0);
  m_menubar->add("GeoAttr/DropPoints/clear last", FL_CTRL+'r', (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3101, FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/DropPoints/drop_point_coords=as-dropped", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3110, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/DropPoints/drop_point_coords=lat-lon",    0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3111, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/DropPoints/drop_point_coords=local-grid", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3112, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("GeoAttr/DropPoints/drop_point_vertex_size=1",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3121, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/DropPoints/drop_point_vertex_size=2",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3122, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/DropPoints/drop_point_vertex_size=3",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3123, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/DropPoints/drop_point_vertex_size=4",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3124, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/DropPoints/drop_point_vertex_size=5",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3125, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/DropPoints/drop_point_vertex_size=6",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3126, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/DropPoints/drop_point_vertex_size=7",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3127, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/DropPoints/drop_point_vertex_size=8",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3128, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/DropPoints/drop_point_vertex_size=9",  0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3129, FL_MENU_RADIO);
  m_menubar->add("GeoAttr/DropPoints/drop_point_vertex_size=10", 0, (Fl_Callback*)MarineGUI::cb_SetGeoAttr, (void*)3130, FL_MENU_RADIO|FL_MENU_RADIO);
}

//----------------------------------------------------------
// Procedure: handle
//      Note: As it stands, this method could be eliminated entirely, and the 
//            default behavior of the parent class should work fine. But if
//            we want to tinker with event handling, this method is the place.

int MarineGUI::handle(int event) 
{
  switch(event) {
  case FL_KEYDOWN:
    // Handle the Zoom Keys
    if(Fl::event_key() == 32)
      cout << "Handling SPACEBAR!!!!!" << endl;
    return(1);
    break;
  default:
    return(Fl_Window::handle(event));
  }
}


//----------------------------------------------------- 
// Procedure: setMenuItemColor

void MarineGUI::setMenuItemColor(string item_str, int r, int g, int b)
{
  Fl_Color new_color = fl_rgb_color(r, g, b);

  const Fl_Menu_Item *item = m_menubar->find_item(item_str.c_str());
  if(item)
    ((Fl_Menu_Item *)item)->labelcolor(new_color);

}

//----------------------------------------------------- 
// Procedure: updateRadios

void MarineGUI::updateRadios()
{
  setMenuAttrib("GeoAttr/Polygons",    "polygon_viewable_all");
  setMenuAttrib("GeoAttr/Polygons",    "polygon_viewable_labels");
  setMenuAttrib("GeoAttr/SegLists",    "seglist_viewable_all");
  setMenuAttrib("GeoAttr/SegLists",    "seglist_viewable_labels");
  setMenuAttrib("GeoAttr/Grids",       "grid_viewable_all");
  setMenuAttrib("GeoAttr/Grids",       "grid_viewable_labels");
  setMenuAttrib("GeoAttr/Grids",       "grid_opaqueness");
  setMenuAttrib("GeoAttr/Points",      "point_viewable_all");
  setMenuAttrib("GeoAttr/Points",      "point_viewable_labels");
  setMenuAttrib("GeoAttr/Vectors",     "vector_viewable_all");
  setMenuAttrib("GeoAttr/Vectors",     "vector_viewable_labels");
  setMenuAttrib("GeoAttr/Circles",     "circle_viewable_all");
  setMenuAttrib("GeoAttr/Circles",     "circle_viewable_labels");
  setMenuAttrib("GeoAttr/Markers",     "marker_viewable_all");
  setMenuAttrib("GeoAttr/Markers",     "marker_viewable_labels");
  setMenuAttrib("GeoAttr/OpArea",      "oparea_viewable_all");
  setMenuAttrib("GeoAttr/OpArea",      "oparea_viewable_labels");
  setMenuAttrib("GeoAttr/Datum",       "datum_viewable");
  setMenuAttrib("GeoAttr/Datum",       "datum_color");
  setMenuAttrib("GeoAttr/Datum",       "datum_size");
  setMenuAttrib("GeoAttr/DropPoints",  "drop_point_vertex_size");
  setMenuAttrib("GeoAttr/DropPoints",  "drop_point_viewable_all");
  setMenuAttrib("GeoAttr/DropPoints",  "drop_point_coords");
  setMenuAttrib("GeoAttr/CommsPulses", "comms_pulse_viewable_all");
  setMenuAttrib("GeoAttr/NodePulses",  "node_pulse_viewable_all");
  setMenuAttrib("GeoAttr/RangePulses", "range_pulse_viewable_all");

  setMenuAttrib("BackView", "tiff_viewable");
  setMenuAttrib("BackView", "hash_viewable");
  setMenuAttrib("BackView", "hash_delta");
}

//----------------------------------------------------- 
// Procedure: setMenuItemColors

void MarineGUI::setMenuItemColors()
{
  setMenuItemColor("GeoAttr/Polygons/polygon_viewable_all=true");
  setMenuItemColor("GeoAttr/Polygons/polygon_viewable_all=false");
  setMenuItemColor("GeoAttr/Polygons/polygon_viewable_all=true");
  setMenuItemColor("GeoAttr/Polygons/polygon_viewable_all=false");
  setMenuItemColor("GeoAttr/Polygons/polygon_viewable_labels=true");
  setMenuItemColor("GeoAttr/Polygons/polygon_viewable_labels=false");
  setMenuItemColor("GeoAttr/SegLists/seglist_viewable_all=true");
  setMenuItemColor("GeoAttr/SegLists/seglist_viewable_all=false");
  setMenuItemColor("GeoAttr/SegLists/seglist_viewable_labels=true");
  setMenuItemColor("GeoAttr/SegLists/seglist_viewable_labels=false");
  setMenuItemColor("GeoAttr/Grids/grid_viewable_all=true");
  setMenuItemColor("GeoAttr/Grids/grid_viewable_all=false");
  setMenuItemColor("GeoAttr/Grids/grid_viewable_labels=true");
  setMenuItemColor("GeoAttr/Grids/grid_viewable_labels=false");
  setMenuItemColor("GeoAttr/Grids/grid_opaqueness=0.1");
  setMenuItemColor("GeoAttr/Grids/grid_opaqueness=0.2");
  setMenuItemColor("GeoAttr/Grids/grid_opaqueness=0.3");
  setMenuItemColor("GeoAttr/Grids/grid_opaqueness=0.4");
  setMenuItemColor("GeoAttr/Grids/grid_opaqueness=0.5");
  setMenuItemColor("GeoAttr/Grids/grid_opaqueness=0.6");
  setMenuItemColor("GeoAttr/Grids/grid_opaqueness=0.7");
  setMenuItemColor("GeoAttr/Grids/grid_opaqueness=0.8");
  setMenuItemColor("GeoAttr/Grids/grid_opaqueness=0.9");
  setMenuItemColor("GeoAttr/Grids/grid_opaqueness=1.0");
  setMenuItemColor("GeoAttr/Points/point_viewable_all=true");
  setMenuItemColor("GeoAttr/Points/point_viewable_all=false");
  setMenuItemColor("GeoAttr/Points/point_viewable_labels=true");
  setMenuItemColor("GeoAttr/Points/point_viewable_labels=false");
  setMenuItemColor("GeoAttr/Vectors/vector_viewable_all=true");
  setMenuItemColor("GeoAttr/Vectors/vector_viewable_all=false");
  setMenuItemColor("GeoAttr/Vectors/vector_viewable_labels=true");
  setMenuItemColor("GeoAttr/Vectors/vector_viewable_labels=false");
  setMenuItemColor("GeoAttr/Circles/circle_viewable_all=true");
  setMenuItemColor("GeoAttr/Circles/circle_viewable_all=false");
  setMenuItemColor("GeoAttr/Circles/circle_viewable_labels=true");
  setMenuItemColor("GeoAttr/Circles/circle_viewable_labels=false");
  setMenuItemColor("GeoAttr/Markers/marker_viewable_all=true");
  setMenuItemColor("GeoAttr/Markers/marker_viewable_all=false");
  setMenuItemColor("GeoAttr/Markers/marker_viewable_labels=true");
  setMenuItemColor("GeoAttr/Markers/marker_viewable_labels=false");
  setMenuItemColor("GeoAttr/OpArea/oparea_viewable_all=true");
  setMenuItemColor("GeoAttr/OpArea/oparea_viewable_all=false");
  setMenuItemColor("GeoAttr/OpArea/oparea_viewable_labels=true");
  setMenuItemColor("GeoAttr/OpArea/oparea_viewable_labels=false");
  setMenuItemColor("GeoAttr/Datum/datum_viewable=true");
  setMenuItemColor("GeoAttr/Datum/datum_viewable=false");
  setMenuItemColor("GeoAttr/Datum/datum_color=red");
  setMenuItemColor("GeoAttr/Datum/datum_color=white");
  setMenuItemColor("GeoAttr/Datum/datum_color=yellow");
  setMenuItemColor("GeoAttr/Datum/datum_color=dark_blue");
  setMenuItemColor("GeoAttr/Datum/datum_color=dark_green");
  setMenuItemColor("GeoAttr/Datum/datum_size=1");
  setMenuItemColor("GeoAttr/Datum/datum_size=2");
  setMenuItemColor("GeoAttr/Datum/datum_size=3");
  setMenuItemColor("GeoAttr/Datum/datum_size=4");
  setMenuItemColor("GeoAttr/Datum/datum_size=5");
  setMenuItemColor("GeoAttr/Datum/datum_size=6");
  setMenuItemColor("GeoAttr/Datum/datum_size=7");
  setMenuItemColor("GeoAttr/Datum/datum_size=8");
  setMenuItemColor("GeoAttr/Datum/datum_size=9");
  setMenuItemColor("GeoAttr/Datum/datum_size=10");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_vertex_size=1");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_vertex_size=2");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_vertex_size=3");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_vertex_size=4");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_vertex_size=5");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_vertex_size=6");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_vertex_size=7");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_vertex_size=8");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_vertex_size=9");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_vertex_size=10");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_viewable_all=true");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_viewable_all=false");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_coords=as-dropped");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_coords=lat-lon");
  setMenuItemColor("GeoAttr/DropPoints/drop_point_coords=local-grid");
  setMenuItemColor("GeoAttr/CommsPulses/comms_pulse_viewable_all=true");
  setMenuItemColor("GeoAttr/CommsPulses/comms_pulse_viewable_all=false");
  setMenuItemColor("GeoAttr/NodePulses/node_pulse_viewable_all=true");
  setMenuItemColor("GeoAttr/NodePulses/node_pulse_viewable_all=false");
  setMenuItemColor("GeoAttr/RangePulses/range_pulse_viewable_all=true");
  setMenuItemColor("GeoAttr/RangePulses/range_pulse_viewable_all=false");
  setMenuItemColor("BackView/tiff_viewable=true");
  setMenuItemColor("BackView/tiff_viewable=false");
  setMenuItemColor("BackView/hash_viewable=true");
  setMenuItemColor("BackView/hash_viewable=false");
  setMenuItemColor("BackView/hash_delta=10");
  setMenuItemColor("BackView/hash_delta=50");
  setMenuItemColor("BackView/hash_delta=100");
  setMenuItemColor("BackView/hash_delta=200");
  setMenuItemColor("BackView/hash_delta=500");
  setMenuItemColor("BackView/hash_delta=1000");
}


//----------------------------------------------------- 
// Procedure: setMenuAttrib
//   Purpose: A way of setting the geometry attribute that assures that the
//            GUI radio buttons are properly updated.
//  
//  Examples: "GeoAttr/SegLists", "seglist_viewable_all", true
//            "GeoAttr/Polygons", "polygon_viewable_labels", false
//            "GeoAttr/Points",   "point_viewable_labels", false

bool MarineGUI::setMenuAttrib(string menu, string attr, string value) 
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
  value = m_mviewer->geosetting(attr);
  //cout << "setMenuAttrib: menu:" << menu << " attr:" << attr << " value:" << value << endl;

  // Form for example, "GeoAttr/SegLists/seglist_viewable_all=true"
  string item_str = menu  + "/" + attr + "=" + value;
  const Fl_Menu_Item *item = m_menubar->find_item(item_str.c_str());
  if(item)
    ((Fl_Menu_Item *)item)->setonly();

  return(true);
}


//----------------------------------------- Zoom In
inline void MarineGUI::cb_Zoom_i(int val) {
  if(val < 0) 
    m_mviewer->setParam("zoom", 1.05);
  else if(val > 0) 
    m_mviewer->setParam("zoom", 0.80);
  else 
    m_mviewer->setParam("zoom", "reset");
  m_mviewer->redraw();
  cout << "MarineGUI::cb_Zoom: " << m_mviewer->getZoom() << endl;
}
void MarineGUI::cb_Zoom(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_Zoom_i(v);
}

//----------------------------------------- HandleUpDown
inline void MarineGUI::cb_HandleUpDown_i(int amt) {
  m_mviewer->setParam("pan_y", ((double)(amt))/10);
  this->updateXY();
  m_mviewer->redraw();
}
void MarineGUI::cb_HandleUpDown(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_HandleUpDown_i(v);
}


//----------------------------------------- Pan X
inline void MarineGUI::cb_HandleLeftRight_i(int amt) {
  m_mviewer->setParam("pan_x", ((double)(amt))/10);
  this->updateXY();
  m_mviewer->redraw();
}
void MarineGUI::cb_HandleLeftRight(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_HandleLeftRight_i(v);
}

//----------------------------------------- ToggleTiffType
inline void MarineGUI::cb_ToggleTiffType_i() {
  m_mviewer->setParam("tiff_type", "toggle");
  m_mviewer->redraw();
}
void MarineGUI::cb_ToggleTiffType(Fl_Widget* o) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_ToggleTiffType_i();
}

//----------------------------------------- ToggleHash
inline void MarineGUI::cb_ToggleHash_i() {
  setMenuAttrib("BackView", "hash_viewable", "toggle");
  m_mviewer->redraw();
}
void MarineGUI::cb_ToggleHash(Fl_Widget* o) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_ToggleHash_i();
}

//----------------------------------------- HashShade
inline void MarineGUI::cb_HashShade_i(int amt) {
  m_mviewer->setParam("hash_shade_mod", (double)(amt)/20.0);
  m_mviewer->redraw();
}
void MarineGUI::cb_HashShade(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_HashShade_i(v);
}

//----------------------------------------- MG_SetGeoAttr
inline void MarineGUI::cb_SetGeoAttr_i(int v) {

  if(v==180) m_mviewer->setParam("tiff_viewable", "true");
  else if(v==181) m_mviewer->setParam("tiff_viewable", "false");
  else if(v==182) setMenuAttrib("BackView", "tiff_viewable", "toggle");

  else if(v==150) m_mviewer->setParam("polygon_viewable_all", "on");
  else if(v==151) m_mviewer->setParam("polygon_viewable_all", "off");
  else if(v==152) setMenuAttrib("GeoAttr/Polygons", "polygon_viewable_all", "toggle");

  else if(v==160) m_mviewer->setParam("polygon_viewable_labels", "on");
  else if(v==161) m_mviewer->setParam("polygon_viewable_labels", "off");
  else if(v==162) setMenuAttrib("GeoAttr/Polygons", "polygon_viewable_labels", "toggle");

  //---------------------------------- SegLists
  else if(v==250) m_mviewer->setParam("seglist_viewable_all", "on");
  else if(v==251) m_mviewer->setParam("seglist_viewable_all", "off");
  else if(v==252) setMenuAttrib("GeoAttr/SegLists", "seglist_viewable_all", "toggle");

  else if(v==260) m_mviewer->setParam("seglist_viewable_labels", "on");
  else if(v==261) m_mviewer->setParam("seglist_viewable_labels", "off");
  else if(v==262) setMenuAttrib("GeoAttr/SegLists", "seglist_viewable_labels", "toggle");

  //---------------------------------- Seglrs
  else if(v==270) m_mviewer->setParam("seglr_viewable_all", "on");
  else if(v==271) m_mviewer->setParam("seglr_viewable_all", "off");
  else if(v==272) setMenuAttrib("GeoAttr/Seglrs", "seglr_viewable_all", "toggle");

  else if(v==280) m_mviewer->setParam("seglr_viewable_labels", "on");
  else if(v==281) m_mviewer->setParam("seglr_viewable_labels", "off");
  else if(v==282) setMenuAttrib("GeoAttr/SegLists", "seglr_viewable_labels", "toggle");

  //---------------------------------- Grids

  else if(v==300) m_mviewer->setParam("grid_viewable_all", "true");
  else if(v==301) m_mviewer->setParam("grid_viewable_all", "false");
  else if(v==302) setMenuAttrib("GeoAttr/Grids", "grid_viewable_all", "toggle");

  else if(v==350) setMenuAttrib("GeoAttr/Grids", "grid_opaqueness", "0.0");
  else if(v==351) setMenuAttrib("GeoAttr/Grids", "grid_opaqueness", "0.1");
  else if(v==352) setMenuAttrib("GeoAttr/Grids", "grid_opaqueness", "0.2");
  else if(v==353) setMenuAttrib("GeoAttr/Grids", "grid_opaqueness", "0.3");
  else if(v==354) setMenuAttrib("GeoAttr/Grids", "grid_opaqueness", "0.4");
  else if(v==355) setMenuAttrib("GeoAttr/Grids", "grid_opaqueness", "0.5");
  else if(v==356) setMenuAttrib("GeoAttr/Grids", "grid_opaqueness", "0.6");
  else if(v==357) setMenuAttrib("GeoAttr/Grids", "grid_opaqueness", "0.7");
  else if(v==358) setMenuAttrib("GeoAttr/Grids", "grid_opaqueness", "0.8");
  else if(v==359) setMenuAttrib("GeoAttr/Grids", "grid_opaqueness", "0.9");
  else if(v==360) setMenuAttrib("GeoAttr/Grids", "grid_opaqueness", "1.0");

  else if(v==330) setMenuAttrib("GeoAttr/Grids", "grid_opaqueness", "more");
  else if(v==331) setMenuAttrib("GeoAttr/Grids", "grid_opaqueness", "less");

  //--------------------------------------------------- Points
  else if(v==550) m_mviewer->setParam("point_viewable_all", "on");
  else if(v==551) m_mviewer->setParam("point_viewable_all", "off");
  else if(v==552) setMenuAttrib("GeoAttr/Points", "point_viewable_all", "toggle");

  else if(v==560) m_mviewer->setParam("point_viewable_labels", "on");
  else if(v==561) m_mviewer->setParam("point_viewable_labels", "off");
  else if(v==562) setMenuAttrib("GeoAttr/Points", "point_viewable_labels", "toggle");


  //--------------------------------------------------- Vectors
  else if(v==750) m_mviewer->setParam("vector_viewable_all", "on");
  else if(v==751) m_mviewer->setParam("vector_viewable_all", "of");
  else if(v==752) setMenuAttrib("GeoAttr/Points", "vector_viewable_all", "toggle");

  else if(v==760) m_mviewer->setParam("vector_viewable_labels", "on");
  else if(v==761) m_mviewer->setParam("vector_viewable_labels", "off");
  else if(v==762) setMenuAttrib("GeoAttr/Points", "vector_viewable_labels", "toggle");

  //---------------------------------------------------- Circles
  else if(v==850) m_mviewer->setParam("circle_viewable_all", "on");
  else if(v==851) m_mviewer->setParam("circle_viewable_all", "off");
  else if(v==852) setMenuAttrib("GeoAttr/Circles", "circle_viewable_all", "toggle");

  else if(v==860) m_mviewer->setParam("circle_viewable_labels", "on");
  else if(v==861) m_mviewer->setParam("circle_viewable_labels", "off");
  else if(v==862) setMenuAttrib("GeoAttr/Circles", "circle_viewable_labels", "toggle");

  //----------------------------------------------------- Datum
  else if(v==400) m_mviewer->setParam("datum_viewable", "on");
  else if(v==401) m_mviewer->setParam("datum_viewable", "off");
  else if(v==402) setMenuAttrib("GeoAttr/Datum", "datum_viewable", "toggle");

  else if(v==410) m_mviewer->setParam("datum_color", "red");
  else if(v==411) m_mviewer->setParam("datum_color", "white");
  else if(v==412) m_mviewer->setParam("datum_color", "yellow");
  else if(v==413) m_mviewer->setParam("datum_color", "dark_blue");
  else if(v==414) m_mviewer->setParam("datum_color", "dark_green");

  else if(v==440) setMenuAttrib("GeoAttr/Datum", "datum_size", "1");
  else if(v==441) setMenuAttrib("GeoAttr/Datum", "datum_size", "2");
  else if(v==442) setMenuAttrib("GeoAttr/Datum", "datum_size", "3");
  else if(v==443) setMenuAttrib("GeoAttr/Datum", "datum_size", "4");
  else if(v==444) setMenuAttrib("GeoAttr/Datum", "datum_size", "5");
  else if(v==445) setMenuAttrib("GeoAttr/Datum", "datum_size", "6");
  else if(v==446) setMenuAttrib("GeoAttr/Datum", "datum_size", "7");
  else if(v==447) setMenuAttrib("GeoAttr/Datum", "datum_size", "8");
  else if(v==448) setMenuAttrib("GeoAttr/Datum", "datum_size", "9");
  else if(v==449) setMenuAttrib("GeoAttr/Datum", "datum_size", "10");
  else if(v==450) setMenuAttrib("GeoAttr/Datum", "datum_size", "delta:1");
  else if(v==451) setMenuAttrib("GeoAttr/Datum", "datum_size", "delta:-1");

  // ----------------------------------------------------- Markers
  else if(v==900) m_mviewer->setParam("marker_viewable_all", "true");
  else if(v==901) m_mviewer->setParam("marker_viewable_all", "false");
  else if(v==902) setMenuAttrib("GeoAttr/Markers", "marker_viewable_all", "toggle");

  else if(v==910) m_mviewer->setParam("marker_viewable_labels", "true");
  else if(v==911) m_mviewer->setParam("marker_viewable_labels", "false");
  else if(v==912) setMenuAttrib("GeoAttr/Markers", "marker_viewable_labels", "toggle");

  else if(v==920) m_mviewer->setParam("marker_scale", "1");
  else if(v==921) m_mviewer->setParam("marker_scale", "scale:0.9");
  else if(v==922) m_mviewer->setParam("marker_scale", "scale:1.1");

  // ----------------------------------------------------- Op Area
  else if(v==950) m_mviewer->setParam("oparea_viewable_all", "true");
  else if(v==951) m_mviewer->setParam("oparea_viewable_all", "false");
  else if(v==952) setMenuAttrib("GeoAttr/OpArea", "oparea_viewable_all", "toggle");

  else if(v==960) m_mviewer->setParam("oparea_viewable_labels", "true");
  else if(v==961) m_mviewer->setParam("oparea_viewable_labels", "false");
  else if(v==962) setMenuAttrib("GeoAttr/OpArea", "oparea_viewable_labels", "toggle");

  else if(v==970) m_mviewer->setParam("op_area_config", "reset");
  else if(v==971) m_mviewer->setParam("op_area_line_shade_mod", "0.95");
  else if(v==972) m_mviewer->setParam("op_area_line_shade_mod", "1.05");

  // ----------------------------------------------------- DropPoints
  else if(v==3100) m_mviewer->setParam("drop_point_edit", "clear");
  else if(v==3101) m_mviewer->setParam("drop_point_edit", "clear_last");
  else if(v==3110) m_mviewer->setParam("drop_point_coords", "as-dropped");
  else if(v==3111) m_mviewer->setParam("drop_point_coords", "lat-lon");
  else if(v==3112) m_mviewer->setParam("drop_point_coords", "local-grid");
  else if(v==3121) m_mviewer->setParam("drop_point_vertex_size", "1");
  else if(v==3122) m_mviewer->setParam("drop_point_vertex_size", "2");
  else if(v==3123) m_mviewer->setParam("drop_point_vertex_size", "3");
  else if(v==3124) m_mviewer->setParam("drop_point_vertex_size", "4");
  else if(v==3125) m_mviewer->setParam("drop_point_vertex_size", "5");
  else if(v==3126) m_mviewer->setParam("drop_point_vertex_size", "6");
  else if(v==3127) m_mviewer->setParam("drop_point_vertex_size", "7");
  else if(v==3128) m_mviewer->setParam("drop_point_vertex_size", "8");
  else if(v==3129) m_mviewer->setParam("drop_point_vertex_size", "9");
  else if(v==3130) m_mviewer->setParam("drop_point_vertex_size", "10");
  else if(v==3131) m_mviewer->setParam("drop_point_viewable_all", "true");
  else if(v==3132) m_mviewer->setParam("drop_point_viewable_all", "false");
  else if(v==3133) setMenuAttrib("GeoAttr/DropPoints", "drop_point_viewable_all", "toggle");

  else if(v==4150) m_mviewer->setParam("range_pulse_viewable_all", "true");
  else if(v==4151) m_mviewer->setParam("range_pulse_viewable_all", "false");
  else if(v==4152) setMenuAttrib("GeoAttr/RangePulses","range_pulse_viewable_all","toggle");

  else if(v==4160) m_mviewer->setParam("comms_pulse_viewable_all", "true");
  else if(v==4161) m_mviewer->setParam("comms_pulse_viewable_all", "false");
  else if(v==4162) setMenuAttrib("GeoAttr/CommsPulses","comms_pulse_viewable_all","toggle");

  else if(v==4170) m_mviewer->setParam("node_pulse_viewable_all", "true");
  else if(v==4171) m_mviewer->setParam("node_pulse_viewable_all", "false");
  else if(v==4172) setMenuAttrib("GeoAttr/NodePulses","node_pulse_viewable_all","toggle");

  else 
    return;

  m_mviewer->redraw();
}
void MarineGUI::cb_SetGeoAttr(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_SetGeoAttr_i(v);
}

//----------------------------------------- HashDelta
inline void MarineGUI::cb_HashDelta_i(int amt) {
  m_mviewer->setParam("hash_delta", intToString(amt));
  m_mviewer->redraw();
}
void MarineGUI::cb_HashDelta(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_HashDelta_i(v);
}

//----------------------------------------- BackShade
inline void MarineGUI::cb_BackShade_i(int amt) {
  m_mviewer->setParam("back_shade_delta", (double)(amt)/20.0);
  m_mviewer->redraw();
}
void MarineGUI::cb_BackShade(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_BackShade_i(v);
}

bool MarineGUI::removeMenuItem(string item_str)
{
  int index = m_menubar->find_index(item_str.c_str());
  //int index = -1; // fixme
  if(index != -1) {
    m_menubar->remove(index);
    return(true);
  }
  return(false);
}

//----------------------------------------- Quit
void MarineGUI::cb_Quit() {
  exit(0);
}







