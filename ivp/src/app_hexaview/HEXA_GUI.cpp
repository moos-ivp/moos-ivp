/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HEXA_GUI.cpp                                         */
/*    DATE: Oct 30, 2005                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "HEXA_GUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

HEXA_GUI::HEXA_GUI(int g_w, int g_h, const char *g_l)
  : MarineGUI(g_w, g_h, g_l) {

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  int info_size=10;

  hviewer  = new HexaViewer(0, 30, w(), h()-100);
  cmviewer = hviewer;

  augmentMenu();

  p_snap = new MY_Output(60, h()-60, 70, 20, "snap:"); 
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

void HEXA_GUI::augmentMenu() 
{
  mbar->add("Polygons/Delete All", 0, (Fl_Callback*)HEXA_GUI::cb_Clear, (void*)0, 0);
  mbar->add("Polygons/Delete Current", FL_CTRL+'d', (Fl_Callback*)HEXA_GUI::cb_ClearActive, (void*)0, 0);
  mbar->add("Polygons/Current --", '-', (Fl_Callback*)HEXA_GUI::cb_AdjustActive, (void*)-1, 0);
  mbar->add("Polygons/Current ++", '+', (Fl_Callback*)HEXA_GUI::cb_AdjustActive, (void*)1, 0);
  mbar->add("Polygons/Move Right", FL_SHIFT + FL_Right, (Fl_Callback*)HEXA_GUI::cb_ShiftX, (void*)10, 0);
  mbar->add("Polygons/Move Left",  FL_SHIFT + FL_Left, (Fl_Callback*)HEXA_GUI::cb_ShiftX, (void*)-10, 0);
  mbar->add("Polygons/Move Up",    FL_SHIFT + FL_Up, (Fl_Callback*)HEXA_GUI::cb_ShiftY, (void*)10, 0);
  mbar->add("Polygons/Move Down",  FL_SHIFT + FL_Down, (Fl_Callback*)HEXA_GUI::cb_ShiftY, (void*)-10, FL_MENU_DIVIDER);
  mbar->add("Polygons/Move Right (slow)", FL_SHIFT + FL_ALT + FL_Right, (Fl_Callback*)HEXA_GUI::cb_ShiftX, (void*)1, 0);
  mbar->add("Polygons/Move Left (slow)",  FL_SHIFT + FL_ALT + FL_Left, (Fl_Callback*)HEXA_GUI::cb_ShiftX, (void*)-1, 0);
  mbar->add("Polygons/Move Up (slow)",    FL_SHIFT + FL_ALT + FL_Up, (Fl_Callback*)HEXA_GUI::cb_ShiftY, (void*)1, 0);
  mbar->add("Polygons/Move Down (slow)",  FL_SHIFT + FL_ALT + FL_Down, (Fl_Callback*)HEXA_GUI::cb_ShiftY, (void*)-1, FL_MENU_DIVIDER);
  mbar->add("Polygons/Toggle View",  ' ', (Fl_Callback*)HEXA_GUI::cb_ToggleView, (void*)0, FL_MENU_DIVIDER);
  
  mbar->add("SnapValue/10.0 meters", 0, (Fl_Callback*)HEXA_GUI::cb_SnapVal, (void*)1000, FL_MENU_RADIO|FL_MENU_VALUE);
  mbar->add("SnapValue/ 5.0 meters", 0, (Fl_Callback*)HEXA_GUI::cb_SnapVal, (void*)500, FL_MENU_RADIO);
  mbar->add("SnapValue/ 2.0 meters", 0, (Fl_Callback*)HEXA_GUI::cb_SnapVal, (void*)200, FL_MENU_RADIO);
  mbar->add("SnapValue/ 1.0 meters", 0, (Fl_Callback*)HEXA_GUI::cb_SnapVal, (void*)100, FL_MENU_RADIO);
  mbar->add("SnapValue/ 0.1 meters", 0, (Fl_Callback*)HEXA_GUI::cb_SnapVal, (void*)10, FL_MENU_RADIO);
  mbar->add("SnapValue/ 0.01 meters", 0, (Fl_Callback*)HEXA_GUI::cb_SnapVal, (void*)1, FL_MENU_RADIO);
  mbar->add("SnapValue/ 0.00 meters", 0, (Fl_Callback*)HEXA_GUI::cb_SnapVal, (void*)0, FL_MENU_RADIO);
  mbar->add("SnapValue/Re-Apply To Current", 0, (Fl_Callback*)HEXA_GUI::cb_ReApplySnapVal, (void*)0, 0);

  mbar->add("Output/Dump Cur Outer Hex", '>', (Fl_Callback*)HEXA_GUI::cb_DumpPolySpec, (void*)0, 0);
  mbar->add("Output/Dump Cur Inner Hex", '<', (Fl_Callback*)HEXA_GUI::cb_DumpPolySpec, (void*)1, 0);

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

int HEXA_GUI::handle(int event) 
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

//----------------------------------------- Clear
inline void HEXA_GUI::cb_Clear_i() {
  hviewer->setParam("clear", "polygons");
  updateXY();
  hviewer->redraw();
}
void HEXA_GUI::cb_Clear(Fl_Widget* o) {
  ((HEXA_GUI*)(o->parent()->user_data()))->cb_Clear_i();
}

//----------------------------------------- ClearActive
inline void HEXA_GUI::cb_ClearActive_i() {
  hviewer->clearActivePoly();
  updateXY();
  hviewer->redraw();
}
void HEXA_GUI::cb_ClearActive(Fl_Widget* o) {
  ((HEXA_GUI*)(o->parent()->user_data()))->cb_ClearActive_i();
}

//----------------------------------------- AdjustActive
inline void HEXA_GUI::cb_AdjustActive_i(int v) {
  hviewer->adjustActive(v);
  updateXY();
  hviewer->redraw();
}
void HEXA_GUI::cb_AdjustActive(Fl_Widget* o, int v) {
  ((HEXA_GUI*)(o->parent()->user_data()))->cb_AdjustActive_i(v);
}

//----------------------------------------- ShiftX
inline void HEXA_GUI::cb_ShiftX_i(int amt) {
  hviewer->shiftHorzPoly((float)(amt));
  updateXY();
  hviewer->redraw();
}
void HEXA_GUI::cb_ShiftX(Fl_Widget* o, int v) {
  ((HEXA_GUI*)(o->parent()->user_data()))->cb_ShiftX_i(v);
}

//----------------------------------------- ShiftY
inline void HEXA_GUI::cb_ShiftY_i(int amt) {
  hviewer->shiftVertPoly((float)(amt));
  updateXY();
  hviewer->redraw();
}
void HEXA_GUI::cb_ShiftY(Fl_Widget* o, int v) {
  ((HEXA_GUI*)(o->parent()->user_data()))->cb_ShiftY_i(v);
}

//----------------------------------------- SnapVal
inline void HEXA_GUI::cb_SnapVal_i(int v) {
  hviewer->setSnap(v/100.0);
  updateXY();
  hviewer->redraw();
}
void HEXA_GUI::cb_SnapVal(Fl_Widget* o, int v) {
  ((HEXA_GUI*)(o->parent()->user_data()))->cb_SnapVal_i(v);
}

//----------------------------------------- ToggleView
inline void HEXA_GUI::cb_ToggleView_i() {
  hviewer->toggleView();
}
void HEXA_GUI::cb_ToggleView(Fl_Widget* o) {
  ((HEXA_GUI*)(o->parent()->user_data()))->cb_ToggleView_i();
}

//----------------------------------------- ReApplySnapVal
inline void HEXA_GUI::cb_ReApplySnapVal_i() {
  hviewer->reApplySnapToCurrent();
  updateXY();
  hviewer->redraw();
}
void HEXA_GUI::cb_ReApplySnapVal(Fl_Widget* o) {
  ((HEXA_GUI*)(o->parent()->user_data()))->cb_ReApplySnapVal_i();
}

//----------------------------------------- dumpPolySpec
void HEXA_GUI::cb_DumpPolySpec_i(int v) {
  string str = hviewer->getPolySpec(v);
  cout << "SPEC: " << str << endl;
}
void HEXA_GUI::cb_DumpPolySpec(Fl_Widget* o, int v) {
  ((HEXA_GUI*)(o->parent()->user_data()))->cb_DumpPolySpec_i(v);
}
  
//----------------------------------------- UpdateXY
void HEXA_GUI::updateXY() {
  
  int index = -1;
  char buff[64];

  // Polygon Spec
  string str = hviewer->getPolySpec(0);
  p_spec->value(str.c_str());

  // Snap Value
  sprintf(buff, "%f", hviewer->getSnap());
  p_snap->value(buff);
}
