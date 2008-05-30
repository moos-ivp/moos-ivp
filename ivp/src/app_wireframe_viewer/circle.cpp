/*****************************************************************/
/*    NAME: Doug Maxwell and Michael Benjamin                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: circle.cpp                                           */
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

#include "circle.h"

#define DEG2RAD 0.01745329
using namespace std;
//-----------------------------------------------------------
// Constructor
Circle::Circle( float r, float a, float b, float c, float red, float green, float blue, float alpha){
//  setCircle(r, a, b, c);
}

//-----------------------------------------------------------
// setRadius  
// TODO  add a check for neg radius
void Circle::setRadius(float r){ radius = r; }

//-----------------------------------------------------------
// getRadius  
float Circle::getRadius(){ return radius; }

//-----------------------------------------------------------
// drawCircle
void Circle::drawCircle(float radius, float a, float b, float c, float red, float green, float blue, float alpha){
  glPushMatrix();
//cout << "Drawing a circle: r " << radius << endl;
  glTranslatef(a, b, c);
  glColor4f(red,green,blue,alpha);
  glBegin(GL_LINE_LOOP);
  for(int i=0; i<360; i++){
    float iRadian = i*DEG2RAD;
    glVertex3f(cos(iRadian)*radius,0.0, sin(iRadian)*radius);
  }
  glEnd();
  glPopMatrix();
  glFlush();
}
