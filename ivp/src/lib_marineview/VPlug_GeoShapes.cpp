/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VPlug_GeoShapes.cpp                                  */
/*    DATE: July 9th, 2008                                       */
/*****************************************************************/

#include <cstdlib>
#include "VPlug_GeoShapes.h"
#include "MBUtils.h"
#include "XYFormatUtilsSegl.h"
#include "XYFormatUtilsPoly.h"
#include "XYFormatUtilsPoint.h"
#include "XYFormatUtilsCircle.h"
#include "ColorParse.h"

using namespace std;

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
  else if(param == "clear") {
    if(value == "seglists")
      m_polygons.clear();
    else if(value == "polygons")
      m_seglists.clear();
    else if(value == "grids")
      m_grids.clear();
    else if(value == "circles")
      m_circles.clear();
    else if(value == "points")
      m_points.clear();
    else if(value == "hexagons")
      m_hexagons.clear();
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
  string new_label = new_poly.get_label();
  if(new_label == "") {
    m_polygons.push_back(new_poly);
    return;
  }

  for(unsigned int i=0; i<m_polygons.size(); i++) {
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
  string new_label = new_segl.get_label();
  if(new_label == "") {
    m_seglists.push_back(new_segl);
    return;
  }
  
  for(unsigned int i=0; i<m_seglists.size(); i++) {
    if(m_seglists[i].get_label() == new_label) {
      m_seglists[i] = new_segl;
      return;
    }
  }
  m_seglists.push_back(new_segl);  
}

//-----------------------------------------------------------
// Procedure: updateGrid

bool VPlug_GeoShapes::updateGrid(const string& delta)
{
  bool ok = true;
  for(unsigned int i=0; i<m_grids.size(); i++)
    ok = ok && m_grids[i].processDelta(delta);
  return(ok);
}

//-----------------------------------------------------------
// Procedure: addGrid

void VPlug_GeoShapes::addGrid(const XYGrid& new_grid)
{
  string new_label = new_grid.getLabel();
  if(new_label == "") {
    m_grids.push_back(new_grid);
    return;
  }
  
  for(unsigned int i=0; i<m_grids.size(); i++) {
    if(m_grids[i].getLabel() == new_label) {
      m_grids[i] = new_grid;
      return;
    }
  }

  m_grids.push_back(new_grid);
}

//-----------------------------------------------------------
// Procedure: addCircle

void VPlug_GeoShapes::addCircle(const XYCircle& circle)
{
  m_circles.push_back(circle);
}


//-----------------------------------------------------------
// Procedure: addHexagon

void VPlug_GeoShapes::addHexagon(const XYHexagon& hexagon)
{
  m_hexagons.push_back(hexagon);
}


//-----------------------------------------------------------
// Procedure: addPoint

void VPlug_GeoShapes::addPoint(const XYPoint& new_point)
{
  string new_label  = new_point.get_label();
  if(new_label == "") {
    m_points.push_back(new_point);
    return;
  }
  
  string new_type   = new_point.get_type();
  string new_source = new_point.get_source();
  for(unsigned int i=0; i<m_points.size(); i++) {
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


//-------------------------------------------------------------
// Procedure: getPolygon(int)

XYPolygon VPlug_GeoShapes::getPolygon(unsigned int index)
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

XYSegList VPlug_GeoShapes::getSegList(unsigned int index)
{
  if(index >= m_seglists.size()) {
    XYSegList null_segl;
    return(null_segl);
  }
  else
    return(m_seglists[index]);
}

//-------------------------------------------------------------
// Procedure: getGrid(int)

XYGrid VPlug_GeoShapes::getGrid(unsigned int index)
{
  if(index >= m_grids.size()) {
    XYGrid null_grid;
    return(null_grid);
  }
  else
    return(m_grids[index]);
}

//-------------------------------------------------------------
// Procedure: getCircle(int)

XYCircle VPlug_GeoShapes::getCircle(unsigned int index)
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

XYHexagon VPlug_GeoShapes::getHexagon(unsigned int index)
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

XYPoint VPlug_GeoShapes::getPoint(unsigned int index)
{
  if(index >= m_points.size()) {
    XYPoint null_point;
    return(null_point);
  }
  else
    return(m_points[index]);
}
