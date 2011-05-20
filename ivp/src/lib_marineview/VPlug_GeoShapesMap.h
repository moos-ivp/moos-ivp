/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: VPlug_GeoShapes.h                                    */
/*    DATE: May 19th, 2011                                       */
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
  virtual ~VPlug_GeoShapesMap() {};

  void   clear(const std::string& vname="");

  bool   addGeoShape(const std::string& param, 
		     const std::string& value, 
		     const std::string& community);
  
  double getXMin() const {return(m_xmin);};
  double getXMax() const {return(m_xmax);};
  double getYMin() const {return(m_ymin);};
  double getYMax() const {return(m_ymax);};

  unsigned int sizeVehicles() const {return(m_geoshapes_map.size());};
  unsigned int sizePolygons() const {return(size("polygons"));};
  unsigned int sizeSegLists() const {return(size("seglists"));};
  unsigned int sizeGrids() const    {return(size("grids"));};
  unsigned int sizeCircles() const  {return(size("circles"));}
  unsigned int sizeHexagons() const {return(size("hexagons"));};
  unsigned int sizePoints() const   {return(size("points"));};
  unsigned int sizeVectors() const  {return(size("vectors"));};
  unsigned int sizeRangePulses() const {return(size("range_pulses"));};
  unsigned int sizeMarkers() const  {return(size("markers"));};

  std::vector<XYPolygon> getPolygons(const std::string&);
  std::vector<XYSegList> getSegLists(const std::string&);
  std::vector<XYHexagon> getHexagons(const std::string&);
  std::vector<XYGrid>    getGrids(const std::string&);
  std::vector<XYCircle>  getCircles(const std::string&);
  std::vector<XYPoint>   getPoints(const std::string&);
  std::vector<XYVector>  getVectors(const std::string&);
  std::vector<XYRangePulse> getRangePulses(const std::string&);
  std::vector<XYMarker>  getMarkers(const std::string&);

  std::vector<std::string> getVehiNames() const {return(m_vnames);};
  
 protected:
  unsigned int size(const std::string&) const;

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
