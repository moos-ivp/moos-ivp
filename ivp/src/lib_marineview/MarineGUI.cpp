/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MarineGUI.cpp                                        */
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
#include <stdlib.h>
#include "MarineGUI.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

MarineGUI::MarineGUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) 
{
  mbar = new Fl_Menu_Bar(0, 0, w(), 25);
  mbar->menu(menu_);
    
  cmviewer = 0;

  augmentMenu();
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
 {"Pan Up ",          FL_Up,  (Fl_Callback*)MarineGUI::cb_PanY, (void*)-200, 0},
 {"Pan Down ",        FL_Down,  (Fl_Callback*)MarineGUI::cb_PanY, (void*)200, 0},
 {"Pan Left ",        FL_Left,  (Fl_Callback*)MarineGUI::cb_PanX, (void*)200, 0},
 {"Pan Right ",       FL_Right,  (Fl_Callback*)MarineGUI::cb_PanX, (void*)-200, FL_MENU_DIVIDER},
 {"Pan Up (slow) ",   FL_ALT + FL_Up, (Fl_Callback*)MarineGUI::cb_PanY, (void*)-10, 0},
 {"Pan Down (slow) ", FL_ALT + FL_Down, (Fl_Callback*)MarineGUI::cb_PanY, (void*)10, 0},
 {"Pan Left (slow) ", FL_ALT + FL_Left, (Fl_Callback*)MarineGUI::cb_PanX, (void*)10, 0},
 {"Pan Right (slow)", FL_ALT + FL_Right, (Fl_Callback*)MarineGUI::cb_PanX, (void*)-10, FL_MENU_DIVIDER},
 {"Pan Up (v. slow) ",   FL_CTRL + FL_Up, (Fl_Callback*)MarineGUI::cb_PanY, (void*)-1, 0},
 {"Pan Down (v. slow) ", FL_CTRL + FL_Down, (Fl_Callback*)MarineGUI::cb_PanY, (void*)1, 0},
 {"Pan Left (v. slow) ", FL_CTRL + FL_Left, (Fl_Callback*)MarineGUI::cb_PanX, (void*)1, 0},
 {"Pan Right (v. slow)", FL_CTRL + FL_Right, (Fl_Callback*)MarineGUI::cb_PanX, (void*)-1, FL_MENU_DIVIDER},
 {"Cross Hairs",      0, (Fl_Callback*)MarineGUI::cb_ToggleCross, (void*)-1, FL_MENU_DIVIDER},
 {"Toggle BGround Off/On",  'b', (Fl_Callback*)MarineGUI::cb_ToggleTiff, (void*)-1, 0},
 {"Toggle BGround Type",  '`', (Fl_Callback*)MarineGUI::cb_ToggleTiffType, (void*)-1, 0},
 {"Background Lighter", FL_CTRL+'b', (Fl_Callback*)MarineGUI::cb_BackShade,  (void*)+1, 0},
 {"Background Darker",  FL_ALT +'b', (Fl_Callback*)MarineGUI::cb_BackShade,  (void*)-1, FL_MENU_DIVIDER},
 {"Toggle Hash",      'h', (Fl_Callback*)MarineGUI::cb_ToggleHash, (void*)-1, 0},
 {"Hash Lighter",     FL_CTRL+'h', (Fl_Callback*)MarineGUI::cb_HashShade,  (void*)+1, 0},
 {"Hash Darker",      FL_ALT +'h', (Fl_Callback*)MarineGUI::cb_HashShade,  (void*)-1, FL_MENU_DIVIDER},
 {"Hash 10",      FL_ALT+'1', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)10,  FL_MENU_RADIO},
 {"Hash 50",      FL_ALT+'2', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)50,  FL_MENU_RADIO},
 {"Hash 100",     FL_ALT+'3', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)100, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Hash 200",     FL_ALT+'4', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)200, FL_MENU_RADIO},
 {"Hash 500",     FL_ALT+'5', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)500, FL_MENU_RADIO},
 {"Hash 1000",     FL_ALT+'6', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)1000, FL_MENU_RADIO},
 {"Hash Auto",    FL_ALT+'7', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)-1, FL_MENU_RADIO},
 {0},

 {"GeoAttr", 0,  0, 0, 64, 0, 0, 14, 0},
 {0},

 {0}
};



//-------------------------------------------------------------------
// Procedure: augmentMenu

void MarineGUI::augmentMenu() 
{
  mbar->add("GeoAttr/Polygons - Edit/display_poly_labels=on", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)150, 0);
  mbar->add("GeoAttr/Polygons - Edit/display_poly_labels=off", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)151, 0);
  mbar->add("GeoAttr/Polygons - Edit/display_poly_labels toggle", 'P', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)152, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/Polygons - Edit/polygon_vertex_color=red", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)100, 0);
  mbar->add("GeoAttr/Polygons - Edit/polygon_vertex_color=white", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)101, 0);
  mbar->add("GeoAttr/Polygons - Edit/polygon_vertex_color=yellow", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)102, 0);
  mbar->add("GeoAttr/Polygons - Edit/polygon_vertex_color=dark_blue", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)103, 0);
  mbar->add("GeoAttr/Polygons - Edit/polygon_vertex_color=dark_green", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)104, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/Polygons - Edit/polygon_edge_color=red", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)120, 0);
  mbar->add("GeoAttr/Polygons - Edit/polygon_edge_color=white", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)121, 0);
  mbar->add("GeoAttr/Polygons - Edit/polygon_edge_color=yellow", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)122, 0);
  mbar->add("GeoAttr/Polygons - Edit/polygon_edge_color=dark_blue", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)123, 0);
  mbar->add("GeoAttr/Polygons - Edit/polygon_edge_color=dark_green", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)124, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/Polygons - Edit/polygon_vertex_size=1", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)140, 0);
  mbar->add("GeoAttr/Polygons - Edit/polygon_vertex_size=2", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)141, 0);
  mbar->add("GeoAttr/Polygons - Edit/polygon_vertex_size=3", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)142, 0);
  mbar->add("GeoAttr/Polygons - Edit/polygon_vertex_size=5", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)143, 0);
  mbar->add("GeoAttr/Polygons - Edit/polygon_vertex_size=8", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)144, 0);
  mbar->add("GeoAttr/Polygons - Edit/polygon_vertex_size=10", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)145, 0);

  mbar->add("GeoAttr/Polygons - Toggle", 'p', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)160, FL_MENU_DIVIDER);

  // --------------------------------- SegLists
  mbar->add("GeoAttr/SegLists - Edit/seglist_vertex_color=red", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)200, 0);
  mbar->add("GeoAttr/SegLists - Edit/seglist_vertex_color=white", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)201, 0);
  mbar->add("GeoAttr/SegLists - Edit/seglist_vertex_color=yellow", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)202, 0);
  mbar->add("GeoAttr/SegLists - Edit/seglist_vertex_color=dark_blue", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)203, 0);
  mbar->add("GeoAttr/SegLists - Edit/seglist_vertex_color=dark_green", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)204, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/SegLists - Edit/seglist_edge_color=red", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)220, 0);
  mbar->add("GeoAttr/SegLists - Edit/seglist_edge_color=white", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)221, 0);
  mbar->add("GeoAttr/SegLists - Edit/seglist_edge_color=yellow", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)222, 0);
  mbar->add("GeoAttr/SegLists - Edit/seglist_edge_color=dark_blue", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)223, 0);
  mbar->add("GeoAttr/SegLists - Edit/seglist_edge_color=dark_green", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)224, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/SegLists - Edit/seglist_vertex_size=1", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)240, 0);
  mbar->add("GeoAttr/SegLists - Edit/seglist_vertex_size=2", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)241, 0);
  mbar->add("GeoAttr/SegLists - Edit/seglist_vertex_size=3", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)242, 0);
  mbar->add("GeoAttr/SegLists - Edit/seglist_vertex_size=5", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)243, 0);
  mbar->add("GeoAttr/SegLists - Edit/seglist_vertex_size=8", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)244, 0);
  mbar->add("GeoAttr/SegLists - Edit/seglist_vertex_size=10", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)245, 0);

  mbar->add("GeoAttr/SegLists - Toggle", 's', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)250, FL_MENU_DIVIDER);

  // --------------------------------- Points
  mbar->add("GeoAttr/Points - Edit/display_point_labels=on", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)500, 0);
  mbar->add("GeoAttr/Points - Edit/display_point_labels=off", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)501, 0);
  mbar->add("GeoAttr/Points - Edit/display_point_labels toggle", 'J', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)502, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/Points - Edit/point_vertex_color=red", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)510, 0);
  mbar->add("GeoAttr/Points - Edit/point_vertex_color=white", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)511, 0);
  mbar->add("GeoAttr/Points - Edit/point_vertex_color=yellow", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)512, 0);
  mbar->add("GeoAttr/Points - Edit/point_vertex_color=dark_blue", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)513, 0);
  mbar->add("GeoAttr/Points - Edit/point_vertex_color=dark_green", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)514, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/Points - Edit/point_vertex_size=1", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)520, 0);
  mbar->add("GeoAttr/Points - Edit/point_vertex_size=2", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)521, 0);
  mbar->add("GeoAttr/Points - Edit/point_vertex_size=3", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)522, 0);
  mbar->add("GeoAttr/Points - Edit/point_vertex_size=5", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)523, 0);
  mbar->add("GeoAttr/Points - Edit/point_vertex_size=8", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)524, 0);
  mbar->add("GeoAttr/Points - Edit/point_vertex_size=10", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)525, FL_MENU_DIVIDER);
  mbar->add("GeoAttr/Points - Edit/point_vertex_size smaller", FL_CTRL+'j', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)530, 0);
  mbar->add("GeoAttr/Points - Edit/point_vertex_size bigger", FL_ALT+'j', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)531, 0);

  mbar->add("GeoAttr/Points - Toggle", 'j', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)550, FL_MENU_DIVIDER);

  // -------------------------------- XYGrids
  mbar->add("GeoAttr/XYGrids - Edit/grid_edge_color=red", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)300, 0);
  mbar->add("GeoAttr/XYGrids - Edit/grid_edge_color=white", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)301, 0);
  mbar->add("GeoAttr/XYGrids - Edit/grid_edge_color=yellow", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)302, 0);
  mbar->add("GeoAttr/XYGrids - Edit/grid_edge_color=dark_blue", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)303, 0);
  mbar->add("GeoAttr/XYGrids - Edit/grid_edge_color=dark_green", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)304, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/XYGrids - Toggle", 'g', (Fl_Callback*)MarineGUI::cb_ToggleGrids, (void*)0, FL_MENU_DIVIDER);

  // --------------------------------- Datum
  mbar->add("GeoAttr/Datum - Edit/datum_color=red", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)400, 0);
  mbar->add("GeoAttr/Datum - Edit/datum_color=white", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)401, 0);
  mbar->add("GeoAttr/Datum - Edit/datum_color=yellow", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)402, 0);
  mbar->add("GeoAttr/Datum - Edit/datum_color=dark_blue", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)403, 0);
  mbar->add("GeoAttr/Datum - Edit/datum_color=dark_green", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)404, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/Datum - Edit/datum_size=1", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)440, 0);
  mbar->add("GeoAttr/Datum - Edit/datum_size=3", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)441, 0);
  mbar->add("GeoAttr/Datum - Edit/datum_size=5", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)442, 0);
  mbar->add("GeoAttr/Datum - Edit/datum_size=10", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)443, 0);
  mbar->add("GeoAttr/Datum - Edit/datum_size=15", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)444, 0);
  mbar->add("GeoAttr/Datum - Edit/datum_size=20", 0, (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)445, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/Datum - Edit/datum_size smaller", FL_CTRL+'d', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)448, 0);
  mbar->add("GeoAttr/Datum - Edit/datum_size larger", FL_ALT+'d', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)449, 0);

  mbar->add("GeoAttr/Datum - Toggle", 'd', (Fl_Callback*)MarineGUI::cb_ToggleDatum, (void*)0, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/Markers - Edit/marker_scale reset", FL_CTRL+FL_ALT+'m', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)2000, 0);
  mbar->add("GeoAttr/Markers - Edit/marker_scale *= 0.80", FL_CTRL+'m', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)2001, 0);
  mbar->add("GeoAttr/Markers - Edit/marker_scale *= 1.25", FL_ALT+'m', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)2002, FL_MENU_DIVIDER);
  mbar->add("GeoAttr/Markers - Edit/draw labels  toggle", 'l', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)2020, FL_MENU_DIVIDER);
  mbar->add("GeoAttr/Markers - Edit/label colors toggle", 'L', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)2030, 0);

  mbar->add("GeoAttr/Markers - Toggle", 'm', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)2050, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/OpArea - Edit/op_area reset", FL_CTRL+FL_ALT+'u', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)2100, 0);
  mbar->add("GeoAttr/OpArea - Edit/op_area lighter *= 0.80", FL_CTRL+'u', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)2101, 0);
  mbar->add("GeoAttr/OpArea - Edit/op_area darker *= 1.25", FL_ALT+'u', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)2102, FL_MENU_DIVIDER);
  mbar->add("GeoAttr/OpArea - Edit/op_area labels  toggle", 'U', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)2120, FL_MENU_DIVIDER);

  mbar->add("GeoAttr/OpArea - Toggle", 'u', (Fl_Callback*)MarineGUI::cb_MG_SetGeoAttr, (void*)2150, FL_MENU_DIVIDER);
}


//----------------------------------------------------------
// Procedure: handle
//      Note: As it stands, this method could be eliminated entirely, and the 
//            default behavior of the parent class should work fine. But if
//            we want to tinker with event handling, this method is the place.

int MarineGUI::handle(int event) 
{
  return(Fl_Window::handle(event));
}

//----------------------------------------- Zoom In
inline void MarineGUI::cb_Zoom_i(int val) {
  if(val < 0) 
    cmviewer->setParam("zoom", 1.05);
  else if(val > 0) 
    cmviewer->setParam("zoom", 0.80);
  else 
    cmviewer->setParam("zoom", "reset");
  cmviewer->redraw();
}
void MarineGUI::cb_Zoom(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_Zoom_i(v);
}

//----------------------------------------- Pan Y
inline void MarineGUI::cb_PanY_i(int amt) {
  cmviewer->setParam("pan_y", ((double)(amt))/10);
  this->updateXY();
  cmviewer->redraw();
}
void MarineGUI::cb_PanY(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_PanY_i(v);
}


//----------------------------------------- Pan X
inline void MarineGUI::cb_PanX_i(int amt) {
  cmviewer->setParam("pan_x", ((double)(amt))/10);
  this->updateXY();
  cmviewer->redraw();
}
void MarineGUI::cb_PanX(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_PanX_i(v);
}

//----------------------------------------- ToggleTiff
inline void MarineGUI::cb_ToggleTiff_i() {
  cmviewer->setParam("tiff_view", "toggle");
  cmviewer->redraw();
}
void MarineGUI::cb_ToggleTiff(Fl_Widget* o) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_ToggleTiff_i();
}

//----------------------------------------- ToggleTiffType
inline void MarineGUI::cb_ToggleTiffType_i() {
  cmviewer->setParam("tiff_type", "toggle");
  cmviewer->redraw();
}
void MarineGUI::cb_ToggleTiffType(Fl_Widget* o) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_ToggleTiffType_i();
}

//----------------------------------------- ToggleHash
inline void MarineGUI::cb_ToggleHash_i() {
  cmviewer->setParam("hash_view", "toggle");
  cmviewer->redraw();
}
void MarineGUI::cb_ToggleHash(Fl_Widget* o) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_ToggleHash_i();
}

//----------------------------------------- HashShade
inline void MarineGUI::cb_HashShade_i(int amt) {
  cmviewer->setParam("hash_shade", (double)(amt)/20.0);
  cmviewer->redraw();
}
void MarineGUI::cb_HashShade(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_HashShade_i(v);
}

//----------------------------------------- MG_SetGeoAttr
inline void MarineGUI::cb_MG_SetGeoAttr_i(int v) {
  if(v==100) cmviewer->setParam("polygon_vertex_color", "red");
  else if(v==101) cmviewer->setParam("polygon_vertex_color", "white");
  else if(v==102) cmviewer->setParam("polygon_vertex_color", "yellow");
  else if(v==103) cmviewer->setParam("polygon_vertex_color", "DarkBlue");
  else if(v==104) cmviewer->setParam("polygon_vertex_color", "DarkGreen");

  else if(v==120) cmviewer->setParam("polygon_edge_color", "red");
  else if(v==121) cmviewer->setParam("polygon_edge_color", "white");
  else if(v==122) cmviewer->setParam("polygon_edge_color", "yellow");
  else if(v==123) cmviewer->setParam("polygon_edge_color", "DarkBlue");
  else if(v==124) cmviewer->setParam("polygon_edge_color", "DarkGreen");

  else if(v==140) cmviewer->setParam("polygon_vertex_size", "1");
  else if(v==141) cmviewer->setParam("polygon_vertex_size", "2");
  else if(v==142) cmviewer->setParam("polygon_vertex_size", "3");
  else if(v==143) cmviewer->setParam("polygon_vertex_size", "5");
  else if(v==144) cmviewer->setParam("polygon_vertex_size", "8");
  else if(v==145) cmviewer->setParam("polygon_vertex_size", "10");

  else if(v==150) cmviewer->setParam("polygon_viewable_labels", "on");
  else if(v==151) cmviewer->setParam("polygon_viewable_labels", "off");
  else if(v==152) cmviewer->setParam("polygon_viewable_labels", "toggle");

  else if(v==160) cmviewer->setParam("polygon_viewable_all", "toggle");
  
  //---------------------------------- SegLists

  else if(v==200) cmviewer->setParam("seglist_vertex_color", "red");
  else if(v==201) cmviewer->setParam("seglist_vertex_color", "white");
  else if(v==202) cmviewer->setParam("seglist_vertex_color", "yellow");
  else if(v==203) cmviewer->setParam("seglist_vertex_color", "dark_blue");
  else if(v==204) cmviewer->setParam("seglist_vertex_color", "dark_green");

  else if(v==220) cmviewer->setParam("seglist_edge_color", "red");
  else if(v==221) cmviewer->setParam("seglist_edge_color", "white");
  else if(v==222) cmviewer->setParam("seglist_edge_color", "yellow");
  else if(v==223) cmviewer->setParam("seglist_edge_color", "dark_blue");
  else if(v==224) cmviewer->setParam("seglist_edge_color", "dark_green");

  else if(v==240) cmviewer->setParam("seglist_vertex_size", "1");
  else if(v==241) cmviewer->setParam("seglist_vertex_size", "2");
  else if(v==242) cmviewer->setParam("seglist_vertex_size", "3");
  else if(v==243) cmviewer->setParam("seglist_vertex_size", "5");
  else if(v==244) cmviewer->setParam("seglist_vertex_size", "8");
  else if(v==245) cmviewer->setParam("seglist_vertex_size", "10");

  else if(v==250) cmviewer->setParam("seglist_viewable_all", "toggle");

  //---------------------------------- Points
  else if(v==500) cmviewer->setParam("point_viewable_labels", "on");
  else if(v==501) cmviewer->setParam("point_viewable_labels", "off");
  else if(v==502) cmviewer->setParam("point_viewable_labels", "toggle");

  else if(v==510) cmviewer->setParam("point_vertex_color", "red");
  else if(v==511) cmviewer->setParam("point_vertex_color", "white");
  else if(v==512) cmviewer->setParam("point_vertex_color", "yellow");
  else if(v==513) cmviewer->setParam("point_vertex_color", "DarkBlue");
  else if(v==514) cmviewer->setParam("point_vertex_color", "DarkGreen");

  else if(v==520) cmviewer->setParam("point_vertex_size", "1");
  else if(v==521) cmviewer->setParam("point_vertex_size", "2");
  else if(v==522) cmviewer->setParam("point_vertex_size", "3");
  else if(v==523) cmviewer->setParam("point_vertex_size", "5");
  else if(v==524) cmviewer->setParam("point_vertex_size", "8");
  else if(v==525) cmviewer->setParam("point_vertex_size", "10");

  else if(v==530) cmviewer->setParam("point_vertex_size", "-1");
  else if(v==531) cmviewer->setParam("point_vertex_size", "+1");

  else if(v==550) cmviewer->setParam("point_viewable_all", "toggle");

  //---------------------------------- Datum

  else if(v==400) cmviewer->setParam("datum_color", "red");
  else if(v==401) cmviewer->setParam("datum_color", "white");
  else if(v==402) cmviewer->setParam("datum_color", "yellow");
  else if(v==403) cmviewer->setParam("datum_color", "dark_blue");
  else if(v==404) cmviewer->setParam("datum_color", "dark_green");

  else if(v==440) cmviewer->setParam("datum_size", 1);
  else if(v==441) cmviewer->setParam("datum_size", 3);
  else if(v==442) cmviewer->setParam("datum_size", 5);
  else if(v==443) cmviewer->setParam("datum_size", 10);
  else if(v==444) cmviewer->setParam("datum_size", 15);
  else if(v==445) cmviewer->setParam("datum_size", 20);
  else if(v==448) cmviewer->setParam("datum_size_add", "smaller");
  else if(v==449) cmviewer->setParam("datum_size_add", "bigger");

  else if(v==2000) cmviewer->setParam("markers_scale_global", "reset");
  else if(v==2001) cmviewer->setParam("markers_scale_global", "smaller");
  else if(v==2002) cmviewer->setParam("markers_scale_global", "bigger");
  else if(v==2020) cmviewer->setParam("markers_labels_viewable", "toggle");
  else if(v==2030) cmviewer->setParam("markers_label_color", "toggle");
  else if(v==2050) cmviewer->setParam("markers_viewable", "toggle");

  else if(v==2100) cmviewer->setParam("op_area_config", "reset");
  else if(v==2101) cmviewer->setParam("op_area_line_shade_mod", "1.05");
  else if(v==2102) cmviewer->setParam("op_area_line_shade_mod", "0.95");
  else if(v==2120) cmviewer->setParam("op_area_viewable_labels", "toggle");
  else if(v==2150) cmviewer->setParam("op_area_viewable_all", "toggle");

  else 
    return;

  cmviewer->redraw();
}
void MarineGUI::cb_MG_SetGeoAttr(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_MG_SetGeoAttr_i(v);
}

//----------------------------------------- ToggleDatum
inline void MarineGUI::cb_ToggleDatum_i() {
  cmviewer->setParam("datum_viewable", "toggle");
  cmviewer->redraw();
}
void MarineGUI::cb_ToggleDatum(Fl_Widget* o) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_ToggleDatum_i();
}

//----------------------------------------- ToggleGrids
inline void MarineGUI::cb_ToggleGrids_i() {
  cmviewer->setParam("grid_viewable_all", "toggle");
  cmviewer->redraw();
}
void MarineGUI::cb_ToggleGrids(Fl_Widget* o) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_ToggleGrids_i();
}

//----------------------------------------- HashDelta
inline void MarineGUI::cb_HashDelta_i(int amt) {
  cmviewer->setParam("hash_delta", (double)(amt));
  cmviewer->redraw();
}
void MarineGUI::cb_HashDelta(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_HashDelta_i(v);
}

//----------------------------------------- BackShade
inline void MarineGUI::cb_BackShade_i(int amt) {
  cmviewer->setParam("back_shade", (double)(amt)/20.0);
  cmviewer->redraw();
}
void MarineGUI::cb_BackShade(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_BackShade_i(v);
}

//----------------------------------------- ToggleCross
inline void MarineGUI::cb_ToggleCross_i() {
  cmviewer->setParam("cross_view", "toggle");
  cmviewer->redraw();
  updateXY();
}
void MarineGUI::cb_ToggleCross(Fl_Widget* o) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_ToggleCross_i();
}

//----------------------------------------- Quit
void MarineGUI::cb_Quit() {
  exit(0);
}

