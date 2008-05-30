/*****************************************************************/
/*    NAME: Doug Maxwell and Michael Benjamin                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: point.cpp                                            */
/*    DATE: December 2007                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <FL/gl.h>
#include <math.h>
#include "point.h"

#define DEG2RAD 0.01745329

//-----------------------------------------------------------
// Constructor
Point::Point(float a, float b, float c, float red, float green, float blue, float alpha){}

void Point::drawPoint(float a, float b, float c, float red, float green, float blue, float alpha){

  //float radius = 1.25;
  //float rect = 12.5;
  float rect = 0.25;

  glPushMatrix();
  glTranslatef(a, b, c);
  glColor4f(red,green,blue,alpha);
//Draw point as a small rectangle
  glRectf(-1*rect, -1*rect, rect, rect);

//Draw point as a tiny axis
/*
  glBegin(GL_LINES);
  glColor3f(1,0,0);
  glVertex3f(-0.05,0,0);
  glVertex3f(0.05,0,0);
  glEnd();
  glBegin(GL_LINES);
  glColor3f(0,1,0);
  glVertex3f(0,-0.05,0);
  glVertex3f(0,0.05,0);
  glEnd();
  glBegin(GL_LINES);
  glColor3f(0,0,1);
  glVertex3f(0,0,-0.05);
  glVertex3f(0,0,0.05);
  glEnd();
*/

//Draw Points as a line fan *very* inefficient
/*
  for(int i=0; i<360; i = i + 4){
    float iRadian = i*DEG2RAD;
    glBegin(GL_LINES);
    glVertex2f(0.0,0.0);
    glVertex2f(cos(iRadian)*radius, sin(iRadian)*radius);
    glEnd();
  }
*/

  glPopMatrix();
}
