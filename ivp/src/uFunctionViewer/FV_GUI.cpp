/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: FV_GUI.cpp                                           */
/*    DATE: May 13th 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "FV_GUI.h"

using namespace std;

//--------------------------------------------------------------
// Constructor

FV_GUI::FV_GUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) 
{
  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  mbar = new Fl_Menu_Bar(0, 0, w(), 25);
  mbar->menu(menu_);
    
  int info_size=10;

  viewer = new FV_Viewer(0, 30, w(), h());

  this->end();
  this->resizable(this);
  this->show();
}

Fl_Menu_Item FV_GUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)FV_GUI::cb_Quit, 0, 0},
 {0},

 {"Pan/Zoom", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Rotate X- ", FL_Down,  (Fl_Callback*)FV_GUI::cb_RotateX, (void*)-1, 0},
 {"Rotate X+ ", FL_Up,  (Fl_Callback*)FV_GUI::cb_RotateX, (void*)1, 0},
 {"Rotate Y- ", 'y',  (Fl_Callback*)FV_GUI::cb_RotateY, (void*)-1, 0},
 {"Rotate Y+ ", 'Y',  (Fl_Callback*)FV_GUI::cb_RotateY, (void*)1, 0},
 {"Rotate Z- ", FL_Left,  (Fl_Callback*)FV_GUI::cb_RotateZ, (void*)-1, 0},
 {"Rotate Z+ ", FL_Right,  (Fl_Callback*)FV_GUI::cb_RotateZ, (void*)1, 0},
 {"Reset ",             ' ',  (Fl_Callback*)FV_GUI::cb_Reset, (void*)0, 0},
 {"ZSlice + ",  ')',  (Fl_Callback*)FV_GUI::cb_ModZSlice, (void*)1, 0},
 {"ZSlice - ",  '(',  (Fl_Callback*)FV_GUI::cb_ModZSlice, (void*)-1,0},
 {"Toggle Frame ",  'f',  (Fl_Callback*)FV_GUI::cb_ToggleFrame, (void*)-1, FL_MENU_DIVIDER},
 {"Stretch X ", ']',  (Fl_Callback*)FV_GUI::cb_StretchX, (void*)1, 0},
 {"Shrink  X ", '[',  (Fl_Callback*)FV_GUI::cb_StretchX, (void*)-1, 0},
 {"Stretch Y ", '}',  (Fl_Callback*)FV_GUI::cb_StretchY, (void*)1, 0},
 {"Shrink  Y ", '{',  (Fl_Callback*)FV_GUI::cb_StretchY, (void*)-1, 0},

 {"Zoom In",          'i', (Fl_Callback*)FV_GUI::cb_Zoom, (void*)-1, 0},
 {"Zoom Out",         'o', (Fl_Callback*)FV_GUI::cb_Zoom, (void*)1, 0},
 {"Zoom Reset",       'Z', (Fl_Callback*)FV_GUI::cb_Zoom, (void*)0, 0},
 {"Pan Up ",          0,  (Fl_Callback*)FV_GUI::cb_PanY, (void*)20, 0},
 {"Pan Down ",        0,  (Fl_Callback*)FV_GUI::cb_PanY, (void*)-20, 0},
 {"Pan Left ",        0,  (Fl_Callback*)FV_GUI::cb_PanX, (void*)-20, 0},
 {"Pan Right ",       0,  (Fl_Callback*)FV_GUI::cb_PanX, (void*)20, 0},
 {0},

 {"IPF", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Base +", 'e', (Fl_Callback*)FV_GUI::cb_ModBaseIPF, (void*)+1, 0},
 {"Base -", 'r', (Fl_Callback*)FV_GUI::cb_ModBaseIPF, (void*)-1, FL_MENU_DIVIDER},
 {0},

 {"Color-Map", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Default",   0, (Fl_Callback*)FV_GUI::cb_ColorMap, (void*)1, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Copper",    0, (Fl_Callback*)FV_GUI::cb_ColorMap, (void*)2, FL_MENU_RADIO},
 {"Bone",      0, (Fl_Callback*)FV_GUI::cb_ColorMap, (void*)3, FL_MENU_RADIO|FL_MENU_DIVIDER},
 {"Intensity -", '-', (Fl_Callback*)FV_GUI::cb_ModIntensity, (void*)-2, 0},
 {"Intensity +", '+', (Fl_Callback*)FV_GUI::cb_ModIntensity, (void*)2, 0},
  {0},

 {"Polar", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Default", 0, (Fl_Callback*)FV_GUI::cb_ModPolar, (void*)0, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Polar-0", 0, (Fl_Callback*)FV_GUI::cb_ModPolar, (void*)1, FL_MENU_RADIO},
 {"Polar-1", 0, (Fl_Callback*)FV_GUI::cb_ModPolar, (void*)2, FL_MENU_RADIO},
 {0},
 {0}
};

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

int FV_GUI::handle(int event) 
{
  bool catch_key = false;
  switch(event) {
  case FL_KEYBOARD:
    if(Fl::event_key()==FL_Down) {
      cb_RotateX_i(-1);
      return(1); 
    }
    if(Fl::event_key()==FL_Up) {
      cb_RotateX_i(+1);
      return(1); 
    }
    if(Fl::event_key()==FL_Right) {
      cb_RotateZ_i(+1);
      return(1); 
    }
    if(Fl::event_key()==FL_Left) {
      cb_RotateZ_i(-1);
      return(1); 
    }
    if(Fl::event_key()==FL_CTRL+FL_Left) {
      return(1); 
    }
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------- Zoom In
inline void FV_GUI::cb_Zoom_i(int val) {
  if(val < 0) viewer->modZoom(1.25);
  if(val > 0) viewer->modZoom(0.80);
  //if(val ==0) viewer->zoomReset();
}
void FV_GUI::cb_Zoom(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_Zoom_i(v);
}

//----------------------------------------- Reset
inline void FV_GUI::cb_Reset_i() {
  viewer->reset();
}
void FV_GUI::cb_Reset(Fl_Widget* o) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_Reset_i();
}

//----------------------------------------- Rotate  X
inline void FV_GUI::cb_RotateX_i(int amt) {
  viewer->rotateX(amt);
}
void FV_GUI::cb_RotateX(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_RotateX_i(v);
}

//----------------------------------------- Rotate  Y
inline void FV_GUI::cb_RotateY_i(int amt) {
  viewer->rotateY(amt);
}
void FV_GUI::cb_RotateY(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_RotateY_i(v);
}

//----------------------------------------- Rotate  Z
inline void FV_GUI::cb_RotateZ_i(int amt) {
  viewer->rotateZ(amt);
}
void FV_GUI::cb_RotateZ(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_RotateZ_i(v);
}

//----------------------------------------- Stretch X
inline void FV_GUI::cb_StretchX_i(int amt) {
  if(amt > 0) viewer->stretchX(1.25);
  if(amt < 0) viewer->stretchX(0.80);
}
void FV_GUI::cb_StretchX(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_StretchX_i(v);
}

//----------------------------------------- Stretch Y
inline void FV_GUI::cb_StretchY_i(int amt) {
  if(amt > 0) viewer->stretchY(1.25);
  if(amt < 0) viewer->stretchY(0.80);
}
void FV_GUI::cb_StretchY(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_StretchY_i(v);
}

//----------------------------------------- Pan Y
inline void FV_GUI::cb_PanY_i(int amt) {
  //viewer->panY(amt);
}
void FV_GUI::cb_PanY(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_PanY_i(v);
}


//----------------------------------------- Pan X
inline void FV_GUI::cb_PanX_i(int amt) {
  //viewer->panX(amt);
}
void FV_GUI::cb_PanX(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_PanX_i(v);
}

//----------------------------------------- Mod BaseIPF
inline void FV_GUI::cb_ModBaseIPF_i(int amt) {
  viewer->modBaseIPF(amt);
}

void FV_GUI::cb_ModBaseIPF(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ModBaseIPF_i(v);
}

//----------------------------------------- Toggle Frame
inline void FV_GUI::cb_ToggleFrame_i() {
  viewer->toggleFrame();
}
void FV_GUI::cb_ToggleFrame(Fl_Widget* o) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ToggleFrame_i();
}

//----------------------------------------- ColorMap
inline void FV_GUI::cb_ColorMap_i(int index) {
  string str = "default";
  if(index ==2)
    str = "copper";
  else if(index == 3)
    str = "bone";
  viewer->modColorMap(str);
}
void FV_GUI::cb_ColorMap(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ColorMap_i(v);
}

//----------------------------------------- ModIntensity
inline void FV_GUI::cb_ModIntensity_i(int amt) {
  float f_amt = (float)(amt) / 100.0;
  viewer->modIntensity(f_amt);
}
void FV_GUI::cb_ModIntensity(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ModIntensity_i(v);
}

//----------------------------------------- ModPolar
inline void FV_GUI::cb_ModPolar_i(int val) {
  viewer->modPolar(val);
}
void FV_GUI::cb_ModPolar(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ModPolar_i(v);
}

//----------------------------------------- ModZSlice
inline void FV_GUI::cb_ModZSlice_i(int amt) {
  viewer->modZSlice(amt);
}
void FV_GUI::cb_ModZSlice(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ModZSlice_i(v);
}

//----------------------------------------- Quit
void FV_GUI::cb_Quit() {
  exit(0);
}




