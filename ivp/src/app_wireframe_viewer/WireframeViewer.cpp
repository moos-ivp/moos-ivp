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
#include <FL/gl.h>
#include <GL/glu.h>
#include "WireframeViewer.h"

#define DEG2RAD 0.01745329

//-----------------------------------------------------------
// Constructor

WireframeViewer::WireframeViewer(int x, int y, int w, int h, char* title)
  : Fl_Gl_Window(x,y,w,h,title)
{
  mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);

  m_rotation = 0.f;
  m_rotation_increment = 1.0f;
  m_animating = false;
  
  m_camera_x = 0;
  m_camera_y = 2;
  m_camera_z = 4;

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
  glClearColor( 0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);
  
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  float lightColor[4] = {1, 1, 1, 1};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
  
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
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
  glFrustum(-1, 1, -1, 1, 1, 120);  // I like 120 deg, your mileage may vary
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0,0,4,0,0,0,0,1,0); 
  //gluLookAt(0,2,4,m_target_x,m_target_y,m_target_z,0,1,0); 
  //setCOI(m_target_x,m_target_y,m_target_z);
  setCameraAngle(); //setup ego centric viewing
  float lightPosition[4] = {5, 5, 5, 1};
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  glPushMatrix();
  glRotatef(m_rotation, 0, 1, 0);  // exo centric x/z planar rotation
  DrawAxes();
  DrawKayak(4.0, 0.0, 2.0);
  DrawKayak(-2.0, 0.0, -2.0);
  glPopMatrix();
}

//-----------------------------------------------------------
// Procedure: drawAxes

void WireframeViewer::DrawAxes()
{
  //visual aid, RED_X, GREEN_Y, BLUE_Z
  //glPushMatrix();
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
  //glPopMatrix();
}

//-----------------------------------------------------------
// Procedure: drawKayak

void WireframeViewer::DrawKayak(float m_kayak_x, float m_kayak_y, float m_kayak_z)
{
  //glPushMatrix();
  glTranslatef(m_kayak_x, m_kayak_y, m_kayak_z);
  glBegin(GL_LINE_LOOP);
  glColor3f(1,1,0);
  glVertex3f(-0.5,0,0);//central member
  glVertex3f(-0.25,-0.25,0.0);
  glVertex3f(0.25,-0.25,0.0);
  glVertex3f(0.5,0,0);
  glEnd();
  glBegin(GL_LINE_LOOP);//forward member 
  glColor3f(1,1,0);
  glVertex3f(-0.25,0.0,1.25);
  glVertex3f(-0.125,-0.125,1.25);
  glVertex3f(0.125,-0.125,1.25);
  glVertex3f(0.25,0.0,1.25);
  glEnd();
  glBegin(GL_LINE_LOOP);//bow
  glColor3f(1,1,0);
  glVertex3f(-0.125,0.0,2.5);
  glVertex3f(-0.0625,-0.0625,2.5);
  glVertex3f(0.0625,-0.0625,2.5);
  glVertex3f(0.125,0.0,2.5);
  glEnd();
  glBegin(GL_LINE_LOOP);//aft member
  glColor3f(1,1,0);
  glVertex3f(-0.25,0.0,-1.25);
  glVertex3f(-0.125,-0.125,-1.25);
  glVertex3f(0.125,-0.125,-1.25);
  glVertex3f(0.25,0.0,-1.25);
  glEnd();
  glBegin(GL_LINE_LOOP);//aft
  glColor3f(1,1,0);
  glVertex3f(-0.125,0.0,-2.5);
  glVertex3f(-0.0625,-0.0625,-2.5);
  glVertex3f(0.0625,-0.0625,-2.5);
  glVertex3f(0.125,0.0,-2.5);
  glEnd();
  glBegin(GL_LINE_LOOP);//deck
  glColor3f(1,1,0);
  glVertex3f(-0.125,0,2.5);
  glVertex3f(-0.25,0,1.25);
  glVertex3f(-0.5,0,0);
  glVertex3f(-0.25,0,-1.25);
  glVertex3f(-0.125,0,-2.5);
  glVertex3f(0.125,0,-2.5);
  glVertex3f(0.25,0,-1.25);
  glVertex3f(0.5,0,0);
  glVertex3f(0.25,0, 1.25);
  glVertex3f(0.125,0,2.5);
  glEnd();
  glBegin(GL_LINE_LOOP);//water level(?)
  glColor3f(1,1,0);
  glVertex3f(-0.0625, -0.0625, 2.5);
  glVertex3f(-0.125, -0.125, 1.25);
  glVertex3f(-0.25, -0.25, 0);
  glVertex3f(-0.125,-0.125,-1.25);
  glVertex3f(-0.0625,-0.0625,-2.5);
  glVertex3f(0.0625, -0.0625, -2.5);
  glVertex3f(0.125, -0.125, -1.25);
  glVertex3f(0.25,-0.25,0);
  glVertex3f(0.125, -0.125, 1.25);
  glVertex3f(0.0625, -0.0625, 2.5);
  glEnd();	
  //glPopMatrix();
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

  glRotatef(m_camera_yaw, 0.0, 1.0, 0.0);
  glRotatef(m_camera_pitch, 1.0, 0.0, 0.0);
  glTranslatef(-m_camera_x, -m_camera_y, -m_camera_z);
}

//-----------------------------------------------------------
// Procedure: setCOI

void WireframeViewer::setCOI(float tx, float ty, float tz)
{
   m_target_x = tx;
   m_target_y = ty;
   m_target_z = tz;

   gluLookAt(m_camera_x, m_camera_y, m_camera_z,m_target_x,m_target_y,m_target_z,0,1,0); 
}

//-----------------------------------------------------------
// Procedure: reset

void WireframeViewer::reset()
{
   m_camera_x = 0; 
   m_camera_y = 2; 
   m_camera_z = 4; 
   m_camera_pitch = 0; 
   m_camera_yaw   = 0;

   m_target_x = 0;
   m_target_y = 0;
   m_target_z = 0;

   gluLookAt(m_camera_x, m_camera_y, m_camera_z,m_target_x,m_target_y,m_target_z,0,1,0); 
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

  glRotatef((m_camera_yaw), 0.0, 1.0, 0.0);
  glRotatef((m_camera_pitch), 1.0, 0.0, 0.0);
  glTranslatef(-m_camera_x, -m_camera_y, -m_camera_z);

  m_old_cursor_pos_x = m_curr_cursor_pos_x;
  m_old_cursor_pos_y = m_curr_cursor_pos_y;

  redraw();
}

//-----------------------------------------------------------
// Procedure: handle_mouse_right
void WireframeViewer::handle_mouse_right()
{}


