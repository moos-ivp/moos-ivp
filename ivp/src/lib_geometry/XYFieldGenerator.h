/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: XYFieldGenerator.h                                   */
/*   DATE: Jan 27th, 2012                                       */
/****************************************************************/

#ifndef XY_FIELD_GENERATOR_HEADER
#define XY_FIELD_GENERATOR_HEADER

#include <vector>
#include <string>
#include "XYPolygon.h"
#include "XYFormatUtilsPoly.h"

class XYFieldGenerator
{
 public:
  XYFieldGenerator();
  virtual ~XYFieldGenerator() {};

  bool addPolygon(std::string);

  XYPoint generatePoint();

 protected: // Config variables
  std::vector<XYPolygon>    m_polygons;

  double   m_snap;

 protected: // State variables
  double   m_poly_min_x;
  double   m_poly_min_y;
  double   m_poly_max_x;
  double   m_poly_max_y;
};

#endif 
