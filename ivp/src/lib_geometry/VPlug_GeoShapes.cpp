/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlug_GeoShapes.cpp                                  */
/*    DATE: July 9th, 2008                                       */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
#include <iostream>
#include <iostream>
#include <cstdlib>
#include "VPlug_GeoShapes.h"
#include "MBUtils.h"
#include "XYFormatUtilsSegl.h"
#include "XYFormatUtilsSeglr.h"
#include "XYFormatUtilsPoly.h"
#include "XYFormatUtilsPoint.h"
#include "XYFormatUtilsCircle.h"
#include "XYFormatUtilsVector.h"
#include "XYFormatUtilsRangePulse.h"
#include "XYFormatUtilsCommsPulse.h"
#include "XYFormatUtilsMarker.h"
#include "XYFormatUtilsConvexGrid.h"
#include "XYFormatUtilsWedge.h"
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

void VPlug_GeoShapes::clear(string shape, string stype)
{
  if((shape == "") && (stype == "")) {
    m_polygons.clear();
    m_seglists.clear();
    m_seglrs.clear();
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
    return;
  }

  if((shape == "polygons") || (shape=="polygon"))
    clearPolygons(stype);
  else if(shape == "points")
    clearPoints(stype);
  else if(shape == "seglists")
    clearSegLists(stype);
  else if(shape == "seglrs")
    clearSeglrs(stype);

  
  updateBounds();
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
  else if(param ==  "seglr")
    return(addSeglr(value));
  else if((param ==  "marker") || (param == "view_marker"))
    return(addMarker(value));
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
// Procedure: manageMemory()

void VPlug_GeoShapes::manageMemory(double curr_time)
{
  //-------------------------------------------------- Points
  map<string,XYPoint>::iterator p1;
  for(p1=m_points.begin(); p1!=m_points.end();) {
    if(p1->second.expired(curr_time))
      p1 = m_points.erase(p1);
    else
      ++p1;
  }
  //-------------------------------------------------- Markers
  map<string,XYMarker>::iterator p2;
  for(p2=m_markers.begin(); p2!=m_markers.end();) {
    if(p2->second.expired(curr_time))
      p2 = m_markers.erase(p2);
    else
      ++p2;
  }

  //-------------------------------------------------- Circles
  map<string,XYCircle>::iterator p3;
  for(p3=m_circles.begin(); p3!=m_circles.end();) {
    if(p3->second.expired(curr_time))
      p3 = m_circles.erase(p3);
    else
      ++p3;
  }

  //-------------------------------------------------- Polygons
  vector<XYPolygon> save_polys;
  for(unsigned int i=0; i<m_polygons.size(); i++) {
    if(!m_polygons[i].expired(curr_time))
      save_polys.push_back(m_polygons[i]);
  }
  m_polygons = save_polys;

  //-------------------------------------------------- SegLists
  vector<XYSegList> save_segls;
  for(unsigned int i=0; i<m_seglists.size(); i++) {
    if(!m_seglists[i].expired(curr_time))
      save_segls.push_back(m_seglists[i]);
  }
  m_seglists = save_segls;

}

//-----------------------------------------------------------
// Procedure: forgetPolygon

void VPlug_GeoShapes::forgetPolygon(string label)
{
  vector<XYPolygon> new_polys;
  for(unsigned int i=0; i<m_polygons.size(); i++) {
    if(m_polygons[i].get_label() != label) 
      new_polys.push_back(m_polygons[i]);
  }
  m_polygons = new_polys;
}


//-----------------------------------------------------------
// Procedure: addPolygon

void VPlug_GeoShapes::addPolygon(const XYPolygon& new_poly)
{
  string new_label = new_poly.get_label();
  if(!new_poly.active()) {
    forgetPolygon(new_label);
    return;
  }
  
  if(new_poly.size()) {
    updateBounds(new_poly.get_min_x(), new_poly.get_max_x(), 
		 new_poly.get_min_y(), new_poly.get_max_y());
  }

  if(new_label == "") {
    m_polygons.push_back(new_poly);
    return;
  }

  unsigned int i, vsize = m_polygons.size();
  for(i=0; i<vsize; i++) {
    if(m_polygons[i].get_label() == new_label) {
      m_polygons[i] = new_poly;
      return;
    }
  }
  m_polygons.push_back(new_poly);  
}

//-----------------------------------------------------------
// Procedure: forgetSegList

void VPlug_GeoShapes::forgetSegList(string label)
{
  vector<XYSegList> new_segls;
  for(unsigned int i=0; i<m_seglists.size(); i++) {
    if(m_seglists[i].get_label() != label) 
      new_segls.push_back(m_seglists[i]);
  }
  m_seglists = new_segls;
}


//-----------------------------------------------------------
// Procedure: addSegList

void VPlug_GeoShapes::addSegList(const XYSegList& new_segl)
{
  string new_label = new_segl.get_label();
  if(!new_segl.active()) {
    forgetSegList(new_label);
    return;
  }

  if(new_segl.size() > 0) {
    updateBounds(new_segl.get_min_x(), new_segl.get_max_x(), 
		 new_segl.get_min_y(), new_segl.get_max_y());
  }

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
// Procedure: forgetSeglr

void VPlug_GeoShapes::forgetSeglr(string label)
{
  vector<XYSeglr> new_seglrs;
  for(unsigned int i=0; i<m_seglrs.size(); i++) {
    if(m_seglrs[i].get_label() != label) 
      new_seglrs.push_back(m_seglrs[i]);
  }
  m_seglrs = new_seglrs;
}


//-----------------------------------------------------------
// Procedure: addSeglr

void VPlug_GeoShapes::addSeglr(const XYSeglr& new_seglr)
{
  string new_label = new_seglr.get_label();
  if(!new_seglr.active()) {
    forgetSeglr(new_label);
    return;
  }

  if(new_seglr.size() > 0) {
    updateBounds(new_seglr.getMinX(), new_seglr.getMaxX(), 
		 new_seglr.getMinY(), new_seglr.getMaxY());
  }

  if(new_label == "") {
    m_seglrs.push_back(new_seglr);
    return;
  }
  
  for(unsigned int i=0; i<m_seglrs.size(); i++) {
    if(m_seglrs[i].get_label() == new_label) {
      m_seglrs[i] = new_seglr;
      return;
    }
  }
  m_seglrs.push_back(new_seglr);
}

//-----------------------------------------------------------
// Procedure: forgetVector()

void VPlug_GeoShapes::forgetVector(string label)
{
  vector<XYVector> new_vectors;
  for(unsigned int i=0; i<m_vectors.size(); i++) {
    if(m_vectors[i].get_label() != label) 
      new_vectors.push_back(m_vectors[i]);
  }
  m_vectors = new_vectors;
}

//-----------------------------------------------------------
// Procedure: addVector()

void VPlug_GeoShapes::addVector(const XYVector& new_vect)
{
  string new_label = new_vect.get_label();
  if(!new_vect.active()) {
    forgetVector(new_label);
    return;
  }

  updateBounds(new_vect.xpos(), new_vect.xpos(),
	       new_vect.ypos(), new_vect.ypos());

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
// Procedure: forgetRangePulse()

void VPlug_GeoShapes::forgetRangePulse(string label)
{
  vector<XYRangePulse> new_pulses;
  for(unsigned int i=0; i<m_range_pulses.size(); i++) {
    if(m_range_pulses[i].get_label() != label) 
      new_pulses.push_back(m_range_pulses[i]);
  }
  m_range_pulses = new_pulses;
}

//-----------------------------------------------------------
// Procedure: addRangePulse()

void VPlug_GeoShapes::addRangePulse(const XYRangePulse& new_pulse)
{
  string new_label = new_pulse.get_label();
  if(!new_pulse.active()) {
    forgetRangePulse(new_label);
    return;
  }

  updateBounds(new_pulse.get_x(), new_pulse.get_x(),
	       new_pulse.get_y(), new_pulse.get_y());

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
// Procedure: forgetCommsPulse()

void VPlug_GeoShapes::forgetCommsPulse(string label)
{
  vector<XYCommsPulse> new_pulses;
  for(unsigned int i=0; i<m_comms_pulses.size(); i++) {
    if(m_comms_pulses[i].get_label() != label) 
      new_pulses.push_back(m_comms_pulses[i]);
  }
  m_comms_pulses = new_pulses;
}

//-----------------------------------------------------------
// Procedure: addCommsPulse()

void VPlug_GeoShapes::addCommsPulse(const XYCommsPulse& new_pulse)
{
  string new_label = new_pulse.get_label();
  if(!new_pulse.active()) {
    forgetCommsPulse(new_label);
    return;
  }

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
  string new_label = new_marker.get_label();
  if(!new_marker.active()) {
    m_markers.erase(new_label);
    return;
  }

  updateBounds(new_marker.get_vx(), new_marker.get_vx(),
	       new_marker.get_vy(), new_marker.get_vy());

  if(new_label == "")
    new_label = "marker_" + uintToString(m_markers.size());
  m_markers[new_label] = new_marker;
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
// Procedure: sizeTotalShapes()

unsigned int VPlug_GeoShapes::sizeTotalShapes() const
{
  return(sizePolygons()    + sizeSegLists() + 
	 sizeCircles()     + sizeHexagons() + 
	 sizePoints()      + sizeVectors()  + 
	 sizeGrids()       + sizeConvexGrids() + 
	 sizeMarkers()     + sizeRangePulses() + 
	 sizeSeglrs()      + sizeCommsPulses());
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

void VPlug_GeoShapes::addCircle(const XYCircle& new_circle, 
				unsigned int drawpts)
{
  string new_label = new_circle.get_label();
  if(!new_circle.active()) {
    m_circles.erase(new_label);
    return;
  }

  updateBounds(new_circle.get_min_x(), new_circle.get_max_x(), 
	       new_circle.get_min_y(), new_circle.get_max_y());

  if(drawpts > 100)
    drawpts = 100;  

#if 1
  if(new_label == "")
    new_label = uintToString(m_circles.size());
  m_circles[new_label] = new_circle;
  m_circles[new_label].setPointCacheAuto(drawpts);
#endif

#if 0
  if(new_label == "") {
    m_circles.push_back(new_circle);
    return;
  }
  
  unsigned int i, vsize = m_circles.size();
  for(i=0; i<vsize; i++) {
    if(m_circles[i].get_label() == new_label) {
      m_circles[i] = new_circle;
      return;
    }
  }
  m_circles.push_back(new_circle);
#endif
}


//-----------------------------------------------------------
// Procedure: forgetWedge()

void VPlug_GeoShapes::forgetWedge(string label)
{
  vector<XYWedge> new_wedges;
  for(unsigned int i=0; i<m_wedges.size(); i++) {
    if(m_wedges[i].get_label() != label) 
      new_wedges.push_back(m_wedges[i]);
  }
  m_wedges = new_wedges;
}

//-----------------------------------------------------------
// Procedure: addWedge

void VPlug_GeoShapes::addWedge(const XYWedge& new_wedge)
{
  string new_label = new_wedge.get_label();
  if(!new_wedge.active()) {
    forgetWedge(new_label);
    return;
  }
  
  updateBounds(new_wedge.getMinX(), new_wedge.getMaxX(), 
	       new_wedge.getMinY(), new_wedge.getMaxY());

  for(unsigned int i=0; i<m_wedges.size(); i++) {
    if(m_wedges[i].get_label() == new_label) {
      m_wedges[i] = new_wedge;
      return;
    }
  }
  m_wedges.push_back(new_wedge);  
}


//-----------------------------------------------------------
// Procedure: forgetHexagon()

void VPlug_GeoShapes::forgetHexagon(string label)
{
  vector<XYHexagon> new_hexagons;
  for(unsigned int i=0; i<m_hexagons.size(); i++) {
    if(m_hexagons[i].get_label() != label) 
      new_hexagons.push_back(m_hexagons[i]);
  }
  m_hexagons = new_hexagons;
}

//-----------------------------------------------------------
// Procedure: addHexagon()

void VPlug_GeoShapes::addHexagon(const XYHexagon& hexagon)
{
  string new_label = hexagon.get_label();
  if(!hexagon.active()) {
    forgetHexagon(new_label);
    return;
  }
  
  updateBounds(hexagon.get_min_x(), hexagon.get_max_x(), 
	       hexagon.get_min_y(), hexagon.get_max_y());

  for(unsigned int i=0; i<m_hexagons.size(); i++) {
    if(m_hexagons[i].get_label() == new_label) {
      m_hexagons[i] = hexagon;
      return;
    }
  }
  m_hexagons.push_back(hexagon);
}


//-----------------------------------------------------------
// Procedure: addPoint()

void VPlug_GeoShapes::addPoint(const XYPoint& new_point)
{
  string new_label  = new_point.get_label();
  if(!new_point.active()) {
    m_points.erase(new_label);
    return;
  }

  double px = new_point.x();
  double py = new_point.y();
  updateBounds(px, px, py, py);

  if(new_label == "")
    new_label = "pt_" + uintToString(m_points.size());
  m_points[new_label] = new_point;
}

//-----------------------------------------------------------
// Procedure: addPolygon

bool VPlug_GeoShapes::addPolygon(const string& poly_str,
				 double timestamp)
{
  XYPolygon new_poly = string2Poly(poly_str);
  if((new_poly.size()==0) && new_poly.active())
    return(false);
  
  if(new_poly.get_time() == 0)
    new_poly.set_time(timestamp);

  addPolygon(new_poly);
  return(true);
}

//-----------------------------------------------------------
// Procedure: addWedge

bool VPlug_GeoShapes::addWedge(const string& wedge_str,
			       unsigned int draw_pts)
{
  XYWedge new_wedge = string2Wedge(wedge_str);
  if(new_wedge.isValid()) {
    new_wedge.initialize();
    addWedge(new_wedge);
  }
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

bool VPlug_GeoShapes::addRangePulse(const string& pulse_str,
				    double timestamp)
{
  XYRangePulse new_pulse = string2RangePulse(pulse_str);
  if(new_pulse.valid()) {
    if(new_pulse.get_time() == 0)
      new_pulse.set_time(timestamp);       
    addRangePulse(new_pulse);
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: addCommsPulse

bool VPlug_GeoShapes::addCommsPulse(const string& pulse_str,
				    double timestamp)
{
  XYCommsPulse new_pulse = string2CommsPulse(pulse_str);
  if(new_pulse.valid()) {
    if(new_pulse.get_time() == 0)
      new_pulse.set_time(timestamp);       
    addCommsPulse(new_pulse);
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: addMarker

bool VPlug_GeoShapes::addMarker(const string& marker_str,
				double timestamp)
{
  XYMarker new_marker = string2Marker(marker_str);
  if(!new_marker.valid())
    return(false);

  if(new_marker.get_time() == 0)
    new_marker.set_time(timestamp);
  
  addMarker(new_marker);
  return(true);
}

//-----------------------------------------------------------
// Procedure: addSegList

bool VPlug_GeoShapes::addSegList(const string& segl_str,
				 double timestamp)
{
  XYSegList new_segl = string2SegList(segl_str);
  if((new_segl.size()==0) && new_segl.active())
    return(false);

  if(new_segl.get_time() == 0)
    new_segl.set_time(timestamp);

  addSegList(new_segl);
  return(true);
}

//-----------------------------------------------------------
// Procedure: addSeglr

bool VPlug_GeoShapes::addSeglr(const string& seglr_str)
{
  XYSeglr new_seglr = string2Seglr(seglr_str);
  if((new_seglr.size()==0) && new_seglr.active())
    return(false);
  addSeglr(new_seglr);
  return(true);
}

//-----------------------------------------------------------
// Procedure: addPoint

bool VPlug_GeoShapes::addPoint(const string& point_str,
			       double timestamp)
{
  XYPoint new_point = string2Point(point_str);
  if(!new_point.valid())
    return(true);

  if(new_point.get_time() == 0)
    new_point.set_time(timestamp);

  addPoint(new_point);
  return(true);
}

//-----------------------------------------------------------
// Procedure: addCircle

bool VPlug_GeoShapes::addCircle(const string& circle_str,
				unsigned int drawpts, double timestamp)
{
  XYCircle new_circle = string2Circle(circle_str);
  if(!new_circle.valid())
    return(false);

  if(new_circle.get_time() == 0)
    new_circle.set_time(timestamp);

  addCircle(new_circle, drawpts);
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
// Procedure: getSeglr(int)

XYSeglr VPlug_GeoShapes::getSeglr(unsigned int index) const
{
  if(index >= m_seglrs.size()) {
    XYSeglr null_seglr;
    return(null_seglr);
  }
  else
    return(m_seglrs[index]);
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


//-----------------------------------------------------------
// Procedure: updateBounds()

void VPlug_GeoShapes::updateBounds()
{
  unsigned int i;
  for(i=0; i<m_polygons.size(); i++) {
    if(m_polygons[i].size() > 0) {
      XYPolygon poly = m_polygons[i];
      updateBounds(poly.get_min_x(), poly.get_max_x(),
		   poly.get_min_y(), poly.get_max_y());
    }
  }
  for(i=0; i<m_seglists.size(); i++) {
    if(m_seglists[i].size() > 0) {
      XYSegList segl = m_seglists[i];
      updateBounds(segl.get_min_x(), segl.get_max_x(),
		   segl.get_min_y(), segl.get_max_y());
    }
  }
  for(i=0; i<m_seglrs.size(); i++) {
    if(m_seglrs[i].size() > 0) {
      XYSeglr seglr = m_seglrs[i];
      updateBounds(seglr.getMinX(), seglr.getMaxX(),
		   seglr.getMinY(), seglr.getMaxY());
    }
  }
  for(i=0; i<m_hexagons.size(); i++) {
    if(m_hexagons[i].size() > 0) {
      XYHexagon hexa = m_hexagons[i];
      updateBounds(hexa.get_min_x(), hexa.get_max_x(),
		   hexa.get_min_y(), hexa.get_max_y());
    }
  }
  for(i=0; i<m_grids.size(); i++) {
    if(m_grids[i].size() > 0) {
      XYSquare square = m_grids[i].getSBound();
      updateBounds(square.get_min_x(), square.get_max_x(),
		   square.get_min_y(), square.get_max_y());
    }
  }
  for(i=0; i<m_convex_grids.size(); i++) {
    if(m_convex_grids[i].size() > 0) {
      XYSquare square = m_convex_grids[i].getSBound();
      updateBounds(square.get_min_x(), square.get_max_x(),
		   square.get_min_y(), square.get_max_y());
    }
  }
  for(i=0; i<m_vectors.size(); i++) {
    XYVector vect = m_vectors[i];
    updateBounds(vect.xpos(), vect.xpos(),
		 vect.ypos(), vect.ypos());
  }
  for(i=0; i<m_range_pulses.size(); i++) {
    XYRangePulse pulse = m_range_pulses[i];
    updateBounds(pulse.get_x(), pulse.get_x(),
		 pulse.get_y(), pulse.get_y());
  }

  map<string, XYPoint>::iterator p1;
  for(p1=m_points.begin(); p1!=m_points.end(); p1++) {
    XYPoint pt = p1->second;
    updateBounds(pt.x(), pt.x(), pt.y(), pt.y());
  }

  map<string, XYMarker>::iterator p2;
  for(p2=m_markers.begin(); p2!=m_markers.end(); p2++) {
    XYMarker marker = p2->second;
    updateBounds(marker.get_vx(), marker.get_vx(), 
		 marker.get_vy(), marker.get_vy());
  }

  map<string, XYCircle>::iterator p3;
  for(p3=m_circles.begin(); p3!=m_circles.end(); p3++) {
    XYCircle circle = p3->second;
    updateBounds(circle.get_min_x(), circle.get_max_x(), 
		 circle.get_min_y(), circle.get_max_y());
  }
}


//-----------------------------------------------------------
// Procedure: clearPolygons

void VPlug_GeoShapes::clearPolygons(string stype)
{
  if(stype == "") {
    m_polygons.clear();
    return;
  }

  vector<XYPolygon> new_polygons;
  for(unsigned int i=0; i<m_polygons.size(); i++)  {
    if(typeMatch(&(m_polygons[i]), stype))
      new_polygons.push_back(m_polygons[i]);
  } 
  m_polygons = new_polygons;
}

//-----------------------------------------------------------
// Procedure: clearSegLists

void VPlug_GeoShapes::clearSegLists(string stype)
{
  if(stype == "") {
    m_seglists.clear();
    return;
  }

  vector<XYSegList> new_segls;
  for(unsigned int i=0; i<m_seglists.size(); i++)  {
    if(typeMatch(&(m_seglists[i]), stype))
      new_segls.push_back(m_seglists[i]);
  } 
  m_seglists = new_segls;
}

//-----------------------------------------------------------
// Procedure: clearSeglrs

void VPlug_GeoShapes::clearSeglrs(string stype)
{
  if(stype == "") {
    m_seglrs.clear();
    return;
  }

  vector<XYSeglr> new_seglrs;
  for(unsigned int i=0; i<m_seglrs.size(); i++)  {
    if(typeMatch(&(m_seglrs[i]), stype))
      new_seglrs.push_back(m_seglrs[i]);
  } 
  m_seglrs = new_seglrs;
}

//-----------------------------------------------------------
// Procedure: clearWedges

void VPlug_GeoShapes::clearWedges(string stype)
{
  if(stype == "") {
    m_wedges.clear();
    return;
  }

  vector<XYWedge> new_wedges;
  for(unsigned int i=0; i<m_wedges.size(); i++)  {
    if(typeMatch(&(m_wedges[i]), stype))
      new_wedges.push_back(m_wedges[i]);
  } 
  m_wedges = new_wedges;
}


//-----------------------------------------------------------
// Procedure: clearPoints

void VPlug_GeoShapes::clearPoints(string stype)
{
  if(stype == "") {
    m_points.clear();
    return;
  }

  map<string, XYPoint> new_points;
  map<string, XYPoint>::iterator p;
  for(p=m_points.begin(); p!=m_points.end(); p++) {
    if(typeMatch(&(p->second), stype))
      new_points[p->first] = p->second;
  }
  m_points = new_points;
}


//-----------------------------------------------------------
// Procedure: typeMatch

bool VPlug_GeoShapes::typeMatch(XYObject* obj, string stype)
{
  if(stype == "") 
    return(true);

  string pattern = stype;
  bool begins    = false;
  bool ends      = false;

  if(stype.at(0) == '*') {
    begins = true;
    pattern = stype.substr(1);
  }
  if(stype.at(stype.length()-1) == '*') {
    begins = true;
    pattern = pattern.substr(0,pattern.length()-1);
  }
    
  string otype = obj->get_type();
  
  if(begins && ends) 
    return(strContains(otype, pattern));
  
  if(begins)
    return(strBegins(otype, pattern));
  
  if(ends)
    return(strEnds(otype, pattern));

  return(otype == pattern);
}
