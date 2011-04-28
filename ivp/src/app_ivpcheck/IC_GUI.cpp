/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: IC_GUI.cpp                                          */
/*    DATE: Apr 19th 2011                                       */
/****************************************************************/

#include <iostream>
#include <cstdlib>
#include "IC_GUI.h"
#include "MBUtils.h"

using namespace std;

//--------------------------------------------------------------
// Constructor

IC_GUI::IC_GUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) 
{
  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  mbar = new Fl_Menu_Bar(0, 0, w(), 25);
  mbar->menu(menu_);
    
  int info_size=10;

  viewer = new IC_Viewer(0, 30, w(), h()-60);

  total_funcs = new MY_Output(90, h()-25, 30, 20, "Total Functions:"); 
  total_funcs->textsize(info_size); 
  total_funcs->labelsize(info_size);

  curr_function = new MY_Output(220, h()-25, 180, 20, "Current Function:"); 
  curr_function->textsize(info_size); 
  curr_function->labelsize(info_size);

  curr_func_size = new MY_Output(450, h()-25, 50, 20, "Pieces:"); 
  curr_func_size->textsize(info_size); 
  curr_func_size->labelsize(info_size);

  curr_func_pwt = new MY_Output(570, h()-25, 50, 20, "Priority:"); 
  curr_func_pwt->textsize(info_size); 
  curr_func_pwt->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();
}



Fl_Menu_Item IC_GUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)IC_GUI::cb_Quit, 0, 0},
 {0},

 {"Pan/Zoom", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Rotate X- ", FL_Down,  (Fl_Callback*)IC_GUI::cb_RotateX, (void*)-1, 0},
 {"Rotate X+ ", FL_Up,  (Fl_Callback*)IC_GUI::cb_RotateX, (void*)1, 0},
 {"Rotate Z- ", FL_Left,  (Fl_Callback*)IC_GUI::cb_RotateZ, (void*)-1, 0},
 {"Rotate Z+ ", FL_Right,  (Fl_Callback*)IC_GUI::cb_RotateZ, (void*)1, 0},
 {"Reset ",          '1',  (Fl_Callback*)IC_GUI::cb_Reset, (void*)1, 0},
 {"Reset 2",         '2',  (Fl_Callback*)IC_GUI::cb_Reset, (void*)2, 0},
 {"Reset 3",         '3',  (Fl_Callback*)IC_GUI::cb_Reset, (void*)3, 0},
 {"Toggle Frame ",   'f',  (Fl_Callback*)IC_GUI::cb_ToggleFrame, (void*)-1, FL_MENU_DIVIDER},
 {"Expand Radius ",  '}',  (Fl_Callback*)IC_GUI::cb_StretchRad, (void*)1, 0},
 {"Shrink Radius ",  '{',  (Fl_Callback*)IC_GUI::cb_StretchRad, (void*)-1, 0},
 {"Base Higher ",    'B',  (Fl_Callback*)IC_GUI::cb_ModBase, (void*)1, 0},
 {"Base Lower ",     'b',  (Fl_Callback*)IC_GUI::cb_ModBase, (void*)-1, 0},
 {"Scale Higher ",   'S',  (Fl_Callback*)IC_GUI::cb_ModScale, (void*)1, 0},
 {"Scale Lower ",    's',  (Fl_Callback*)IC_GUI::cb_ModScale, (void*)-1, 0},
 {"Zoom In",         'i', (Fl_Callback*)IC_GUI::cb_Zoom, (void*)-1, 0},
 {"Zoom Out",        'o', (Fl_Callback*)IC_GUI::cb_Zoom, (void*)1, 0},
 {"Zoom Reset", FL_ALT+'Z', (Fl_Callback*)IC_GUI::cb_Zoom, (void*)0, 0},
 {0},

 {"Color-Map", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Default",   0, (Fl_Callback*)IC_GUI::cb_ColorMap, (void*)1, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Copper",    0, (Fl_Callback*)IC_GUI::cb_ColorMap, (void*)2, FL_MENU_RADIO},
 {"Bone",      0, (Fl_Callback*)IC_GUI::cb_ColorMap, (void*)3, FL_MENU_RADIO|FL_MENU_DIVIDER},
 {0},

 {"Function-Select", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Increment",   ' ', (Fl_Callback*)IC_GUI::cb_IncCurrFunction, (void*)1, 0},
 {"Decrement",   FL_ALT+' ',  (Fl_Callback*)IC_GUI::cb_IncCurrFunction, (void*)-1, 0},
 {"Collective",  'c',  (Fl_Callback*)IC_GUI::cb_ToggleCollectiveView, (void*)0, 0},
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

int IC_GUI::handle(int event) 
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
inline void IC_GUI::cb_Zoom_i(int val) {
  if(val < 0) viewer->setParam("mod_zoom", 1.25);
  if(val > 0) viewer->setParam("mod_zoom", 0.80);
}
void IC_GUI::cb_Zoom(Fl_Widget* o, int v) {
  ((IC_GUI*)(o->parent()->user_data()))->cb_Zoom_i(v);
}

//----------------------------------------- Reset
inline void IC_GUI::cb_Reset_i(int val) {
  if(val==1)  viewer->setParam("reset_view", "1");
  if(val==2)  viewer->setParam("reset_view", "2");
  if(val==3)  viewer->setParam("reset_view", "3");
}
void IC_GUI::cb_Reset(Fl_Widget* o, int v) {
  ((IC_GUI*)(o->parent()->user_data()))->cb_Reset_i(v);
}

//----------------------------------------- Rotate  X
inline void IC_GUI::cb_RotateX_i(int amt) {
  viewer->setParam("mod_x_rotation", (double)(amt));
}
void IC_GUI::cb_RotateX(Fl_Widget* o, int v) {
  ((IC_GUI*)(o->parent()->user_data()))->cb_RotateX_i(v);
}

//----------------------------------------- Rotate  Z
inline void IC_GUI::cb_RotateZ_i(int amt) {
  viewer->setParam("mod_z_rotation", (double)(amt));
}
void IC_GUI::cb_RotateZ(Fl_Widget* o, int v) {
  ((IC_GUI*)(o->parent()->user_data()))->cb_RotateZ_i(v);
}

//----------------------------------------- Stretch Radius
inline void IC_GUI::cb_StretchRad_i(int amt) {
  if(amt > 0) viewer->setParam("mod_radius", 1.25);
  if(amt < 0) viewer->setParam("mod_radius", 0.80);
}
void IC_GUI::cb_StretchRad(Fl_Widget* o, int v) {
  ((IC_GUI*)(o->parent()->user_data()))->cb_StretchRad_i(v);
}

//----------------------------------------- Mod Base
inline void IC_GUI::cb_ModBase_i(int amt) {
  if(amt > 0) viewer->setParam("mod_base",  10);
  if(amt < 0) viewer->setParam("mod_base", -10);
}
void IC_GUI::cb_ModBase(Fl_Widget* o, int v) {
  ((IC_GUI*)(o->parent()->user_data()))->cb_ModBase_i(v);
}

//----------------------------------------- Mod Scale
inline void IC_GUI::cb_ModScale_i(int amt) {
  if(amt > 0) viewer->setParam("mod_scale", 1.25);
  if(amt < 0) viewer->setParam("mod_scale", 0.80);
}
void IC_GUI::cb_ModScale(Fl_Widget* o, int v) {
  ((IC_GUI*)(o->parent()->user_data()))->cb_ModScale_i(v);
}

//----------------------------------------- Toggle Frame
inline void IC_GUI::cb_ToggleFrame_i() {
  viewer->setParam("draw_frame", "toggle");
}
void IC_GUI::cb_ToggleFrame(Fl_Widget* o) {
  ((IC_GUI*)(o->parent()->user_data()))->cb_ToggleFrame_i();
}

//----------------------------------------- ColorMap
inline void IC_GUI::cb_ColorMap_i(int index) {
  string str = "default";
  if(index ==2)
    str = "copper";
  else if(index == 3)
    str = "bone";
  //model->modColorMap(str);
  viewer->redraw();
}
void IC_GUI::cb_ColorMap(Fl_Widget* o, int v) {
  ((IC_GUI*)(o->parent()->user_data()))->cb_ColorMap_i(v);
}

//----------------------------------------- IncCurrFunction
inline void IC_GUI::cb_IncCurrFunction_i(int amt) {
  if(!viewer)
    return;
  viewer->incCurrFunction();
  updateFields();
}
void IC_GUI::cb_IncCurrFunction(Fl_Widget* o, int v) {
  ((IC_GUI*)(o->parent()->user_data()))->cb_IncCurrFunction_i(v);
}

//----------------------------------------- ToggleCollectiveView
inline void IC_GUI::cb_ToggleCollectiveView_i() {
  if(!viewer)
    return;
  
  viewer->setCollective("toggle");
  updateFields();
}
void IC_GUI::cb_ToggleCollectiveView(Fl_Widget* o) {
  ((IC_GUI*)(o->parent()->user_data()))->cb_ToggleCollectiveView_i();
}

//----------------------------------------- UpdateFields
void IC_GUI::updateFields() 
{
  if(!viewer)
    return;
  
  string funcs = viewer->getTotalFunctions();
  total_funcs->value(funcs.c_str());

  string descriptor = "null";
  descriptor = viewer->getCurrDescriptor();
  curr_function->value(descriptor.c_str());
  
  string pieces = viewer->getCurrPieces();
  curr_func_size->value(pieces.c_str());

  string pwt = viewer->getCurrPriority();
  curr_func_pwt->value(pwt.c_str());


  viewer->redraw();
}

//----------------------------------------- Quit
void IC_GUI::cb_Quit() {
  exit(0);
}

