/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WrapDetectortor.cpp                                  */
/*    DATE: Jan 9th, 2023                                        */
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
#include "WrapDetector.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor()

WrapDetector::WrapDetector()
{
  // Init state vars
  m_osx = 0;
  m_osy = 0;
  m_empty = true;
  m_wraps = 0;

  // Init config vars
  m_min_leg  = 1;
  m_max_legs = 5000;
}

//---------------------------------------------------------------
// Procedure: clear()

void WrapDetector::clear()
{
  // clear state vars
  m_osx = 0;
  m_osy = 0;
  m_empty = true;
  m_wraps = 0;
  m_os_legs.clear();
}

//---------------------------------------------------------------
// Procedure: setMinLeg()
//      Note: This is the threshold for whether a newly updated
//            ownship position is sufficiently far enough from the
//            previous point. This is to reduce computational load
//            of line segment intersection tests.

void WrapDetector::setMinLeg(double min_leg)
{
  if(min_leg < 0)
    return;

  m_min_leg = min_leg;
}

//---------------------------------------------------------------
// Procedure: setMaxLegs()
//      Note: An upper bound on the number of legs helps guard
//            against a memory leak introduced by a user.

void WrapDetector::setMaxLegs(unsigned int max_legs)
{
  m_max_legs = max_legs;
}

//---------------------------------------------------------------
// Procedure: updatePosition()

void WrapDetector::updatePosition(double osx, double osy)
{
  if(m_empty) {
    m_osx = osx;
    m_osy = osy;
    m_empty = false;
    return;
  }

  XYSegment new_seg(m_osx, m_osy, osx, osy);
  if(new_seg.length() < m_min_leg)
    return;
  
  bool wrapped = false;

  if(m_os_legs.size() != 0) {
    // Temporarily remove last segment since we know it will
    // intersect with the new segment.
    XYSegment final_seg = m_os_legs.back();
    m_os_legs.pop_back();

    list<XYSegment>::iterator p;
    for(p=m_os_legs.begin(); p!=m_os_legs.end(); p++) {
      XYSegment this_seg = *p;
      if(this_seg.intersects(new_seg))
	wrapped = true;
    }
    // Now re-instate the last segment to back of the list
    m_os_legs.push_back(final_seg);
  }

  if(!wrapped) {
    m_os_legs.push_back(new_seg);
    if(m_os_legs.size() > m_max_legs)
      m_os_legs.pop_front();

    m_osx = osx;
    m_osy = osy;
    return;
  }

  // IF wrap was detected, the history is essentially cleared.
  // Further wraps will be counted only from this point forward.
  m_wraps++;

  cout << "Wrap detected!!!" << endl;
  
  m_osx = 0;
  m_osy = 0;
  m_empty = true;
  m_os_legs.clear();
}



