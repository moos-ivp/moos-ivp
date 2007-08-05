/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PolyViewer.cpp                                       */
/*    DATE: May 1st, 2005                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <string>
#include <iostream>
#include <math.h>
#include <tiffio.h>
#include "PolyViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "CircularUtils.h"
#include "AngleUtils.h"

using namespace std;

PolyViewer::PolyViewer(int x, int y, 
			 int width, int height, const char *l)
  : CommonMarineViewer(x,y,width,height,l)
{
  m_snap_val    = 10.0;
  m_active_poly = 0;

  // Modes:   0:place 1:edit 2:delete 3:insert 4:test_view
  m_drop_mode = 0;     
  m_test_mx   = 0.0;
  m_test_my   = 0.0;
  m_test_mxx  = 0.0;
  m_test_myy  = 0.0;
  m_mode8_angle = 45;
  
  counter = 0;
}

//-------------------------------------------------------------
// Procedure: handle()

int PolyViewer::handle(int event)
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
}

//-------------------------------------------------------------
// Procedure: draw()

void PolyViewer::draw()
{
  CommonMarineViewer::draw();


  int vsize = m_poly.size();
  for(int i=0; i<vsize; i++) {
    if(i == m_active_poly)
      drawPoly(m_poly[i], true, false);
    else
      drawPoly(m_poly[i], false, false);
  }

  if(m_drop_mode == 4)
    drawMode4();

  if(m_drop_mode == 5)
    drawMode5();

  if(m_drop_mode == 6)
    drawMode6();

  if(m_drop_mode == 7)
    drawMode7();

  if(m_drop_mode == 8)
    drawMode8();

  if(m_drop_mode == 9)
    drawMode9();

  drawGrids();
}

//-------------------------------------------------------------
// Procedure: handle_left_mouse

void PolyViewer::handle_left_mouse(int vx, int vy)
{
  int vsize = m_poly.size();

  double ix = view2img("x", vx);
  double iy = view2img("y", vy);
  double mx = img2meters("x", ix);
  double my = img2meters("y", iy);
  double sx = snapToStep(mx, m_snap_val);
  double sy = snapToStep(my, m_snap_val);

  if(m_drop_mode == 0) {
    if(vsize == 0) {
      m_active_poly = 0;
      XYPolygon newpoly;
      char buff[2];
      buff[0] = 65+vsize;
      buff[1] = '\0';
      newpoly.set_label(buff);
      m_poly.push_back(newpoly);
      vsize++;
    }
    m_poly[m_active_poly].add_vertex(sx, sy);
  }
  if(m_drop_mode == 1) {
    if(vsize > 0)
      m_poly[m_active_poly].alter_vertex(sx, sy);
  }
  if(m_drop_mode == 2) {
    if(vsize > 0)
      m_poly[m_active_poly].delete_vertex(mx, my);
  }
  if(m_drop_mode == 3) {
    if(vsize > 0)
      m_poly[m_active_poly].insert_vertex(sx, sy);
  }
  if(m_drop_mode == 4) {
    m_test_mx = sx;
    m_test_my = sy;
  }
  if(m_drop_mode == 5) {
    m_test_mxx = sx;
    m_test_myy = sy;
  }
  if(m_drop_mode == 6) {
    m_test_mxx = sx;
    m_test_myy = sy;
  }
  if(m_drop_mode == 7) {
    m_test_mx = sx;
    m_test_my = sy;
  }
  if(m_drop_mode == 8) {
    m_test_mx = sx;
    m_test_my = sy;
  }
  if(m_drop_mode == 9) {
    m_test_mxx = sx;
    m_test_myy = sy;
  }
  redraw();
}

//-------------------------------------------------------------
// Procedure: handle_right_mouse

void PolyViewer::handle_right_mouse(int vx, int vy)
{
  int vsize = m_poly.size(); 
  if(vsize == 0)
    return;

  double ix = view2img("x", vx);
  double iy = view2img("y", vy);
  double mx = img2meters("x", ix);
  double my = img2meters("y", iy);
  double sx = snapToStep(mx, m_snap_val);
  double sy = snapToStep(my, m_snap_val);

  if((m_drop_mode == 6) || (m_drop_mode == 9)) {
    m_test_mx = sx;
    m_test_my = sy;
    redraw();
    return;
  }

  m_active_poly = -1;
  for(int i=0; i<vsize; i++) {
    if(m_poly[i].contains(mx, my))
      m_active_poly = i;
  }

  if(m_active_poly == -1) {
    if(m_poly[vsize-1].size() < 3) {
      m_poly[vsize-1].clear();
      m_active_poly = vsize-1;
    }
    else {
      XYPolygon newpoly;
      char buff[2];
      buff[0] = 65+vsize;
      buff[1] = '\0';
      newpoly.set_label(buff);
      m_poly.push_back(newpoly);
      m_active_poly = vsize;
    }
    // If current poly is empty, makes sense to always be in 
    // place-mode
    m_drop_mode = 0;
  }

  redraw();
}

// ----------------------------------------------------------
// Procedure: getImgPctX
//   Purpose: For a given x position, return its position, in 
//            terms of percentage of the background image.

float PolyViewer::getImgPctX(int index)
{
  if(index == -1) {
    float x_pos = (m_back_img.get_img_width() / 2.0) - m_vshift_x;
    float x_pct = m_back_img.pixToPctX(x_pos);
    return(x_pct);
  }

  return(0.0);
}

// ----------------------------------------------------------
// Procedure: getImgPctY
//   Purpose: For a given y position, return its position, in 
//            terms of percentage of the background image.

float PolyViewer::getImgPctY(int index)
{
  if(index == -1) {
    float y_pos = (m_back_img.get_img_height() / 2.0) - m_vshift_y;
    float y_pct = m_back_img.pixToPctY(y_pos);
    return(y_pct);
  }

  return(0.0);
}

// ---------------------------------------------------------
// Procedure: getMetersX
//   Purpose: For a given x position, return its position, in 
//            terms of delta meters from the zero position.

float PolyViewer::getMetersX(int index)
{
  if(m_drop_mode == 4) {
    return(m_test_mx);
  }
  if((m_drop_mode == 5) || (m_drop_mode == 6) || (m_drop_mode==8)) {
    return(m_test_mxx);
  }
  else {
    int iwidth = m_back_img.get_img_width();
    float x_pos = ((float)(iwidth) / 2.0) - (float)(m_vshift_x);
    float x_pct = m_back_img.pixToPctX(x_pos);
    float x_pct_cent = m_back_img.get_img_centx();
    float x_pct_mtrs = m_back_img.get_img_meters();
    float meters = (x_pct - x_pct_cent) / (x_pct_mtrs / 100.0);
    float offset = m_back_img.get_img_offset_x();
    meters += offset;
    return(meters);
  }
}

// ----------------------------------------------------------
// Procedure: getMetersY
//   Purpose: For a given y position, return its position, in 
//            terms of delta meters from the zero position.

float PolyViewer::getMetersY(int index)
{
  if(m_drop_mode == 4) {
    return(m_test_mx);
  }
  if((m_drop_mode == 5) || (m_drop_mode == 6) || (m_drop_mode==8)) {
    return(m_test_myy);
  }
  else {
    int iheight = m_back_img.get_img_height();
    float y_pos = ((float)(iheight) / 2.0) - (float)(m_vshift_y);
    float y_pct = m_back_img.pixToPctY(y_pos);
    float y_pct_cent = m_back_img.get_img_centy();
    float y_pct_mtrs = m_back_img.get_img_meters();
    float meters = (y_pct - y_pct_cent) / (y_pct_mtrs / 100.0);
    float offset = m_back_img.get_img_offset_y();
    meters += offset;
    return(meters);
  }
}

// ----------------------------------------------------------
// Procedure: getLonX
//   Purpose: For a given x position, return its position, in 
//            terms of longitude.

float PolyViewer::getLonX(int index)
{
  if(index == -1) {
    float x_pos = ((float)(w()) / 2.0) - (float)(m_vshift_x);
    float x_pct = m_back_img.pixToPctX(x_pos);
    float x_pct_cent = m_back_img.get_img_centx();
    float x_pct_mtrs = m_back_img.get_img_meters();
    float meters = (x_pct - x_pct_cent) / (x_pct_mtrs / 100.0);
    float offset = m_back_img.get_img_offset_x();
    meters += offset;
    return(meters2LatLon("lon", meters));
  }

  return(0.0);
}

// ----------------------------------------------------------
// Procedure: getLatY
//   Purpose: For a given x position, return its position, in 
//            terms of latitude.

float PolyViewer::getLatY(int index)
{
  if(index == -1) {
    float y_pos = ((float)(h()) / 2.0) - (float)(m_vshift_y);
    float y_pct = m_back_img.pixToPctY(y_pos);
    float y_pct_cent = m_back_img.get_img_centy();
    float y_pct_mtrs = m_back_img.get_img_meters();
    float meters = (y_pct - y_pct_cent) / (y_pct_mtrs / 100.0);
    float offset = m_back_img.get_img_offset_y();
    meters += offset;
    return(meters2LatLon("lat", meters));
  }

  return(0.0);
}

// ----------------------------------------------------------
// Procedure: getPolySpec
//   Purpose: 

string PolyViewer::getPolySpec()
{
  if(m_poly.size() == 0) 
    return("");
  else {
    int s = m_poly.size();
    return(m_poly[m_active_poly].get_spec());
  }
}

// ----------------------------------------------------------
// Procedure: adjustActive
//   Purpose: 

void PolyViewer::adjustActive(int v)
{
  int old_ix = m_active_poly;

  m_active_poly += v;
  if(m_active_poly < 0)
    m_active_poly = 0;
  if(m_active_poly > m_poly.size()-1)
    m_active_poly = m_poly.size()-1;
 
  //cout << counter << "oldix: " << old_ix << endl;
  //cout << counter << "m_active_poly: " << m_active_poly << endl;
 
  if(m_active_poly != old_ix)
    redraw();
}

// ----------------------------------------------------------
// Procedure: shiftHorzPoly
//   Purpose: 

void PolyViewer::shiftHorzPoly(double shift_val)
{
  //cout << "PolyViewer: shift_val: " << shift_val << endl;
  m_poly[m_active_poly].shift_horz(shift_val);
}

// ----------------------------------------------------------
// Procedure: shiftVertPoly
//   Purpose: 

void PolyViewer::shiftVertPoly(double shift_val)
{
  m_poly[m_active_poly].shift_vert(shift_val);
}

// ----------------------------------------------------------
// Procedure: rotatePoly
//   Purpose: 

void PolyViewer::rotatePoly(int rval)
{
  m_poly[m_active_poly].rotate(rval);
}

// ----------------------------------------------------------
// Procedure: growPoly
//   Purpose: 

void PolyViewer::growPoly(int gval)
{
  double dgval = (double)(gval) / 100.0;
  m_poly[m_active_poly].grow_by_pct(dgval);
}

// ----------------------------------------------------------
// Procedure: reversePoly
//   Purpose: 

void PolyViewer::reversePoly()
{
  m_poly[m_active_poly].reverse();
}

// ----------------------------------------------------------
// Procedure: duplicateActive
//   Purpose: 

void PolyViewer::duplicateActive()
{
  XYPolygon new_poly = m_poly[m_active_poly];
  new_poly.shift_vert(-10);
  new_poly.shift_horz(10);
  addPoly(new_poly);
  m_active_poly = m_poly.size()-1;
}

// ----------------------------------------------------------
// Procedure: clearActivePoly
//   Purpose: 

void PolyViewer::clearActivePoly()
{
  vector<XYPolygon> new_poly;

  for(int i=0; i < m_poly.size(); i++)
    if(i != m_active_poly)
      new_poly.push_back(m_poly[i]);
  m_poly = new_poly;
  m_active_poly = 0;
}

// ----------------------------------------------------------
// Procedure: reApplySnapToCurrent
//   Purpose: 

void PolyViewer::reApplySnapToCurrent()
{
  m_poly[m_active_poly].apply_snap(m_snap_val);
}

//-------------------------------------------------------------
// Procedure: drawMode4

void PolyViewer::drawMode4()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img("x", 0);
  float ty = meters2img("y", 0);
  float qx = img2view("x", tx);
  float qy = img2view("y", ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  glPointSize(4.0);
  // First draw the "test" point on the screen
  glColor3f(1.0,0.89,0.77);  // Bisque red ff e4 c4
  glBegin(GL_POINTS);
  glVertex2f(m_test_mx, m_test_my);
  glEnd();

  XYPolygon poly = m_poly[m_active_poly];

  double tdist = poly.dist_to_poly(m_test_mx, m_test_my);
  cout << "Distance: " << tdist << endl;

  int i, vsize = poly.size();
  if(vsize < 1)
    return;

  for(i=0; i<vsize; i++) {
    if(poly.vertex_is_viewable(i, m_test_mx, m_test_my)) {
      double pvx = poly.get_vx(i);
      double pvy = poly.get_vy(i);
      glBegin(GL_LINE_STRIP);
      glVertex2f(m_test_mx, m_test_my);
      glVertex2f(pvx, pvy);
      glEnd();
    }
  }

#if 1
  for(i=0; i<vsize; i++) {
    double pt_xa = poly.get_vx(i);
    double pt_ya = poly.get_vy(i);
    double pt_xb, pt_yb;
    if(i>0) {
      pt_xb = poly.get_vx(i-1);
      pt_yb = poly.get_vy(i-1);
    }
    else {
      pt_xb = poly.get_vx(vsize-1);
      pt_yb = poly.get_vy(vsize-1);
    }
    double rang = segmentAngle(m_test_mx, m_test_my, 
			       pt_xa, pt_ya, pt_xb, pt_yb);
    cout << "Angle [" << i << "]:" << rang << endl;
  }
#endif

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawMode5

void PolyViewer::drawMode5()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img("x", 0);
  float ty = meters2img("y", 0);
  float qx = img2view("x", tx);
  float qy = img2view("y", ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  glPointSize(4.0);
  // First draw the "test" point on the screen
  glColor3f(1.0,0.89,0.77);  // Bisque ff e4 c4
  glBegin(GL_POINTS);
  glVertex2f(m_test_mx, m_test_my);
  glEnd();

  glColor3f(0.541,0.169,0.886);  // BlueViolet 8a 2b e2
  glBegin(GL_POINTS);
  glVertex2f(m_test_mxx, m_test_myy);
  glEnd();


  XYPolygon poly = m_poly[m_active_poly];

  int vsize = poly.size();
  if(vsize < 2)
    return;

  double x1 = m_test_mx;
  double y1 = m_test_my;
  double x2 = m_test_mxx;
  double y2 = m_test_myy;

  double x3 = poly.get_vx(0);
  double y3 = poly.get_vy(0);
  double x4 = poly.get_vx(1);
  double y4 = poly.get_vy(1);

  bool res = segmentsCross(x1,y1,x2,y2,x3,y3,x4,y4);
  if(res)
    glColor3f(1,0,0);  // Red ff 00 00 
  else
    glColor3f(0.933,0.91,0.667);  // PeleGoldenRod ee e8 aa

  glBegin(GL_LINE_STRIP);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawMode6

void PolyViewer::drawMode6()
{
  if(m_circle.size() == 0)
    return;

  drawCircles();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img("x", 0);
  float ty = meters2img("y", 0);
  float qx = img2view("x", tx);
  float qy = img2view("y", ty);

  double x1 = m_test_mx;
  double y1 = m_test_my;
  double x2 = m_test_mxx;
  double y2 = m_test_myy;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  glPointSize(4.0);

  // First draw the first endpoint on the screen
  glColor3f(1.0,0.89,0.77);  // Bisque ff e4 c4
  glBegin(GL_POINTS);
  glVertex2f(x1, y1);
  glEnd();

  // Then draw the other endpoint with a different color
  glColor3f(0.541,0.169,0.886);  // BlueViolet 8a 2b e2
  glBegin(GL_POINTS);
  glVertex2f(x2, y2);
  glEnd();

  // Then draw the line between them with different color
  glColor3f(0.933,0.91,0.667);  // PeleGoldenRod ee e8 aa
  glBegin(GL_LINE_STRIP);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();

  XYCircle circle = m_circle[0];

  double rx1, ry1, rx2, ry2;
  int count = circle.segIntersectPts(x1,y1,x2,y2,rx1,ry1,rx2,ry2);
  
  if(count > 0) {
    glColor3f(1.0, 0, 0);  // RED
    glBegin(GL_POINTS);
    glVertex2f(rx1, ry1);
    if(count > 1)
      glVertex2f(rx2, ry2);
    glEnd();
  }

  double len = circle.segIntersectLen(x1,y1,x2,y2);
  cout << "Intersect Length: " << len << endl;

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawMode7

void PolyViewer::drawMode7()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img("x", 0);
  float ty = meters2img("y", 0);
  float qx = img2view("x", tx);
  float qy = img2view("y", ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  glPointSize(4.0);

  XYPolygon poly = m_poly[m_active_poly];

  XYPolygon spoly = poly;
  spoly.apply_snap(1.0);

  int vsize = poly.size();
  if(vsize < 2)
    return;

  double x1 = spoly.get_vx(0);
  double y1 = spoly.get_vy(0);
  double x2 = spoly.get_vx(1);
  double y2 = spoly.get_vy(1);
  double x3 = m_test_mx;
  double y3 = m_test_my;

  double nx, ny;
  perpSegIntPt(x1,y1,x2,y2,x3,y3,nx,ny);

  double angle = relAng(x1,y1,x2,y2);
  double dist  = distPointToPoint(nx, ny, x2, y2);
  if(dist > 40) dist = 40; 
  double lx, ly;
  projectPoint(angle, dist, nx, ny, lx, ly);

  // First draw the "test" point on the screen
  glColor3f(1.0,0.89,0.77);  // Bisque ff e4 c4
  glBegin(GL_POINTS);
  glVertex2f(m_test_mx, m_test_my);
  glEnd();

  glColor3f(1.0,0.0,0.0);  // RED
  glBegin(GL_POINTS);
  glVertex2f(x1, y1);
  glEnd();

  glColor3f(0.0,1.0,0.0);  // GREEN
  glBegin(GL_POINTS);
  glVertex2f(x2, y2);
  glEnd();

  glColor3f(1.0,1.0,1.0);  // WHITE
  glBegin(GL_POINTS);
  glVertex2f(lx, ly);
  glEnd();

  glColor3f(0.933,0.91,0.667);  // PeleGoldenRod ee e8 aa

  glBegin(GL_LINE_STRIP);
  glVertex2f(x3, y3);
  glVertex2f(nx, ny);
  glEnd();

  glFlush();
  glPopMatrix();
}



//-------------------------------------------------------------
// Procedure: drawMode8

void PolyViewer::drawMode8()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img("x", 0);
  float ty = meters2img("y", 0);
  float qx = img2view("x", tx);
  float qy = img2view("y", ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  glPointSize(4.0);
  // First draw the "test" point on the screen
  glColor3f(1.0,0.89,0.77);  // Bisque red ff e4 c4
  glBegin(GL_POINTS);
  glVertex2f(m_test_mx, m_test_my);
  glEnd();

  XYPolygon poly = m_poly[m_active_poly];

  int i, vsize = poly.size();
  if(vsize < 1)
    return;

  double tdist = poly.dist_to_poly(m_test_mx, m_test_my, m_mode8_angle);
  cout << "Distance (" << m_mode8_angle << "): " << tdist << endl;

  double endx, endy;
  if(tdist > 0) {
    projectPoint(m_mode8_angle, tdist, m_test_mx, m_test_my, endx, endy);
    glBegin(GL_LINE_STRIP);
    glVertex2f(m_test_mx, m_test_my);
    glVertex2f(endx, endy);
    glEnd();

  }
  glColor3f(1.0,0.0,0.0);  // Bisque red ff e4 c4
  projectPoint(m_mode8_angle, 30, m_test_mx, m_test_my, endx, endy);
  glBegin(GL_LINE_STRIP);
  glVertex2f(m_test_mx, m_test_my);
  glVertex2f(endx, endy);
  glEnd();

  glFlush();
  glPopMatrix();
}


//-------------------------------------------------------------
// Procedure: drawMode9
//      Note: Mode for testing arc geometric properties.

void PolyViewer::drawMode9()
{
  if(m_arc.size() == 0)
    return;
  drawArc(m_arc_poly[0]);

  XYArc arc = m_arc[0];
  
  double radius = arc.getRad();
  double start_x = arc.getAX1();
  double start_y = arc.getAY1();
  double start_ang = arc.getLangle() - 90;
  if(start_ang < 0)
    start_ang += 360;
  drawVector(start_x, start_y, start_ang);


  double dist = 100;
  bool   port = true;
  double rx, ry, rheading;
  arcFlight(start_x, start_y, start_ang, radius, 
	    dist, port, rx, ry, rheading);
  drawVector(rx, ry, rheading);


  // TEST #2
  XYArc new_arc = arcFlight(start_x, start_y, start_ang, radius, 
			    dist, port);
  string str = new_arc.toString();
  str += ",40";  // Num pts to approx w/ polygon
  XYPolygon arc_poly;
  bool ok = arc_poly.initialize(str);
  if(ok)
    drawArc(arc_poly, 1);


  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img("x", 0);
  float ty = meters2img("y", 0);
  float qx = img2view("x", tx);
  float qy = img2view("y", ty);

  double x1 = m_test_mx;
  double y1 = m_test_my;
  double x2 = m_test_mxx;
  double y2 = m_test_myy;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  // First draw the first endpoint on the screen
  glPointSize(4.0);
  glColor3f(1.0,0.89,0.77);  // Bisque ff e4 c4
  glBegin(GL_POINTS);
  glVertex2f(x1, y1);
  glEnd();

  // Then draw the other endpoint with a different color
  glColor3f(0.541,0.169,0.886);  // BlueViolet 8a 2b e2
  glBegin(GL_POINTS);
  glVertex2f(x2, y2);
  glEnd();

  // Then draw the line between them with different color
  glColor3f(0.933,0.91,0.667);  // PeleGoldenRod ee e8 aa
  glBegin(GL_LINE_STRIP);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();

  double rx1, ry1, rx2, ry2;
  int count = arc.segIntersectPts(x1,y1,x2,y2,rx1,ry1,rx2,ry2);
  
  // Draw the intersection points if there are any
  if(count > 0) {
    glColor3f(1.0, 0, 0);  // RED
    glBegin(GL_POINTS);
    glVertex2f(rx1, ry1);
    if(count > 1)
      glVertex2f(rx2, ry2);
    glEnd();
  }

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: addCircle

void PolyViewer::addCircle(XYCircle g_circle)
{
  double c_x = g_circle.getX();
  double c_y = g_circle.getY();
  double c_r = g_circle.getRad();
  string str = "radial:" + doubleToString(c_x) + ",";
  str += doubleToString(c_y) + "," + doubleToString(c_r) + ",36";
  XYPolygon circle_poly;
  bool ok = circle_poly.initialize(str);
  if(!ok)
    return;
  
  m_circle.push_back(g_circle);
  m_circle_poly.push_back(circle_poly);

}

//-------------------------------------------------------------
// Procedure: drawCircles

void PolyViewer::drawCircles()
{
  int vsize = m_circle.size();
  for(int i=0; i<vsize; i++)
    drawCircle(i);
}

//-------------------------------------------------------------
// Procedure: drawCircle

void PolyViewer::drawCircle(int ix)
{
  if((ix < 0) || (ix >= m_circle.size()))
    return;
  drawPoly(m_circle_poly[ix]);
}

//-------------------------------------------------------------
// Procedure: addArc

void PolyViewer::addArc(XYArc g_arc)
{
  string str = g_arc.toString();

  str += ",72";  // Num pts to approx w/ polygon

  XYPolygon arc_poly;
  bool ok = arc_poly.initialize(str);
  if(!ok)
    return;
  
  m_arc.push_back(g_arc);
  m_arc_poly.push_back(arc_poly);

}

//-------------------------------------------------------------
// Procedure: drawArcs

void PolyViewer::drawArcs()
{
  int vsize = m_arc.size();
  for(int i=0; i<vsize; i++)
    drawArc(m_arc_poly[i]);
}

//-------------------------------------------------------------
// Procedure: drawArc

void PolyViewer::drawArc(XYPolygon &g_arc_poly, int g_style)
{
  if(g_style == 0)
    drawPoly(g_arc_poly);
  else
    drawPoly(g_arc_poly, false, false, 
	     0.88, 0.44, 0.00,
	     0.00, 0.00, 0.00,
	     0.70, 0.70, 0.70,
	     0.99, 0.33, 0.33,
	     0.33, 0.33, 0.99);
}

//-------------------------------------------------------------
// Procedure: drawVector

void PolyViewer::drawVector(double g_x, double g_y, double g_angle)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  float tx = meters2img("x", 0);
  float ty = meters2img("y", 0);
  float qx = img2view("x", tx);
  float qy = img2view("y", ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  double endx, endy;
  projectPoint(g_angle, 100, g_x, g_y, endx, endy);

  // First draw the points on the screen
  glPointSize(4.0);
  glColor3f(1.0,0.89,0.77);  // Bisque red ff e4 c4
  glBegin(GL_POINTS);
  glVertex2f(g_x, g_y);
  glEnd();
  glColor3f(0.5,1.0,0.5); 
  glBegin(GL_POINTS);
  glVertex2f(endx, endy);
  glEnd();

  glPointSize(1.0);
  glColor3f(1.0,0.0,0.0);  // Bisque red ff e4 c4
  glBegin(GL_LINE_STRIP);
  glVertex2f(g_x, g_y);
  glVertex2f(endx, endy);
  glEnd();

  glFlush();
  glPopMatrix();
}


