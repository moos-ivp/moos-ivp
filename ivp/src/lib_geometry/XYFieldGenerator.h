/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYFieldGenerator.h                                   */
/*    DATE: Jan 27th, 2012                                       */
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
  virtual ~XYFieldGenerator() {}

  bool addPolygon(std::string);
  bool addPolygon(XYPolygon);
  bool setSnap(double);
  void setBufferDist(double v)     {m_buffer_dist=v;}
  void setMaxTries(unsigned int v) {m_max_tries=v;}
  void setTargAmt(unsigned int v)  {m_targ_amt=v;}
  void setFlexBuffer(bool v=true)  {m_flex_buffer=v;}
  void setVerbose(bool v=true)     {m_verbose=v;}

  void clear();
  
  XYPoint generatePoint();
  bool    generatePoints(unsigned int amt=0);

  bool    addRandomPoint();
  bool    addAllRandomPoints(unsigned int amt);

  bool    addPoint(const XYPoint&);
  bool    addPoint(double vx, double vy);

  
  XYPoint getNewestPoint() const;
  
  std::vector<XYPoint> getPoints() {return(m_vpoints);}
  std::vector<double>  getNearestVals(bool force=false);
  double               getGlobalNearest(bool force=false);
    
  unsigned int polygonCount() {return(m_polygons.size());}
  unsigned int size()         {return(m_polygons.size());}
  XYPolygon    getPolygon(unsigned int);

 protected: // Config variables
  bool isPointTooClose(const XYPoint&);
  bool isPointTooClose(double vx, double vy);
  bool isPointInRegion(const XYPoint&);
  bool isPointInRegion(double vx, double vy);

  void updateGlobalNearestVals(bool force=false);
  
 protected: // Config variables
  std::vector<XYPolygon> m_polygons;

  unsigned int m_max_tries;
  unsigned int m_targ_amt;
  
  double   m_buffer_dist;
  double   m_snap;

  double   m_flex_buffer;

  bool     m_verbose;
  
 protected: // State variables
  double   m_poly_min_x;
  double   m_poly_min_y;
  double   m_poly_max_x;
  double   m_poly_max_y;

  std::vector<XYPoint> m_vpoints;
  std::vector<double>  m_nearest;
  double               m_global_nearest;
};

#endif 









