/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WormHole.cpp                                         */
/*    DATE: Jan 21st 2021                                        */
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

#include <cmath>
#include <iostream>
#include "WormHole.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

WormHole::WormHole(string tag)
{
  m_tag   = tag;
  m_delay = 0;

  m_id_change  = true;
  m_connection = "from_madrid";
}

//----------------------------------------------------------------
// Procedure: setMadridPoly()

bool WormHole::setMadridPoly(XYPolygon poly)
{ 
  if(!poly.is_convex())
    return(false);

  m_madrid_poly = poly;
  return(true);
}

//----------------------------------------------------------------
// Procedure: setWeberPoly()

bool WormHole::setWeberPoly(XYPolygon poly)
{ 
  if(!poly.is_convex())
    return(false);

  m_weber_poly = poly;
  return(true);
}

//----------------------------------------------------------------
// Procedure: setConnectionType()

void WormHole::setConnectionType(string str)
{ 
  if((str != "both") && (str != "from_madrid") && (str != "from_weber"))
    return;
  m_connection = str;
}

//----------------------------------------------------------------
// Procedure: getPolys()

vector<XYPolygon> WormHole::getPolys() const
{
  vector<XYPolygon> polys;

  if(m_weber_poly.is_convex())
    polys.push_back(m_weber_poly);
  if(m_madrid_poly.is_convex())
    polys.push_back(m_madrid_poly);

  return(polys);
}

//----------------------------------------------------------------
// Procedure: getConfigSummary()

vector<string> WormHole::getConfigSummary() const
{
  vector<string> summary;
  
  string mpoly_spec = "null";
  string wpoly_spec = "null";

  if(m_madrid_poly.is_convex())
    mpoly_spec = m_madrid_poly.get_spec();
  if(m_weber_poly.is_convex())
    wpoly_spec = m_weber_poly.get_spec();

  string header = "tag=" + m_tag;
  header += ", connection=" + m_connection; 
  header += ", delay=" + doubleToStringX(m_delay);
  header += ", id_change=" + boolToString(m_id_change);

  summary.push_back(header);
  summary.push_back("madrid_poly: " + mpoly_spec);
  summary.push_back("weber_poly:  " + wpoly_spec);

  return(summary);
}

//----------------------------------------------------------------
// Procedure: crossPositionWeberToMadrid()

void WormHole::crossPositionWeberToMadrid(double wx, double wy,
					  double& mx, double& my)
{
  getCrossPosition(m_weber_poly, m_madrid_poly, wx, wy, mx, my);
}


//----------------------------------------------------------------
// Procedure: crossPositionMadridToWeber()

void WormHole::crossPositionMadridToWeber(double mx, double my,
					  double& wx, double& wy)
{
  getCrossPosition(m_madrid_poly, m_weber_poly, mx, my, wx, wy);
}


//----------------------------------------------------------------
// Procedure: getCrossPosition()

void WormHole::getCrossPosition(XYPolygon polya, XYPolygon polyb,
				double ax, double ay,
				double& bx, double& by)
{
  if(!polya.is_convex() || !polyb.is_convex())
    return;
  
  double centroid_ax = polya.get_centroid_x();
  double centroid_ay = polya.get_centroid_y();

  double range_a  = hypot(ax-centroid_ax, ay-centroid_ay);
  double relang_a = relAng(centroid_ax, centroid_ay, ax, ay); 

  double centroid_bx = polyb.get_centroid_x();
  double centroid_by = polyb.get_centroid_y();

  projectPoint(relang_a, range_a, centroid_bx, centroid_by, bx, by);

  // If polys are identical in shape, this should always pass. But by
  // checking for this we allow that polys may be of different shape.
  if(polyb.contains(bx,by))
    return;

  // If the new point is not in the b polygon, then find the point that
  // is closest. 
  
  double modx, mody;
  bool mod_ok = polyb.closest_point_on_poly(bx, by, modx, mody);
  if(mod_ok) {
    bx = modx;
    by = mody;
  }
}








