/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYHexagon.h                                          */
/*    DATE: Oct 30, 2005                                         */
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
 
#ifndef XY_HEXAGON_HEADER
#define XY_HEXAGON_HEADER

#include "XYPolygon.h"

class XYHexagon : public XYPolygon {
public:
  XYHexagon() {m_cx=0; m_cy=0; m_cz=0; m_dist=0;}
  ~XYHexagon() {}

  // The only two ways to build a pentagon
  bool   initialize(double, double, double);
  bool   initialize(std::string);       

  double get_cx()   {return(m_cx);}
  double get_cy()   {return(m_cy);}
  double get_cz()   {return(m_cz);}
  double get_dist() {return(m_dist);}

  XYHexagon addNeighbor(int);

  // We disable these operations allowable on the superclass
  bool   add_vertex(double, double) {return(false);}   
  bool   alter_vertex(double, double) {return(false);}
  bool   delete_vertex(double, double) {return(false);}
  bool   insert_vertex(double, double) {return(false);}

protected:
  double m_cx;
  double m_cy;
  double m_cz;
  double m_dist;
};

#endif



















