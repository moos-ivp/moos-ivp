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

class VPlug_GeoShapes {
public:
  VPlug_GeoShapes();
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
  bool addGrid(const std::string&);
  bool updateGrid(const std::string&);

  int sizePolygons() {return(m_polygons.size());};
  int sizeSegLists() {return(m_seglists.size());};
  int sizeGrids()    {return(m_grids.size());};
  int sizeCircles()  {return(m_circles.size());};
  int sizeHexagons() {return(m_hexagons.size());};
  int sizePoints()   {return(m_points.size());};

  void remove(const std::string& gtype, int index);

  XYPolygon& poly(int i)  {return(m_polygons[i]);};
  XYSegList& segl(int i)  {return(m_seglists[i]);};
  XYHexagon& hexa(int i)  {return(m_hexagons[i]);};
  XYGrid&    grid(int i)  {return(m_grids[i]);};
  XYCircle&  circ(int i)  {return(m_circles[i]);};
  XYPoint&   point(int i) {return(m_points[i]);};

  XYPolygon getPolygon(int);
  XYSegList getSegList(int);
  XYGrid    getGrid(int);
  XYCircle  getCircle(int);
  XYHexagon getHexagon(int);
  XYPoint   getPoint(int);

  bool   setColorMapping(std::string);
  bool   setColorMapping(std::string, std::string);
  std::vector<double> geocolor(const std::string&, 
			       std::string color_default="");

  bool   setViewableMapping(std::string, std::string);
  bool   viewable(const std::string&, bool view_default=true);
  
  bool   setGSizeMapping(std::string, std::string);
  double geosize(const std::string&, double size_default=0);

  std::vector<std::string> getParamReport() const;

protected:
  std::vector<XYPolygon>  m_polygons;
  std::vector<XYSegList>  m_seglists;
  std::vector<XYHexagon>  m_hexagons;
  std::vector<XYGrid>     m_grids;
  std::vector<XYCircle>   m_circles;
  std::vector<XYPoint>    m_points;
  
  std::map<std::string, std::vector<double> >  m_color_map;
  std::map<std::string, bool>                  m_viewable_map;
  std::map<std::string, double>                m_gsize_map;

  // The below is strictly for parameter reporting and not used
  // in drawing. Should be identical to the m_color_map except
  // in form (string vs. vector<double>) only.
  std::map<std::string, std::string>           m_color_string_map;
};

#endif
