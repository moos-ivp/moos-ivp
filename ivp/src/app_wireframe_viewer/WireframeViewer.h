/*****************************************************************/
/*    NAME: Doug Maxwell - Mike Benjamin                         */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: WireframeViewer.h                                    */
/*    DATE: November                                             */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef WIREFRAME_VIEWER_HEADER
#define WIREFRAME_VIEWER_HEADER

#include <FL/Fl_Gl_Window.H>
#include <iostream>
#include <string>
#include "circle.h"
#include "point.h"
#include "ray.h"
#include "arc.h"
#include "kayak.h"

#define ENTITIES 200

using namespace std;

class WireframeViewer : public Fl_Gl_Window{
 public:
  WireframeViewer(int x, int y, int wid, int hgt, char* title=0);
  virtual ~WireframeViewer() {};

  Circle circle[ENTITIES];
  Arc arc[ENTITIES];
  Point point[ENTITIES];
  Ray line[ENTITIES];
  Kayak kayak[ENTITIES];

  virtual void draw();
  virtual void resize(int, int, int, int);
  virtual int handle(int event);
  virtual void handle_mouse_left();
  virtual void handle_mouse_right();

  void  InitializeGL();
  void  printCamCoords();
  void  DrawAxes();
  void  DrawGameBoard();
  void  DrawFloor();
  void  DrawKayak(float, float, float);

  bool  isAnimating()          {return(m_animating);};
  bool  isCOI()		       {return(m_COI);};
  float getRotationIncrement() {return(m_rotation_increment);};
  float getRotationAngle()     {return(m_rotation);};
  float getZoom()	       {return(m_zoom);};
  float getCameraPitch()       {return(m_camera_pitch);};
  float getCameraYaw()         {return(m_camera_yaw);};

  void  setRotation(float v)   {m_rotation  = v;};
  void  setCameraX(float v)    {m_camera_x  = v;};
  void  setCameraY(float v)    {m_camera_y  = v;};
  void  setCameraZ(float v)    {m_camera_z  = v;};
  void  incCameraX(float v)    {m_camera_x += v;};
  void  incCameraY(float v)    {m_camera_y += v;};
  void  incCameraZ(float v)    {m_camera_z += v;};

  void  setTargetX(float v)    {m_target_x = v;};
  void  setTargetY(float v)    {m_target_y = v;};
  void  setTargetZ(float v)    {m_target_z = v;};

  void  modZoom(float v) {};
  void  reset();
  void  setViewpoint(float, float, float);

  void  setViewMode(int v)     {m_view_mode = v;};
  void  setCameraYaw(float);
  void  incCameraYaw(float);
  void  setCameraPitch(float);
  void  incCameraPitch(float);
  void  setCameraAngle();
  //void  setCOI(float, float, float);
  void  setCOI(int);
  void  translateCamera(float, float, float);

  void  toggleAnimation() {m_animating = !m_animating;};
  void  toggleGrid() { m_gameboard_grid = !m_gameboard_grid; };
  void  toggleFloor() { m_floor = !m_floor; };
  void  toggleCOI() {m_COI = !m_COI;};
  void  toggleCamPos() {m_camera_position_text = !m_camera_position_text;};

 //protected:
  float m_rotation;
  float m_rotation_increment;
  bool  m_animating;
  bool  m_COI;
  bool  m_gameboard_grid;
  bool  m_camera_position_text;
  bool  m_floor;
  int   m_floor_increment;

  float m_camera_x;
  float m_camera_y;
  float m_camera_z;

  float m_camera_pitch;  // 0:down  180:up
  float m_camera_yaw;    // 0:north 270:west
  float m_camera_roll;

  float m_target_x;
  float m_target_y;
  float m_target_z;

  int   m_view_mode;

  int m_curr_cursor_pos_x;
  int m_curr_cursor_pos_y;
  int m_old_cursor_pos_x;
  int m_old_cursor_pos_y;  
 
  float m_zoom;

  int m_nav_multiplier;

  float m_circle_radius[ENTITIES];
  float m_circle_x[ENTITIES];
  float m_circle_y[ENTITIES];
  float m_circle_z[ENTITIES];
  float m_circle_red[ENTITIES];
  float m_circle_green[ENTITIES];
  float m_circle_blue[ENTITIES];
  float m_circle_alpha[ENTITIES];

  float m_arc_radius[ENTITIES];
  float m_arc_x[ENTITIES];
  float m_arc_y[ENTITIES];
  float m_arc_z[ENTITIES];
  float m_arc_angle_start[ENTITIES];
  float m_arc_angle_end[ENTITIES];
  float m_arc_red[ENTITIES];
  float m_arc_green[ENTITIES];
  float m_arc_blue[ENTITIES];
  float m_arc_alpha[ENTITIES];

  float m_point_x[ENTITIES];
  float m_point_y[ENTITIES];
  float m_point_z[ENTITIES];
  float m_point_red[ENTITIES];
  float m_point_green[ENTITIES];
  float m_point_blue[ENTITIES];
  float m_point_alpha[ENTITIES];
  
  float m_line_start_x[ENTITIES];
  float m_line_start_y[ENTITIES];
  float m_line_start_z[ENTITIES];
  float m_line_end_x[ENTITIES];
  float m_line_end_y[ENTITIES];
  float m_line_end_z[ENTITIES];
  float m_line_red[ENTITIES];
  float m_line_green[ENTITIES];
  float m_line_blue[ENTITIES];
  float m_line_alpha[ENTITIES];

  float m_kayak_cent_x[ENTITIES];
  float m_kayak_cent_y[ENTITIES];
  float m_kayak_cent_z[ENTITIES];
  float m_kayak_red[ENTITIES];
  float m_kayak_green[ENTITIES];
  float m_kayak_blue[ENTITIES];
  float m_kayak_alpha[ENTITIES];
  int m_kayak_counter;

  

  int m_viewpoint_list[ENTITIES];

  string m_DataFile;
};

#endif
