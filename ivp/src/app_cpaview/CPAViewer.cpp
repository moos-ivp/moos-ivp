/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CPAViewer.cpp                                        */
/*    DATE: Feb 12, 2007                                         */
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

#include <iostream>
#include <cmath>
#include "CPAViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

CPAViewer::CPAViewer(CPA_Model *g_model, int g_x, int g_y, 
		     int g_width, int g_height, const char *g_l)
  : MarineViewer(g_x, g_y, g_width, g_height, g_l)
{
  m_cpa_model = g_model;

  m_snap_val = 1.0;    // meters

  m_vshift_x = -80;
  m_vshift_y = -100;
  m_zoom     = 0.8;

  setParam("tiff_view", "off");
  setParam("hash_view", "on");
  setParam("hash_shade", -1.0);
  setParam("hash_shade", 0.75);
  setParam("back_shade", -1.0);
  setParam("back_shade", 0.85);

  configureRadials(4, 100);

  m_start_pan_x = -80;
  m_start_pan_y = -100;
  resetPan();
}


//-------------------------------------------------------------
// Procedure: handle()

int CPAViewer::handle(int event)
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

void CPAViewer::draw()
{
  MarineViewer::draw();
  if(m_hash_offon)
    drawHash();

  vector<XYPolygon> polys   = m_geoshapes.getPolygons();
  vector<XYSegList> segls   = m_geoshapes.getSegLists();


  const map<string, XYPoint>& points = m_geoshapes.getPoints();

  drawPolygons(polys);
  drawPoints(points);
  drawSegLists(segls);

  draw_os_point(6, 0.2, 0.2, 1.0);

  unsigned int i, contact_count = m_cpa_model->size();
  for(i=0; i<contact_count; i++)
    draw_cn_vector(i, 6, 1.0, 0.2, 0.2);
}

//-------------------------------------------------------------
// Procedure: handle_left_mouse

void CPAViewer::handle_left_mouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, m_snap_val);
  double sy = snapToStep(my, m_snap_val);

  if(Fl::event_state(FL_SHIFT)) {
    m_cpa_model->set_cnx(sx);
    m_cpa_model->set_cny(sy);
  }
  else if(Fl::event_state(FL_CTRL)) {
    double cnx = m_cpa_model->get_param("cnx", 0);
    double cny = m_cpa_model->get_param("cny", 0);
    double bearing = relAng(cnx, cny, sx, sy);
    m_cpa_model->set_cncrs(bearing);
  }

  redraw();
}

//-------------------------------------------------------------
// Procedure: handle_right_mouse

void CPAViewer::handle_right_mouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, m_snap_val);
  double sy = snapToStep(my, m_snap_val);

  m_cpa_model->set_cnx(sx);
  m_cpa_model->set_cny(sy);

  redraw();
}

// ----------------------------------------------------------
// Procedure: draw_cn_vector
//   Purpose: 

void CPAViewer::draw_cn_vector(int    index, 
			       double vertex_size,
			       double vertex_r,
			       double vertex_g,
			       double vertex_b)
{
  double cnx   = m_cpa_model->get_param("cnx", index);
  double cny   = m_cpa_model->get_param("cny", index);
  double cnspd = m_cpa_model->get_param("cnspd", index);
  double cncrs = m_cpa_model->get_param("cncrs", index);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  glPointSize(vertex_size * m_zoom);
  glColor3f(vertex_r, vertex_g, vertex_b); 
  glBegin(GL_POINTS);
  glVertex2f(cnx, cny);
  glEnd();


  double distance = cnspd * 60;
  
  double cn_newx, cn_newy;
  
  projectPoint(cncrs, distance, cnx, cny, cn_newx, cn_newy);

  glPointSize(vertex_size * m_zoom);
  glColor3f(vertex_r, vertex_g, vertex_b); 
  glLineWidth(3.0);
  glBegin(GL_LINE_STRIP);
  glVertex2f(cnx, cny);
  glVertex2f(cn_newx, cn_newy);
  glEnd();
  glLineWidth(1.0);

  double ptx1, pty1, ptx2, pty2, ptx3, pty3;
  projectPoint(cncrs, 8, cn_newx, cn_newy, ptx1, pty1);
  projectPoint(cncrs-90, 3, cn_newx, cn_newy, ptx2, pty2);
  projectPoint(cncrs+90, 3, cn_newx, cn_newy, ptx3, pty3);

  glEnable(GL_BLEND);
  glColor4f(vertex_r, vertex_g, vertex_b, 0.1);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_POLYGON);
  glVertex2f(ptx1, pty1);
  glVertex2f(ptx2, pty2);
  glVertex2f(ptx3, pty3);
  glEnd();
  glDisable(GL_BLEND);

  glFlush();
  glPopMatrix();
}

// ----------------------------------------------------------
// Procedure: draw_os_point
//   Purpose: 

void CPAViewer::draw_os_point(double vertex_size,
			      double vertex_r,
			      double vertex_g,
			      double vertex_b)
{
  double osx = m_cpa_model->get_param("osx");
  double osy = m_cpa_model->get_param("osy");

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  glPointSize(vertex_size * m_zoom);
  glColor3f(vertex_r, vertex_g, vertex_b); 
  glBegin(GL_POINTS);
  glVertex2f(osx, osy);
  glEnd();

  glFlush();
  glPopMatrix();
}


// ----------------------------------------------------------
// Procedure: configureRadials
//   Purpose: 

void CPAViewer::configureRadials(unsigned int radials, double dist)
{
  m_radials.clear();

  unsigned int i;
  for(i=0; i<radials; i++) {
    string str = "radial:: x=0, y=0, pts=120";
    str += (", radius=" + doubleToString((double)((i+1)*dist),0));
    str += (", label="  + doubleToString((double)((i+1)*dist),0));
    XYPolygon poly = string2Poly(str);
    poly.set_vertex_size(0);
    poly.set_color("edge", "dark_blue");
    poly.set_color("label", "dark_blue");
    string spec = poly.get_spec();

    bool handled = MarineViewer::setParam("view_polygon", spec);
    cout << "Handled Radial Poly:" << handled << endl;
  }
}

// ----------------------------------------------------------
// Procedure: resetPan
//   Purpose: 

void CPAViewer::resetPan()
{
  m_vshift_x = m_start_pan_x;
  m_vshift_y = m_start_pan_y;  
}

