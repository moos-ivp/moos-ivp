/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TurnGenerator.cpp                                    */
/*    DATE: July 27th 2020                                       */
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
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "TurnGenerator.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

TurnGenerator::TurnGenerator()
{
  m_start_osx = 0;
  m_start_osy = 0;
  m_start_osh = 0;
  
  m_endx  = 0;
  m_endy  = 0;
  m_ptgap = 5;
  m_turn_radius = 10;

  m_port_turn = false;
  
  m_start_osx_set = false;
  m_start_osy_set = false;
  m_start_osh_set = false;
  
  m_endx_set = false;
  m_endy_set = false;
  m_endh_set = false;
  m_ptgap_set = false;
  m_turn_radius_set = false;
}

//-------------------------------------------------------------
// Procedure: setTurnRadius()              Overloadable Virtual

bool TurnGenerator::setTurnRadius(double radius)
{
  if(radius <=0)
    return(false);

  m_turn_radius = radius;
  m_turn_radius_set = true;

  return(true);
}

//-------------------------------------------------------------
// Procedure: setEndPos()                  Overloadable Virtual

void TurnGenerator::setEndPos(double endx, double endy)
{
  m_endx = endx;
  m_endy = endy;

  m_endx_set = true;
  m_endy_set = true;
}

//-------------------------------------------------------------
// Procedure: setHeading()                 Overloadable Virtual

void TurnGenerator::setEndHeading(double endh)
{
  m_endh = angle360(endh);
  m_endh_set = true;
}


//-------------------------------------------------------------
// Procedure: setStartPos()

void TurnGenerator::setStartPos(double osx, double osy, double osh)
{
  m_start_osx = osx;
  m_start_osy = osy;
  m_start_osh = angle360(osh);

  m_start_osx_set = true;
  m_start_osy_set = true;
  m_start_osh_set = true;
}

//-------------------------------------------------------------
// Procedure: setPointGap()

bool TurnGenerator::setPointGap(double gap)
{
  if(gap <=0)
    return(false);

  m_ptgap = gap;
  m_ptgap_set = true;
  
  return(true);
}


//-------------------------------------------------------------
// Procedure: getTurnDir()

string TurnGenerator::getTurnDir() const
{
  if(m_port_turn)
    return("port");

  return("star");
}


//-------------------------------------------------------------
// Procedure: valid()

bool TurnGenerator::valid() const
{
  if(!m_start_osx_set || !m_start_osy_set || !m_start_osh_set)
    return(false);
  //if(!m_endx_set || !m_endy_set) 
  //  return(false);
  if(!m_ptgap_set || !m_turn_radius_set) 
    return(false);
  
  return(true);
}


//-------------------------------------------------------------
// Procedure: getTurnLen()

double TurnGenerator::getTurnLen() const
{
  if(m_points.size() == 0)
    return(0);

  double total_dist = 0;
  
  double prev_x = m_start_osx;
  double prev_y = m_start_osy;
  for(unsigned int i=0; i<m_points.size(); i++) {
    double curr_x = m_points[i].get_vx();
    double curr_y = m_points[i].get_vy();
    double dist = hypot(prev_x - curr_x, prev_y - curr_y);
    prev_x = curr_x;
    prev_y = curr_y;
    total_dist += dist;
  }

  return(total_dist);
}

//-------------------------------------------------------------
// Procedure: getPtsX()

vector<double> TurnGenerator::getPtsX() const
{
  vector<double> rvector;
  for(unsigned int i=0; i<m_points.size(); i++)
    rvector.push_back(m_points[i].get_vx());

  return(rvector);
}

//-------------------------------------------------------------
// Procedure: getPtsY()

vector<double> TurnGenerator::getPtsY() const
{
  vector<double> rvector;
  for(unsigned int i=0; i<m_points.size(); i++)
    rvector.push_back(m_points[i].get_vy());

  return(rvector);
}

//-------------------------------------------------------------
// Procedure: mergePoints()
//   Purpose: Go through list of pts and if two pts are closer
//            than thresh, then replace with the average of the
//            two pts. First and last points are unmovable.
//   Returns: The number of reduced points

unsigned int TurnGenerator::mergePoints(double thresh)
{
  // Sanity check: Must have at least three points
  if(m_points.size() < 3)
    return(0);

  unsigned int min_ix = 0;
  double min_dist = -1;

  // Part 1 - find the two closest points
  unsigned int psize = m_points.size();
  for(unsigned int i=0; i<psize-1; i++) {
    double x1 = m_points[i].get_vx();
    double y1 = m_points[i].get_vy();
    double x2 = m_points[i+1].get_vx();
    double y2 = m_points[i+1].get_vy();
    double dist = hypot(x1-x2, y1-y2);

    if(dist < thresh) {
      if((i==0) || (dist < min_dist)) {
	min_dist = dist;
	min_ix = i;
      }
    }
  }    

  // Part 2 - Merge the two closest points
  if(min_dist >= 0) {
    XYPoint newpt;
    if(min_ix == 0)
      newpt = m_points[0];
    else if(min_ix == (psize-2))
      newpt = m_points[psize-1];
    else {
      double x1 = m_points[min_ix].get_vx();
      double y1 = m_points[min_ix].get_vy();
      double x2 = m_points[min_ix+1].get_vx();
      double y2 = m_points[min_ix+1].get_vy();
      newpt.set_vx((x1 + x2)/2);
      newpt.set_vy((y1 + y2)/2);
    }

    vector<XYPoint> new_points;
    for(unsigned int i=0; i<psize; ) {
      if(i == min_ix) {
	new_points.push_back(newpt);
	i += 2;
      }
      else {
	new_points.push_back(m_points[i]);
	i += 1;
      }
    }

    m_points = new_points;
  }
	
  return(1);
}


