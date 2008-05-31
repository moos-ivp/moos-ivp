/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GeoViewer.cpp                                        */
/*    DATE: May 1st, 2005 (Major Modifications November 2007)    */
/*****************************************************************/

#include <string>
#include <iostream>
#include <math.h>
#include "GeoViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "CircularUtils.h"
#include "AngleUtils.h"
#include "XYBuildUtils.h"

using namespace std;

GeoViewer::GeoViewer(int x, int y, 
			 int width, int height, const char *l)
  : MarineViewer(x,y,width,height,l)
{
  m_snap_val    = 10.0;
  m_active_poly = 0;

  // Modes:   0:place 1:edit 2:delete 3:insert 
  m_drop_mode = 0;     
}

//-------------------------------------------------------------
// Procedure: handle()

int GeoViewer::handle(int event)
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

void GeoViewer::draw()
{
  MarineViewer::draw();

  unsigned int vsize = m_poly.size();
  for(unsigned int i=0; i<vsize; i++) {
    if(i == m_active_poly)
      drawPoly(m_poly[i], true, false, 1.0, 1.0, 0.0);
    else
      drawPoly(m_poly[i], false, false);
  }

  drawGrids();
  drawHexagons();
}

//-------------------------------------------------------------
// Procedure: handle_left_mouse

void GeoViewer::handle_left_mouse(int vx, int vy)
{
  int vsize = m_poly.size();

  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
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
  redraw();
}

//-------------------------------------------------------------
// Procedure: handle_right_mouse

void GeoViewer::handle_right_mouse(int vx, int vy)
{
  unsigned int vsize = m_poly.size(); 
  if(vsize == 0)
    return;

  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  //double sx = snapToStep(mx, m_snap_val);
  //double sy = snapToStep(my, m_snap_val);
  
  m_active_poly = 0;
  bool found = false;

  for(unsigned int i=0; i<vsize; i++) {
    if(m_poly[i].contains(mx, my)) {
      m_active_poly = i;
      found = true;
    }
  }

  if(!found)
    createNew();

  redraw();
}


//-------------------------------------------------------------
// Procedure: setParam

bool GeoViewer::setParam(string param, float pval)
{
  if(setCommonParam(param, pval))
    return(true);

  else if(param == "mode") {
    m_drop_mode = (int)(pval);
    return(true);
  }
  else if(param == "snap") {
    m_snap_val = pval;
    return(true);
  }
  else
    return(false);
}

//-------------------------------------------------------------
// Procedure: createNew

void GeoViewer::createNew()
{
  unsigned int vsize = m_poly.size(); 
  if(vsize == 0)
    return;
  
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
  
  redraw();
}

// ----------------------------------------------------------
// Procedure: getPolySpec
//   Purpose: 

string GeoViewer::getPolySpec()
{
  if(m_poly.size() == 0) 
    return("");
  else {
    //int s = m_poly.size();
    return(m_poly[m_active_poly].get_spec());
  }
}

// ----------------------------------------------------------
// Procedure: adjustActive
//   Purpose: 

void GeoViewer::adjustActive(int v)
{
  unsigned int old_ix = m_active_poly;

  m_active_poly += v;

  if(m_active_poly < 0)
    m_active_poly = 0;

  if(m_active_poly > m_poly.size()-1)
    m_active_poly = m_poly.size()-1;

  if(m_active_poly != old_ix)
    redraw();
}

// ----------------------------------------------------------
// Procedure: shiftHorzPoly
//   Purpose: 

void GeoViewer::shiftHorzPoly(double shift_val)
{
  if(m_active_poly >= m_poly.size())
    return;

  m_poly[m_active_poly].shift_horz(shift_val);
}

// ----------------------------------------------------------
// Procedure: shiftVertPoly
//   Purpose: 

void GeoViewer::shiftVertPoly(double shift_val)
{
  if(m_active_poly >= m_poly.size())
    return;

  m_poly[m_active_poly].shift_vert(shift_val);
}

// ----------------------------------------------------------
// Procedure: rotatePoly
//   Purpose: 

void GeoViewer::rotatePoly(unsigned int rval)
{
  if(m_active_poly >= m_poly.size())
    return;

  m_poly[m_active_poly].rotate(rval);
}

// ----------------------------------------------------------
// Procedure: growPoly
//   Purpose: 

void GeoViewer::growPoly(unsigned int gval)
{
  if(m_active_poly >= m_poly.size())
    return;

  double dgval = (double)(gval) / 100.0;
  m_poly[m_active_poly].grow_by_pct(dgval);
}

// ----------------------------------------------------------
// Procedure: reversePoly
//   Purpose: 

void GeoViewer::reversePoly()
{
  if(m_active_poly >= m_poly.size())
    return;

  m_poly[m_active_poly].reverse();
}

// ----------------------------------------------------------
// Procedure: duplicateActive
//   Purpose: 

void GeoViewer::duplicateActive()
{
  if(m_active_poly >= m_poly.size())
    return;

  XYPolygon new_poly = m_poly[m_active_poly];
  new_poly.shift_vert(-10);
  new_poly.shift_horz(10);
  addPoly(new_poly);
  m_active_poly = m_poly.size()-1;
}

// ----------------------------------------------------------
// Procedure: clearActivePoly
//   Purpose: 

void GeoViewer::clearActivePoly()
{
  vector<XYPolygon> new_poly;

  for(unsigned int i=0; i<m_poly.size(); i++)
    if(i != m_active_poly)
      new_poly.push_back(m_poly[i]);
  m_poly = new_poly;
  m_active_poly = 0;
}

// ----------------------------------------------------------
// Procedure: reApplySnapToCurrent
//   Purpose: 

void GeoViewer::reApplySnapToCurrent()
{
  m_poly[m_active_poly].apply_snap(m_snap_val);
}

//-------------------------------------------------------------
// Procedure: addCircle

void GeoViewer::addCircle(XYCircle g_circle)
{
  double c_x = g_circle.getX();
  double c_y = g_circle.getY();
  double c_r = g_circle.getRad();
  string str = "radial:" + doubleToString(c_x) + ",";
  str += doubleToString(c_y) + "," + doubleToString(c_r) + ",36";
  XYPolygon circle_poly = stringToPoly(str);
  if(circle_poly.size() == 0)
    return;
  
  m_circle.push_back(g_circle);
  m_circle_poly.push_back(circle_poly);

}

//-------------------------------------------------------------
// Procedure: drawCircles

void GeoViewer::drawCircles()
{
  int vsize = m_circle.size();
  for(int i=0; i<vsize; i++)
    drawCircle(i);
}

//-------------------------------------------------------------
// Procedure: drawCircle

void GeoViewer::drawCircle(unsigned int ix)
{
  if((ix < 0) || (ix >= m_circle.size()))
    return;
  drawPoly(m_circle_poly[ix]);
}

//-------------------------------------------------------------
// Procedure: drawVector

void GeoViewer::drawVector(double g_x, double g_y, double g_angle)
{
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


