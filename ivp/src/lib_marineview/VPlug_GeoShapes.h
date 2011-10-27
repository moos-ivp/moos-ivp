/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlug_GeoShapes.h                                    */
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

#ifndef VPLUG_GEOSHAPES_HEADER
#define VPLUG_GEOSHAPES_HEADER

#include <vector>
#include <map>
#include <string>
#include "XYPolygon.h"
#include "XYSegList.h"
#include "XYHexagon.h"
#include "XYGrid.h"
#include "XYCircle.h"
#include "XYPoint.h"
#include "XYVector.h"
#include "XYRangePulse.h"
#include "XYMarker.h"
#include "ColorPack.h"

class VPlug_GeoShapes {
public:
  VPlug_GeoShapes();
  virtual ~VPlug_GeoShapes() {};

  bool setParam(const std::string& param, std::string value);
  
  void    clear();
  double  getXMin() const {return(m_xmin);};
  double  getXMax() const {return(m_xmax);};
  double  getYMin() const {return(m_ymin);};
  double  getYMax() const {return(m_ymax);};

  void addPolygon(const XYPolygon&);
  void addSegList(const XYSegList&);
  void addGrid(const XYGrid&);
  void addCircle(const XYCircle&);
  void addHexagon(const XYHexagon&);
  void addPoint(const XYPoint&);
  void addVector(const XYVector&);
  void addRangePulse(const XYRangePulse&);
  void addMarker(const XYMarker&);

  bool addPolygon(const std::string&);
  bool addSegList(const std::string&);
  bool addGrid(const std::string&);
  bool addCircle(const std::string&);
  bool addPoint(const std::string&);
  bool addVector(const std::string&);
  bool addRangePulse(const std::string&);
  bool addMarker(const std::string&);

  bool updateGrid(const std::string&);

  unsigned int sizePolygons() const {return(m_polygons.size());};
  unsigned int sizeSegLists() const {return(m_seglists.size());};
  unsigned int sizeGrids() const    {return(m_grids.size());};
  unsigned int sizeCircles() const  {return(m_circles.size());};
  unsigned int sizeHexagons() const {return(m_hexagons.size());};
  unsigned int sizePoints() const   {return(m_points.size());};
  unsigned int sizeVectors() const  {return(m_vectors.size());};
  unsigned int sizeRangePulses() const {return(m_range_pulses.size());};
  unsigned int sizeMarkers() const {return(m_markers.size());};

  XYPolygon& poly(unsigned int i)   {return(m_polygons[i]);};
  XYSegList& segl(unsigned int i)   {return(m_seglists[i]);};
  XYHexagon& hexa(unsigned int i)   {return(m_hexagons[i]);};
  XYGrid&    grid(unsigned int i)   {return(m_grids[i]);};
  XYCircle&  circ(unsigned int i)   {return(m_circles[i]);};
  XYPoint&   point(unsigned int i)  {return(m_points[i]);};
  XYVector&  vector(unsigned int i) {return(m_vectors[i]);};
  XYRangePulse& range_pulse(unsigned int i) {return(m_range_pulses[i]);};
  XYMarker&  marker(unsigned int i) {return(m_markers[i]);};

  std::vector<XYPolygon> getPolygons() const {return(m_polygons);};
  std::vector<XYSegList> getSegLists() const {return(m_seglists);};
  std::vector<XYHexagon> getHexagons() const {return(m_hexagons);};
  std::vector<XYGrid>    getGrids() const    {return(m_grids);};
  std::vector<XYCircle>  getCircles() const  {return(m_circles);};
  std::vector<XYPoint>   getPoints() const   {return(m_points);};
  std::vector<XYVector>  getVectors() const  {return(m_vectors);};
  std::vector<XYRangePulse> getRangePulses() const {return(m_range_pulses);};
  std::vector<XYMarker>  getMarkers() const  {return(m_markers);};

  XYPolygon    getPolygon(unsigned int) const;
  XYSegList    getSegList(unsigned int) const;
  XYGrid       getGrid(unsigned int) const;
  XYCircle     getCircle(unsigned int) const;
  XYHexagon    getHexagon(unsigned int) const;
  XYPoint      getPoint(unsigned int) const;
  XYVector     getVector(unsigned int) const;
  XYRangePulse getRangePulse(unsigned int) const;
  XYMarker     getMarker(unsigned int) const;

 protected:
  void updateBounds(double xl, double xh, double yl, double yh);

protected:
  std::vector<XYPolygon>    m_polygons;
  std::vector<XYSegList>    m_seglists;
  std::vector<XYHexagon>    m_hexagons;
  std::vector<XYGrid>       m_grids;
  std::vector<XYCircle>     m_circles;
  std::vector<XYPoint>      m_points;
  std::vector<XYVector>     m_vectors;
  std::vector<XYRangePulse> m_range_pulses;
  std::vector<XYMarker>     m_markers;

  double  m_xmin;
  double  m_xmax;
  double  m_ymin;
  double  m_ymax;
};

#endif

