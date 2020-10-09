/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlug_GeoShapes.cpp                                  */
/*    DATE: May 19th, 2011                                       */
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
#include "VPlug_GeoShapesMap.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

VPlug_GeoShapesMap::VPlug_GeoShapesMap()
{
  m_xmin = 0;
  m_xmax = 0;
  m_ymin = 0;
  m_ymax = 0;
}

//----------------------------------------------------------------
// Procedure: clear()

void VPlug_GeoShapesMap::clear(const string& vname)
{
  if((vname == "") || (vname == "all")) {
    m_xmin = 0;
    m_xmax = 0;
    m_ymin = 0;
    m_ymax = 0;
    m_geoshapes_map.clear();
    m_vnames.clear();
  }
  else
    m_geoshapes_map[vname].clear();

}

//----------------------------------------------------------------
// Procedure: clear()

void VPlug_GeoShapesMap::clear(string vname, string shape, string stype)
{
  if((shape == "") && (stype == ""))
    clear(vname);

  if(vname != "")
    m_geoshapes_map[vname].clear(shape, stype);
  else {
    map<string, VPlug_GeoShapes>::iterator p;
    for(p=m_geoshapes_map.begin(); p!=m_geoshapes_map.end(); p++) {
      p->second.clear(shape, stype);
    }
  }
}

//----------------------------------------------------------------
// Procedure: addGeoShape()

bool VPlug_GeoShapesMap::addGeoShape(const string& param_orig, 
				     const string& value, 
				     const string& vname,
				     double timestamp)
{
  bool handled = false;
  unsigned int starting_map_size = m_geoshapes_map.size();

  string param = toupper(param_orig);

  if(param == "VIEW_POINT")
    handled = m_geoshapes_map[vname].addPoint(value, timestamp);
  else if(param == "VIEW_POLYGON")
    handled = m_geoshapes_map[vname].addPolygon(value, timestamp);
  else if(param == "VIEW_SEGLIST")
    handled = m_geoshapes_map[vname].addSegList(value, timestamp);
  else if(param == "VIEW_SEGLR")
    handled = m_geoshapes_map[vname].addSeglr(value);
  else if(param == "VIEW_WEDGE")
    handled = m_geoshapes_map[vname].addWedge(value);
  else if(param == "VIEW_VECTOR")
    handled = m_geoshapes_map[vname].addVector(value);
  else if(param == "VIEW_CIRCLE")
    handled = m_geoshapes_map[vname].addCircle(value, 18, timestamp);
  else if(param == "VIEW_RANGE_PULSE")
    handled = m_geoshapes_map[vname].addRangePulse(value, timestamp);
  else if(param == "VIEW_COMMS_PULSE")
    handled = m_geoshapes_map[vname].addCommsPulse(value, timestamp);
  else if((param == "VIEW_MARKER") || (param == "MARKER")) 
    handled = m_geoshapes_map[vname].addMarker(value, timestamp);
  else if(param == "GRID_CONFIG")
    handled = m_geoshapes_map[vname].addGrid(value);
  else if(param == "GRID_DELTA")
    handled = m_geoshapes_map[vname].updateGrid(value);
  else if(param == "VIEW_GRID")
    handled = m_geoshapes_map[vname].addConvexGrid(value);

  //if(handled)
  //  updateBounds(m_geoshapes_map[vname]);

  unsigned int new_map_size = m_geoshapes_map.size();
  if(new_map_size > starting_map_size)
    refreshVehiNames();

  return(handled);
}

//----------------------------------------------------------------
// Procedure: manageMemory()

void VPlug_GeoShapesMap::manageMemory(double curr_time)
{
  map<string, VPlug_GeoShapes>::iterator p;
  for(p=m_geoshapes_map.begin(); p!=m_geoshapes_map.end(); p++)
    p->second.manageMemory(curr_time);
}


//----------------------------------------------------------------
// Procedure: getPolygons
// Procedure: getSegLists
// Procedure: getHexagons
// Procedure: getGrids
// Procedure: getCircles
// Procedure: getPoints
// Procedure: getVectors
// Procedure: getRangePulses
// Procedure: getCommsPulses
// Procedure: getMarkers

vector<XYPolygon> VPlug_GeoShapesMap::getPolygons(const string& vname)
{
  return(m_geoshapes_map[vname].getPolygons());
}
vector<XYSegList> VPlug_GeoShapesMap::getSegLists(const string& vname)
{
  return(m_geoshapes_map[vname].getSegLists());
}
vector<XYSeglr> VPlug_GeoShapesMap::getSeglrs(const string& vname)
{
  return(m_geoshapes_map[vname].getSeglrs());
}
vector<XYWedge> VPlug_GeoShapesMap::getWedges(const string& vname)
{
  return(m_geoshapes_map[vname].getWedges());
}
vector<XYHexagon> VPlug_GeoShapesMap::getHexagons(const string& vname)
{
  return(m_geoshapes_map[vname].getHexagons());
}
vector<XYGrid> VPlug_GeoShapesMap::getGrids(const string& vname)
{
  return(m_geoshapes_map[vname].getGrids());
}
vector<XYConvexGrid> VPlug_GeoShapesMap::getConvexGrids(const string& vname)
{
  return(m_geoshapes_map[vname].getConvexGrids());
}
const map<string, XYCircle>& VPlug_GeoShapesMap::getCircles(const string& vname)
{
  return(m_geoshapes_map[vname].getCircles());
}
const map<string, XYPoint>& VPlug_GeoShapesMap::getPoints(const string& vname)
{
  return(m_geoshapes_map[vname].getPoints());
}
vector<XYVector> VPlug_GeoShapesMap::getVectors(const string& vname)
{
  return(m_geoshapes_map[vname].getVectors());
}
vector<XYRangePulse> VPlug_GeoShapesMap::getRangePulses(const string& vname)
{
  return(m_geoshapes_map[vname].getRangePulses());
}
vector<XYCommsPulse> VPlug_GeoShapesMap::getCommsPulses(const string& vname)
{
  return(m_geoshapes_map[vname].getCommsPulses());
}
const map<string, XYMarker>& VPlug_GeoShapesMap::getMarkers(const string& vname)
{
  return(m_geoshapes_map[vname].getMarkers());
}


//----------------------------------------------------------------
// Procedure: size

unsigned int VPlug_GeoShapesMap::size(const string& gtype,
				      const string& vname) const
{
  unsigned int return_size = 0;
  map<string, VPlug_GeoShapes>::const_iterator p;
  for(p=m_geoshapes_map.begin(); p!= m_geoshapes_map.end(); p++) {
    if((vname == "") || (p->first == vname)) {
      if(gtype == "markers")
	return_size += p->second.sizeMarkers();
      else if(gtype == "range_pulses")
	return_size += p->second.sizeRangePulses();
      else if(gtype == "comms_pulses")
	return_size += p->second.sizeCommsPulses();
      else if(gtype == "points")
	return_size += p->second.sizePolygons();
      else if(gtype == "polygons")
	return_size += p->second.sizePolygons();
      else if(gtype == "wedges")
	return_size += p->second.sizeWedges();
      else if(gtype == "seglists")
	return_size += p->second.sizeSegLists();
      else if(gtype == "seglrs")
	return_size += p->second.sizeSeglrs();
      else if(gtype == "total_shapes")
	return_size += p->second.sizeTotalShapes();
      else if(gtype == "vectors")
	return_size += p->second.sizeVectors();
      else if(gtype == "circles")
	return_size += p->second.sizeCircles();
      else if(gtype == "grids")
      return_size += p->second.sizeGrids();
      else if(gtype == "convex_grids")
	return_size += p->second.sizeConvexGrids();
      else if(gtype == "hexagons")
	return_size += p->second.sizeHexagons();
    }  
  }
  return(return_size);
}

//----------------------------------------------------------------
// Procedure: refreshVehiNames

void VPlug_GeoShapesMap::refreshVehiNames()
{
  m_vnames.clear();
  map<string, VPlug_GeoShapes>::const_iterator p;
  for(p=m_geoshapes_map.begin(); p!= m_geoshapes_map.end(); p++)
    m_vnames.push_back(p->first);
}

//----------------------------------------------------------------
// Procedure: updateBounds()

void VPlug_GeoShapesMap::updateBounds(const VPlug_GeoShapes& vplug)
{
  if(vplug.getXMin() < m_xmin)
    m_xmin = vplug.getXMin();
  if(vplug.getXMax() > m_xmax)
    m_xmax = vplug.getXMax();

  if(vplug.getYMin() < m_ymin)
    m_ymin = vplug.getYMin();
  if(vplug.getYMax() > m_ymax)
    m_ymax = vplug.getYMax();
}










