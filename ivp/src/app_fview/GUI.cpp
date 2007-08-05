/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GUI.cpp                                              */
/*    DATE: Apr 15th 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "GUI.h"

using namespace std;

//--------------------------------------------------------------
// Constructor

GUI::GUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) 
{
  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  mbar = new Fl_Menu_Bar(0, 0, w(), 25);
  mbar->menu(menu_);
    
  int info_size=10;

  viewer = new Viewer(0, 30, w(), h());

  this->end();
  this->resizable(this);
  this->show();
}

Fl_Menu_Item GUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)GUI::cb_Quit, 0, 0},
 {0},

 {"Pan/Zoom", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Rotate X- ", FL_Down,  (Fl_Callback*)GUI::cb_RotateX, (void*)-1, 0},
 {"Rotate X+ ", FL_Up,  (Fl_Callback*)GUI::cb_RotateX, (void*)1, 0},
 {"Rotate Y- ", 'y',  (Fl_Callback*)GUI::cb_RotateY, (void*)-1, 0},
 {"Rotate Y+ ", 'Y',  (Fl_Callback*)GUI::cb_RotateY, (void*)1, 0},
 {"Rotate Z- ", FL_Left,  (Fl_Callback*)GUI::cb_RotateZ, (void*)-1, 0},
 {"Rotate Z+ ", FL_Right,  (Fl_Callback*)GUI::cb_RotateZ, (void*)1, 0},
 {"Reset ",             ' ',  (Fl_Callback*)GUI::cb_Reset, (void*)0, 0},
 {"ZSlice + ",  ')',  (Fl_Callback*)GUI::cb_ModZSlice, (void*)1, 0},
 {"ZSlice - ",  '(',  (Fl_Callback*)GUI::cb_ModZSlice, (void*)-1,0},
 {"Toggle Frame ",  'f',  (Fl_Callback*)GUI::cb_ToggleFrame, (void*)-1, FL_MENU_DIVIDER},
 {"Stretch X ", ']',  (Fl_Callback*)GUI::cb_StretchX, (void*)1, 0},
 {"Shrink  X ", '[',  (Fl_Callback*)GUI::cb_StretchX, (void*)-1, 0},
 {"Stretch Y ", '}',  (Fl_Callback*)GUI::cb_StretchY, (void*)1, 0},
 {"Shrink  Y ", '{',  (Fl_Callback*)GUI::cb_StretchY, (void*)-1, 0},


 {"Zoom In",          'i', (Fl_Callback*)GUI::cb_Zoom, (void*)-1, 0},
 {"Zoom Out",         'o', (Fl_Callback*)GUI::cb_Zoom, (void*)1, 0},
 {"Zoom Reset",       'Z', (Fl_Callback*)GUI::cb_Zoom, (void*)0, 0},
 {"Pan Up ",          0,  (Fl_Callback*)GUI::cb_PanY, (void*)20, 0},
 {"Pan Down ",        0,  (Fl_Callback*)GUI::cb_PanY, (void*)-20, 0},
 {"Pan Left ",        0,  (Fl_Callback*)GUI::cb_PanX, (void*)-20, 0},
 {"Pan Right ",       0,  (Fl_Callback*)GUI::cb_PanX, (void*)20, 0},
 {0},

 {"AOF", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Scale +",     'S', (Fl_Callback*)GUI::cb_ModScale, (void*)+2, 0},
 {"Scale -",     's', (Fl_Callback*)GUI::cb_ModScale, (void*)-2, 0},
 {"Base +",      'B', (Fl_Callback*)GUI::cb_ModBaseAOF, (void*)+1, 0},
 {"Base -",      'b', (Fl_Callback*)GUI::cb_ModBaseAOF, (void*)-1, 0},
 {"AOF Patch +", 'P', (Fl_Callback*)GUI::cb_ModPatchAOF, (void*)+1, 0},
 {"AOF Patch -", 'p', (Fl_Callback*)GUI::cb_ModPatchAOF, (void*)-1, 0},
 {"Toggle AOF",  'a', (Fl_Callback*)GUI::cb_ToggleAOF, (void*)0, 0},
 {0},

 {"IPF", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Toggle IPF ", 'z', (Fl_Callback*)GUI::cb_ToggleIPF,  (void*)0, FL_MENU_RADIO|FL_MENU_DIVIDER},
 {"Base +", 'e', (Fl_Callback*)GUI::cb_ModBaseIPF, (void*)+1, 0},
 {"Base -", 'r', (Fl_Callback*)GUI::cb_ModBaseIPF, (void*)-1, FL_MENU_DIVIDER},
 {"Uniform", 'u', (Fl_Callback*)GUI::cb_ModIndexIPF,(void*)-1, FL_MENU_RADIO},
 {"View 0", '0', (Fl_Callback*)GUI::cb_ModIndexIPF, (void*)0, FL_MENU_RADIO},
 {"View 1", '1', (Fl_Callback*)GUI::cb_ModIndexIPF, (void*)1, FL_MENU_RADIO},
 {"View 2", '2', (Fl_Callback*)GUI::cb_ModIndexIPF, (void*)2, FL_MENU_RADIO},
 {"View 3", '3', (Fl_Callback*)GUI::cb_ModIndexIPF, (void*)3, FL_MENU_RADIO},
 {"View 4", '4', (Fl_Callback*)GUI::cb_ModIndexIPF, (void*)4, FL_MENU_RADIO},
 {"View 5", '5', (Fl_Callback*)GUI::cb_ModIndexIPF, (void*)5, FL_MENU_RADIO},
 {"View 6", '6', (Fl_Callback*)GUI::cb_ModIndexIPF, (void*)6, FL_MENU_RADIO},
 {"View 7", '7', (Fl_Callback*)GUI::cb_ModIndexIPF, (void*)7, FL_MENU_RADIO},
 {"View 8", '8', (Fl_Callback*)GUI::cb_ModIndexIPF, (void*)8, FL_MENU_RADIO},
 {"View 9", '9', (Fl_Callback*)GUI::cb_ModIndexIPF, (void*)9, FL_MENU_RADIO},
 {0},

 {"Uniform-IPF", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Toggle Strict", '`', (Fl_Callback*)GUI::cb_ToggleStrict, (void*)0, FL_MENU_RADIO|FL_MENU_DIVIDER},
 {"Uniform 1",   0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)1, FL_MENU_RADIO},
 {"Uniform 2",   0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)2, FL_MENU_RADIO},
 {"Uniform 3",   0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)3, FL_MENU_RADIO},
 {"Uniform 4",   0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)4, FL_MENU_RADIO},
 {"Uniform 5",   0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)5, FL_MENU_RADIO},
 {"Uniform 7",   0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)7, FL_MENU_RADIO},
 {"Uniform 10",  0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)10, FL_MENU_RADIO},
 {"Uniform 15",  0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)15, FL_MENU_RADIO},
 {"Uniform 20",  0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)20, FL_MENU_RADIO},
 {"Uniform 25",  0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)25, FL_MENU_RADIO},
 {"Uniform 30",  0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)30, FL_MENU_RADIO},
 {"Uniform 40",  0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)40, FL_MENU_RADIO},
 {"Uniform 50",  0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)50, FL_MENU_RADIO},
 {"Uniform 75",  0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)75, FL_MENU_RADIO},
 {"Uniform 100", 0, (Fl_Callback*)GUI::cb_MakeUniform, (void*)100, FL_MENU_RADIO},
 {0},

 {"Color-Map", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Default",   0, (Fl_Callback*)GUI::cb_ColorMap, (void*)1, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Copper",    0, (Fl_Callback*)GUI::cb_ColorMap, (void*)2, FL_MENU_RADIO},
 {"Bone",      0, (Fl_Callback*)GUI::cb_ColorMap, (void*)3, FL_MENU_RADIO|FL_MENU_DIVIDER},
 {"Intensity -", '-', (Fl_Callback*)GUI::cb_ModIntensity, (void*)-2, 0},
 {"Intensity +", '+', (Fl_Callback*)GUI::cb_ModIntensity, (void*)2, 0},
  {0},

 {"Polar", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Default", 0, (Fl_Callback*)GUI::cb_ModPolar, (void*)0, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Polar-0", 0, (Fl_Callback*)GUI::cb_ModPolar, (void*)1, FL_MENU_RADIO},
 {"Polar-1", 0, (Fl_Callback*)GUI::cb_ModPolar, (void*)2, FL_MENU_RADIO},
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

int GUI::handle(int event) 
{
  bool catch_key = false;
  switch(event) {
  case FL_KEYBOARD:
    if(Fl::event_key()==FL_Down) {
      //cb_PanY_i(-20);
      cb_RotateX_i(-1);
      return(1); 
    }
    if(Fl::event_key()==FL_Up) {
      //cb_PanY_i(20);
      cb_RotateX_i(+1);
      return(1); 
    }
    if(Fl::event_key()==FL_Right) {
      //cb_PanX_i(20);
      cb_RotateZ_i(+1);
      return(1); 
    }
    if(Fl::event_key()==FL_Left) {
      //cb_PanX_i(-20);
      cb_RotateZ_i(-1);
      return(1); 
    }
    if(Fl::event_key()==FL_CTRL+FL_Left) {
      //cb_PanX_i(1);
      return(1); 
    }
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------- Zoom In
inline void GUI::cb_Zoom_i(int val) {
  if(val < 0) viewer->modZoom(1.25);
  if(val > 0) viewer->modZoom(0.80);
  //if(val ==0) viewer->zoomReset();
}
void GUI::cb_Zoom(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_Zoom_i(v);
}

//----------------------------------------- Reset
inline void GUI::cb_Reset_i() {
#if 0
  for(int i=0; i<20; i++) {
    cb_RotateZ_i(1);
    sleep(1);
    Fl::flush();
  }
#endif
  viewer->runScript();
  //viewer->reset();
}
void GUI::cb_Reset(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_Reset_i();
}

//----------------------------------------- Rotate  X
inline void GUI::cb_RotateX_i(int amt) {
  viewer->rotateX(amt);
}
void GUI::cb_RotateX(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_RotateX_i(v);
}

//----------------------------------------- Rotate  Y
inline void GUI::cb_RotateY_i(int amt) {
  viewer->rotateY(amt);
}
void GUI::cb_RotateY(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_RotateY_i(v);
}

//----------------------------------------- Rotate  Z
inline void GUI::cb_RotateZ_i(int amt) {
  viewer->rotateZ((double)(amt)/5.0);
}
void GUI::cb_RotateZ(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_RotateZ_i(v);
}

//----------------------------------------- Stretch X
inline void GUI::cb_StretchX_i(int amt) {
  if(amt > 0) viewer->stretchX(1.25);
  if(amt < 0) viewer->stretchX(0.80);
}
void GUI::cb_StretchX(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_StretchX_i(v);
}

//----------------------------------------- Stretch Y
inline void GUI::cb_StretchY_i(int amt) {
  if(amt > 0) viewer->stretchY(1.25);
  if(amt < 0) viewer->stretchY(0.80);
}
void GUI::cb_StretchY(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_StretchY_i(v);
}

//----------------------------------------- Pan Y
inline void GUI::cb_PanY_i(int amt) {
  //viewer->panY(amt);
}
void GUI::cb_PanY(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_PanY_i(v);
}


//----------------------------------------- Pan X
inline void GUI::cb_PanX_i(int amt) {
  //viewer->panX(amt);
}
void GUI::cb_PanX(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_PanX_i(v);
}

//----------------------------------------- Mod Scale
inline void GUI::cb_ModScale_i(int amt) {
  viewer->modScale(((double)amt)/100.0);
}
void GUI::cb_ModScale(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ModScale_i(v);
}

//----------------------------------------- Mod BaseAOF
inline void GUI::cb_ModBaseAOF_i(int amt) {
  viewer->modBaseAOF(amt);
}

void GUI::cb_ModBaseAOF(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ModBaseAOF_i(v);
}

//----------------------------------------- Mod BaseIPF
inline void GUI::cb_ModBaseIPF_i(int amt) {
  viewer->modBaseIPF(amt);
}

void GUI::cb_ModBaseIPF(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ModBaseIPF_i(v);
}

//----------------------------------------- Mod PatchAOF
inline void GUI::cb_ModPatchAOF_i(int amt) {
  viewer->modPatchAOF(amt);
}
void GUI::cb_ModPatchAOF(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ModPatchAOF_i(v);
}

//----------------------------------------- Mod IndexIPF
inline void GUI::cb_ModIndexIPF_i(int ix) {
  viewer->modIndexIPF(ix);
}
void GUI::cb_ModIndexIPF(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ModIndexIPF_i(v);
}

//----------------------------------------- Toggle AOF
inline void GUI::cb_ToggleAOF_i() {
  viewer->toggleAOF();
}
void GUI::cb_ToggleAOF(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_ToggleAOF_i();
}

//----------------------------------------- Toggle IPF
inline void GUI::cb_ToggleIPF_i() {
  viewer->toggleIPF();
}
void GUI::cb_ToggleIPF(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_ToggleIPF_i();
}

//----------------------------------------- Toggle Frame
inline void GUI::cb_ToggleFrame_i() {
  viewer->toggleFrame();
}
void GUI::cb_ToggleFrame(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_ToggleFrame_i();
}

//----------------------------------------- Toggle Strict
inline void GUI::cb_ToggleStrict_i() {
  viewer->toggleStrict();
  viewer->makeUniformIPF();
}
void GUI::cb_ToggleStrict(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_ToggleStrict_i();
}

//----------------------------------------- MakeUniform
inline void GUI::cb_MakeUniform_i(int amt) {
  viewer->makeUniformIPF(amt);
}
void GUI::cb_MakeUniform(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_MakeUniform_i(v);
}

//----------------------------------------- ColorMap
inline void GUI::cb_ColorMap_i(int index) {
  string str = "default";
  if(index ==2)
    str = "copper";
  else if(index == 3)
    str = "bone";
  viewer->modColorMap(str);
}
void GUI::cb_ColorMap(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ColorMap_i(v);
}

//----------------------------------------- ModIntensity
inline void GUI::cb_ModIntensity_i(int amt) {
  float f_amt = (float)(amt) / 100.0;
  viewer->modIntensity(f_amt);
}
void GUI::cb_ModIntensity(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ModIntensity_i(v);
}

//----------------------------------------- ModPolar
inline void GUI::cb_ModPolar_i(int val) {
  viewer->modPolar(val);
}
void GUI::cb_ModPolar(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ModPolar_i(v);
}

//----------------------------------------- ModZSlice
inline void GUI::cb_ModZSlice_i(int amt) {
  viewer->modZSlice(amt);
}
void GUI::cb_ModZSlice(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ModZSlice_i(v);
}

//----------------------------------------- Quit
void GUI::cb_Quit() {
  exit(0);
}
