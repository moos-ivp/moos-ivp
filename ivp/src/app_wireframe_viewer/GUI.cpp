/*****************************************************************/
/*    NAME: Doug Maxwell and Michael Benjamin                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GUI.cpp                                              */
/*    DATE: November 2007                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <math.h>
#include "GUI.h"

#define DEG2RAD 0.01745329

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

  viewer = new WireframeViewer(0, 30, w(), h());

  this->end();
  this->resizable(this);
  this->show();
}

Fl_Menu_Item GUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)GUI::cb_Quit, 0, 0},
 {0},

 {"Navigation", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Camera X- ", 'x',  (Fl_Callback*)GUI::cb_CameraX, (void*)-1, 0},
 {"Camera X+ ", 'X',  (Fl_Callback*)GUI::cb_CameraX, (void*)1, 0},
 {"Camera Y- ", 'y',  (Fl_Callback*)GUI::cb_CameraY, (void*)-1, 0},
 {"Camera Y+ ", 'Y',  (Fl_Callback*)GUI::cb_CameraY, (void*)1, 0},
 {"Camera Z- ", 'z',  (Fl_Callback*)GUI::cb_CameraZ, (void*)-1, 0},
 {"Camera Z+ ", 'Z',  (Fl_Callback*)GUI::cb_CameraZ, (void*)1, FL_MENU_DIVIDER},
 {"Reset ",     'r',  (Fl_Callback*)GUI::cb_Reset, (void*)0, FL_MENU_DIVIDER},
 {"ViewMode 0 ",  '0',  (Fl_Callback*)GUI::cb_ViewMode, (void*)0, 0},
 {"ViewMode 1 ",  '1',  (Fl_Callback*)GUI::cb_ViewMode, (void*)1, 0},
 {"ViewMode 2 ",  0,  (Fl_Callback*)GUI::cb_ViewMode, (void*)2, FL_MENU_DIVIDER},

 {"Zoom In",        'i', (Fl_Callback*)GUI::cb_Zoom, (void*)1, 0},
 {"Zoom Out",       'o', (Fl_Callback*)GUI::cb_Zoom, (void*)-1, FL_MENU_DIVIDER},
 {"Pan Left ",      '4',  (Fl_Callback*)GUI::cb_PanLR, (void*)1, 0},
 {"Pan Right ",     '6',  (Fl_Callback*)GUI::cb_PanLR, (void*)-1, 0},
 {"Pan Up ",        '8',  (Fl_Callback*)GUI::cb_PanUD, (void*)1, 0},
 {"Pan Down ",      '2',  (Fl_Callback*)GUI::cb_PanUD, (void*)-1, FL_MENU_DIVIDER},
 {"Scene Rotate Left ",  FL_Delete,  (Fl_Callback*)GUI::cb_Rotate, (void*)-100, 0},
 {"Scene Rotate Right ", FL_Page_Down, (Fl_Callback*)GUI::cb_Rotate, (void*)100, FL_MENU_DIVIDER},
 {"Camera Look Left ", FL_Left, (Fl_Callback*)GUI::cb_CameraYawRotate, (void*)-1, 0},
 {"Camera Look Right ", FL_Right, (Fl_Callback*)GUI::cb_CameraYawRotate, (void*)1, 0},
 {"Camera Look Up ", FL_Up, (Fl_Callback*)GUI::cb_CameraPitchRotate, (void*)-1, 0},
 {"Camera Look Down ", FL_Down, (Fl_Callback*)GUI::cb_CameraPitchRotate, (void*)1, 0},
 {0},

 {"COI", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Origin o", 'o',  (Fl_Callback*)GUI::cb_COI_Origin, (void*)0,},
 {"Next Kayak n", 'n', (Fl_Callback*)GUI::cb_COI_Next, (void*)0,},
 {0},

 {"Animation", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Toggle-Animation", ' ',  (Fl_Callback*)GUI::cb_ToggleAnimation, (void*)-1, 0},
 {0},

 {0}
};

//----------------------------------------------------------
// Procedure: handle
//    As it stands, this method could be eliminated entirely, and the 
//    default behavior of the parent class should work fine. But if
//    we want to tinker with event handling, this method is the place.

int GUI::handle(int event) 
{
  return(Fl_Window::handle(event));
}

//----------------------------------------- Zoom In
inline void GUI::cb_Zoom_i(int val) {
  viewer->m_camera_x+=(float)(sin((viewer->m_camera_yaw)*DEG2RAD))*(float)(val);
  viewer->m_camera_z-=(float)(cos((viewer->m_camera_yaw)*DEG2RAD))*(float)(val);
  viewer->m_camera_y-=(float)(sin((viewer->m_camera_pitch)*DEG2RAD))*(float)(val);
  viewer->redraw();
}
void GUI::cb_Zoom(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_Zoom_i(v);
}

//----------------------------------------- Reset
inline void GUI::cb_Reset_i() {
  viewer->reset();
  viewer->redraw();
}

void GUI::cb_Reset(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_Reset_i();
}

//----------------------------------------- Camera X
inline void GUI::cb_CameraX_i(int amt) {
  viewer->incCameraX(amt);
}
void GUI::cb_CameraX(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_CameraX_i(v);
}

//----------------------------------------- Camera Y
inline void GUI::cb_CameraY_i(int amt) {
  viewer->incCameraY(amt);
}
void GUI::cb_CameraY(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_CameraY_i(v);
}

//----------------------------------------- Camera Z
inline void GUI::cb_CameraZ_i(int amt) {
  viewer->incCameraZ((double)(amt)/5.0);
}
void GUI::cb_CameraZ(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_CameraZ_i(v);
}

//----------------------------------------- Scene Rotate
inline void GUI::cb_Rotate_i(int amt) {
  float increment = (float)(amt);
  float rotation  = viewer->getRotationAngle();
  float new_rval  = rotation + increment / 360;
  viewer->setRotation(new_rval);
  viewer->redraw();
}
void GUI::cb_Rotate(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_Rotate_i(v);
}

//-----------------------------------------  Camera Yaw Rotate
inline void GUI::cb_CameraYawRotate_i(int amt){
  float increment = (float)(amt);
  viewer->m_camera_yaw+=(increment);
  viewer->redraw();
}
inline void GUI::cb_CameraYawRotate(Fl_Widget* o, int v){
  ((GUI*)(o->parent()->user_data()))->cb_CameraYawRotate_i(v);
}

//-----------------------------------------  Camera  Pitch Rotate
inline void GUI::cb_CameraPitchRotate_i(int amt){
  float increment = (float)(amt);
  viewer->m_camera_pitch+=(increment);
  viewer->redraw();
}
inline void GUI::cb_CameraPitchRotate(Fl_Widget* o, int v){
  ((GUI*)(o->parent()->user_data()))->cb_CameraPitchRotate_i(v);
}

//----------------------------------------- PanLR
inline void GUI::cb_PanLR_i(int amt) {
  float increment = (float)(amt);
  viewer->translateCamera((viewer->m_camera_x + increment), viewer->m_camera_y , viewer->m_camera_z);
  viewer->redraw();
}
void GUI::cb_PanLR(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_PanLR_i(v);
}


//----------------------------------------- PanUD
inline void GUI::cb_PanUD_i(int amt) {
  float increment = (float)(amt);
  viewer->translateCamera(viewer->m_camera_x, (viewer->m_camera_y + increment) , viewer->m_camera_z);
  viewer->redraw();
}
void GUI::cb_PanUD(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_PanUD_i(v);
}


//----------------------------------------- ViewMode
inline void GUI::cb_ViewMode_i(int val) {
  viewer->setViewMode(val);
}
void GUI::cb_ViewMode(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ViewMode_i(v);
}

//----------------------------------------- Set COI to Origin
inline void GUI::cb_COI_Origin_i() {
  viewer->setCOI(0, 0, 0);
  viewer->redraw();
}
void GUI::cb_COI_Origin(Fl_Widget* o){
  ((GUI*)(o->parent()->user_data()))->cb_COI_Origin_i();
}

//----------------------------------------- Set COI to next object
inline void GUI::cb_COI_Next_i() {
// Will make an object array to cycle through here
  viewer->setCOI(4, 0, 2);
  viewer->redraw();
}
void GUI::cb_COI_Next(Fl_Widget* o){
  ((GUI*)(o->parent()->user_data()))->cb_COI_Next_i();
}

//----------------------------------------- ToggleAnimation
inline void GUI::cb_ToggleAnimation_i() {
  viewer->toggleAnimation();
}
void GUI::cb_ToggleAnimation(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_ToggleAnimation_i();
}

//----------------------------------------- Quit
void GUI::cb_Quit() {
  exit(0);
}

