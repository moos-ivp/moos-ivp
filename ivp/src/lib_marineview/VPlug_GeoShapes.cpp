/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlug_GeoShapes.cpp                                  */
/*    DATE: July 9th, 2008                                       */
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
#include <iostream>
#include <cstdlib>
#include "VPlug_GeoShapes.h"
#include "MBUtils.h"
#include "XYFormatUtilsSegl.h"
#include "XYFormatUtilsPoly.h"
#include "XYFormatUtilsPoint.h"
#include "XYFormatUtilsCircle.h"
#include "XYFormatUtilsVector.h"
#include "XYFormatUtilsRangePulse.h"
#include "XYFormatUtilsCommsPulse.h"
#include "XYFormatUtilsMarker.h"
#include "XYFormatUtilsConvexGrid.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

VPlug_GeoShapes::VPlug_GeoShapes()
{
  m_xmin = 0;
  m_xmax = 0;
  m_ymin = 0;
  m_ymax = 0;
}

//-----------------------------------------------------------
// Procedure: clear()

void VPlug_GeoShapes::clear()
{
  m_polygons.clear();
  m_seglists.clear();
  m_hexagons.clear();
  m_grids.clear();
  m_circles.clear();
  m_points.clear();
  m_vectors.clear();
  m_range_pulses.clear();
  m_markers.clear();

  m_xmin = 0;
  m_xmax = 0;
  m_ymin = 0;
  m_ymax = 0;
}

//-----------------------------------------------------------
// Procedure: setParam()

bool VPlug_GeoShapes::setParam(const string& param, string value)
{
  // First check the parameters that may be arriving in streams
  if((param ==  "poly") || (param == "polygon"))
    return(addPolygon(value));
  else if((param ==  "segl") || (param == "seglist"))
    return(addSegList(value));
  else if((param ==  "grid") || (param == "xygrid"))
    return(addGrid(value));
  else if(param ==  "convex_grid")
    return(addConvexGrid(value));
  else if(param == "clear") {
    if(value == "seglists")
      m_polygons.clear();
    else if(value == "polygons")
      m_seglists.clear();
    else if(value == "grids") {
      m_grids.clear();
      m_convex_grids.clear();
    }
    else if(value == "circles")
      m_circles.clear();
    else if(value == "points")
      m_points.clear();
    else if(value == "hexagons")
      m_hexagons.clear();
    else if(value == "vectors")
      m_vectors.clear();
    else
      return(false);
  }
  else
    return(false);
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: addPolygon

void VPlug_GeoShapes::addPolygon(const XYPolygon& new_poly)
{
  updateBounds(new_poly.get_min_x(), new_poly.get_max_x(), 
	       new_poly.get_min_y(), new_poly.get_max_y());

  string new_label = new_poly.get_label();
  if(new_label == "") {
    m_polygons.push_back(new_poly);
    return;
  }

  unsigned int i, vsize = m_polygons.size();
  for(i=0; i<vsize; i++) {
    if((m_polygons[i].get_label()  == new_label) &&
       (m_polygons[i].get_type()   == new_poly.get_type())   &&
       (m_polygons[i].get_source() == new_poly.get_source())) {
      m_polygons[i] = new_poly;
      return;
    }
  }
  m_polygons.push_back(new_poly);  
}

//-----------------------------------------------------------
// Procedure: addSegList

void VPlug_GeoShapes::addSegList(const XYSegList& new_segl)
{
  updateBounds(new_segl.get_min_x(), new_segl.get_max_x(), 
	       new_segl.get_min_y(), new_segl.get_max_y());

  string new_label = new_segl.get_label();
  if(new_label == "") {
    m_seglists.push_back(new_segl);
    return;
  }
  
  unsigned int i, vsize = m_seglists.size();
  for(i=0; i<vsize; i++) {
    if(m_seglists[i].get_label() == new_label) {
      m_seglists[i] = new_segl;
      return;
    }
  }
  m_seglists.push_back(new_segl);  
}

//-----------------------------------------------------------
// Procedure: addVector

void VPlug_GeoShapes::addVector(const XYVector& new_vect)
{
  updateBounds(new_vect.xpos(), new_vect.xpos(),
	       new_vect.ypos(), new_vect.ypos());

  string new_label = new_vect.get_label();
  if(new_label == "") {
    m_vectors.push_back(new_vect);
    return;
  }
  
  unsigned int i, vsize = m_vectors.size();
  for(i=0; i<vsize; i++) {
    if(m_vectors[i].get_label() == new_label) {
      m_vectors[i] = new_vect;
      return;
    }
  }
  m_vectors.push_back(new_vect);  
}

//-----------------------------------------------------------
// Procedure: addRangePulse

void VPlug_GeoShapes::addRangePulse(const XYRangePulse& new_pulse)
{
  updateBounds(new_pulse.get_x(), new_pulse.get_x(),
	       new_pulse.get_y(), new_pulse.get_y());

  string new_label = new_pulse.get_label();
  if(new_label == "") {
    m_range_pulses.push_back(new_pulse);
    return;
  }
  
  unsigned int i, vsize = m_range_pulses.size();
  for(i=0; i<vsize; i++) {
    if(m_range_pulses[i].get_label() == new_label) {
      m_range_pulses[i] = new_pulse;
      return;
    }
  }
  m_range_pulses.push_back(new_pulse);  
}

//-----------------------------------------------------------
// Procedure: addCommsPulse

void VPlug_GeoShapes::addCommsPulse(const XYCommsPulse& new_pulse)
{
  string new_label = new_pulse.get_label();
  if(new_label == "") {
    m_comms_pulses.push_back(new_pulse);
    return;
  }
  
  unsigned int i, vsize = m_comms_pulses.size();
  for(i=0; i<vsize; i++) {
    if(m_comms_pulses[i].get_label() == new_label) {
      m_comms_pulses[i] = new_pulse;
      return;
    }
  }
  m_comms_pulses.push_back(new_pulse);  
}

//-----------------------------------------------------------
// Procedure: addMarker

void VPlug_GeoShapes::addMarker(const XYMarker& new_marker)
{
  updateBounds(new_marker.get_vx(), new_marker.get_vx(),
	       new_marker.get_vy(), new_marker.get_vy());

  string new_label = new_marker.get_label();
  if(new_label == "") {
    m_markers.push_back(new_marker);
    return;
  }
  
  unsigned int i, vsize = m_markers.size();
  for(i=0; i<vsize; i++) {
    if(m_markers[i].get_label() == new_label) {
      m_markers[i] = new_marker;
      return;
    }
  }
  m_markers.push_back(new_marker);  
}

//-----------------------------------------------------------
// Procedure: updateGrid

bool VPlug_GeoShapes::updateGrid(const string& delta)
{
  bool ok = true;
  unsigned int i, vsize = m_grids.size();
  for(i=0; i<vsize; i++)
    ok = ok && m_grids[i].processDelta(delta);
  return(ok);
}

//-----------------------------------------------------------
// Procedure: updateConvexGrid

bool VPlug_GeoShapes::updateConvexGrid(const string& delta)
{
  bool ok = true;
#if 0
  unsigned int i, vsize = m_convex_grids.size();
  for(i=0; i<vsize; i++)
    ok = ok && m_convex_grids[i].processDelta(delta);
#endif
  return(ok);
}

//-----------------------------------------------------------
// Procedure: addGrid

void VPlug_GeoShapes::addGrid(const XYGrid& new_grid)
{
  XYSquare square = new_grid.getSBound();
  updateBounds(square.get_min_x(), square.get_max_x(), 
	       square.get_min_y(), square.get_max_y());

  string new_label = new_grid.getLabel();
  if(new_label == "") {
    m_grids.push_back(new_grid);
    return;
  }
  
  unsigned int i, vsize = m_grids.size();
  for(i=0; i<vsize; i++) {
    if(m_grids[i].getLabel() == new_label) {
      m_grids[i] = new_grid;
      return;
    }
  }

  m_grids.push_back(new_grid);
}

//-----------------------------------------------------------
// Procedure: addConvexGrid

void VPlug_GeoShapes::addConvexGrid(const XYConvexGrid& new_grid)
{
  XYSquare square = new_grid.getSBound();
  updateBounds(square.get_min_x(), square.get_max_x(), 
	       square.get_min_y(), square.get_max_y());

  string new_label = new_grid.get_label();
  if(new_label == "") {
    m_convex_grids.push_back(new_grid);
    return;
  }
  
  unsigned int i, vsize = m_convex_grids.size();
  for(i=0; i<vsize; i++) {
    if(m_convex_grids[i].get_label() == new_label) {
      m_convex_grids[i] = new_grid;
      return;
    }
  }
  
  m_convex_grids.push_back(new_grid);
}

//-----------------------------------------------------------
// Procedure: addCircle

void VPlug_GeoShapes::addCircle(const XYCircle& circle)
{
  updateBounds(circle.get_min_x(), circle.get_max_x(), 
	       circle.get_min_y(), circle.get_max_y());

  m_circles.push_back(circle);
}


//-----------------------------------------------------------
// Procedure: addHexagon

void VPlug_GeoShapes::addHexagon(const XYHexagon& hexagon)
{
  updateBounds(hexagon.get_min_x(), hexagon.get_max_x(), 
	       hexagon.get_min_y(), hexagon.get_max_y());

  m_hexagons.push_back(hexagon);
}


//-----------------------------------------------------------
// Procedure: addPoint

void VPlug_GeoShapes::addPoint(const XYPoint& new_point)
{
  updateBounds(new_point.x(), new_point.x(), 
	       new_point.y(), new_point.y());

  string new_label  = new_point.get_label();
  if(new_label == "") {
    m_points.push_back(new_point);
    return;
  }
  
  string new_type   = new_point.get_type();
  string new_source = new_point.get_source();
  unsigned int i, vsize = m_points.size();
  for(i=0; i<vsize; i++) {
    if((m_points[i].get_label()  == new_label)  &&
       (m_points[i].get_type()   == new_type)   &&
       (m_points[i].get_source() == new_source)) {
      m_points[i] = new_point;
      return;
    }
  }
  
  m_points.push_back(new_point);
}

//-----------------------------------------------------------
// Procedure: addPolygon

bool VPlug_GeoShapes::addPolygon(const string& poly_str)
{
  XYPolygon new_poly = string2Poly(poly_str);
  if(new_poly.size() == 0)
    return(false);
  addPolygon(new_poly);
  return(true);
}

//-----------------------------------------------------------
// Procedure: addVector

bool VPlug_GeoShapes::addVector(const string& vect_str)
{
  XYVector new_vect = string2Vector(vect_str);
  addVector(new_vect);
  return(true);
}

//-----------------------------------------------------------
// Procedure: addRangePulse

bool VPlug_GeoShapes::addRangePulse(const string& pulse_str)
{
  XYRangePulse new_pulse = string2RangePulse(pulse_str);
  if(new_pulse.valid()) {
    addRangePulse(new_pulse);
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: addCommsPulse

bool VPlug_GeoShapes::addCommsPulse(const string& pulse_str)
{
  XYCommsPulse new_pulse = string2CommsPulse(pulse_str);
  if(new_pulse.valid()) {
    addCommsPulse(new_pulse);
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: addMarker

bool VPlug_GeoShapes::addMarker(const string& marker_str)
{
  XYMarker new_marker = string2Marker(marker_str);
  if(new_marker.valid()) {
    addMarker(new_marker);
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: addSegList

bool VPlug_GeoShapes::addSegList(const string& segl_str)
{
  XYSegList new_segl = string2SegList(segl_str);
  if(new_segl.size() == 0)
    return(false);
   addSegList(new_segl);
  return(true);
}


//-----------------------------------------------------------
// Procedure: addPoint

bool VPlug_GeoShapes::addPoint(const string& point_str)
{
  XYPoint new_point = string2Point(point_str);
  if(!new_point.valid())
    return(false);
  addPoint(new_point);
  return(true);
}

//-----------------------------------------------------------
// Procedure: addCircle

bool VPlug_GeoShapes::addCircle(const string& circle_str)
{
  XYCircle new_circle = string2Circle(circle_str);
  if(!new_circle.valid())
    return(false);
  addCircle(new_circle);
  return(true);
}

//-----------------------------------------------------------
// Procedure: addGrid

bool VPlug_GeoShapes::addGrid(const string& grid_str)
{
  XYGrid new_grid;
  bool ok = new_grid.initialize(grid_str);
  if(!ok)
    return(false);
  addGrid(new_grid);
  return(true);
}

//-----------------------------------------------------------
// Procedure: addConvexGrid

bool VPlug_GeoShapes::addConvexGrid(const string& grid_str)
{
  XYConvexGrid new_grid = string2ConvexGrid(grid_str);
  if(!new_grid.valid())
    return(false);
  addConvexGrid(new_grid);
  return(true);
}


//-------------------------------------------------------------
// Procedure: getPolygon(int)

XYPolygon VPlug_GeoShapes::getPolygon(unsigned int index) const
{
  if(index >= m_polygons.size()) {
    XYPolygon null_poly;
    return(null_poly);
  }
  else
    return(m_polygons[index]);
}

//-------------------------------------------------------------
// Procedure: getSegList(int)

XYSegList VPlug_GeoShapes::getSegList(unsigned int index) const
{
  if(index >= m_seglists.size()) {
    XYSegList null_segl;
    return(null_segl);
  }
  else
    return(m_seglists[index]);
}

//-------------------------------------------------------------
// Procedure: getGrid(uint)

XYGrid VPlug_GeoShapes::getGrid(unsigned int index) const
{
  if(index >= m_grids.size()) {
    XYGrid null_grid;
    return(null_grid);
  }
  else
    return(m_grids[index]);
}

//-------------------------------------------------------------
// Procedure: getConvexGrid(uint)

XYConvexGrid VPlug_GeoShapes::getConvexGrid(unsigned int index) const
{
  if(index >= m_convex_grids.size()) {
    XYConvexGrid null_grid;
    return(null_grid);
  }
  else
    return(m_convex_grids[index]);
}

//-------------------------------------------------------------
// Procedure: getCircle(int)

XYCircle VPlug_GeoShapes::getCircle(unsigned int index) const
{
  if(index >= m_circles.size()) {
    XYCircle null_circle;
    return(null_circle);
  }
  else
    return(m_circles[index]);
}

//-------------------------------------------------------------
// Procedure: getHexagon(int)

XYHexagon VPlug_GeoShapes::getHexagon(unsigned int index) const
{
  if(index >= m_hexagons.size()) {
    XYHexagon null_hexagon;
    return(null_hexagon);
  }
  else
    return(m_hexagons[index]);
}

//-------------------------------------------------------------
// Procedure: getPoint(int)

XYPoint VPlug_GeoShapes::getPoint(unsigned int index) const
{
  if(index >= m_points.size()) {
    XYPoint null_point;
    return(null_point);
  }
  else
    return(m_points[index]);
}

//-------------------------------------------------------------
// Procedure: getVector(int)

XYVector VPlug_GeoShapes::getVector(unsigned int index) const
{
  if(index >= m_vectors.size()) {
    XYVector null_vector;
    return(null_vector);
  }
  else
    return(m_vectors[index]);
}

//-------------------------------------------------------------
// Procedure: getRangePulse(int)

XYRangePulse VPlug_GeoShapes::getRangePulse(unsigned int index) const
{
  if(index >= m_range_pulses.size()) {
    XYRangePulse null_range_pulse;
    return(null_range_pulse);
  }
  else
    return(m_range_pulses[index]);
}

//-------------------------------------------------------------
// Procedure: getMarker(int)

XYMarker VPlug_GeoShapes::getMarker(unsigned int index) const
{
  if(index >= m_markers.size()) {
    XYMarker null_marker;
    return(null_marker);
  }
  else
    return(m_markers[index]);
}


//-----------------------------------------------------------
// Procedure: updateBounds()

void VPlug_GeoShapes::updateBounds(double xl, double xh, 
				   double yl, double yh)
{
  if(xl < m_xmin)
    m_xmin = xl;
  if(xh > m_xmax)
    m_xmax = xh;

  if(yl < m_ymin)
    m_ymin = yl;
  if(yh > m_ymax)
    m_ymax = yh;
}


