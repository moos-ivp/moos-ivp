/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYHexagon.cpp                                        */
/*    DATE: Oct 30th, 2005                                       */
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

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include "XYHexagon.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: initialize

bool XYHexagon::initialize(string str)
{
  string mstr = stripBlankEnds(str);
  vector<string> svector = parseString(mstr, ',');
  int vsize = svector.size();
  if(vsize != 3)
    return(false);
  for(int i=0; i<3; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    if(!isNumber(svector[i]))
      return(false);
  }
  
  double m_cx = atof(svector[0].c_str());
  double m_cy = atof(svector[1].c_str());
  double m_dist = atof(svector[2].c_str());
  
  return(initialize(m_cx, m_cy, m_dist));
}

//---------------------------------------------------------------
// Procedure: initialize
//
//
//                     (5)         (0)                         |
//                       o---------o                           |
//                     /             \                         |
//                    /               \                        |
//                   /                 \                       |
//               (4)o       cx,cy       o (1)                  |
//                   \                 /                       |
//                    \               /                        |
//                     \             /                         |
//                       o---------o                           |
//                     (3)         (2)                         |
//                                                             |
//                                                         
//                                                          

bool XYHexagon::initialize(double gcx, double gcy, double gdist)
{
  this->clear();
    
  if(gdist <= 0)
    return(false);

  m_cx   = gcx;
  m_cy   = gcy;
  m_dist = gdist;

  double x0, x1, x2, x3, x4, x5;
  double y0, y1, y2, y3, y4, y5;

  x0 = m_cx + (m_dist/2);    y0 = m_cy + m_dist;
  x1 = m_cx + m_dist;        y1 = m_cy;
  x2 = m_cx + (m_dist/2);    y2 = m_cy - m_dist;
  x3 = m_cx - (m_dist/2);    y3 = m_cy - m_dist;
  x4 = m_cx - m_dist;        y4 = m_cy;
  x5 = m_cx - (m_dist/2);    y5 = m_cy + m_dist;

  XYPolygon::add_vertex(x0, y0);
  XYPolygon::add_vertex(x1, y1);
  XYPolygon::add_vertex(x2, y2);
  XYPolygon::add_vertex(x3, y3);
  XYPolygon::add_vertex(x4, y4);
  XYPolygon::add_vertex(x5, y5);

  return(true);
}

//---------------------------------------------------------------
// Procedure: addNeighbor
//
//
//                           (0)                               |
//                       o---------o                           |
//                     /             \                         |
//                (5) /               \ (1)                    |
//                   /                 \                       |
//                  o       cx,cy       o                      |
//                   \                 /                       |
//                (4) \               / (2)                    |
//                     \             /                         |
//                       o---------o                           |
//                           (3)                               |
//                                                           

XYHexagon XYHexagon::addNeighbor(int nix)
{
  assert(vertex_x.size() == 6);
  assert(nix >= 0);
  assert(nix <= 5);

  XYHexagon newhex;

  if(nix == 0)
    newhex.initialize(m_cx, m_cy+(2*m_dist), m_dist);
  else if(nix == 1)
    newhex.initialize(m_cx+(1.5*m_dist), m_cy+m_dist, m_dist);
  else if(nix == 2)
    newhex.initialize(m_cx+(1.5*m_dist), m_cy-m_dist, m_dist);
  else if(nix == 3)
    newhex.initialize(m_cx, m_cy-(2*m_dist), m_dist);
  else if(nix == 4)
    newhex.initialize(m_cx-(1.5*m_dist), m_cy-m_dist, m_dist);
  else if(nix == 5)
    newhex.initialize(m_cx-(1.5*m_dist), m_cy+m_dist, m_dist);

  return(newhex);
}









