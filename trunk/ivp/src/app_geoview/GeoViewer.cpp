/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GeoViewer.cpp                                        */
/*    DATE: May 1st, 2005 (Major Modifications November 2007)    */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <string>
#include <cmath>
#include "GeoViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "CircularUtils.h"
#include "AngleUtils.h"

using namespace std;

//------------------------------------------------------------
// Constructor

GeoViewer::GeoViewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
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
  if(m_geo_settings.viewable("hash_viewable")) {
    double xl = m_geoshapes.getXMin() - 1000;
    double xh = m_geoshapes.getXMax() + 1000;
    double yl = m_geoshapes.getYMin() - 1000;
    double yh = m_geoshapes.getYMax() + 1000;
    drawHash(xl, xh, yl, yh);
  }

  // Rather than call "drawPolygons()" in the superclass, we implement 
  // the routine here so we can draw the "active" poly differently.
  if(m_geo_settings.viewable("polygon_viewable_all", true) == true) {
    vector<XYPolygon> polygons = m_geoshapes.getPolygons();
    unsigned int i, vsize = polygons.size();
    for(i=0; i<vsize; i++) {
      bool filled = (i == m_active_poly);
      if(filled)
	polygons[i].set_color("fill", "dark_green");
      drawPolygon(polygons[i]);
    }
  }
  
  vector<XYGrid>    grids   = m_geoshapes.getGrids();
  vector<XYSegList> segls   = m_geoshapes.getSegLists();

  const map<string, XYPoint> points = m_geoshapes.getPoints();

  drawSegLists(segls);
  drawGrids(grids);
  drawPoints(points);
  drawHexagons();

}

//-------------------------------------------------------------
// Procedure: handle_left_mouse

void GeoViewer::handle_left_mouse(int vx, int vy)
{
  int vsize = m_geoshapes.sizePolygons();

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
      string new_label;
      int vvsize = vsize;
      while(vvsize >= 0) {
	int rem = vvsize % 26;
	char next_char = 65 + rem;
	new_label += next_char;
	vvsize = vvsize - 26;
      }
      newpoly.set_param("edge_size", "1");
      newpoly.set_label(new_label);
      newpoly.add_vertex(sx, sy);
      m_geoshapes.addPolygon(newpoly);
    }
    else
      m_geoshapes.poly(m_active_poly).add_vertex(sx, sy);
  }
  if(m_drop_mode == 1) {
    if(vsize > 0)
      m_geoshapes.poly(m_active_poly).alter_vertex(sx, sy);
  }
  if(m_drop_mode == 2) {
    if(vsize > 0)
      m_geoshapes.poly(m_active_poly).delete_vertex(mx, my);
  }
  if(m_drop_mode == 3) {
    if(vsize > 0)
      m_geoshapes.poly(m_active_poly).insert_vertex(sx, sy);
  }
  redraw();
}

//-------------------------------------------------------------
// Procedure: handle_right_mouse

void GeoViewer::handle_right_mouse(int vx, int vy)
{
  unsigned int vsize = m_geoshapes.sizePolygons(); 
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
    if(m_geoshapes.poly(i).contains(mx, my)) {
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

bool GeoViewer::setParam(string param, string value)
{
  param = tolower(stripBlankEnds(param));
  value = stripBlankEnds(value);
  
  if(MarineViewer::setParam(param, value))
    return(true);

  bool handled = false;
  if(param == "view_polygon")
    handled = m_geoshapes.addPolygon(value);
  else if(param == "view_seglist")
    handled = m_geoshapes.addSegList(value);
  else if(param == "view_point") {
    cout << "Adding a point:" << endl; 
    handled = m_geoshapes.addPoint(value);
    cout << "handled:" << handled << endl; 
  }

  else if(param == "view_vector")
    handled = m_geoshapes.addVector(value);
  else if(param == "view_circle")
    handled = m_geoshapes.addCircle(value);
  else if(param == "view_range_pulse")
    handled = m_geoshapes.addRangePulse(value);
  else if((param == "view_marker") || (param == "marker"))
    handled = m_geoshapes.addMarker(value);
  else if(param == "grid_config")
    handled = m_geoshapes.addGrid(value);
  else if(param == "grid_delta")
    handled = m_geoshapes.updateGrid(value);
  else
    handled = handled || m_vehi_settings.setParam(param, value);

  return(handled);
}

//-------------------------------------------------------------
// Procedure: setParam

bool GeoViewer::setParam(string param, double pval)
{
  if(MarineViewer::setParam(param, pval))
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
  unsigned int vsize = m_geoshapes.sizePolygons(); 
  if(vsize == 0)
    return;
  
  // Is the <3 check here really necessary?
  if(m_geoshapes.poly(vsize-1).size() < 3) {
    m_geoshapes.poly(vsize-1).clear();
    m_active_poly = vsize-1;
  }
  else {
    XYPolygon newpoly;

    newpoly.set_param("edge_size", "1");
    string new_label;
    int vvsize = vsize;
    while(vvsize >= 0) {
      int rem = vvsize % 26;
      char next_char = 65 + rem;
      new_label += next_char;
      vvsize = vvsize - 26;
    }
    newpoly.set_label(new_label);
    m_geoshapes.addPolygon(newpoly);
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
  if(m_geoshapes.sizePolygons() == 0) 
    return("");
  else
    return(m_geoshapes.poly(m_active_poly).get_spec());
}

// ----------------------------------------------------------
// Procedure: adjustActive
//   Purpose: 

void GeoViewer::adjustActive(int v)
{
  unsigned int old_ix = m_active_poly;

  if(((int)(m_active_poly) + v) < 0)
    m_active_poly = 0;
  else
    m_active_poly += v;
  
  if(m_active_poly > m_geoshapes.sizePolygons()-1)
    m_active_poly = m_geoshapes.sizePolygons()-1;
  
  if(m_active_poly != old_ix)
    redraw();
}

// ----------------------------------------------------------
// Procedure: shiftHorzPoly
//   Purpose: 

void GeoViewer::shiftHorzPoly(double shift_val)
{
  if(m_active_poly >= m_geoshapes.sizePolygons())
    return;
  
  m_geoshapes.poly(m_active_poly).shift_horz(shift_val);
}

// ----------------------------------------------------------
// Procedure: shiftVertPoly
//   Purpose: 

void GeoViewer::shiftVertPoly(double shift_val)
{
  if(m_active_poly >= m_geoshapes.sizePolygons())
    return;
  
  m_geoshapes.poly(m_active_poly).shift_vert(shift_val);
}

// ----------------------------------------------------------
// Procedure: rotatePoly
//   Purpose: Rotate the polygon around its center by the given
//            number of degrees. Each point in the polygon is 
//            rotated around the calculated center of the polygon

void GeoViewer::rotatePoly(int rval)
{
  if(m_active_poly >= m_geoshapes.sizePolygons())
    return;

  m_geoshapes.poly(m_active_poly).rotate(rval);
}

// ----------------------------------------------------------
// Procedure: growPoly
//   Purpose: Grow the size of the poly by the given percentage.
//            Each point in the polygon grows in distance from 
//            calculated center of the polygon

void GeoViewer::growPoly(int gval)
{
  if(m_active_poly >= m_geoshapes.sizePolygons())
    return;

  double dgval = (double)(gval) / 100.0;
  m_geoshapes.poly(m_active_poly).grow_by_pct(dgval);
}

// ----------------------------------------------------------
// Procedure: reversePoly
//   Purpose: Reverse the ordering of points in the polygon.

void GeoViewer::reversePoly()
{
  if(m_active_poly >= m_geoshapes.sizePolygons())
    return;

  m_geoshapes.poly(m_active_poly).reverse();
}

// ----------------------------------------------------------
// Procedure: duplicateActive
//   Purpose: 

void GeoViewer::duplicateActive()
{
  if(m_active_poly >= m_geoshapes.sizePolygons())
    return;
  
  XYPolygon new_poly = m_geoshapes.getPolygon(m_active_poly);
  new_poly.shift_vert(-10);
  new_poly.shift_horz(10);
  m_geoshapes.addPolygon(new_poly);
  m_active_poly = m_geoshapes.sizePolygons()-1;
}

// ----------------------------------------------------------
// Procedure: clearActivePoly
//   Purpose: 

void GeoViewer::clearActivePoly()
{
  if(m_active_poly >= m_geoshapes.sizePolygons())
    return;
  
  m_geoshapes.poly(m_active_poly).clear();
}

// ----------------------------------------------------------
// Procedure: reApplySnapToCurrent
//   Purpose: 

void GeoViewer::reApplySnapToCurrent()
{
  if(m_active_poly >= m_geoshapes.sizePolygons())
    return;
  
  m_geoshapes.poly(m_active_poly).apply_snap(m_snap_val);
}










