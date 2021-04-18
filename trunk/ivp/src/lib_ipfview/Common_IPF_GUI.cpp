/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Common_IPF_GUI.cpp                                   */
/*    DATE: June 24th 2016                                       */
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
#include "Common_IPF_GUI.h"
#include "MBUtils.h"

using namespace std;

//--------------------------------------------------------------
// Constructor

Common_IPF_GUI::Common_IPF_GUI(int wid, int hgt, const char *label)
  : Fl_Window(wid, hgt, label) 
{
  m_menubar = new Fl_Menu_Bar(0, 0, w(), 25);
  augmentMenu();
  setMenuColors();
  
  m_start_hgt = hgt;
  m_start_wid = wid;

  m_viewer = 0;
}

//--------------------------------------------------------------
// Procedure: setViewerParam()

bool Common_IPF_GUI::setViewerParam(string param, string value)
{
  if(m_viewer)
    return(m_viewer->setParam(param, value));
  return(false);
}

//--------------------------------------------------------------
// Procedure: setViewerParam()

bool Common_IPF_GUI::setViewerParam(string param, double value)
{
  if(m_viewer)
    return(m_viewer->setParam(param, value));
  return(true);
}

//--------------------------------------------------------------
// Procedure: augmentMenu()

void Common_IPF_GUI::augmentMenu()
{
  //===========================================================
  // File Menu 
  //===========================================================
  m_menubar->add("File/Quit ", FL_CTRL+'q',
		 (Fl_Callback*)Common_IPF_GUI::cb_Quit, 0, 0);

  //===========================================================
  // Rotate Zoom Menu
  //===========================================================
  m_menubar->add("RotateZoom/Rotate X- ", FL_Down,
		 (Fl_Callback*)Common_IPF_GUI::cb_RotateX,(void*)-1, 0);
  m_menubar->add("RotateZoom/Rotate X+ ", FL_Up,
		 (Fl_Callback*)Common_IPF_GUI::cb_RotateX, (void*)1, 0);
  m_menubar->add("RotateZoom/Rotate Z- ", FL_Left,
		 (Fl_Callback*)Common_IPF_GUI::cb_RotateZ, (void*)-1, 0);
  m_menubar->add("RotateZoom/Rotate Z+ ", FL_Right,
		 (Fl_Callback*)Common_IPF_GUI::cb_RotateZ, (void*)1, 0);
  m_menubar->add("RotateZoom/Reset1 ", '1',
		 (Fl_Callback*)Common_IPF_GUI::cb_Reset, (void*)1, 0);
  m_menubar->add("RotateZoom/Reset2 ", '2',
		 (Fl_Callback*)Common_IPF_GUI::cb_Reset, (void*)2, 0);
  m_menubar->add("RotateZoom/Reset3 ", '3',
		 (Fl_Callback*)Common_IPF_GUI::cb_Reset, (void*)3, 0);
  m_menubar->add("RotateZoom/Reset4 ", '=',
		 (Fl_Callback*)Common_IPF_GUI::cb_ModFrameBaseIPF, (void*)99, 0);
  m_menubar->add("RotateZoom/Toggle Frame ", 'f',
		 (Fl_Callback*)Common_IPF_GUI::cb_ToggleFrame, (void*)-1,
		 FL_MENU_DIVIDER);
  m_menubar->add("RotateZoom/FrameHgt--",FL_ALT+'f',
		 (Fl_Callback*)Common_IPF_GUI::cb_FrameHgt, (void*)-1, 0);
  m_menubar->add("RotateZoom/FrameHgt++",FL_CTRL+'f',
		 (Fl_Callback*)Common_IPF_GUI::cb_FrameHgt, (void*)1,
		 FL_MENU_DIVIDER);
  m_menubar->add("RotateZoom/Frame Shade Lighter", '<',
		 (Fl_Callback*)Common_IPF_GUI::cb_FrameShade, (void*)-1, 0);
  m_menubar->add("RotateZoom/Frame Shade Darker",  '>',
		 (Fl_Callback*)Common_IPF_GUI::cb_FrameShade, (void*)1,
		 FL_MENU_DIVIDER);
  m_menubar->add("RotateZoom/Zoom In",  'i',
		 (Fl_Callback*)Common_IPF_GUI::cb_Zoom, (void*)-1, 0);
  m_menubar->add("RotateZoom/Zoom Out",  'o',
		 (Fl_Callback*)Common_IPF_GUI::cb_Zoom, (void*)1, 0);
  m_menubar->add("RotateZoom/Scale +", 'S',
		 (Fl_Callback*)Common_IPF_GUI::cb_ModScale, (void*)+2, 0);
  m_menubar->add("RotateZoom/Scale -", 's',
		 (Fl_Callback*)Common_IPF_GUI::cb_ModScale, (void*)-2, 0);
  
  //===========================================================
  // IPF Menu
  //===========================================================
  m_menubar->add("IPF/Toggle Function ",   'F',
		 (Fl_Callback*)Common_IPF_GUI::cb_ToggleIPF, (void*)-1,
		 FL_MENU_DIVIDER);
  m_menubar->add("IPF/FunctionBase -", '(',
		 (Fl_Callback*)Common_IPF_GUI::cb_ModBaseIPF, (void*)-10);
  m_menubar->add("IPF/FunctionBase +", ')',
		 (Fl_Callback*)Common_IPF_GUI::cb_ModBaseIPF, (void*)+10,
		 FL_MENU_DIVIDER);
  m_menubar->add("IPF/Frame and FunctionBase -", '{',
		 (Fl_Callback*)Common_IPF_GUI::cb_ModFrameBaseIPF, (void*)-10);
  m_menubar->add("IPF/Frame and FunctionBase +", '}',
		 (Fl_Callback*)Common_IPF_GUI::cb_ModFrameBaseIPF, (void*)+10,
		 FL_MENU_DIVIDER);
  m_menubar->add("IPF/Toggle Pieces", 'p',
		 (Fl_Callback*)Common_IPF_GUI::cb_ToggleDrawPcs, (void*)5,
		 FL_MENU_DIVIDER);
  m_menubar->add("IPF/Polar0", 0,
		 (Fl_Callback*)Common_IPF_GUI::cb_Polar, (void*)0, 0);
  m_menubar->add("IPF/Polar1", 0,
		 (Fl_Callback*)Common_IPF_GUI::cb_Polar, (void*)1, 0);
  m_menubar->add("IPF/Polar2", 0,
		 (Fl_Callback*)Common_IPF_GUI::cb_Polar, (void*)2,
		 FL_MENU_DIVIDER);
  m_menubar->add("IPF/Render Lines", 'l',
		 (Fl_Callback*)Common_IPF_GUI::cb_TogglePieceLines, (void*)0,
		 FL_MENU_DIVIDER);

  //===========================================================
  // IPF Menu
  //===========================================================
  m_menubar->add("Ship/DrawShip Toggle", 'h',
		 (Fl_Callback*)Common_IPF_GUI::cb_ToggleDrawShip, (void*)0, 0);
  m_menubar->add("Ship/ShipScale--", 'k',
		 (Fl_Callback*)Common_IPF_GUI::cb_ModShipScale, (void*)90, 0);
  m_menubar->add("Ship/ShipScale++", 'j',
		 (Fl_Callback*)Common_IPF_GUI::cb_ModShipScale, (void*)125, 0);

  //===========================================================
  // ColorMap Menu
  //===========================================================
  m_menubar->add("Color-Map/Default", 0,
		 (Fl_Callback*)Common_IPF_GUI::cb_ColorMap, (void*)1,
		 FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("Color-Map/Copper", 0,
		 (Fl_Callback*)Common_IPF_GUI::cb_ColorMap, (void*)2,
		 FL_MENU_RADIO);
  m_menubar->add("Color-Map/Bone", 0,
		 (Fl_Callback*)Common_IPF_GUI::cb_ColorMap, (void*)3,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);
  m_menubar->add("Color-Map/Back-White", 'w',
		 (Fl_Callback*)Common_IPF_GUI::cb_ColorBack, (void*)0, 0);
  m_menubar->add("Color-Map/Back-Blue",  'B',
		 (Fl_Callback*)Common_IPF_GUI::cb_ColorBack, (void*)1, 0);
  m_menubar->add("Color-Map/Mac-Beige", 'b',
		 (Fl_Callback*)Common_IPF_GUI::cb_ColorBack,(void*)2, 0);
}

//----------------------------------------------------------------
// Procedure: setMenuColors()

void Common_IPF_GUI::setMenuColors()
{
  setMenuItemColor("File/Quit ");
  setMenuItemColor("RotateZoom/Rotate X- ");
  setMenuItemColor("RotateZoom/Rotate X+ ");
  setMenuItemColor("RotateZoom/Rotate Z- ");
  setMenuItemColor("RotateZoom/Rotate Z+ ");
  setMenuItemColor("RotateZoom/Reset1 ");
  setMenuItemColor("RotateZoom/Reset2 ");
  setMenuItemColor("RotateZoom/Reset3 ");
  setMenuItemColor("RotateZoom/Reset4 ");
  setMenuItemColor("RotateZoom/Toggle Frame ");
  setMenuItemColor("RotateZoom/FrameHgt--");
  setMenuItemColor("RotateZoom/FrameHgt++");
  setMenuItemColor("RotateZoom/Zoom In");
  setMenuItemColor("RotateZoom/Zoom Out");
  setMenuItemColor("RotateZoom/Zoom Reset");
  setMenuItemColor("RotateZoom/Scale +");
  setMenuItemColor("RotateZoom/Scale -");
  setMenuItemColor("IPF/Toggle Function ");
  setMenuItemColor("IPF/Base +");
  setMenuItemColor("IPF/Base -");
  setMenuItemColor("IPF/Polar0");
  setMenuItemColor("IPF/Polar1");
  setMenuItemColor("IPF/Polar2");
  setMenuItemColor("IPF/Render Lines");
  setMenuItemColor("Color-Map/Default");
  setMenuItemColor("Color-Map/Copper");
  setMenuItemColor("Color-Map/Bone");
  setMenuItemColor("Color-Map/Back-White");
  setMenuItemColor("Color-Map/Back-Blue");
  setMenuItemColor("Color-Map/Mac-Beige");
  setMenuItemColor("Ship/DrawShip Toggle");
  setMenuItemColor("Ship/ShipScale--");
  setMenuItemColor("Ship/ShipScale++");
}


//--------------------------------------------------------------
// Procedure: setMenuItemColor()

void Common_IPF_GUI::setMenuItemColor(string item_str)
{
  Fl_Color new_color = fl_rgb_color(31, 71, 155);
  const Fl_Menu_Item *item = m_menubar->find_item(item_str.c_str());
  if(item)
    ((Fl_Menu_Item *)item)->labelcolor(new_color);
}

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

int Common_IPF_GUI::handle(int event) 
{
  switch(event) {
  case FL_KEYDOWN:
    if(Fl::event_key() == FL_Down)
      cb_RotateX_i(-1);
    else if(Fl::event_key() == FL_Up)
      cb_RotateX_i(1);
    else if(Fl::event_key() == FL_Left)
      cb_RotateZ_i(-1);
    else if(Fl::event_key() == FL_Right)
      cb_RotateZ_i(1);
    else
      return(0);
    return(1);
    break;
  default:
    return(0);
  }
}


//----------------------------------------- Zoom In
inline void Common_IPF_GUI::cb_Zoom_i(int val) {
  if(val < 0)
    m_viewer->setParam("mod_zoom", 1.25);
  if(val > 0)
    m_viewer->setParam("mod_zoom", 0.80);
}
void Common_IPF_GUI::cb_Zoom(Fl_Widget* o, int v) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_Zoom_i(v);
}

//----------------------------------------- Reset
inline void Common_IPF_GUI::cb_Reset_i(int i) {
  if(i==1)
    m_viewer->setParam("reset_view", "1");
  else if(i==2)
    m_viewer->setParam("reset_view", "2");
  else if(i==3)
    m_viewer->setParam("reset_view", "3");
  else if(i==4)
    m_viewer->setParam("reset_view", "4");
}
void Common_IPF_GUI::cb_Reset(Fl_Widget* o, int i) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_Reset_i(i);
}


//----------------------------------------- Rotate  X
inline void Common_IPF_GUI::cb_RotateX_i(int amt) {
  m_viewer->setParam("mod_x_rotation", (double)(amt));
}
void Common_IPF_GUI::cb_RotateX(Fl_Widget* o, int v) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_RotateX_i(v);
}

//----------------------------------------- Rotate  Z
inline void Common_IPF_GUI::cb_RotateZ_i(int amt) {
  //cout << "In Common_IPF_GUI::cb_RotateZ_i" << endl;
  m_viewer->setParam("mod_z_rotation", (double)(amt));
}
void Common_IPF_GUI::cb_RotateZ(Fl_Widget* o, int v) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_RotateZ_i(v);
}

//----------------------------------------- Mod Scale
inline void Common_IPF_GUI::cb_ModScale_i(int amt) {
  m_viewer->setParam("mod_scale", (((double)amt)/100.0));
}
void Common_IPF_GUI::cb_ModScale(Fl_Widget* o, int v) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_ModScale_i(v);
}

//----------------------------------------- Mod Ship Scale
inline void Common_IPF_GUI::cb_ModShipScale_i(int amt) {
  m_viewer->setParam("mod_ship_scale", (((double)amt)/100.0));
}
void Common_IPF_GUI::cb_ModShipScale(Fl_Widget* o, int v) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_ModShipScale_i(v);
}

//----------------------------------------- Mod BaseIPF
inline void Common_IPF_GUI::cb_ModBaseIPF_i(int amt) {
  m_viewer->setParam("mod_base_ipf", amt);
}

void Common_IPF_GUI::cb_ModBaseIPF(Fl_Widget* o, int v) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_ModBaseIPF_i(v);
}

//----------------------------------------- Mod FrameBaseIPF
inline void Common_IPF_GUI::cb_ModFrameBaseIPF_i(int amt) {
  if(amt == 99)
    m_viewer->setParam("toggle_frame_on_top", amt);    
  else {
    m_viewer->setParam("mod_base_ipf", amt);
    m_viewer->setParam("mod_base_frame", amt);
  }
}

void Common_IPF_GUI::cb_ModFrameBaseIPF(Fl_Widget* o, int v) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_ModFrameBaseIPF_i(v);
}

//----------------------------------------- Toggle Frame
inline void Common_IPF_GUI::cb_ToggleFrame_i() {
  m_viewer->setParam("draw_frame", "toggle");
}
void Common_IPF_GUI::cb_ToggleFrame(Fl_Widget* o) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_ToggleFrame_i();
}

//----------------------------------------- Toggle Draw IPF
inline void Common_IPF_GUI::cb_ToggleIPF_i() {
  m_viewer->setParam("draw_ipf", "toggle");
  m_viewer->redraw();
}
void Common_IPF_GUI::cb_ToggleIPF(Fl_Widget* o) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_ToggleIPF_i();
}

//----------------------------------------- Toggle Draw Ship
inline void Common_IPF_GUI::cb_ToggleDrawShip_i() {
  m_viewer->setParam("draw_ship", "toggle");
  m_viewer->redraw();
}
void Common_IPF_GUI::cb_ToggleDrawShip(Fl_Widget* o) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_ToggleDrawShip_i();
}

//----------------------------------------- Toggle Piece Lines
inline void Common_IPF_GUI::cb_TogglePieceLines_i() {
  m_viewer->setParam("draw_pclines", "toggle");
  m_viewer->redraw();
}
void Common_IPF_GUI::cb_TogglePieceLines(Fl_Widget* o) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_TogglePieceLines_i();
}

//----------------------------------------- Toggle Draw Pieces
inline void Common_IPF_GUI::cb_ToggleDrawPcs_i() {
  m_viewer->setParam("draw_pieces", "toggle");
  m_viewer->redraw();
  updateXY();
}
void Common_IPF_GUI::cb_ToggleDrawPcs(Fl_Widget* o) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_ToggleDrawPcs_i();
}

//----------------------------------------- Frame Height
inline void Common_IPF_GUI::cb_FrameHgt_i(int amt) {
  m_viewer->setParam("mod_frame_height", (double)amt);
}
void Common_IPF_GUI::cb_FrameHgt(Fl_Widget* o, int v) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_FrameHgt_i(v);
}

//----------------------------------------- FrameShade
inline void Common_IPF_GUI::cb_FrameShade_i(int amt) {
  if(amt < 0)
    m_viewer->setParam("frame_color", "lighter");
  else
    m_viewer->setParam("frame_color", "darker");
}
void Common_IPF_GUI::cb_FrameShade(Fl_Widget* o, int v) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_FrameShade_i(v);
}

//----------------------------------------- ColorMap
inline void Common_IPF_GUI::cb_ColorMap_i(int index) {
  string str = "default";
  if(index ==2)
    str = "copper";
  else if(index == 3)
    str = "bone";
  m_viewer->setColorMap(str);
  m_viewer->redraw();
}
void Common_IPF_GUI::cb_ColorMap(Fl_Widget* o, int v) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_ColorMap_i(v);
}

//----------------------------------------- ColorBack
inline void Common_IPF_GUI::cb_ColorBack_i(int index) {
  if(index == 0)
    m_viewer->setParam("clear_color", "white");
  else if(index == 1)  // PurplishBlue
    m_viewer->setParam("clear_color", "0.285,0.242,0.469");
  else if(index == 2)  // PurplishBlue
    m_viewer->setParam("clear_color", "macbeige");
  else
    return;
  m_viewer->redraw();
}
void Common_IPF_GUI::cb_ColorBack(Fl_Widget* o, int v) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_ColorBack_i(v);
}

//----------------------------------------- cb_Polar
inline void Common_IPF_GUI::cb_Polar_i(int index) {
  if((index >= 0) && (index <=2))
    m_viewer->setParam("polar", index);
  else
    return;
  m_viewer->redraw();
}
void Common_IPF_GUI::cb_Polar(Fl_Widget* o, int v) {
  ((Common_IPF_GUI*)(o->parent()->user_data()))->cb_Polar_i(v);
}

//----------------------------------------- Quit
void Common_IPF_GUI::cb_Quit() {
  exit(0);
}

