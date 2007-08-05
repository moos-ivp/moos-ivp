/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CommonMarineGUI.cpp                                  */
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

#include "CommonMarineGUI.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

CommonMarineGUI::CommonMarineGUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) 
{
  mbar = new Fl_Menu_Bar(0, 0, w(), 25);
  mbar->menu(menu_);
    
  cmviewer = 0;
}

//-------------------------------------------------------------------

Fl_Menu_Item CommonMarineGUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)CommonMarineGUI::cb_Quit, 0, 0},
 {0},

 {"BackView", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Zoom In",          'i', (Fl_Callback*)CommonMarineGUI::cb_Zoom, (void*)-1, 0},
 {"Zoom Out",         'o', (Fl_Callback*)CommonMarineGUI::cb_Zoom, (void*)1, 0},
 {"Zoom Reset",       FL_CTRL+'z', (Fl_Callback*)CommonMarineGUI::cb_Zoom, (void*)0, FL_MENU_DIVIDER},
 {"Pan Up ",          FL_Up,  (Fl_Callback*)CommonMarineGUI::cb_PanY, (void*)-20, 0},
 {"Pan Down ",        FL_Down,  (Fl_Callback*)CommonMarineGUI::cb_PanY, (void*)20, 0},
 {"Pan Left ",        FL_Left,  (Fl_Callback*)CommonMarineGUI::cb_PanX, (void*)20, 0},
 {"Pan Right ",       FL_Right,  (Fl_Callback*)CommonMarineGUI::cb_PanX, (void*)-20, FL_MENU_DIVIDER},
 {"Pan Up (slow) ",   FL_ALT + FL_Up, (Fl_Callback*)CommonMarineGUI::cb_PanY, (void*)-1, 0},
 {"Pan Down (slow) ", FL_ALT + FL_Down, (Fl_Callback*)CommonMarineGUI::cb_PanY, (void*)1, 0},
 {"Pan Left (slow) ", FL_ALT + FL_Left, (Fl_Callback*)CommonMarineGUI::cb_PanX, (void*)1, 0},
 {"Pan Right (slow)", FL_ALT + FL_Right, (Fl_Callback*)CommonMarineGUI::cb_PanX, (void*)-1, FL_MENU_DIVIDER},
 {"Cross Hairs",      'c', (Fl_Callback*)CommonMarineGUI::cb_ToggleCross, (void*)-1, FL_MENU_DIVIDER},
 {"Toggle Background",  'b', (Fl_Callback*)CommonMarineGUI::cb_ToggleTiff, (void*)-1, 0},
 {"Background Lighter", FL_CTRL+'b', (Fl_Callback*)CommonMarineGUI::cb_BackShade,  (void*)+1, 0},
 {"Background Darker",  FL_ALT +'b', (Fl_Callback*)CommonMarineGUI::cb_BackShade,  (void*)-1, FL_MENU_DIVIDER},
 {"Toggle Hash",      'h', (Fl_Callback*)CommonMarineGUI::cb_ToggleHash, (void*)-1, 0},
 {"Hash Lighter",     FL_CTRL+'h', (Fl_Callback*)CommonMarineGUI::cb_HashShade,  (void*)+1, 0},
 {"Hash Darker",      FL_ALT +'h', (Fl_Callback*)CommonMarineGUI::cb_HashShade,  (void*)-1, FL_MENU_DIVIDER},
 {"Hash 10",      FL_ALT+'1', (Fl_Callback*)CommonMarineGUI::cb_HashDelta,  (void*)10,  FL_MENU_RADIO},
 {"Hash 50",      FL_ALT+'2', (Fl_Callback*)CommonMarineGUI::cb_HashDelta,  (void*)50,  FL_MENU_RADIO},
 {"Hash 100",     FL_ALT+'3', (Fl_Callback*)CommonMarineGUI::cb_HashDelta,  (void*)100, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Hash 200",     FL_ALT+'4', (Fl_Callback*)CommonMarineGUI::cb_HashDelta,  (void*)200, FL_MENU_RADIO},
 {0},
 {0}
};

//----------------------------------------------------------
// Procedure: handle
//      Note: As it stands, this method could be eliminated entirely, and the 
//            default behavior of the parent class should work fine. But if
//            we want to tinker with event handling, this method is the place.

int CommonMarineGUI::handle(int event) 
{
  return(Fl_Window::handle(event));
}

//-------------------------------------------------------------------
// Procedure: readTiff

void CommonMarineGUI::readTiff(const char *filename)
{
  bool result = cmviewer->readTiff(filename);

  if(!result) 
    cmviewer->readTiff();

  cmviewer->setTexture();
}  

//----------------------------------------------------------
// Procedure: addGrid

void CommonMarineGUI::addGrid(XYGrid new_grid)
{
  if(cmviewer) {
    cmviewer->addGrid(new_grid);
    grid_labels.push_back(new_grid.getLabel());
  }
}

//----------------------------------------------------------
// Procedure: addPoly

void CommonMarineGUI::addPoly(XYPolygon new_poly)
{
  if(cmviewer)
    cmviewer->addPoly(new_poly);
}

//----------------------------------------------------------
// Procedure: addCircle

void CommonMarineGUI::addCircle(XYCircle new_circ)
{
  if(cmviewer)
    cmviewer->addCircle(new_circ);
}

//----------------------------------------- Zoom In
inline void CommonMarineGUI::cb_Zoom_i(int val) {
  if(val < 0) cmviewer->zoom(1.05);
  if(val > 0) cmviewer->zoom(0.80);
  if(val ==0) cmviewer->zoomReset();
}
void CommonMarineGUI::cb_Zoom(Fl_Widget* o, int v) {
  ((CommonMarineGUI*)(o->parent()->user_data()))->cb_Zoom_i(v);
}

//----------------------------------------- Pan Y
inline void CommonMarineGUI::cb_PanY_i(int amt) {
  cmviewer->panY((float)(amt));
  this->updateXY();
  cmviewer->redraw();
  this->redraw();
}
void CommonMarineGUI::cb_PanY(Fl_Widget* o, int v) {
  ((CommonMarineGUI*)(o->parent()->user_data()))->cb_PanY_i(v);
}


//----------------------------------------- Pan X
inline void CommonMarineGUI::cb_PanX_i(int amt) {
  cmviewer->panX((float)(amt));
  this->updateXY();
}
void CommonMarineGUI::cb_PanX(Fl_Widget* o, int v) {
  ((CommonMarineGUI*)(o->parent()->user_data()))->cb_PanX_i(v);
}

//----------------------------------------- ToggleTiff
inline void CommonMarineGUI::cb_ToggleTiff_i() {
  cmviewer->toggleTiffView();
  cmviewer->redraw();
}
void CommonMarineGUI::cb_ToggleTiff(Fl_Widget* o) {
  ((CommonMarineGUI*)(o->parent()->user_data()))->cb_ToggleTiff_i();
}

//----------------------------------------- ToggleHash
inline void CommonMarineGUI::cb_ToggleHash_i() {
  cmviewer->toggleHashView();
  cmviewer->redraw();
}
void CommonMarineGUI::cb_ToggleHash(Fl_Widget* o) {
  ((CommonMarineGUI*)(o->parent()->user_data()))->cb_ToggleHash_i();
}

//----------------------------------------- HashShade
inline void CommonMarineGUI::cb_HashShade_i(int amt) {
  cmviewer->hashShade((float)(amt)/20.0);
  cmviewer->redraw();
}
void CommonMarineGUI::cb_HashShade(Fl_Widget* o, int v) {
  ((CommonMarineGUI*)(o->parent()->user_data()))->cb_HashShade_i(v);
}

//----------------------------------------- HashDelta
inline void CommonMarineGUI::cb_HashDelta_i(int amt) {
  cmviewer->hashDelta((float)(amt));
  cmviewer->redraw();
}
void CommonMarineGUI::cb_HashDelta(Fl_Widget* o, int v) {
  ((CommonMarineGUI*)(o->parent()->user_data()))->cb_HashDelta_i(v);
}

//----------------------------------------- BackShade
inline void CommonMarineGUI::cb_BackShade_i(int amt) {
  cmviewer->backShade((float)(amt)/20.0);
  cmviewer->redraw();
}
void CommonMarineGUI::cb_BackShade(Fl_Widget* o, int v) {
  ((CommonMarineGUI*)(o->parent()->user_data()))->cb_BackShade_i(v);
}

//----------------------------------------- Quit
void CommonMarineGUI::cb_Quit() {
  exit(0);
}

//----------------------------------------- ToggleCross
inline void CommonMarineGUI::cb_ToggleCross_i() {
  cmviewer->toggleCrossView();
  cmviewer->redraw();
  updateXY();
}
void CommonMarineGUI::cb_ToggleCross(Fl_Widget* o) {
  ((CommonMarineGUI*)(o->parent()->user_data()))->cb_ToggleCross_i();
}

