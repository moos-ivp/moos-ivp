/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CPAViewer.cpp                                        */
/*    DATE: Feb 12, 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <math.h>
#include "CPAViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

CPAViewer::CPAViewer(CPA_Model *g_model, int g_x, int g_y, 
		     int g_width, int g_height, const char *g_l)
  : MarineViewer(g_x, g_y, g_width, g_height, g_l)
{
  m_cpa_model = g_model;

  m_snap_val = 1.0;    // meters

  m_vshift_x = 60;
  m_vshift_y = 40;
  m_zoom     = 2.55;

  setParam("tiff_view", "off");
  setParam("hash_view", "off");
  setParam("hash_shade", -1.0);
  setParam("hash_shade", 0.75);
  setParam("back_shade", -1.0);
  setParam("back_shade", 0.85);
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

  draw_os_point(6, 0.2, 0.2, 1.0);

  int contact_count = m_cpa_model->size();
  for(int i=0; i<contact_count; i++)
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

  m_cpa_model->set_osx(sx);
  m_cpa_model->set_osy(sy);

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

  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

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

  float tx = meters2img('x', 0);
  float ty = meters2img('y', 0);
  float qx = img2view('x', tx);
  float qy = img2view('y', ty);

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


