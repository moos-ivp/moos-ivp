/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / MIT Cambridge MA            */
/*    FILE: FldProjector.cpp                                     */
/*    DATE: Nov 19th, 2023                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "FldProjector.h"

using namespace std;

//--------------------------------------------------------
// Constructor()

FldProjector::FldProjector(double rx, double ry, double ang)
{
  m_root_x  = rx;
  m_root_y  = ry;
  m_angle   = ang;
  m_grid    = 10;
  m_cells_x = 26;
  m_cells_y = 26;
  m_offset  = 20;
}

//--------------------------------------------------------
// Procedure: buildProjection()

void FldProjector::buildProjection()
{
  // Part 1: Find the AA corner based on the root and the
  // offset.
  // 
  //                          o (root)           
  //                          |                  
  //   (aax,aay)     offset < |                  
  //   AA                     | (mx,my)                AZ
  //   o----------------------o------------------------o
  //   |                                               |
  //   |                                               |
  //   o----------------------o------------------------o
  //   ZA                                              ZZ
  
  double mx,my;
  projectPoint(m_angle+90, m_offset, m_root_x, m_root_y, mx, my);

  double half_fld_dist = ((m_cells_x-1)/2) * m_grid;
  
  double aax,aay;
  projectPoint(m_angle+180, half_fld_dist, mx, my, aax, aay);
  
  char row = 'A';
  char col = 'A';
  for(unsigned int i=0; i<m_cells_x; i++) {
    double bx,by;
    double idist = m_grid * i;
    double ang = angle360(m_angle + 90);
    projectPoint(ang, idist, aax, aay, bx, by);

    for(unsigned int j=0; j<m_cells_y; j++) {

      double px,py;
      double jdist = m_grid * j;
      projectPoint(m_angle, jdist, bx, by, px, py);

      XYPoint pt(px,py);
      
      char row_i = row + i;
      char col_j = col + j;
      string key = "AA";
      key[0] = row_i;
      key[1] = col_j;

      m_map_projections[key] = pt;      
    }
  }
}

//--------------------------------------------------------
// Procedure: print()

void FldProjector::print()
{
  // Generate a header line for readability/reproducability
  cout << "# $ projfield";
  cout << " --x=" << doubleToStringX(m_root_x,2);
  cout << " --y=" << doubleToStringX(m_root_y,2);
  cout << " --ang=" << doubleToStringX(m_angle,2);
  cout << " --grid=" << doubleToStringX(m_grid,2);
  cout << " --offset=" << doubleToStringX(m_offset,2);
  cout << " --cx=" << doubleToStringX(m_cells_x,2);
  cout << " --cy=" << doubleToStringX(m_cells_y,2) << endl;

  map<string, XYPoint>::iterator p;
  for(p=m_map_projections.begin(); p!=m_map_projections.end(); p++) {
    string key = p->first;
    XYPoint pt = p->second;
    double x = pt.x();
    double y = pt.y();
    string xstr = doubleToStringX(x,2);
    string ystr = doubleToStringX(y,2);

    cout << key << "=x=" << xstr << ",y=" << ystr << endl;
    cout << key << "T=" << xstr << "," << ystr << endl;
  }
}
