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
 {"Cross Hairs",      'c', (Fl_Callback*)MarineGUI::cb_ToggleCross, (void*)-1, FL_MENU_DIVIDER},
 {"Toggle BGround Off/On",  'b', (Fl_Callback*)MarineGUI::cb_ToggleTiff, (void*)-1, 0},
 {"Toggle BGround Type",  '`', (Fl_Callback*)MarineGUI::cb_ToggleTiffType, (void*)-1, 0},
 {"Background Lighter", FL_CTRL+'b', (Fl_Callback*)MarineGUI::cb_BackShade,  (void*)+1, 0},
 {"Background Darker",  FL_ALT +'b', (Fl_Callback*)MarineGUI::cb_BackShade,  (void*)-1, FL_MENU_DIVIDER},
 {"Toggle Hash",      'h', (Fl_Callback*)MarineGUI::cb_ToggleHash, (void*)-1, 0},
 {"Hash Lighter",     FL_CTRL+'h', (Fl_Callback*)MarineGUI::cb_HashShade,  (void*)+1, 0},
 {"Hash Darker",      FL_ALT +'h', (Fl_Callback*)MarineGUI::cb_HashShade,  (void*)-1, FL_MENU_DIVIDER},
 {"Toggle Grid(s)",      'g', (Fl_Callback*)MarineGUI::cb_ToggleGrids, (void*)0, FL_MENU_DIVIDER},
 {"Toggle Datum",        'd', (Fl_Callback*)MarineGUI::cb_ToggleDatum, (void*)0, 0},
 {"Datum Bigger", FL_CTRL+'d', (Fl_Callback*)MarineGUI::cb_DatumSize,  (void*)+1, 0},
 {"Datum Smaller", FL_ALT +'d', (Fl_Callback*)MarineGUI::cb_DatumSize,  (void*)-1, FL_MENU_DIVIDER},
 {"Hash 10",      FL_ALT+'1', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)10,  FL_MENU_RADIO},
 {"Hash 50",      FL_ALT+'2', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)50,  FL_MENU_RADIO},
 {"Hash 100",     FL_ALT+'3', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)100, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Hash 200",     FL_ALT+'4', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)200, FL_MENU_RADIO},
 {"Hash 500",     FL_ALT+'5', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)500, FL_MENU_RADIO},
 {"Hash 1000",     FL_ALT+'6', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)1000, FL_MENU_RADIO},
 {"Hash Auto",    FL_ALT+'7', (Fl_Callback*)MarineGUI::cb_HashDelta,  (void*)-1, FL_MENU_RADIO},
 {0},
 {0}
};

//----------------------------------------------------------
// Procedure: handle
//      Note: As it stands, this method could be eliminated entirely, and the 
//            default behavior of the parent class should work fine. But if
//            we want to tinker with event handling, this method is the place.

int MarineGUI::handle(int event) 
{
  return(Fl_Window::handle(event));
}

//-------------------------------------------------------------------
// Procedure: readTiff

void MarineGUI::readTiff(string filename)
{
  if(!cmviewer)
    return;

  cout << "MarineGUI:readTiff: " << filename << endl;

  bool ok = cmviewer->readTiff(filename);

  if(!ok) 
    ok = cmviewer->readTiff("");

  if(ok)
    cmviewer->setTexture();
}  

//-------------------------------------------------------------------
// Procedure: readTiffB

void MarineGUI::readTiffB(string filename)
{
  cmviewer->readTiffB(filename);
}  

//----------------------------------------------------------
// Procedure: addGrid

void MarineGUI::addGrid(XYGrid new_grid)
{
  if(cmviewer) {
    cmviewer->addGrid(new_grid);
    grid_labels.push_back(new_grid.getLabel());
  }
}

//----------------------------------------------------------
// Procedure: addPoly

void MarineGUI::addPoly(XYPolygon new_poly)
{
  if(cmviewer)
    cmviewer->addPoly(new_poly);
}

//----------------------------------------------------------
// Procedure: addCircle

void MarineGUI::addCircle(XYCircle new_circ)
{
  if(cmviewer)
    cmviewer->addCircle(new_circ);
}

//----------------------------------------- Zoom In
inline void MarineGUI::cb_Zoom_i(int val) {
  if(val < 0) cmviewer->setParam("zoom", 1.05);
  if(val > 0) cmviewer->setParam("zoom", 0.80);
  if(val ==0) cmviewer->setParam("zoom", "reset");
}
void MarineGUI::cb_Zoom(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_Zoom_i(v);
}

//----------------------------------------- Pan Y
inline void MarineGUI::cb_PanY_i(int amt) {
  cmviewer->setParam("pan_y", ((float)(amt))/10);
  this->updateXY();
}
void MarineGUI::cb_PanY(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_PanY_i(v);
}


//----------------------------------------- Pan X
inline void MarineGUI::cb_PanX_i(int amt) {
  cmviewer->setParam("pan_x", ((float)(amt))/10);
  this->updateXY();
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
  cmviewer->setParam("hash_shade", (float)(amt)/20.0);
  cmviewer->redraw();
}
void MarineGUI::cb_HashShade(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_HashShade_i(v);
}

//----------------------------------------- ToggleDatum
inline void MarineGUI::cb_ToggleDatum_i() {
  cmviewer->setParam("display_datum", "toggle");
  cmviewer->redraw();
}
void MarineGUI::cb_ToggleDatum(Fl_Widget* o) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_ToggleDatum_i();
}

//----------------------------------------- ToggleGrids
inline void MarineGUI::cb_ToggleGrids_i() {
  cmviewer->setParam("display_grids", "toggle");
  cmviewer->redraw();
}
void MarineGUI::cb_ToggleGrids(Fl_Widget* o) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_ToggleGrids_i();
}

//----------------------------------------- DatumSize
inline void MarineGUI::cb_DatumSize_i(int amt) {
  cmviewer->setParam("datum_size", (float)(amt));
  cmviewer->redraw();
}
void MarineGUI::cb_DatumSize(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_DatumSize_i(v);
}

//----------------------------------------- HashDelta
inline void MarineGUI::cb_HashDelta_i(int amt) {
  cmviewer->setParam("hash_delta", (float)(amt));
  cmviewer->redraw();
}
void MarineGUI::cb_HashDelta(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_HashDelta_i(v);
}

//----------------------------------------- BackShade
inline void MarineGUI::cb_BackShade_i(int amt) {
  cmviewer->setParam("back_shade", (float)(amt)/20.0);
  cmviewer->redraw();
}
void MarineGUI::cb_BackShade(Fl_Widget* o, int v) {
  ((MarineGUI*)(o->parent()->user_data()))->cb_BackShade_i(v);
}

//----------------------------------------- Quit
void MarineGUI::cb_Quit() {
  exit(0);
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


