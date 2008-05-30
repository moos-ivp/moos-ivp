/*****************************************************************/
/*    NAME: Doug Maxwell and Michael Benjamin                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: plane.cpp                                            */
/*    DATE: December 2007                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
#include <FL/gl.h>
#include <math.h>
#include "plane.h"

//-----------------------------------------------------------
// Constructor
Plane::Plane(float a, float b, float c, float side, float red, float green, float blue, float alpha){}

void Plane::drawPlane(
	float a, float b, float c, float side, float red, float green, float blue, float alpha){
  float rect = side/2.0;
  glPushMatrix();
  glTranslatef(a, b, c);
  glColor4f(red,green,blue,alpha);
  glRectf(-1*rect, -1*rect, rect, rect);
  glPopMatrix();
}
