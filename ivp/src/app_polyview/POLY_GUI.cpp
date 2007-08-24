/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: POLY_GUI.cpp                                         */
/*    DATE: May 1st, 2005                                        */
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
#include "POLY_GUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

POLY_GUI::POLY_GUI(int g_w, int g_h, const char *g_l)
  : MarineGUI(g_w, g_h, g_l) {

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  int info_size=10;

  pviewer  = new PolyViewer(0, 30, w(), h()-100);
  cmviewer = pviewer;

  augmentMenu();

  x_mtr = new MY_Output(60, h()-60, 70, 20, "meters x:"); 
  x_mtr->textsize(info_size); 
  x_mtr->labelsize(info_size);

  y_mtr = new MY_Output(200, h()-60, 70, 20, "meters y:"); 
  y_mtr->textsize(info_size); 
  y_mtr->labelsize(info_size);

  p_mode = new MY_Output(340, h()-60, 70, 20, "mode:"); 
  p_mode->textsize(info_size); 
  p_mode->labelsize(info_size);

  p_snap = new MY_Output(480, h()-60, 70, 20, "snap:"); 
  p_snap->textsize(info_size); 
  p_snap->labelsize(info_size);

  p_spec = new MY_Output(60, h()-30, w()-70, 20, "polygon:"); 
  p_spec->textsize(info_size); 
  p_spec->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
// Procedure: augmentMenu

void POLY_GUI::augmentMenu() 
{
  mbar->add("EditMode/Append Vertex", 'p', (Fl_Callback*)POLY_GUI::cb_DropMode, (void*)0, FL_MENU_RADIO|FL_MENU_VALUE);
  mbar->add("EditMode/Edit Vertex",   'e', (Fl_Callback*)POLY_GUI::cb_DropMode, (void*)1, FL_MENU_RADIO);
  mbar->add("EditMode/Delete Vertex", 'd', (Fl_Callback*)POLY_GUI::cb_DropMode, (void*)2, FL_MENU_RADIO);
  mbar->add("EditMode/Insert Vertex", 'a', (Fl_Callback*)POLY_GUI::cb_DropMode, (void*)3, FL_MENU_RADIO);
  mbar->add("EditMode/Test Mode4",    '4', (Fl_Callback*)POLY_GUI::cb_DropMode, (void*)4, FL_MENU_RADIO);
  mbar->add("EditMode/Test Mode5",    '5', (Fl_Callback*)POLY_GUI::cb_DropMode, (void*)5, FL_MENU_RADIO);
  mbar->add("EditMode/Test Mode6",    '6', (Fl_Callback*)POLY_GUI::cb_DropMode, (void*)6, FL_MENU_RADIO);
  mbar->add("EditMode/Test Mode7",    '7', (Fl_Callback*)POLY_GUI::cb_DropMode, (void*)7, FL_MENU_RADIO);
  mbar->add("EditMode/Test Mode8",    '8', (Fl_Callback*)POLY_GUI::cb_DropMode, (void*)8, FL_MENU_RADIO);
  mbar->add("EditMode/Test Mode9",    '9', (Fl_Callback*)POLY_GUI::cb_DropMode, (void*)9, FL_MENU_RADIO);
  mbar->add("EditMode/Mode8Angle+",   ')', (Fl_Callback*)POLY_GUI::cb_Mode8Angle, (void*)+1, FL_MENU_RADIO);
  mbar->add("EditMode/Mode8Angle-",   '(', (Fl_Callback*)POLY_GUI::cb_Mode8Angle, (void*)-1, FL_MENU_RADIO);
  mbar->add("EditMode/Center View",   'c', (Fl_Callback*)POLY_GUI::cb_DropMode, (void*)-1, FL_MENU_RADIO);
  
  mbar->add("Polygons/Delete All", 0, (Fl_Callback*)POLY_GUI::cb_Clear, (void*)0, 0);
  mbar->add("Polygons/Delete Current", FL_CTRL+'d', (Fl_Callback*)POLY_GUI::cb_ClearActive, (void*)0, 0);
  mbar->add("Polygons/Current --", '-', (Fl_Callback*)POLY_GUI::cb_AdjustActive, (void*)-1, 0);
  mbar->add("Polygons/Current ++", '+', (Fl_Callback*)POLY_GUI::cb_AdjustActive, (void*)1, 0);
  mbar->add("Polygons/DumpSpec",   '>', (Fl_Callback*)POLY_GUI::cb_DumpPolySpec, (void*)0, 0);
  mbar->add("Polygons/Duplicate",  '2', (Fl_Callback*)POLY_GUI::cb_Duplicate, (void*)0, FL_MENU_DIVIDER);
  mbar->add("Polygons/Move Right", FL_SHIFT + FL_Right, (Fl_Callback*)POLY_GUI::cb_ShiftX, (void*)10, 0);
  mbar->add("Polygons/Move Left",  FL_SHIFT + FL_Left, (Fl_Callback*)POLY_GUI::cb_ShiftX, (void*)-10, 0);
  mbar->add("Polygons/Move Up",    FL_SHIFT + FL_Up, (Fl_Callback*)POLY_GUI::cb_ShiftY, (void*)10, 0);
  mbar->add("Polygons/Move Down",  FL_SHIFT + FL_Down, (Fl_Callback*)POLY_GUI::cb_ShiftY, (void*)-10, FL_MENU_DIVIDER);
  mbar->add("Polygons/Move Right (slow)", FL_SHIFT + FL_ALT + FL_Right, (Fl_Callback*)POLY_GUI::cb_ShiftX, (void*)1, 0);
  mbar->add("Polygons/Move Left (slow)",  FL_SHIFT + FL_ALT + FL_Left, (Fl_Callback*)POLY_GUI::cb_ShiftX, (void*)-1, 0);
  mbar->add("Polygons/Move Up (slow)",    FL_SHIFT + FL_ALT + FL_Up, (Fl_Callback*)POLY_GUI::cb_ShiftY, (void*)1, 0);
  mbar->add("Polygons/Move Down (slow)",  FL_SHIFT + FL_ALT + FL_Down, (Fl_Callback*)POLY_GUI::cb_ShiftY, (void*)-1, FL_MENU_DIVIDER);
  mbar->add("Polygons/Rotate Right", '}', (Fl_Callback*)POLY_GUI::cb_Rotate, (void*)1, 0);
  mbar->add("Polygons/Rotate Left",  '{', (Fl_Callback*)POLY_GUI::cb_Rotate, (void*)-1, 0);
  mbar->add("Polygons/Enlarge", ']', (Fl_Callback*)POLY_GUI::cb_Grow, (void*)1, 0);
  mbar->add("Polygons/Shrink",  '[', (Fl_Callback*)POLY_GUI::cb_Grow, (void*)-1, 0);
  mbar->add("Polygons/Reverse", 'R', (Fl_Callback*)POLY_GUI::cb_Reverse, (void*)0, 0);
  
  mbar->add("SnapValue/200.0 meters", 0, (Fl_Callback*)POLY_GUI::cb_SnapVal, (void*)20000, FL_MENU_RADIO);
  mbar->add("SnapValue/100.0 meters", 0, (Fl_Callback*)POLY_GUI::cb_SnapVal, (void*)10000, FL_MENU_RADIO);
  mbar->add("SnapValue/50.0 meters", 0, (Fl_Callback*)POLY_GUI::cb_SnapVal, (void*)5000, FL_MENU_RADIO);
  mbar->add("SnapValue/10.0 meters", 0, (Fl_Callback*)POLY_GUI::cb_SnapVal, (void*)1000, FL_MENU_RADIO|FL_MENU_VALUE);
  mbar->add("SnapValue/ 5.0 meters", 0, (Fl_Callback*)POLY_GUI::cb_SnapVal, (void*)500, FL_MENU_RADIO);
  mbar->add("SnapValue/ 2.0 meters", 0, (Fl_Callback*)POLY_GUI::cb_SnapVal, (void*)200, FL_MENU_RADIO);
  mbar->add("SnapValue/ 1.0 meters", 0, (Fl_Callback*)POLY_GUI::cb_SnapVal, (void*)100, FL_MENU_RADIO);
  mbar->add("SnapValue/ 0.1 meters", 0, (Fl_Callback*)POLY_GUI::cb_SnapVal, (void*)10, FL_MENU_RADIO);
  mbar->add("SnapValue/ 0.01 meters", 0, (Fl_Callback*)POLY_GUI::cb_SnapVal, (void*)1, FL_MENU_RADIO);
  mbar->add("SnapValue/ 0.00 meters", 0, (Fl_Callback*)POLY_GUI::cb_SnapVal, (void*)0, FL_MENU_RADIO);
  mbar->add("SnapValue/Re-Apply To Current", 0, (Fl_Callback*)POLY_GUI::cb_ReApplySnapVal, (void*)0, 0);
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

int POLY_GUI::handle(int event) 
{
  int step = 1;
  switch(event) {
  case FL_PUSH:
    Fl_Window::handle(event);
    updateXY();
    return(1);
    break;
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------------------------
// Procedure: addCircle

void POLY_GUI::addCircle(XYCircle new_circle)
{
  if(pviewer)
    pviewer->addCircle(new_circle);
}

//----------------------------------------------------------
// Procedure: addArc

void POLY_GUI::addArc(XYArc new_arc)
{
  if(pviewer)
    pviewer->addArc(new_arc);
}

//----------------------------------------- Clear
inline void POLY_GUI::cb_Clear_i() {
  pviewer->setParam("clear", "polygons");
  updateXY();
  pviewer->redraw();
}
void POLY_GUI::cb_Clear(Fl_Widget* o) {
  ((POLY_GUI*)(o->parent()->user_data()))->cb_Clear_i();
}

//----------------------------------------- ClearActive
inline void POLY_GUI::cb_ClearActive_i() {
  pviewer->clearActivePoly();
  updateXY();
  pviewer->redraw();
}
void POLY_GUI::cb_ClearActive(Fl_Widget* o) {
  ((POLY_GUI*)(o->parent()->user_data()))->cb_ClearActive_i();
}

//----------------------------------------- AdjustActive
inline void POLY_GUI::cb_AdjustActive_i(int v) {
  pviewer->adjustActive(v);
  updateXY();
  pviewer->redraw();
}
void POLY_GUI::cb_AdjustActive(Fl_Widget* o, int v) {
  ((POLY_GUI*)(o->parent()->user_data()))->cb_AdjustActive_i(v);
}

//----------------------------------------- DropMode
inline void POLY_GUI::cb_DropMode_i(int v) {
  pviewer->setMode(v);
  updateXY();
  pviewer->redraw();
}
void POLY_GUI::cb_DropMode(Fl_Widget* o, int v) {
  ((POLY_GUI*)(o->parent()->user_data()))->cb_DropMode_i(v);
}

//----------------------------------------- Rotate
inline void POLY_GUI::cb_Rotate_i(int v) {
  pviewer->rotatePoly(v);
  updateXY();
  pviewer->redraw();
}
void POLY_GUI::cb_Rotate(Fl_Widget* o, int v) {
  ((POLY_GUI*)(o->parent()->user_data()))->cb_Rotate_i(v);
}

//----------------------------------------- ShiftX
inline void POLY_GUI::cb_ShiftX_i(int amt) {
  pviewer->shiftHorzPoly((float)(amt));
  updateXY();
  pviewer->redraw();
}
void POLY_GUI::cb_ShiftX(Fl_Widget* o, int v) {
  ((POLY_GUI*)(o->parent()->user_data()))->cb_ShiftX_i(v);
}

//----------------------------------------- ShiftY
inline void POLY_GUI::cb_ShiftY_i(int amt) {
  pviewer->shiftVertPoly((float)(amt));
  updateXY();
  pviewer->redraw();
}
void POLY_GUI::cb_ShiftY(Fl_Widget* o, int v) {
  ((POLY_GUI*)(o->parent()->user_data()))->cb_ShiftY_i(v);
}

//----------------------------------------- Grow
inline void POLY_GUI::cb_Grow_i(int v) {
  pviewer->growPoly(v);
  updateXY();
  pviewer->redraw();
}
void POLY_GUI::cb_Grow(Fl_Widget* o, int v) {
  ((POLY_GUI*)(o->parent()->user_data()))->cb_Grow_i(v);
}

//----------------------------------------- Reverse
inline void POLY_GUI::cb_Reverse_i() {
  pviewer->reversePoly();
  updateXY();
  pviewer->redraw();
}
void POLY_GUI::cb_Reverse(Fl_Widget* o) {
  ((POLY_GUI*)(o->parent()->user_data()))->cb_Reverse_i();
}

//----------------------------------------- SnapVal
inline void POLY_GUI::cb_SnapVal_i(int v) {
  pviewer->setSnap(v/100.0);
  updateXY();
  pviewer->redraw();
}
void POLY_GUI::cb_SnapVal(Fl_Widget* o, int v) {
  ((POLY_GUI*)(o->parent()->user_data()))->cb_SnapVal_i(v);
}

//----------------------------------------- SnapVal
inline void POLY_GUI::cb_Mode8Angle_i(int v) {
  pviewer->adjustMode8Angle(v);
  pviewer->redraw();
}
void POLY_GUI::cb_Mode8Angle(Fl_Widget* o, int v) {
  ((POLY_GUI*)(o->parent()->user_data()))->cb_Mode8Angle_i(v);
}

//----------------------------------------- ReApplySnapVal
inline void POLY_GUI::cb_ReApplySnapVal_i() {
  pviewer->reApplySnapToCurrent();
  updateXY();
  pviewer->redraw();
}
void POLY_GUI::cb_ReApplySnapVal(Fl_Widget* o) {
  ((POLY_GUI*)(o->parent()->user_data()))->cb_ReApplySnapVal_i();
}

//----------------------------------------- Duplicate
inline void POLY_GUI::cb_Duplicate_i() {
  pviewer->duplicateActive();
  pviewer->redraw();
}
void POLY_GUI::cb_Duplicate(Fl_Widget* o) {
  ((POLY_GUI*)(o->parent()->user_data()))->cb_Duplicate_i();
}

//----------------------------------------- dumpPolySpec
void POLY_GUI::cb_DumpPolySpec_i() {
  string str = pviewer->getPolySpec();
  cout << "SPEC: " << str << endl;
}
void POLY_GUI::cb_DumpPolySpec(Fl_Widget* o) {
  ((POLY_GUI*)(o->parent()->user_data()))->cb_DumpPolySpec_i();
}
  
//----------------------------------------- UpdateXY
void POLY_GUI::updateXY() {
  
  int index = -1;
  char buff[64];

  // Meters X
  string x_str = doubleToString(pviewer->getMetersX(index), 2);
  //string x_str = doubleToString(pviewer->getImgPctX(index), 6);
  x_mtr->value(x_str.c_str());

  // Meters Y
  string y_str = doubleToString(pviewer->getMetersY(index), 2);
  //string y_str = doubleToString(pviewer->getImgPctY(index), 6);
  y_mtr->value(y_str.c_str());

  // Polygon Spec
  string str = pviewer->getPolySpec();
  p_spec->value(str.c_str());

  // Polygon Mode
  int mode = pviewer->getMode();
  string dmode = "Unknown";
  if(mode == 0) dmode = "Place";
  if(mode == 1) dmode = "Edit";
  if(mode == 2) dmode = "Delete";
  if(mode == 3) dmode = "Insert";
  if(mode == 4) dmode = "TestTypeA";
  if(mode == 5) dmode = "TestTypeB";
  if(mode == -1) dmode = "CenterView";
  p_mode->value(dmode.c_str());

  // Snap Value
  sprintf(buff, "%f", pviewer->getSnap());
  p_snap->value(buff);
}

