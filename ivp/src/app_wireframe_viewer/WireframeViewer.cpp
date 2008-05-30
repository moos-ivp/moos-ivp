/*****************************************************************/
/*    NAME: Doug Maxwell - Mike Benjamin                         */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: WireframeViewer.cpp                                  */
/*    DATE: November 2007                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "WireframeViewer.h"

#define DEG2RAD 0.01745329
#define RAD2DEG 57.2957795
#define ENTITIES 200
//-----------------------------------------------------------
// Constructor

WireframeViewer::WireframeViewer(int x, int y, int w, int h, char* title)
  : Fl_Gl_Window(x,y,w,h,title)
{
  mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);

  m_rotation = 0.0f;
  m_rotation_increment = 1.0f;
  m_animating = false;
  m_gameboard_grid = true;
  m_camera_position_text = false;
  m_floor = false;
  m_floor_increment = 1.0f;
  
  m_camera_x = 0;
  m_camera_y = 30;
  m_camera_z = 80;

  m_camera_pitch = 0;
  m_camera_yaw   = 0;

  m_target_x = 0;
  m_target_y = 0;
  m_target_z = 0;

  m_view_mode = 0;

  m_curr_cursor_pos_x = 0;
  m_curr_cursor_pos_y = 0;
  m_old_cursor_pos_x = 0;
  m_old_cursor_pos_y = 0;

  m_zoom      = 0;
  m_nav_multiplier = 1;

  for(int i = 0; i < ENTITIES; i++){//initialize everything
      m_circle_radius[i] = 0.0;
      m_circle_x[i] = m_circle_y[i] = m_circle_z[i] = 0.0;
      m_circle_red[i] = m_circle_green[i] = m_circle_blue[i] = m_circle_alpha[i] = 0.0;

      m_point_x[i] = m_point_y[i] = m_point_z[i] = 200000.0;
      m_point_red[i] = m_point_green[i] = m_point_blue[i] = 0.0;
      m_point_alpha[i] = 0.0;

      m_line_start_x[i] = m_line_start_y[i] = m_line_start_z[i] = 0.0;
      m_line_end_x[i] = m_line_end_y[i] = m_line_end_z[i] = 0.0;
      m_line_red[i] = m_line_green[i] = m_line_blue[i] = m_line_alpha[i] = 0.0;
      
      m_arc_radius[i] = m_arc_x[i] = m_arc_y[i] = m_arc_z[i] = 0.0;
      m_arc_angle_start[i]= m_arc_angle_end[i] = 0.0;
      m_arc_red[i] = m_arc_green[i] = m_arc_blue[i] = m_arc_alpha[i] = 0.0;

      m_kayak_cent_x[i] = m_kayak_cent_y[i] = m_kayak_cent_z[i] = 200000.0;
      m_kayak_red[i] = m_kayak_green[i] = m_kayak_blue[i] = 0.0; 
      m_kayak_alpha[i] = 0.0;
      m_kayak_counter = 0;

      m_viewpoint_list[i] = 0;
  }
}

//-----------------------------------------------------------
// Procedure: handle

int WireframeViewer::handle(int event)
{
  return(Fl_Gl_Window::handle(event));
}

	
//-----------------------------------------------------------
// Procedure: InitializeGL

void WireframeViewer::InitializeGL(){
  GLfloat values[2];
  glGetFloatv(GL_LINE_WIDTH_GRANULARITY, values);
  glGetFloatv(GL_LINE_WIDTH_RANGE, values);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
  glLineWidth(1.5);
  glClearColor( 0.2, 0.1, 0.7, 0);
  glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  float ambientLight[] = {0.2, 0.2, 0.2, 1.0};
  float position1[] = {0, 1, 0, 0};
  float position2[] = {0, 0, 1, 0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_POSITION, position1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT1, GL_POSITION, position2);    
}

//-----------------------------------------------------------
// Procedure: draw

void WireframeViewer::draw(){
  static bool firstTime = true;
  if(firstTime){
    InitializeGL();
    firstTime = false;
  }
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //house cleaning
  
  //set up view transform
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, 1, 1, 1000000);
  //printCamCoords();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  setCameraAngle(); //setup ego centric viewing

  glRotatef(m_rotation, 0, 1, 0);
  //Keep these grids in separate nodes so they may be manipulated independantly.  dbm
  glPushMatrix();
    DrawGameBoard();
  glPopMatrix();
  glPushMatrix();
    DrawFloor();
  glPopMatrix();
  glPushMatrix();
  for(int i = 0; i < ENTITIES; i++){
    circle[i].drawCircle(
	m_circle_radius[i], m_circle_x[i], m_circle_y[i], m_circle_z[i],
	m_circle_red[i], m_circle_green[i],m_circle_blue[i], m_circle_alpha[i]);
    point[i].drawPoint(
	m_point_x[i], m_point_y[i], m_point_z[i],
	m_point_red[i], m_point_green[i],m_point_blue[i], m_point_alpha[i]);    
    line[i].drawLine(m_line_start_x[i], m_line_start_y[i], m_line_start_z[i],
	m_line_end_x[i], m_line_end_y[i], m_line_end_z[i],
	m_line_red[i], m_line_green[i],m_line_blue[i], m_line_alpha[i]);
    arc[i].drawArc(
	m_arc_radius[i], m_arc_x[i], m_arc_y[i], m_arc_z[i], m_arc_angle_start[i], m_arc_angle_end[i],
	m_arc_red[i], m_arc_green[i],m_arc_blue[i],m_arc_alpha[i]);
    kayak[i].drawKayak(
	m_kayak_cent_x[i], m_kayak_cent_y[i], m_kayak_cent_z[i],
	m_kayak_red[i], m_kayak_green[i], m_kayak_blue[i], m_kayak_alpha[i]);
  }
  glPopMatrix();

  glPushMatrix();
    glOrtho(0.0, w(), 0.0, h(), -1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    printCamCoords();
  glPopMatrix();

  glFlush();
}

//-----------------------------------------------------------
// Procedure: resize
void WireframeViewer::resize(int x, int y, int w, int h){
  Fl_Gl_Window::resize(x,y,w,h);
  glLoadIdentity();
  glViewport(0,0,w,h);
  glOrtho(-w,w,-h,h,-1,1);
  redraw();
}

//-----------------------------------------------------------
// Procedure: printCamCoords
void WireframeViewer::printCamCoords(){
  if(m_camera_position_text){
    char str[100];

    while(m_rotation < 0)
    m_rotation += 360.0;
    while(m_rotation > 360.0)
    m_rotation -= 360;

    sprintf(str, "Camera Position: X = %.2f Y = %.2f Z = %.2f Pitch = %.2f Scene Rotation = %.2f",
	m_camera_x, m_camera_y, m_camera_z, m_camera_pitch, m_rotation);
    glDisable(GL_DEPTH_TEST);
    glColor4f(0.0, 1.0, 1.0,0.0);
    gl_font(1,24);
    glColor4f(1.0, 1.0, 1.0, 0.0);    
    glRasterPos2f(0.0, 0.0);  
    gl_draw(str, strlen(str));
    glEnable(GL_DEPTH_TEST);
    redraw();
  }
}

//-----------------------------------------------------------
// Procedure: drawAxes

void WireframeViewer::DrawAxes()
{
  glBegin(GL_LINES);
  glColor3f(1,0,0);
  glVertex3f(-25.0,0,0);
  glVertex3f(25.0,0,0);
  glEnd();
  glBegin(GL_LINES);
  glColor3f(0,1,0);
  glVertex3f(0,-25.0,0);
  glVertex3f(0,25.0,0);
  glEnd();
  glBegin(GL_LINES);
  glColor3f(0,0,1);
  glVertex3f(0,0,-25);
  glVertex3f(0,0,25);
  glEnd();
}

//-----------------------------------------------------------
// Procedure: drawGameBoard

void WireframeViewer::DrawGameBoard()
{
  float rect = 50.0;
  glTranslatef(0.0, -0.2, 0.0);
  if(m_gameboard_grid){
	for(int i=-rect; i<=rect; i+=25){
	glBegin(GL_LINES);
	glColor3f(0.1, 0.1, 0.1);
	glVertex3f(rect, 0, i);
	glVertex3f(-rect, 0, i);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(i, 0, rect);
	glVertex3f(i, 0, -rect);
	glEnd();
	}
}
/*
  glTranslatef(0.0, -0.05, 0.0);
    glRotatef(-90.0, 1.0, 0.0, 0.0);  
    glColor3f(0.85, 0.85, 0.85);
    glRectf(-1*rect, -1*rect, rect, rect);
*/
//  }
}

//-----------------------------------------------------------
// Procedure: drawFloor

void WireframeViewer::DrawFloor()
{
  if(m_floor){
  float rect = 50.0;
  glTranslatef(0.0, (-25.0 + m_floor_increment), 0.0);
  if(m_gameboard_grid){
	for(int i=-rect; i<=rect; i+=25){
	glBegin(GL_LINES);
	glColor3f(0.1, 0.1, 0.1);
	glVertex3f(rect, 0, i);
	glVertex3f(-rect, 0, i);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(i, 0, rect);
	glVertex3f(i, 0, -rect);
	glEnd();
	}
   }

  glTranslatef(0.0, -0.05, 0.0);
    glRotatef(-90.0, 1.0, 0.0, 0.0);  
    glColor3f(0.15, 0.85, 0.85);
    glRectf(-1*rect, -1*rect, rect, rect);

  }
}

//-----------------------------------------------------------
// Procedure: setCameraAngle

void WireframeViewer::setCameraAngle()
{
  while(m_camera_pitch < 0)
    m_camera_pitch += 360.0;
  while(m_camera_pitch > 360.0)
    m_camera_pitch -= 360;

  while(m_camera_yaw < 0)
    m_camera_yaw += 360.0;
  while(m_camera_yaw > 360.0)
    m_camera_yaw -= 360; 
//glPushMatrix();
  glRotatef(m_camera_yaw, 0.0, 1.0, 0.0);
  glRotatef(m_camera_pitch, 1.0, 0.0, 0.0);

  glTranslatef(-m_camera_x, -m_camera_y, -m_camera_z);
//glPopMatrix();
}

//-----------------------------------------------------------
// Procedure: setCOI

void WireframeViewer::setCOI(int ind)
{
   cout << __LINE__ << ": T x y z:  "<<m_kayak_cent_x[ind]<<" "<<m_kayak_cent_y[ind]<<" "<<m_kayak_cent_z[ind]<< endl;
#if 0
   float distXZ = sqrt(
	(m_kayak_cent_x[ind]-m_camera_x)*(m_kayak_cent_x[ind]-m_camera_x)+
	(m_kayak_cent_z[ind]-m_camera_z)*(m_kayak_cent_z[ind]-m_camera_z));

  m_camera_yaw = 90.0-asin((m_camera_z-m_kayak_cent_z[ind])/distXZ)*RAD2DEG;


  float distXY = sqrt(
	(m_kayak_cent_x[ind]-m_camera_x)*(m_kayak_cent_x[ind]-m_camera_x)+
	(m_kayak_cent_y[ind]-m_camera_y)*(m_kayak_cent_y[ind]-m_camera_y));
  
  m_camera_pitch = asin((m_kayak_cent_x[ind]-m_camera_x)/distXY)*RAD2DEG;

  cout << __LINE__ << ": Yaw: " << m_camera_yaw << " Pitch: " << m_camera_pitch << endl;

    while(m_camera_pitch < 0)
    m_camera_pitch += 360.0;
  while(m_camera_pitch > 360.0)
    m_camera_pitch -= 360;

  while(m_camera_yaw < 0)
    m_camera_yaw += 360.0;
  while(m_camera_yaw > 360.0)
    m_camera_yaw -= 360; 

  cout << __LINE__ << ": Yaw: " << m_camera_yaw << " Pitch: " << m_camera_pitch << endl;

  glRotatef(m_camera_yaw, 0.0, 1.0, 0.0);
  glRotatef(m_camera_pitch, 1.0, 0.0, 0.0);
#endif
   //m_camera_x = 0;
   //m_camera_y = 20;
   //m_camera_z = 60;

   m_target_x = m_kayak_cent_x[ind];
   m_target_y = m_kayak_cent_y[ind];
   m_target_z = m_kayak_cent_z[ind];

  gluLookAt(m_camera_x, m_camera_y, m_camera_z,m_target_x,m_target_y,m_target_z,0,1,0);
}

//-----------------------------------------------------------
// Procedure: reset

void WireframeViewer::reset()
{

   m_camera_x = 0;
   m_camera_y = 30;
   m_camera_z = 80;
   m_camera_pitch = 0; 
   m_camera_yaw   = 0;

   m_target_x = 0;
   m_target_y = 0;
   m_target_z = 0;

   m_rotation = 0;

   gluLookAt(m_camera_x, m_camera_y, m_camera_z,m_target_x,m_target_y,m_target_z,0,1,0); 
}

//----------------------------------------------------------
// Procedure: set Viewpoint

void WireframeViewer::setViewpoint(float vp_x, float vp_y, float vp_z)
{  
//TODO
}
//-----------------------------------------------------------
// Procedure: translate

void WireframeViewer::translateCamera(float cx, float cy, float cz)
{
   m_camera_x = cx;
   m_camera_y = cy;
   m_camera_z = cz;
   glTranslatef(m_camera_x, m_camera_y, m_camera_z);
}

//-----------------------------------------------------------
// Procedure: handle_mouse_left

void WireframeViewer::handle_mouse_left()
{
  while(m_camera_pitch < 0)
    m_camera_pitch += 360.0;
  while(m_camera_pitch > 360.0)
    m_camera_pitch -= 360;

  while(m_camera_yaw < 0)
    m_camera_yaw += 360.0;
  while(m_camera_yaw > 360.0)
    m_camera_yaw -= 360; 
//glPushMatrix();
  glRotatef((m_camera_yaw), 0.0, 1.0, 0.0);
  glRotatef((m_camera_pitch), 1.0, 0.0, 0.0);
//  glTranslatef(-m_camera_x, -m_camera_y, -m_camera_z);
//glPopMatrix();
  m_old_cursor_pos_x = m_curr_cursor_pos_x;
  m_old_cursor_pos_y = m_curr_cursor_pos_y;

  redraw();
//glPopMatrix();
}

//-----------------------------------------------------------
// Procedure: handle_mouse_right
void WireframeViewer::handle_mouse_right()
{}
