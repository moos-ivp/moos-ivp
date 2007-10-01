/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GeoHandler.h                                         */
/*    DATE: September 30th, 2007                                 */
/*****************************************************************/

#ifndef GEO_HANDLER_HEADER
#define GEO_HANDLER_HEADER

#include <vector>
#include <string>
#include "MOOSGeodesy.h"


class GeoHandler
{
 public:
  GeoHandler();
  
  bool  setDatum(std::string, std::string);
  bool  setDatum(double, double);

  void  setLat(int, double);
  void  setLon(int, double);
  void  setMetersX(int, double);
  void  setMetersY(int, double);
  
  void  query();

 protected:
  void  queryPosLocal2LatLong();
  void  queryPosLatLong2Local();
  void  queryDistLatLongLatLong();

private:
  double m_lat1;
  double m_lon1;
  double m_lat2;
  double m_lon2;

  bool   m_lat1_set;
  bool   m_lon1_set;
  bool   m_lat2_set;
  bool   m_lon2_set;

  double m_x1;
  double m_y1;
  double m_x2;
  double m_y2;

  bool   m_x1_set;
  bool   m_y1_set;
  bool   m_x2_set;
  bool   m_y2_set;

  double m_datum_lat;
  double m_datum_lon;
  bool   m_datum_set;

  CMOOSGeodesy m_geodesy;
};

#endif 

