/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VPlug-GeoShapes.h                                    */
/*    DATE: July 9th, 2008                                       */
/*****************************************************************/

#ifndef VPLUG_GEOSHAPES_HEADER
#define VPLUG_GEOSHAPES_HEADER

#include <vector>
#include <string>
#include "XYPolygon.h"
#include "XYSegList.h"
#include "XYGrid.h"
#include "XYCircle.h"

class VPlug_GeoShapes {
public:
  VPlug_GeoShapes();
  virtual ~VPlug_GeoShapes() {};

  void addPolygon(const XYPolygon&);
  void addSegList(const XYSegList&);
  void addGrid(const XYGrid&);
  void addCircle(const XYCircle&);

  bool addPolygon(const std::string&);
  bool addSegList(const std::string&);

  bool setParam(std::string param, std::string value);
  bool setParam(std::string param, double);
  
  unsigned int size(const std::string& s="all");

  void remove(const std::string& gtype, int index);

  XYPolygon getPolygon(int);
  XYSegList getSegList(int);
  XYGrid    getGrid(int);
  XYCircle  getCircle(int);

  bool   setColorMapping(std::string);
  bool   setColorMapping(std::string, std::string);
  std::vector<double> getColorMapping(std::string, std::string s="");

  bool   setViewableMapping(std::string, std::string);
  bool   getViewableMapping(std::string, bool default=true);

  bool   setGSizeMapping(std::string, std::string);
  double getGSizeMapping(std::string, double default=0);

protected:
  std::vector<XYPolygon>  m_polygons;
  std::vector<XYSegList>  m_seglists;
  std::vector<XYGrid>     m_grids;
  std::vector<XYCircle>   m_circles;
  
  std::map<std::string, std::vector<double> >  m_color_map;
  std::map<std::string, bool>                  m_viewable_map;
  std::map<std::string, double>                m_gsize_map;
};

#endif
