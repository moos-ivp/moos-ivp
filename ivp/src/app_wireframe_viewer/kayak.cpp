/*****************************************************************/
/*    NAME: Doug Maxwell and Michael Benjamin                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: kayak.cpp                                            */
/*    DATE: December 2007                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <FL/Fl.H>
#include <FL/gl.h>

// See http://www.cmake.org/pipermail/cmake/2003-March/003482.html
#if OPSYS_IS_LINUX
   #include <GL/glu.h>
#elif OPSYS_IS_OS_X
   #include <OpenGL/glu.h>
#else
   #error "Unknown OS"
#endif

#include "kayak.h"

#define DEG2RAD 0.01745329

//-----------------------------------------------------------
// Constructor
Kayak::Kayak(float a, float b, float c, float red, float green, float blue, float alpha){}

//-----------------------------------------------------------
// setKayak
void Kayak::setKayak(
	float cent_x, float cent_y, float cent_z, 
	float red, float green, float blue, float alpha)
{
	kx = cent_x;
	ky = cent_y;
	kz = cent_z;
	kred = red;
	kblue = blue;
	kgreen = green;
	kalpha = alpha;
}
//-----------------------------------------------------------
// drawKayak
void Kayak::drawKayak(
	float cent_x, float cent_y, float cent_z, 
	float red, float green, float blue, float alpha)
{
  glPushMatrix();
  glColor4f(red,green,blue,alpha);
  glTranslatef(cent_x, cent_y, cent_z);
  glBegin(GL_LINE_LOOP);
  //glColor3f(1,1,0);
  glVertex3f(-0.5,0,0);//central member
  glVertex3f(-0.25,-0.25,0.0);
  glVertex3f(0.25,-0.25,0.0);
  glVertex3f(0.5,0,0);
  glEnd();
  glBegin(GL_LINE_LOOP);//forward member 
  //glColor3f(1,1,0);
  glVertex3f(-0.25,0.0,1.25);
  glVertex3f(-0.125,-0.125,1.25);
  glVertex3f(0.125,-0.125,1.25);
  glVertex3f(0.25,0.0,1.25);
  glEnd();
  glBegin(GL_LINE_LOOP);//bow
  //glColor3f(1,1,0);
  glVertex3f(-0.125,0.0,2.5);
  glVertex3f(-0.0625,-0.0625,2.5);
  glVertex3f(0.0625,-0.0625,2.5);
  glVertex3f(0.125,0.0,2.5);
  glEnd();
  glBegin(GL_LINE_LOOP);//aft member
  //glColor3f(1,1,0);
  glVertex3f(-0.25,0.0,-1.25);
  glVertex3f(-0.125,-0.125,-1.25);
  glVertex3f(0.125,-0.125,-1.25);
  glVertex3f(0.25,0.0,-1.25);
  glEnd();
  glBegin(GL_LINE_LOOP);//aft
  //glColor3f(1,1,0);
  glVertex3f(-0.125,0.0,-2.5);
  glVertex3f(-0.0625,-0.0625,-2.5);
  glVertex3f(0.0625,-0.0625,-2.5);
  glVertex3f(0.125,0.0,-2.5);
  glEnd();
  glBegin(GL_LINE_LOOP);//deck
  //glColor3f(1,1,0);
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
  //glColor3f(1,1,0);
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
  glPopMatrix();
}
