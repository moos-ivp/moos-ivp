/*****************************************************************/
/*    NAME: Doug Maxwell and Michael Benjamin                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ray.cpp                                              */
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
#include <FL/Fl.H>
#include <FL/gl.h>
#include <GL/glu.h>
#include "ray.h"

#define DEG2RAD 0.01745329

using namespace std;

//-----------------------------------------------------------
// Constructor
Ray::Ray(float a, float b, float c, float d, float e, float f, float red, float green, float blue, float alpha){ 
	//setLine(a, b, c, d, e, f);
}

//-----------------------------------------------------------
// Procedure: setLine
void Ray::setLine(float a, float b, float c, float d, float e, float f){
	start_x = a;
	start_y = b;
	start_z = c;
	end_x = d;
	end_y = e;
	end_z = c;
}
//-----------------------------------------------------------
// Procedure: drawLine
void Ray::drawLine(float a, float b, float c, float d, float e, float f, float red, float green, float blue, float alpha){
  glPushMatrix();
//  cout<<"Drawing a Line Start: x "<< a << "y "<< b << "z " << c << endl;
//  cout<<"Drawing a Line End: x "<< d << "y "<< e << "z " << f << endl;
  glBegin(GL_LINES);
  glColor4f(red,green,blue,alpha);
  glVertex3f(a,b,c);
  glVertex3f(d,e,f);
  glEnd();
  glPopMatrix();
}

//-----------------------------------------------------------
// TODO
// When ready for true vectors
// Procedure: drawRay
