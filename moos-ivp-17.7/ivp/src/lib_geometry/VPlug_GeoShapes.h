/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlug_GeoShapes.h                                    */
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

#ifndef VPLUG_GEOSHAPES_HEADER
#define VPLUG_GEOSHAPES_HEADER

#include <vector>
#include <map>
#include <string>
#include "XYPolygon.h"
#include "XYSegList.h"
#include "XYHexagon.h"
#include "XYGrid.h"
#include "XYConvexGrid.h"
#include "XYCircle.h"
#include "XYWedge.h"
#include "XYArc.h"
#include "XYPoint.h"
#include "XYVector.h"
#include "XYRangePulse.h"
#include "XYCommsPulse.h"
#include "XYSeglr.h"
#include "XYMarker.h"
#include "ColorPack.h"

class VPlug_GeoShapes {
public:
  VPlug_GeoShapes();
  virtual ~VPlug_GeoShapes() {}

  bool setParam(const std::string& param, std::string value);
  
  void    clear(std::string shape="", std::string stype="");

  double  getXMin() const {return(m_xmin);}
  double  getXMax() const {return(m_xmax);}
  double  getYMin() const {return(m_ymin);}
  double  getYMax() const {return(m_ymax);}

  void addPolygon(const XYPolygon&);
  void addSegList(const XYSegList&);
  void addSeglr(const XYSeglr&);
  void addCircle(const XYCircle&, unsigned int drawpts=18);
  void addWedge(const XYWedge&);
  void addHexagon(const XYHexagon&);
  void addPoint(const XYPoint&);
  void addArc(const XYArc&);
  void addVector(const XYVector&);
  void addGrid(const XYGrid&);
  void addConvexGrid(const XYConvexGrid&);
  void addRangePulse(const XYRangePulse&);
  void addCommsPulse(const XYCommsPulse&);
  void addMarker(const XYMarker&);

  bool addPolygon(const std::string&);
  bool addSegList(const std::string&);
  bool addSeglr(const std::string&);
  bool addCircle(const std::string&, unsigned int drawpts=18);
  bool addWedge(const std::string&, unsigned int drawpts=18);
  bool addPoint(const std::string&);
  bool addArc(const std::string&);
  bool addVector(const std::string&);
  bool addGrid(const std::string&);
  bool addConvexGrid(const std::string&);
  bool addRangePulse(const std::string&, double timestamp=0);
  bool addCommsPulse(const std::string&, double timestamp=0);
  bool addMarker(const std::string&);

  bool updateGrid(const std::string&);
  bool updateConvexGrid(const std::string&);

  unsigned int sizePolygons() const    {return(m_polygons.size());}
  unsigned int sizeSegLists() const    {return(m_seglists.size());}
  unsigned int sizeSeglrs() const      {return(m_seglrs.size());}
  unsigned int sizeCircles() const     {return(m_circles.size());}
  unsigned int sizeWedges() const      {return(m_wedges.size());}
  unsigned int sizeHexagons() const    {return(m_hexagons.size());}
  unsigned int sizePoints() const      {return(m_points.size());}
  unsigned int sizeArcs() const        {return(m_arcs.size());}
  unsigned int sizeVectors() const     {return(m_vectors.size());}
  unsigned int sizeGrids() const       {return(m_grids.size());}
  unsigned int sizeConvexGrids() const {return(m_convex_grids.size());}
  unsigned int sizeRangePulses() const {return(m_range_pulses.size());}
  unsigned int sizeCommsPulses() const {return(m_comms_pulses.size());}
  unsigned int sizeMarkers() const     {return(m_markers.size());}
  unsigned int sizeTotalShapes() const;

  std::vector<XYPolygon> getPolygons() const {return(m_polygons);}
  std::vector<XYWedge>   getWedges() const   {return(m_wedges);}
  std::vector<XYSegList> getSegLists() const {return(m_seglists);}
  std::vector<XYSeglr  > getSeglrs() const   {return(m_seglrs);}
  std::vector<XYArc> getArcs()         const {return(m_arcs);}
  std::vector<XYHexagon> getHexagons() const {return(m_hexagons);}
  std::vector<XYVector>  getVectors() const  {return(m_vectors);}
  std::vector<XYGrid>    getGrids() const    {return(m_grids);}
  std::vector<XYConvexGrid> getConvexGrids() const {return(m_convex_grids);}
  std::vector<XYRangePulse> getRangePulses() const {return(m_range_pulses);}
  std::vector<XYCommsPulse> getCommsPulses() const {return(m_comms_pulses);}

  const std::map<std::string, XYPoint>&  getPoints() const  {return(m_points);}
  const std::map<std::string, XYCircle>& getCircles() const {return(m_circles);}
  const std::map<std::string, XYMarker>& getMarkers() const {return(m_markers);}

  XYPolygon& poly(unsigned int i)   {return(m_polygons[i]);}
  XYSegList& segl(unsigned int i)   {return(m_seglists[i]);}
  XYSeglr&   seglr(unsigned int i)  {return(m_seglrs[i]);}

  XYPolygon    getPolygon(unsigned int) const;
  XYSegList    getSegList(unsigned int) const;
  XYSeglr      getSeglr(unsigned int) const;

 protected:
  void updateBounds(double xl, double xh, double yl, double yh);
  void updateBounds();

  void clearPolygons(std::string stype="");
  void clearWedges(std::string stype="");
  void clearArcs(std::string stype="");
  void clearSegLists(std::string stype="");
  void clearSeglrs(std::string stype="");
  void clearHexagons(std::string stype="");
  void clearGrids(std::string  stype="");
  void clearCircles(std::string stype="");
  void clearPoints(std::string  stype="");
  void clearVectors(std::string stype="");
  void clearPulses(std::string  stype="");
  void clearMarkers(std::string stype="");

  bool typeMatch(XYObject*, std::string stype);

protected:
  std::vector<XYPolygon>    m_polygons;
  std::vector<XYSegList>    m_seglists;
  std::vector<XYSeglr>      m_seglrs;
  std::vector<XYWedge>      m_wedges;
  std::vector<XYArc>        m_arcs;
  std::vector<XYHexagon>    m_hexagons;
  std::vector<XYGrid>       m_grids;
  std::vector<XYConvexGrid> m_convex_grids;
  std::vector<XYVector>     m_vectors;
  std::vector<XYRangePulse> m_range_pulses;
  std::vector<XYCommsPulse> m_comms_pulses;

  // map from label to object (map more efficient than vector)
  std::map<std::string, XYPoint>  m_points;
  std::map<std::string, XYMarker> m_markers;
  std::map<std::string, XYCircle> m_circles;

  double  m_xmin;
  double  m_xmax;
  double  m_ymin;
  double  m_ymax;
};

#endif




