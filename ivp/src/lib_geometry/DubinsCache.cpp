/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: DubinsCache.cpp                                      */
/*    DATE: Nov 19th 2018                                        */
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

#include <iostream>
#include <cmath> 
#include "DubinsCache.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "ArcUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

DubinsCache::DubinsCache() 
{
  // Initialize config vars
  m_osx = 0;
  m_osy = 0;
  m_osh = 0;
  m_turn_radius = -1;
  m_hdg_choices = 360;
}

//----------------------------------------------------------------
// Procedure: setParams

bool DubinsCache::setParams(double osx, double osy, double osh,
			    double radius)
{
  if(radius < 0)
    return(false);

  m_dturns.clear();
  
  m_osx = osx;
  m_osy = osy;
  m_osh = osh;
  m_turn_radius = radius;
  
  return(true);
}

//----------------------------------------------------------------
// Procedure: buildCache()

bool DubinsCache::buildCache(unsigned int hdg_choices)
{
  // Turn radius must by non-negative, hdg_choices must be at least
  // every 30 degrees.
  if((m_turn_radius < 0) || (hdg_choices < 12))
    return(false);
    
  m_dturns.clear();
  for(unsigned int i=0; i<m_hdg_choices; i++) {
    double delta = 360.0 / (double)(m_hdg_choices);
    double desired_hdg = delta * (double)(i);

    DubinsTurn dturn(m_osx, m_osy, m_osh, m_turn_radius);
    dturn.setTurn(desired_hdg);
    m_dturns.push_back(dturn);
  }

  return(true);
}


//----------------------------------------------------------------
// Procedure: getArc()

bool DubinsCache::getArc(double new_hdg, double& ax,
			 double& ay, double& ar, double& langle,
			 double& rangle) const
{
  if((new_hdg < 0) || (new_hdg >= 360))
    new_hdg = angle360(new_hdg);

  double delta = 360.0 / (double)(m_hdg_choices);
  unsigned int ix = (unsigned int)(new_hdg / delta);

  return(getArcIX(ix, ax, ay, ar, langle, rangle));
}


//----------------------------------------------------------------
// Procedure: getArcIX()

bool DubinsCache::getArcIX(unsigned int ix, double& ax,
			   double& ay, double& ar,
			   double& langle, double& rangle) const
{
  // This should never happen by the above logic but handle anyway
  if((ix >= m_hdg_choices) || (ix >= m_dturns.size())) {
    ax = 0;
    ay = 0;
    ar = 0;
    langle = 0;
    rangle = 0;
    return(false);
  }

  // General case
  ax = m_dturns[ix].getArcCX();
  ay = m_dturns[ix].getArcCY();
  ar = m_dturns[ix].getArcRad();
  langle = m_dturns[ix].getArcLangle();
  rangle = m_dturns[ix].getArcRangle();
  
  return(true);
}


//----------------------------------------------------------------
// Procedure: getRay()

bool DubinsCache::getRay(double new_hdg, double& px, double& py) const
{
  if((new_hdg < 0) || (new_hdg >= 360))
    new_hdg = angle360(new_hdg);

  double delta = 360.0 / (double)(m_hdg_choices);
  unsigned int ix = (unsigned int)(new_hdg / delta);

  return(getRayIX(ix, px, py));
}



//----------------------------------------------------------------
// Procedure: getRayIX()

bool DubinsCache::getRayIX(unsigned int ix, double& px, double& py) const
{
  // This should never happen by the above logic but handle anyway
  if((ix >= m_hdg_choices) || (ix >= m_dturns.size())) {
    px = 0;
    py = 0;
    return(false);
  }

  // General case
  px = m_dturns[ix].getOSRayX();
  py = m_dturns[ix].getOSRayY();
  return(true);
}

//----------------------------------------------------------------
// Procedure: getArcLen()

double DubinsCache::getArcLen(double new_hdg) const
{
  if((new_hdg < 0) || (new_hdg >= 360))
    new_hdg = angle360(new_hdg);

  double delta = 360.0 / (double)(m_hdg_choices);
  unsigned int ix = (unsigned int)(new_hdg / delta);
  
  return(getArcLenIX(ix));
}


//----------------------------------------------------------------
// Procedure: getArcLenIX()

double DubinsCache::getArcLenIX(unsigned int ix) const
{
  // This should never happen by the above logic but handle anyway
  if((ix >= m_hdg_choices) || (ix >= m_dturns.size())) {
    return(-1);
  }

  return(m_dturns[ix].getArcLen());
}


//----------------------------------------------------------------
// Procedure: getTurnHdgIX()

double DubinsCache::getTurnHdgIX(unsigned int ix) const
{
  // This should never happen by the above logic but handle anyway
  if((ix >= m_hdg_choices) || (ix >= m_dturns.size())) {
    return(-1);
  }

  return(m_dturns[ix].getTurnHdg());
}


//----------------------------------------------------------------
// Procedure: starTurn()

bool DubinsCache::starTurnIX(unsigned int ix) const
{
  // This should never happen by the above logic but handle anyway
  if((ix >= m_hdg_choices) || (ix >= m_dturns.size())) {
    return(false);
  }

  return(m_dturns[ix].starTurn());
}

//----------------------------------------------------------------
// Procedure: portTurn()

bool DubinsCache::portTurnIX(unsigned int ix) const
{
  // This should never happen by the above logic but handle anyway
  if((ix >= m_hdg_choices) || (ix >= m_dturns.size())) {
    return(false);
  }

  return(m_dturns[ix].portTurn());
}

//----------------------------------------------------------------
// Procedure: getMaxPortTurn
//   Purpose: Determine the max possible turn to the port before the
//            turn request would be treated as a turn to starboard

double DubinsCache::getMaxPortTurn(double& ax, double& ay, double& ar,
				   double& langle, double& rangle) const
{
  if(m_dturns.size() == 0)
    return(-1);

  unsigned int found_ix = 0;
  bool found = false;
  
  cout << "getMaxPortTurn ****E" << endl;

  for(unsigned int i=0; (i<m_dturns.size()) && !found ; i++) {
    //    cout << "[" << i << "]: " << boolToString(m_dturns[i].starTurn()) << endl;
    if(m_dturns[i].portTurn() && (i != 0)) {
      found_ix = i;
      found = true;
    }
  }
      

#if 0
  for(unsigned int i=0; i<m_dturns.size() && (found_ix == 0); i++) 
    if(m_dturns[i].portTurn()) 
      found_ix = i;
#endif
  
  getArcIX(found_ix, ax, ay, ar, langle, rangle);
  return(m_dturns[found_ix].getTurnHdg());
}


//----------------------------------------------------------------
// Procedure: getMaxStarTurn
//   Purpose: Determine the max possible turn to starboard before the
//            turn request would be treated as a turn to the port
//   Returns: The desired_heading that would represent the max
//            starboard turn. Ex, if osh=45, may return 225.

double DubinsCache::getMaxStarTurn(double& ax, double& ay, double& ar,
				   double& langle, double& rangle) const
{
  if(m_dturns.size() == 0)
    return(-1);

  unsigned int found_ix = 0;
  bool found = false;
  
  cout << "getMaxStarTurn ****E" << endl;

  bool in_star_group = false;
  for(unsigned int i=0; (i<m_dturns.size()) && !found ; i++) {
    //    cout << "[" << i << "]: " << boolToString(m_dturns[i].starTurn()) << endl;
    if(m_dturns[i].starTurn())
      in_star_group = true;
    else {
      if(in_star_group) {
	found_ix = i-1;
	found = true;
      }
    }
  }
      
#if 0
  for(unsigned int i=0; i<m_dturns.size() && (found_ix == 0); i++) {
    cout << "[" << i << "]: " << boolToString(m_dturns[i].portTurn()) << endl;
    if(m_dturns[i].portTurn() && (i>0)) {
      found_ix = i-1;
    }
  }
#endif

  getArcIX(found_ix, ax, ay, ar, langle, rangle);
  return(m_dturns[found_ix].getTurnHdg());

}


