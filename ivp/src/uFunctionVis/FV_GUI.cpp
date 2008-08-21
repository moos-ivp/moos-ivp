/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: FV_GUI.cpp                                          */
/*    DATE: May 13th 2006                                       */
/****************************************************************/

#include <iostream>
#include <stdlib.h>
#include "FV_GUI.h"
#include "MBUtils.h"

using namespace std;

//--------------------------------------------------------------
// Constructor

FV_GUI::FV_GUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) 
{
  model = 0;

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  mbar = new Fl_Menu_Bar(0, 0, w(), 25);
  mbar->menu(menu_);
    
  int info_size=10;

  viewer = new FV_Viewer(0, 30, w(), h()-60);

  curr_iteration = new MY_Output(60, h()-25, 50, 20, "Iteration:"); 
  curr_iteration->textsize(info_size); 
  curr_iteration->labelsize(info_size);

  curr_func_size = new MY_Output(150, h()-25, 50, 20, "Pieces:"); 
  curr_func_size->textsize(info_size); 
  curr_func_size->labelsize(info_size);

  curr_function = new MY_Output(300, h()-25, 200, 20, "Current Function:"); 
  curr_function->textsize(info_size); 
  curr_function->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();
  this->updateFields();
}

Fl_Menu_Item FV_GUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)FV_GUI::cb_Quit, 0, 0},
 {0},

 {"Pan/Zoom", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Rotate X- ", FL_Down,  (Fl_Callback*)FV_GUI::cb_RotateX, (void*)-1, 0},
 {"Rotate X+ ", FL_Up,  (Fl_Callback*)FV_GUI::cb_RotateX, (void*)1, 0},
 {"Rotate Z- ", FL_Left,  (Fl_Callback*)FV_GUI::cb_RotateZ, (void*)-1, 0},
 {"Rotate Z+ ", FL_Right,  (Fl_Callback*)FV_GUI::cb_RotateZ, (void*)1, 0},
 {"Reset ",          '1',  (Fl_Callback*)FV_GUI::cb_Reset, (void*)1, 0},
 {"Reset 2",         '2',  (Fl_Callback*)FV_GUI::cb_Reset, (void*)2, 0},
 {"Reset 3",         '3',  (Fl_Callback*)FV_GUI::cb_Reset, (void*)3, 0},
 {"Toggle Frame ",   'f',  (Fl_Callback*)FV_GUI::cb_ToggleFrame, (void*)-1, FL_MENU_DIVIDER},
 {"Expand Radius ",  '}',  (Fl_Callback*)FV_GUI::cb_StretchRad, (void*)1, 0},
 {"Shrink Radius ",  '{',  (Fl_Callback*)FV_GUI::cb_StretchRad, (void*)-1, 0},
 {"Base Higher ",    'B',  (Fl_Callback*)FV_GUI::cb_ModBase, (void*)1, 0},
 {"Base Lower ",     'b',  (Fl_Callback*)FV_GUI::cb_ModBase, (void*)-1, 0},
 {"Scale Higher ",   'S',  (Fl_Callback*)FV_GUI::cb_ModScale, (void*)1, 0},
 {"Scale Lower ",    's',  (Fl_Callback*)FV_GUI::cb_ModScale, (void*)-1, 0},
 {"Zoom In",         'i', (Fl_Callback*)FV_GUI::cb_Zoom, (void*)-1, 0},
 {"Zoom Out",        'o', (Fl_Callback*)FV_GUI::cb_Zoom, (void*)1, 0},
 {"Zoom Reset", FL_ALT+'Z', (Fl_Callback*)FV_GUI::cb_Zoom, (void*)0, 0},
 {0},

 {"Color-Map", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Default",   0, (Fl_Callback*)FV_GUI::cb_ColorMap, (void*)1, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Copper",    0, (Fl_Callback*)FV_GUI::cb_ColorMap, (void*)2, FL_MENU_RADIO},
 {"Bone",      0, (Fl_Callback*)FV_GUI::cb_ColorMap, (void*)3, FL_MENU_RADIO|FL_MENU_DIVIDER},
 {"ColorRangeLow -", 'z', (Fl_Callback*)FV_GUI::cb_ModColorRange, (void*)0, 0},
 {"ColorRangeLow +", 'Z', (Fl_Callback*)FV_GUI::cb_ModColorRange, (void*)1, 0},
 {"ColorRangeHigh -", 'x', (Fl_Callback*)FV_GUI::cb_ModColorRange, (void*)2, 0},
 {"ColorRangeHigh +", 'X', (Fl_Callback*)FV_GUI::cb_ModColorRange, (void*)3, 0},
 {"ColorRangeReset +", FL_ALT+'x', (Fl_Callback*)FV_GUI::cb_ModColorRange, (void*)4, 0},
 {0},

 {"Function-Select", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Increment",   ' ', (Fl_Callback*)FV_GUI::cb_IncCurrFunction, (void*)1, 0},
 {"Decrement",   FL_ALT+' ',  (Fl_Callback*)FV_GUI::cb_IncCurrFunction, (void*)-1, 0},
 {"Collective",  'c',  (Fl_Callback*)FV_GUI::cb_ToggleCollectiveView, (void*)0, 0},
 {"Lock/UnLock", 'l', (Fl_Callback*)FV_GUI::cb_ToggleLockIPF, (void*)2, 0},
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
  if(val < 0) viewer->setParam("mod_zoom", 1.25);
  if(val > 0) viewer->setParam("mod_zoom", 0.80);
}
void FV_GUI::cb_Zoom(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_Zoom_i(v);
}

//----------------------------------------- Reset
inline void FV_GUI::cb_Reset_i(int val) {
  if(val==1)  viewer->setParam("reset_view", "1");
  if(val==2)  viewer->setParam("reset_view", "2");
  if(val==3)  viewer->setParam("reset_view", "3");
}
void FV_GUI::cb_Reset(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_Reset_i(v);
}

//----------------------------------------- Rotate  X
inline void FV_GUI::cb_RotateX_i(int amt) {
  viewer->setParam("mod_x_rotation", (double)(amt));
}
void FV_GUI::cb_RotateX(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_RotateX_i(v);
}

//----------------------------------------- Rotate  Z
inline void FV_GUI::cb_RotateZ_i(int amt) {
  viewer->setParam("mod_z_rotation", (double)(amt));
}
void FV_GUI::cb_RotateZ(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_RotateZ_i(v);
}

//----------------------------------------- Stretch Radius
inline void FV_GUI::cb_StretchRad_i(int amt) {
  if(amt > 0) viewer->setParam("mod_radius", 1.25);
  if(amt < 0) viewer->setParam("mod_radius", 0.80);
}
void FV_GUI::cb_StretchRad(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_StretchRad_i(v);
}

//----------------------------------------- Mod Base
inline void FV_GUI::cb_ModBase_i(int amt) {
  if(amt > 0) viewer->setParam("mod_base",  10);
  if(amt < 0) viewer->setParam("mod_base", -10);
}
void FV_GUI::cb_ModBase(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ModBase_i(v);
}

//----------------------------------------- Mod Scale
inline void FV_GUI::cb_ModScale_i(int amt) {
  if(amt > 0) viewer->setParam("mod_scale", 1.25);
  if(amt < 0) viewer->setParam("mod_scale", 0.80);
}
void FV_GUI::cb_ModScale(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ModScale_i(v);
}

//----------------------------------------- Toggle Frame
inline void FV_GUI::cb_ToggleFrame_i() {
  viewer->setParam("draw_frame", "toggle");
}
void FV_GUI::cb_ToggleFrame(Fl_Widget* o) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ToggleFrame_i();
}

//----------------------------------------- ToggleLockIPF
inline void FV_GUI::cb_ToggleLockIPF_i() {
  model->toggleLockIPF();
}
void FV_GUI::cb_ToggleLockIPF(Fl_Widget* o) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ToggleLockIPF_i();
}

//----------------------------------------- ColorMap
inline void FV_GUI::cb_ColorMap_i(int index) {
  string str = "default";
  if(index ==2)
    str = "copper";
  else if(index == 3)
    str = "bone";
  model->modColorMap(str);
  viewer->redraw();
}
void FV_GUI::cb_ColorMap(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ColorMap_i(v);
}

//----------------------------------------- ModColorRange
inline void FV_GUI::cb_ModColorRange_i(int index) {
  if(index == 0) model->modLowAdjust(1);
  if(index == 1) model->modLowAdjust(-1);
  if(index == 2) model->modHighAdjust(1);
  if(index == 3) model->modHighAdjust(-1);
  if(index == 4) model->resetAdjust();
}
void FV_GUI::cb_ModColorRange(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ModColorRange_i(v);
}

//----------------------------------------- IncCurrFunction
inline void FV_GUI::cb_IncCurrFunction_i(int amt) {
  if(!model)
    return;
  if(amt > 0)
    model->incDescriptor();
  else
    model->decDescriptor();
  updateFields();
}
void FV_GUI::cb_IncCurrFunction(Fl_Widget* o, int v) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_IncCurrFunction_i(v);
}

//----------------------------------------- ToggleCollectiveView
inline void FV_GUI::cb_ToggleCollectiveView_i() {
  if(!model)
    return;
  
  model->toggleCollective();
  updateFields();
}
void FV_GUI::cb_ToggleCollectiveView(Fl_Widget* o) {
  ((FV_GUI*)(o->parent()->user_data()))->cb_ToggleCollectiveView_i();
}

//----------------------------------------- UpdateFields
void FV_GUI::updateFields() 
{
  if(!model)
    return;

  string descriptor;
  if(model)
    descriptor = model->getCurrDescriptor();
  if(descriptor == "")
    descriptor = " - no function - ";
  curr_function->value(descriptor.c_str());

  string func_size;
  if(model)
    func_size = model->getCurrFuncSize();

  string iteration = intToString(model->getCurrIteration());
  if(model->isLocked())
    iteration = "<" + iteration + ">";
  curr_iteration->value(iteration.c_str());

  viewer->redraw();
}

//----------------------------------------- Quit
void FV_GUI::cb_Quit() {
  exit(0);
}

