/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlug_GeoShapes.h                                    */
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

#ifndef VPLUG_GEO_SHAPES_MAP_HEADER
#define VPLUG_GEO_SHAPES_MAP_HEADER

#include <vector>
#include <map>
#include <string>
#include "VPlug_GeoShapes.h"

class VPlug_GeoShapesMap {
public:
  VPlug_GeoShapesMap();
  virtual ~VPlug_GeoShapesMap() {}

  void   clear(const std::string& vname="");
  void   clear(std::string vname, std::string shape, std::string stype);

  bool   addGeoShape(const std::string& param, 
		     const std::string& value, 
		     const std::string& community, 
		     double time=0);

  void   manageMemory(double curr_time);
  
  double getXMin() const {return(m_xmin);}
  double getXMax() const {return(m_xmax);}
  double getYMin() const {return(m_ymin);}
  double getYMax() const {return(m_ymax);}

  unsigned int sizeVehicles() const    {return(m_geoshapes_map.size());}
  unsigned int sizePolygons() const    {return(size("polygons"));}
  unsigned int sizeWedges() const      {return(size("wedges"));}
  unsigned int sizeSegLists() const    {return(size("seglists"));}
  unsigned int sizeSeglrs() const      {return(size("seglrs"));}
  unsigned int sizeCircles() const     {return(size("circles"));}
  unsigned int sizeHexagons() const    {return(size("hexagons"));}
  unsigned int sizePoints() const      {return(size("points"));}
  unsigned int sizeVectors() const     {return(size("vectors"));}
  unsigned int sizeGrids() const       {return(size("grids"));}
  unsigned int sizeConvexGrids() const {return(size("convex_grids"));}
  unsigned int sizeRangePulses() const {return(size("range_pulses"));}
  unsigned int sizeCommsPulses() const {return(size("comms_pulses"));}
  unsigned int sizeMarkers() const     {return(size("markers"));}
  unsigned int sizeTotalShapes() const {return(size("total_shapes"));}

  std::vector<XYPolygon> getPolygons(const std::string&);
  std::vector<XYWedge>   getWedges(const std::string&);
  std::vector<XYSegList> getSegLists(const std::string&);
  std::vector<XYSeglr>   getSeglrs(const std::string&);
  std::vector<XYHexagon> getHexagons(const std::string&);

  const std::map<std::string, XYCircle>& getCircles(const std::string&);
  const std::map<std::string, XYMarker>& getMarkers(const std::string&);
  const std::map<std::string, XYPoint>&   getPoints(const std::string&);

  std::vector<XYVector>     getVectors(const std::string&);
  std::vector<XYGrid>       getGrids(const std::string&);
  std::vector<XYConvexGrid> getConvexGrids(const std::string&);
  std::vector<XYRangePulse> getRangePulses(const std::string&);
  std::vector<XYCommsPulse> getCommsPulses(const std::string&);

  std::vector<std::string> getVehiNames() const {return(m_vnames);}

  unsigned int size(const std::string&, const std::string& vname="") const;
  
 protected:

  void  refreshVehiNames();
  void  updateBounds(const VPlug_GeoShapes&);

protected:
  std::map<std::string, VPlug_GeoShapes> m_geoshapes_map;

  std::vector<std::string> m_vnames;

  double  m_xmin;
  double  m_xmax;
  double  m_ymin;
  double  m_ymax;
};

#endif









