/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HexaViewer.cpp                                       */
/*    DATE: Oct 30, 2005                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <math.h>
#include "HexaViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "XYHexagon.h"

using namespace std;

HexaViewer::HexaViewer(int x, int y, 
		       int width, int height, const char *l)
  : MarineViewer(x,y,width,height,l)
{
  m_snap_val    = 10.0;
  m_active_poly = 0;
  pdist         = 65;
  view_state    = 2;
}

//-------------------------------------------------------------
// Procedure: handle()

int HexaViewer::handle(int event)
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

void HexaViewer::draw()
{
  MarineViewer::draw();

  int i, vsize = m_poly.size();
  if((view_state == 0) || (view_state == 2)) {
    for(i=0; i<vsize; i++)
      if(i == m_active_poly) 
	drawPoly(m_poly[i], true, false);
      else
	drawPoly(m_poly[i], false, false);
  }
  
  if((view_state == 1) || (view_state == 2)) {
    for(i=0; i<vsize; i++)
      drawPoly(m_hexa[i], false, true);
  }

  drawGrids();
}

//-------------------------------------------------------------
// Procedure: handle_left_mouse

void HexaViewer::handle_left_mouse(int vx, int vy)
{
  int vsize = m_poly.size();

  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, m_snap_val);
  double sy = snapToStep(my, m_snap_val);

  XYHexagon hexagon;
  hexagon.initialize(sx, sy, pdist);
  addPoly(hexagon);
  m_active_poly = m_poly.size()-1;

  XYHexagon inner_hex;
  inner_hex.initialize(sx, sy, (pdist * 0.7));
  m_hexa.push_back(inner_hex);

  redraw();
}

//-------------------------------------------------------------
// Procedure: handle_right_mouse

void HexaViewer::handle_right_mouse(int vx, int vy)
{
  int i;
  int vsize = m_poly.size(); 
  if(vsize == 0)
    return;

  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, m_snap_val);
  double sy = snapToStep(my, m_snap_val);

  m_active_poly = -1;
  for(i=0; i<vsize; i++) {
    if(m_poly[i].contains(mx, my)) {
      m_active_poly = i;
      m_active_poly = m_poly.size()-1;
      redraw();
      return;
    }
  }

  vector<double> vdist;
  for(i=0; i<vsize; i++)
    vdist.push_back(-1);

  // Determine which of the current polygons is closest
  double min_dist  = -1;
  int    min_index = -1;
  for(i=0; i<vsize; i++) {
    if(m_poly[i].is_convex()) {
      if(!m_poly[i].contains(mx, my)) {
	vdist[i] = m_poly[i].dist_to_poly(mx, my);
	if((vdist[i] < min_dist) || (min_dist == -1)) {
	  min_dist = vdist[i];
	  min_index = i;
	}
      }
    }
  }

  // Now try to add a new one
  if(min_index != -1) {
    XYPolygon p = m_poly[min_index];
    if(p.size() == 6) {
      double dist = p.get_vx(2) - p.get_vx(3);
      double cx   = p.get_vx(1) - dist;
      double cy   = p.get_vy(1);
      
      double nx,ny;
      double rang = relAng(cx,cy,mx,my);
      if((rang > 30) && (rang <= 90)) {
	nx = cx + (1.5*dist);
	ny = cy + dist;
      }
      else if((rang > 90) && (rang <= 150)) {
	nx = cx + (1.5*dist);
	ny = cy - dist;
      }
      else if((rang > 150) && (rang <= 210)) {
	nx = cx;
	ny = cy - (2*dist);
      }
      else if((rang > 210) && (rang <= 270)) {
	nx = cx - (1.5*dist);
	ny = cy - dist;
      }
      else if((rang > 270) && (rang <= 330)) {
	nx = cx - (1.5*dist);
	ny = cy + dist;
      }
      else {
	nx = cx;
	ny = cy + (2*dist);
      }

      XYHexagon hexagon;
      hexagon.initialize(nx, ny, pdist);
      addPoly(hexagon);
      m_active_poly = m_poly.size()-1;

      XYHexagon inner_hex;
      inner_hex.initialize(nx, ny, (pdist * 0.7));
      m_hexa.push_back(inner_hex);

    }
  }

  redraw();
}

// ----------------------------------------------------------
// Procedure: getPolySpec
//   Purpose: 

string HexaViewer::getPolySpec(int v)
{
  if((m_poly.size() == 0) || (m_active_poly==-1))
    return("");

  if(v==0)
    return(m_poly[m_active_poly].get_spec());
  else if(v==1)
    return(m_hexa[m_active_poly].get_spec());
  else
    return("");
}

// ----------------------------------------------------------
// Procedure: adjustActive
//   Purpose: 

void HexaViewer::adjustActive(int v)
{
  int old_ix = m_active_poly;

  int psize = m_poly.size();
  m_active_poly += v;
  //cout << "m_poly_size: " << psize << endl;
  //cout << "m_active_poly:" << m_active_poly << endl;
  if(m_active_poly < -1)
    m_active_poly = -1;
  else if(m_active_poly > (psize - 1))
    m_active_poly = psize-1;
 
  if(m_active_poly != old_ix)
    redraw();

  //cout << "  m_active_poly:" << m_active_poly << endl;
}

// ----------------------------------------------------------
// Procedure: adjustActive
//   Purpose: 
#if 0
void HexaViewer::adjustActive(int v)
{
  int old_ix = m_active_poly;

  m_active_poly += v;
  //cout << "m_poly_size: " << m_poly.size() << endl;
  //cout << "m_active_poly:" << m_active_poly << endl;
  if(m_active_poly < -1)
    m_active_poly = -1;
  else if(m_active_poly > (m_poly.size() - 1))
    m_active_poly = m_poly.size()-1;
 
  if(m_active_poly != old_ix)
    redraw();

  //cout << "  m_active_poly:" << m_active_poly << endl;
}
#endif

// ----------------------------------------------------------
// Procedure: shiftHorzPoly
//   Purpose: 

void HexaViewer::shiftHorzPoly(double shift_val)
{
  if(m_active_poly != -1) {
    m_poly[m_active_poly].shift_horz(shift_val);
    m_hexa[m_active_poly].shift_horz(shift_val);
  }
  else 
    for(int i=0; i<m_poly.size(); i++) {
      m_poly[i].shift_horz(shift_val);
      m_hexa[i].shift_horz(shift_val);
    }
    

}

// ----------------------------------------------------------
// Procedure: shiftVertPoly
//   Purpose: 

void HexaViewer::shiftVertPoly(double shift_val)
{
  if(m_active_poly != -1) {
    m_poly[m_active_poly].shift_vert(shift_val);
    m_hexa[m_active_poly].shift_vert(shift_val);
  }
  else 
    for(int i=0; i<m_poly.size(); i++) {
      m_poly[i].shift_vert(shift_val);
      m_hexa[i].shift_vert(shift_val);
    }
}

// ----------------------------------------------------------
// Procedure: clearActivePoly
//   Purpose: 

void HexaViewer::clearActivePoly()
{
  vector<XYPolygon> new_poly;
  int i, psize = m_poly.size();

  for(i=0; i < psize; i++)
    if(i != m_active_poly)
      new_poly.push_back(m_poly[i]);
  m_poly = new_poly;

  vector<XYHexagon> new_hexa;
  for(i=0; i < psize; i++)
    if(i != m_active_poly)
      new_hexa.push_back(m_hexa[i]);
  m_hexa = new_hexa;

  m_active_poly = 0;

}

// ----------------------------------------------------------
// Procedure: clearActivePoly
//   Purpose: 

void HexaViewer::toggleView()
{
  view_state++;
  if(view_state > 2)
    view_state = 0;
  redraw();
}

// ----------------------------------------------------------
// Procedure: reApplySnapToCurrent
//   Purpose: 

void HexaViewer::reApplySnapToCurrent()
{
  m_poly[m_active_poly].apply_snap(m_snap_val);
}





