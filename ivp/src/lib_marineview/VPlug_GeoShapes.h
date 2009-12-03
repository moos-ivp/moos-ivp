/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VPlug_GeoShapes.h                                    */
/*    DATE: July 9th, 2008                                       */
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
#include "ColorPack.h"

class VPlug_GeoShapes {
public:
  VPlug_GeoShapes() {};
  virtual ~VPlug_GeoShapes() {};

  bool setParam(const std::string& param, std::string value);
  
  void addPolygon(const XYPolygon&);
  void addSegList(const XYSegList&);
  void addGrid(const XYGrid&);
  void addCircle(const XYCircle&);
  void addHexagon(const XYHexagon&);
  void addPoint(const XYPoint&);

  bool addPolygon(const std::string&);
  bool addSegList(const std::string&);
  bool addPoint(const std::string&);
  bool addCircle(const std::string&);
  bool addGrid(const std::string&);
  bool updateGrid(const std::string&);

  unsigned int sizePolygons() {return(m_polygons.size());};
  unsigned int sizeSegLists() {return(m_seglists.size());};
  unsigned int sizeGrids()    {return(m_grids.size());};
  unsigned int sizeCircles()  {return(m_circles.size());};
  unsigned int sizeHexagons() {return(m_hexagons.size());};
  unsigned int sizePoints()   {return(m_points.size());};

  XYPolygon& poly(unsigned int i)  {return(m_polygons[i]);};
  XYSegList& segl(unsigned int i)  {return(m_seglists[i]);};
  XYHexagon& hexa(unsigned int i)  {return(m_hexagons[i]);};
  XYGrid&    grid(unsigned int i)  {return(m_grids[i]);};
  XYCircle&  circ(unsigned int i)  {return(m_circles[i]);};
  XYPoint&   point(unsigned int i) {return(m_points[i]);};

  std::vector<XYPolygon> getPolygons() {return(m_polygons);};
  std::vector<XYSegList> getSegLists() {return(m_seglists);};
  std::vector<XYHexagon> getHexagons() {return(m_hexagons);};
  std::vector<XYGrid>    getGrids()    {return(m_grids);};
  std::vector<XYCircle>  getCircles()  {return(m_circles);};
  std::vector<XYPoint>   getPoints()   {return(m_points);};

  XYPolygon getPolygon(unsigned int);
  XYSegList getSegList(unsigned int);
  XYGrid    getGrid(unsigned int);
  XYCircle  getCircle(unsigned int);
  XYHexagon getHexagon(unsigned int);
  XYPoint   getPoint(unsigned int);

protected:
  std::vector<XYPolygon>  m_polygons;
  std::vector<XYSegList>  m_seglists;
  std::vector<XYHexagon>  m_hexagons;
  std::vector<XYGrid>     m_grids;
  std::vector<XYCircle>   m_circles;
  std::vector<XYPoint>    m_points;
};

#endif
