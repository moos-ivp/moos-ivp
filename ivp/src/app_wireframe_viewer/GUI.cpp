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
#include <FL/Fl_Gl_Window.H>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
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
 {"Open", FL_CTRL+'o', (Fl_Callback*)GUI::cb_Open, 0, 0},
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
 {"Zoom In",        'i', (Fl_Callback*)GUI::cb_Zoom, (void*)1, 0},
 {"Zoom Out",       'o', (Fl_Callback*)GUI::cb_Zoom, (void*)-1, FL_MENU_DIVIDER},
 {"Pan Left ",      '4',  (Fl_Callback*)GUI::cb_PanLR, (void*)-1, 0},
 {"Pan Right ",     '6',  (Fl_Callback*)GUI::cb_PanLR, (void*)1, 0},
 {"Pan Up ",        '8',  (Fl_Callback*)GUI::cb_PanUD, (void*)1, 0},
 {"Pan Down ",      '2',  (Fl_Callback*)GUI::cb_PanUD, (void*)-1, FL_MENU_DIVIDER},
 {"Scene Rotate Left ",  FL_Delete,  (Fl_Callback*)GUI::cb_Rotate, (void*)-100, 0},
 {"Scene Rotate Right ", FL_End, (Fl_Callback*)GUI::cb_Rotate, (void*)100, FL_MENU_DIVIDER},
 {"Camera Look Left ", FL_Left, (Fl_Callback*)GUI::cb_CameraYawRotate, (void*)-1, 0},
 {"Camera Look Right ", FL_Right, (Fl_Callback*)GUI::cb_CameraYawRotate, (void*)1, 0},
 {"Camera Look Up ", FL_Up, (Fl_Callback*)GUI::cb_CameraPitchRotate, (void*)-1, 0},
 {"Camera Look Down ", FL_Down, (Fl_Callback*)GUI::cb_CameraPitchRotate, (void*)1, 0},
 {0},

 {"Nav Speed", 0,  0, 0, 64, 0, 0, 14, 0},
 {"1x ", FL_CTRL+'1', (Fl_Callback*)GUI::cb_ViewSpeed, (void*)1, 0},
 {"10x ", FL_CTRL+'2', (Fl_Callback*)GUI::cb_ViewSpeed, (void*)10, 0},
 {"50x ", FL_CTRL+'3', (Fl_Callback*)GUI::cb_ViewSpeed, (void*)50, 0},
 {"100x ", FL_CTRL+'4', (Fl_Callback*)GUI::cb_ViewSpeed, (void*)100, 0},
 {"1000x ", FL_CTRL+'5', (Fl_Callback*)GUI::cb_ViewSpeed, (void*)1000, 0},
 {0},
/*
 {"View Mode", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Freelook ",  '0',  (Fl_Callback*)GUI::cb_ViewMode, (void*)0, 0},
 {"Target ",  '1',  (Fl_Callback*)GUI::cb_ViewMode, (void*)1, FL_MENU_DIVIDER},
 {0},

 {"COI", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Origin o", 'o',  (Fl_Callback*)GUI::cb_COI_Origin, (void*)0,0},
 {"Next n", 'n', (Fl_Callback*)GUI::cb_COI_Next, (void*)0,0},
 {0},
*/
 {"Game Board", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Toggle Grid", 'g',  (Fl_Callback*)GUI::cb_Toggle_Grid, (void*)0,0},
 {"Toggle Cam Position", 'c',  (Fl_Callback*)GUI::cb_Toggle_Cam_Pos, (void*)0,0},
 {"Toggle Floor", 'f', (Fl_Callback*)GUI::cb_Toggle_Floor, (void*)0,0},
 {"Inc Floor Depth", 'd', (Fl_Callback*)GUI::cb_Floor_Depth, (void*)1, 0},
 {"Dec Floor Depth", FL_CTRL+'d', (Fl_Callback*)GUI::cb_Floor_Depth, (void*)-1, 0},
 {0},

 {"Animation", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Toggle-Animation", ' ',  (Fl_Callback*)GUI::cb_ToggleAnimation, (void*)-1, 0},
 {0},

 {0}
};

//----------------------------------------------------------
// Procedure: handle

int GUI::handle(int event) 
{
  int cursor_pos_x, cursor_pos_y;
  switch(event){
  case FL_PUSH:
    viewer->m_curr_cursor_pos_x = Fl::event_x();
    viewer->m_curr_cursor_pos_y = Fl::event_y();
    viewer->m_old_cursor_pos_x = Fl::event_x();
    viewer->m_old_cursor_pos_y = Fl::event_y();    
  case FL_DRAG:
    viewer->m_curr_cursor_pos_x = Fl::event_x();
    viewer->m_curr_cursor_pos_y = Fl::event_y();

    cursor_pos_x = viewer->m_curr_cursor_pos_x - viewer->m_old_cursor_pos_x;
    cursor_pos_y = viewer->m_curr_cursor_pos_y - viewer->m_old_cursor_pos_y;

    viewer->m_camera_pitch+=(cursor_pos_y);
    //viewer->m_camera_yaw+=(cursor_pos_x);
    viewer->m_rotation+=(cursor_pos_x);

    if(Fl_Window::handle(event) != 1){
	if(Fl::event_button() == FL_LEFT_MOUSE)
	  viewer->handle_mouse_left();
	if(Fl::event_button() == FL_RIGHT_MOUSE)
	  viewer->handle_mouse_right();
    }      
    //return 1;
    break;   
  default:
    return(Fl_Window::handle(event));   
  }
}
//----------------------------------------- Floor Depth Inc
inline void GUI::cb_Floor_Depth_i(int v){
  viewer->m_floor_increment+v;
  viewer->redraw();
}
void GUI::cb_Floor_Depth(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_Floor_Depth_i(v);
}

//----------------------------------------- Zoom In
inline void GUI::cb_Zoom_i(int val) {
  viewer->m_camera_x+=(float)(sin((viewer->m_camera_yaw)*DEG2RAD))*(float)(val*viewer->m_nav_multiplier);
  viewer->m_camera_z-=(float)(cos((viewer->m_camera_yaw)*DEG2RAD))*(float)(val*viewer->m_nav_multiplier);
  viewer->m_camera_y-=(float)(sin((viewer->m_camera_pitch)*DEG2RAD))*(float)(val*viewer->m_nav_multiplier);
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
  viewer->redraw();
}
void GUI::cb_CameraX(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_CameraX_i(v);
}

//----------------------------------------- Camera Y
inline void GUI::cb_CameraY_i(int amt) {
  viewer->incCameraY(amt);
  viewer->redraw();
}
void GUI::cb_CameraY(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_CameraY_i(v);
}

//----------------------------------------- Camera Z
inline void GUI::cb_CameraZ_i(int amt) {
  viewer->incCameraZ(amt);
  viewer->redraw();
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
  //viewer->m_camera_yaw+=(increment);
  viewer->m_rotation+=(increment);
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
  viewer->translateCamera((viewer->m_camera_x+increment*viewer->m_nav_multiplier),viewer->m_camera_y,viewer->m_camera_z);
/*
  viewer->m_camera_x+=(float)(sin((viewer->m_camera_yaw)*DEG2RAD))*(float)(val*viewer->m_nav_multiplier);
  viewer->m_camera_z-=(float)(cos((viewer->m_camera_yaw)*DEG2RAD))*(float)(val*viewer->m_nav_multiplier);
  viewer->m_camera_y-=(float)(sin((viewer->m_camera_pitch)*DEG2RAD))*(float)(val*viewer->m_nav_multiplier); 
*/
  viewer->redraw();
}
void GUI::cb_PanLR(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_PanLR_i(v);
}


//----------------------------------------- PanUD
inline void GUI::cb_PanUD_i(int amt) {
  float increment = (float)(amt);
  viewer->translateCamera(viewer->m_camera_x, (viewer->m_camera_y + increment*viewer->m_nav_multiplier), viewer->m_camera_z);
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
//  viewer->setCOI(0, 0, 0);
  viewer->reset();
  viewer->redraw();
}
void GUI::cb_COI_Origin(Fl_Widget* o){
  ((GUI*)(o->parent()->user_data()))->cb_COI_Origin_i();
}

//----------------------------------------- Set COI to next object
inline void GUI::cb_COI_Next_i() {
// Will make an object array to cycle through here
  //viewer->setCOI(viewer->m_kayak_cent_x[0], viewer->m_kayak_cent_y[0], viewer->m_kayak_cent_z[0]);
  int index = 0; //m_kayak_counter-1;
  viewer->setCOI(index);
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

//------------------------------------------Toggle Grid
inline void GUI::cb_Toggle_Grid_i() {
  viewer->toggleGrid();
  viewer->redraw();
}
void GUI::cb_Toggle_Grid(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_Toggle_Grid_i();
}

inline void GUI::cb_Toggle_Floor_i() {
  viewer->toggleFloor();
  viewer->redraw();
}
void GUI::cb_Toggle_Floor(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_Toggle_Floor_i();
}

//--------------------------------------------Toggle Camera Position
inline void GUI::cb_Toggle_Cam_Pos_i(){
  viewer->toggleCamPos();
  //viewer->redraw();
}
void GUI::cb_Toggle_Cam_Pos(Fl_Widget* o){
  ((GUI*)(o->parent()->user_data()))->cb_Toggle_Cam_Pos_i();
}

//--------------------------------------------Nav Speed
inline void GUI::cb_ViewSpeed_i(int vSpeed){
  viewer->m_nav_multiplier = vSpeed;
}

void GUI::cb_ViewSpeed(Fl_Widget* o, int vSpeed){
  ((GUI*)(o->parent()->user_data()))->cb_ViewSpeed_i(vSpeed);
}
//--------------------------------------------Open
inline void GUI::cb_Open_i(){
  string line;
  string key;
  float temp = 0;
  float temp_x = 0, temp_y = 0, temp_z = 0, 
	temp_xx = 0, temp_yy = 0, temp_zz = 0,
	temp_red = 0, temp_green = 0, temp_blue = 0, temp_alpha = 0;
  int circle_counter = 0, point_counter = 0, line_counter = 0, 
	viewpoint_counter = 0, arc_counter = 0, kayak_counter = 0;
  Fl_File_Chooser chooser(".", "*.dat", Fl_File_Chooser::MULTI, "Select Data File");
  chooser.show();
  while(chooser.shown()){Fl::wait();}
  if(chooser.value() == NULL){return;}
  else if(chooser.value() != NULL){
    viewer->m_DataFile = chooser.value();
    ifstream inFile( chooser.value(), ios::in);
    while(!inFile.eof()){
      inFile >> key >> temp >> 
	temp_x >> temp_y >> temp_z >> 
	temp_xx >> temp_yy >> temp_zz >>
	temp_red >> temp_green >> temp_blue >> temp_alpha;
      if( key == "circle" ){
        viewer->m_circle_radius[circle_counter] = temp;
        viewer->m_circle_x[circle_counter] = temp_x;
	viewer->m_circle_y[circle_counter] = temp_y;
	viewer->m_circle_z[circle_counter] = temp_z;
	viewer->m_circle_red[circle_counter] = temp_red;
	viewer->m_circle_green[circle_counter] = temp_green;
	viewer->m_circle_blue[circle_counter] = temp_blue;
	viewer->m_circle_alpha[circle_counter] = temp_alpha;
        viewer->circle[circle_counter].drawCircle(
		viewer->m_circle_radius[circle_counter], 
		viewer->m_circle_x[circle_counter], 
		viewer->m_circle_y[circle_counter], 
		viewer->m_circle_z[circle_counter],
		viewer->m_circle_red[circle_counter],
		viewer->m_circle_green[circle_counter],
		viewer->m_circle_blue[circle_counter],
		viewer->m_circle_alpha[circle_counter]);		
        circle_counter++;
      }
      if( key == "arc" ){
        viewer->m_arc_radius[arc_counter] = temp;
        viewer->m_arc_x[arc_counter] = temp_x;
	viewer->m_arc_y[arc_counter] = temp_y;
	viewer->m_arc_z[arc_counter] = temp_z;
	viewer->m_arc_angle_start[arc_counter] = temp_xx;
	viewer->m_arc_angle_end[arc_counter] = temp_yy;
	viewer->m_arc_red[arc_counter] = temp_red;
	viewer->m_arc_green[arc_counter] = temp_green;
	viewer->m_arc_blue[arc_counter] = temp_blue;
	viewer->m_arc_alpha[arc_counter] = temp_alpha;
        viewer->arc[arc_counter].drawArc(
		viewer->m_arc_radius[arc_counter], 
		viewer->m_arc_x[arc_counter], 
		viewer->m_arc_y[arc_counter], 
		viewer->m_arc_z[arc_counter],
		viewer->m_arc_angle_start[arc_counter],
		viewer->m_arc_angle_end[arc_counter],
		viewer->m_arc_red[arc_counter],
		viewer->m_arc_green[arc_counter],
		viewer->m_arc_blue[arc_counter],
		viewer->m_arc_alpha[arc_counter]);	
        arc_counter++;
      }
      else if( key == "point" ){
	viewer->m_point_x[point_counter] = temp_x;
	viewer->m_point_y[point_counter] = temp_y;
	viewer->m_point_z[point_counter] = temp_z;
	viewer->m_point_red[point_counter] = temp_red;
	viewer->m_point_green[point_counter] = temp_green;
	viewer->m_point_blue[point_counter] = temp_blue;
	viewer->m_point_alpha[point_counter] = temp_alpha;
	viewer->point[point_counter].drawPoint(
		viewer->m_point_x[point_counter],
		viewer->m_point_y[point_counter],
		viewer->m_point_z[point_counter],
		viewer->m_point_red[point_counter],
		viewer->m_point_green[point_counter],
		viewer->m_point_blue[point_counter],
		viewer->m_point_alpha[point_counter]);
	point_counter++;
      }      
      else if( key == "kayak" ){
	viewer->m_kayak_cent_x[kayak_counter] = temp_x;
	viewer->m_kayak_cent_y[kayak_counter] = temp_y;
	viewer->m_kayak_cent_z[kayak_counter] = temp_z;
	viewer->m_kayak_red[kayak_counter] = temp_red;
	viewer->m_kayak_green[kayak_counter] = temp_green;
	viewer->m_kayak_blue[kayak_counter] = temp_blue;
	viewer->m_kayak_alpha[kayak_counter] = temp_alpha;
	viewer->kayak[kayak_counter].drawKayak(
		viewer->m_kayak_cent_x[kayak_counter],
		viewer->m_kayak_cent_y[kayak_counter],
		viewer->m_kayak_cent_z[kayak_counter],
		viewer->m_kayak_red[kayak_counter],
		viewer->m_kayak_green[kayak_counter],
		viewer->m_kayak_blue[kayak_counter],
		viewer->m_kayak_alpha[kayak_counter]);
	kayak_counter++;
	viewer->m_kayak_counter++;
      }
      else if( key == "line" ){
	viewer->m_line_start_x[line_counter] = temp_x;
	viewer->m_line_start_y[line_counter] = temp_y;
	viewer->m_line_start_z[line_counter] = temp_z;
	viewer->m_line_end_x[line_counter]   = temp_xx;
	viewer->m_line_end_y[line_counter]   = temp_yy;
	viewer->m_line_end_z[line_counter]   = temp_zz;
	viewer->m_line_red[line_counter] 	= temp_red;
	viewer->m_line_green[line_counter] 	= temp_green;
	viewer->m_line_blue[line_counter] 	= temp_blue;
	viewer->m_line_alpha[line_counter] 	= temp_alpha;
	viewer->line[line_counter].drawLine(
		viewer->m_line_start_x[line_counter],
		viewer->m_line_start_y[line_counter],
		viewer->m_line_start_z[line_counter],
		viewer->m_line_end_x[line_counter],
		viewer->m_line_end_y[line_counter],
		viewer->m_line_end_z[line_counter],
		viewer->m_line_red[line_counter],
		viewer->m_line_green[line_counter],
		viewer->m_line_blue[line_counter],
		viewer->m_line_alpha[line_counter]);
	line_counter++;
      }
      else if( key == "viewpoint" ){
	viewer->m_viewpoint_list[viewpoint_counter] = (int)temp_xx;
	viewer->setViewpoint(temp_x, temp_y, temp_z);
        viewpoint_counter++;
      }
      else if( key == "" ){ return; }
    }
  }
  viewer->redraw();
}
void GUI::cb_Open(Fl_Widget* o){
  ((GUI*)(o->parent()->user_data()))->cb_Open_i();
}
//----------------------------------------- Quit
void GUI::cb_Quit() {
  exit(0);
}
