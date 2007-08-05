/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: QuadViewer.cpp                                       */
/*    DATE: June 30th, 2005                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include <iostream>
#include <math.h>
#include "QuadViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"

using namespace std;

QuadViewer::QuadViewer(int gx, int gy, int gw, int gh, const char *gl)
  : Fl_Gl_Window(gx,gy,gw,gh,gl)
{
  valid_cache = false;
  curr_point  = 1;

  xlow  = 0 - (gw / 2);
  xhigh = gw / 2;
  ylow  = 0 - (gh / 2);
  yhigh = gh / 2;

  px1 = -200;
  py1 = 0;
  px2 = 0;
  py2 = 200;
  px3 = 200;
  py3 = 0;

  min_y = 0;
  min_x = 0;
  max_y = 0; 
  max_x = 0;

  set_coeffs(px1, py1, px2, py2, px3, py3, qm, qn, qb);
  set_bounds();
  set_xcoeffs();
}

//-------------------------------------------------------------
// Procedure: handle()

int QuadViewer::handle(int event)
{
  int vx, vy;
  switch(event) {
  case FL_PUSH:
    vx = Fl::event_x();
    vy = h() - Fl::event_y();
    if(Fl_Window::handle(event) != 1) {
      if(Fl::event_button() == FL_LEFT_MOUSE)
	handle_left_mouse(vx, vy);
      if(Fl::event_button() == FL_RIGHT_MOUSE)
	handle_right_mouse(vx, vy);
    }
    return(1);
    break;
  default:
    return(Fl_Gl_Window::handle(event));
  }
  return(0);
}

//-------------------------------------------------------------
// Procedure: draw()

void QuadViewer::draw()
{
  glClearColor(0.5,0.5,0.5 ,0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glViewport(0, 0, w(), h());

  set_coeffs(px1, py1, px2, py2, px3, py3, qm, qn, qb);
  set_bounds();
  set_xcoeffs();

  draw_axes();
  draw_plot();
  draw_points();
}

//-------------------------------------------------------------
// Procedure: resize()

void QuadViewer::resize(int gx, int gy, int gw, int gh)
{
  valid_cache = false;

  xlow  = 0 - (gw / 2);
  xhigh = gw / 2;
  ylow  = 0 - (gh / 2);
  yhigh = gh / 2;

  Fl_Gl_Window::resize(gx, gy, gw, gh);
}

//-------------------------------------------------------------
// Procedure: get_pstring

string QuadViewer::get_pstring(int ix)
{
  string sx = "E";
  string sy = "E";
  if(ix == 1) {
    sx = doubleToString(px1);
    sy = doubleToString(py1);
  }
  if(ix == 2) {
    sx = doubleToString(px2);
    sy = doubleToString(py2);
  }
  if(ix == 3) {
    sx = doubleToString(px3);
    sy = doubleToString(py3);
  }
  sx = dstringCompact(sx);
  sy = dstringCompact(sy);

  string str = "(" + sx + ", " + sy + ")";
  return(str);
}

//-------------------------------------------------------------
// Procedure: get_coeff

string QuadViewer::get_coeff(int ix)
{
  string str = "error";
  if(ix == 0)
    str = doubleToString(qb);
  if(ix == 1)
    str = doubleToString(qn);
  if(ix == 2)
    str = doubleToString(qm);

  str = dstringCompact(str);
 return(str);
}

//-------------------------------------------------------------
// Procedure: move_x

void QuadViewer::move_x(int amt)
{
  double dval = (double)(amt);

  if(curr_point == 1) {
    double new_x = px1 + dval;
    if((new_x <= px2) && (new_x <= px3))
      px1 = new_x;
  }
  if(curr_point == 2) {
    double new_x = px2 + dval;
    if((new_x >= px1) && (new_x <= px3))
      px2 = new_x;
  }
  if(curr_point == 3) {
    double new_x = px3 + dval;
    if((new_x >= px1) && (new_x >= px2))
      px3 = new_x;
  }
  valid_cache = false;
}

//-------------------------------------------------------------
// Procedure: curr_pt

void QuadViewer::curr_pt(int ix)
{
  if((ix >= 1) && (ix <= 3))
    curr_point = ix;
}


//-------------------------------------------------------------
// Procedure: move_y

void QuadViewer::move_y(int amt)
{
  if(curr_point == 1)
    py1 += (double)(amt);
  if(curr_point == 2)
    py2 += (double)(amt);
  if(curr_point == 3)
    py3 += (double)(amt);
  valid_cache = false;
}



//-------------------------------------------------------------
// Procedure: handle_left_mouse

int QuadViewer::handle_left_mouse(int vx, int vy)
{
  double dvx = (double)vx + xlow;
  double dvy = (double)vy + ylow;

  if(curr_point == 1) {
    if((dvx <= px2) && (dvx <= px3)) {
      px1 = dvx; 
      py1 = dvy;
    }
  }
  else if(curr_point == 2) {
    if((dvx >= px1) && (dvx <= px3)) {
      px2 = dvx; 
      py2 = dvy;
    }
  }
  else if(curr_point == 3) {
    if((dvx >= px1) && (dvx >= px2)) {
      px3 = dvx; 
      py3 = dvy;
    }
  }

  valid_cache = false;
  redraw();
  return(0);
}

//-------------------------------------------------------------
// Procedure: handle_right_mouse

int QuadViewer::handle_right_mouse(int vx, int vy)
{
  double dvx = (double)vx + xlow;
  double dvy = (double)vy + ylow;

  double dist1 = hypot(dvx-px1, dvy-py1);
  curr_point = 1;
  double lowest_dist = dist1;


  double dist2 = hypot(dvx-px2, dvy-py2);
  if(dist2 < lowest_dist) {
    curr_point = 2;
    lowest_dist = dist2;
  }

  double dist3 = hypot(dvx-px3, dvy-py3);
  if(dist3 < lowest_dist)
    curr_point = 3;

  redraw();

  return(0);
}

//-------------------------------------------------------------
// Procedure: fill_cache

bool QuadViewer::fill_cache()
{
  valid_cache = false;

  cache_x1.clear();
  cache_y1.clear();

  for(double x=xlow; x<xhigh; x++) {
    cache_x1.push_back(x);
    double fx = (x*x*qm) + (x*qn) + qb;
    cache_y1.push_back(fx);
  }

  cache_x2.clear();
  cache_y2.clear();

  for(double x=xlow; x<xhigh; x++) {
    cache_x2.push_back(x);
    double fx = (x*x*qxm) + (x*qxn) + qxb;
    cache_y2.push_back(fx);
  }

  valid_cache = true;
  return(true);
}

//-------------------------------------------------------------
// Procedure: set_coeffs
//      Note: Use Gaussian elimination to solve a group of three
//            equations with three unknowns.
//
//            qm(x1^2) + qn(x1) + qb = y1
//            qm(x2^2) + qn(x2) + qb = y2
//            qm(x3^2) + qn(x3) + qb = y3
//
//                                -1
//            |m11 m12 m13|   |qm|     |m14|
//            |m21 m22 m23| * |qn|   = |m24|
//            |m31 m32 m33|   |qb|     |m34|

void QuadViewer::set_coeffs(double  x1, double  y1, double  x2,
			    double  y2, double  x3, double  y3, 
			    double& rm, double& rn, double& rb)
  
{
  if((x1==x2) || (x1==x3) || (x2==x3))
    return;

  double m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34;
  m11 = x1*x1;    m12 = x1;    m13 = 1;    m14 = y1;  
  m21 = x2*x2;    m22 = x2;    m23 = 1;    m24 = y2;
  m31 = x3*x3;    m32 = x3;    m33 = 1;    m34 = y3;

  // operation 1
  m31 = m21 - m31;
  m32 = m22 - m32;
  m33 = 0;
  m34 = m24 - m34;

  // operation 2
  m21 = m11 - m21;
  m22 = m12 - m22;
  m23 = 0;
  m24 = m14 - m24;

  double t22 = m22;
  // operation 3
  m21 *= m32; // Make m22 and m32 equal
  m22 *= m32;
  m24 *= m32;

  m31 *= t22;
  m32 *= t22;
  m34 *= t22;

  m31 = m21 - m31;
  m32 = 0;
  m33 = 0;
  m34 = m24 - m34;

  // solve for qm
  rm = m34 / m31;

  // solve for qn
  rn = (m24 -(m21 * rm)) / m22; 

  // solve for qb
  rb = (m14 - (m11 * rm) - (m12 * rn)) / m13; 
}


//-------------------------------------------------------------
// Procedure: set_bounds
//     Notes: 

void QuadViewer::set_bounds()
{
  min_y = py1;
  min_x = px1;
  max_y = py1;
  max_x = px1;

  if(py2 > max_y) {
    max_y = py2;
    max_x = px2;
  }
  if(py2 < min_y) {
    min_y = py2;
    min_x = px2;
  }
  if(py3 > max_y) {
    max_y = py3;
    max_x = px3;
  }
  if(py3 < min_y) {
    min_y = py3;
    min_x = px3;
  }

  double peak_x = -qn / (2.0 *qm);
  bool out_low = (peak_x < px1) && (peak_x < px2) && (peak_x < px3);
  bool out_hgh = (peak_x > px1) && (peak_x > px2) && (peak_x > px3);
  if(!out_low && !out_hgh) {
    double peak_y = (qm * peak_x * peak_x) + (qn * peak_x) + qb;
    if(peak_y > max_y) {
      max_y = peak_y;
      max_x = peak_x;
    }
    if(peak_y < min_y) {
      min_y = peak_y;
      min_x = peak_x;
    }
  }
}


//-------------------------------------------------------------
// Procedure: set_xcoeffs
//     Notes: 

void QuadViewer::set_xcoeffs()
{
  if((max_y > py1) && (max_y > py2) && (max_y > py3)) {
    double new_maxy = py1;
    if(py2 > new_maxy) new_maxy = py2;
    if(py3 > new_maxy) new_maxy = py3;
    set_coeffs(px1, py1, max_x, new_maxy, px3, py3, qxm, qxn, qxb);
    return;
  }
  if((min_y < py1) && (min_y < py2) && (min_y < py3)) {
    double new_miny = py1;
    if(py2 < new_miny) new_miny = py2;
    if(py3 < new_miny) new_miny = py3;
    set_coeffs(px1, py1, min_x, new_miny, px3, py3, qxm, qxn, qxb);
    return;
  }
}


//-------------------------------------------------------------
// Procedure: draw_logplot
//  483d8b   .282 .239 .545   DarSlateBlue
//  8b0000   .545 .000 .000   DarkRed
//  2f4f4f   .184 .310 .310   DarkSlateGrey
//  daa520   .855 .647 .125   GoldenRod
//  e9967a   .914 .588 .478   DarkSalmon
//  8fbc8f   .561 .737 .561   DarkSeaGreen
//  bdb76b                    DarkKhaki
//  fafad2   .980 .980 .824   LightGoldenRodYellow

void QuadViewer::draw_plot()
{
  if(!valid_cache)
    fill_cache();
  if(!valid_cache)
    return;
    
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef((xhigh-xlow)/2, (yhigh-ylow)/2, 0);

  // Draw the LINES of the plot
  glColor4f(0.282,  0.239,  0.545,  0.1);  // DarkSlateBlue
  glBegin(GL_LINE_STRIP);
  for(int i=0; i<cache_x1.size(); i++)
    glVertex2f(cache_x1[i], cache_y1[i]);
  glEnd();
  // Draw the POINTS of the plot
  glPointSize(2.0);
  glColor4f(0.561,  0.737,  0.561,  0.1);  // DarkSeaGreen
  glBegin(GL_POINTS);
  for(int i=0; i<cache_x1.size(); i++)
    glVertex2f(cache_x1[i], cache_y1[i]);
  glEnd();

  // Draw the LINES of the plot
  glColor4f(0.545,  0.000,  0.000,  0.1);  // DarkRed
  glBegin(GL_LINE_STRIP);
  for(int i=0; i<cache_x2.size(); i++)
    glVertex2f(cache_x2[i], cache_y2[i]);
  glEnd();
  // Draw the POINTS of the plot
  glPointSize(2.0);
  glColor4f(0.980,  0.980,  0.824,  0.1);  // LightGoldenRod
  glBegin(GL_POINTS);
  for(int i=0; i<cache_x2.size(); i++)
    glVertex2f(cache_x2[i], cache_y2[i]);
  glEnd();

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: draw_axes

void QuadViewer::draw_axes()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef((xhigh-xlow)/2, (yhigh-ylow)/2, 0);

  glColor4f(1.0,  1.0,  1.0,  0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(xlow+5,  0);
  glVertex2f(xhigh-5, 0);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f(0, ylow+5);
  glVertex2f(0, yhigh-5);
  glEnd();

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: draw_points

void QuadViewer::draw_points()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef((xhigh-xlow)/2, (yhigh-ylow)/2, 0);
  glPointSize(3.0);

  //  8b0000   .545 .000 .000   DarkRed
  glColor4f(0.545,  0.000,  0.000,  0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(px1, py1);
  glVertex2f(px2, py2);
  glVertex2f(px3, py3);
  glEnd();

  //  e9967a   .914 .588 .478   DarkSalmon
  glColor4f(0.914,  0.588,  0.478,  0.1);
  glBegin(GL_POINTS);
  glVertex2f(px1, py1);
  glVertex2f(px2, py2);
  glVertex2f(px3, py3);
  glEnd();

  // Now redraw the current point YELLOW
  glColor4f(1.000,  1.000,  0.000,  0.1);
  glBegin(GL_POINTS);
  if(curr_point == 1)
    glVertex2f(px1, py1);
  if(curr_point == 2)
    glVertex2f(px2, py2);
  if(curr_point == 3)
    glVertex2f(px3, py3);
  glEnd();

  glFlush();
  glPopMatrix();
}




