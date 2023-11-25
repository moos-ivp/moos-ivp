/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYPolyExpander.h                                     */
/*    DATE: Sep 7th, 2019                                        */
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
 
#ifndef XY_POLYGON_EXPANDER_HEADER
#define XY_POLYGON_EXPANDER_HEADER

#include <vector>
#include <string>
#include "XYPolygon.h"
#include "XYSegList.h"

class XYPolyExpander {
public:
  XYPolyExpander();
  ~XYPolyExpander() {}

public: // Config setting
  bool      setPoly(XYPolygon poly);
  void      setDegreeDelta(double);
  void      setVertexProximityThresh(double);
  void      disableSettling() {m_settling_enabled=false;}
 
public: // Primary function  
  XYPolygon getBufferPoly(double buff);

protected: // helper functions
  bool      expandSegments();
  bool      buildCorners();
  bool      buildNewPoly();
  void      clear();

private: // Config variables
  
  XYPolygon m_poly_orig;
  XYPolygon m_poly_buff;

  double    m_buff;
  double    m_deg_delta;
  double    m_vertex_proximity_thresh;

  bool      m_settling_enabled;
  
private: // State variables

  // Variables for holding the temporary edge vertices
  // Index aligns with indices of poly_orig
  std::vector<double> m_px1;
  std::vector<double> m_py1;
  std::vector<double> m_px2;
  std::vector<double> m_py2;
  
  // Variables for holding the temporary corner vertices
  // Outer Index aligns with indices of poly_orig
  std::vector<std::vector<double> > m_ipx;
  std::vector<std::vector<double> > m_ipy;
};

#endif


