/*****************************************************************/
/*    NAME: Doug Maxwell - Mike Benjamin                         */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: arc.cpp                                              */
/*    DATE: December 2007                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <FL/gl.h>

// See http://www.cmake.org/pipermail/cmake/2003-March/003482.html
#if OPSYS_IS_LINUX
   #include <GL/glu.h>
#elif OPSYS_IS_OS_X
   #include <OpenGL/glu.h>
#else
   #error "Unknown OS"
#endif

#include "arc.h"

#define DEG2RAD 0.01745329

using namespace std;

//-----------------------------------------------------------
// Constructor:  radius, start angle, end angle, center
Arc::Arc( float radius, float startAngle, float endAngle, 
float x, float y, float z, float r, float g, float b, float alpha){
}

//-----------------------------------------------------------
// setRadius  
// TODO  add a check for neg radius
void Arc::setRadius(float r){ radius = r; }

//-----------------------------------------------------------
// getRadius  
float Arc::getRadius(){ return radius; }

//-----------------------------------------------------------
// drawCircle
void Arc::drawArc(float radius, float x, float y, float z, float startAngle, float endAngle, 
float r, float g, float b, float alpha){
  //cout << "in drawArc" << endl;
  glPushMatrix();
  glTranslatef(x,y,z);
  glColor4f(r,g,b,alpha);
  glBegin(GL_LINE_STRIP);
  for(float i=startAngle*DEG2RAD; i<endAngle*DEG2RAD; i = i+0.05){
    glVertex3f(cos(i)*radius,0.0,sin(i)*radius);
	//cout << "drawing... x: " << cos(iRadian)*radius << " y: " << sin(iRadian)*radius << endl;
  }
  glEnd();
  glPopMatrix();
}
