/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYHazardSet.cpp                                      */
/*    DATE: Mar 12, 2012                                         */
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

#include "XYHazardSet.h"
#include "MBUtils.h"
#include "GeomUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: addHazard

void XYHazardSet::addHazard(XYHazard hazard)
{
  m_hazards.push_back(hazard);
}

//-----------------------------------------------------------
// Procedure: addHazard

void XYHazardSet::addHazard(double x, double y, string type, string label)
{
  XYHazard hazard;
  hazard.setX(x);
  hazard.setY(y);
  hazard.setType(type);
  hazard.setLabel(label);
  m_hazards.push_back(hazard);
}

//-----------------------------------------------------------
// Procedure: getHazard

XYHazard XYHazardSet::getHazard(unsigned int index)
{
  if(index >= m_hazards.size()) {
    XYHazard null_hazard;
    return(null_hazard);
  }
  
  return(m_hazards[index]);
}

//-----------------------------------------------------------
// Procedure: findHazardMatch

XYHazard XYHazardSet::findHazardMatch(XYHazard hazard, double thresh_dist)
{
  double       closest_dist = 0;
  unsigned int closest_ix;

  double x1 = hazard.getX();
  double y1 = hazard.getY();

  unsigned int i, vsize = m_hazards.size();
  for(i=0; i<vsize; i++) {
    double x2 = m_hazards[i].getX();
    double y2 = m_hazards[i].getY();
    double dist = distPointToPoint(x1, y1, x2, y2);
    if((i==0) || (dist < closest_dist)) {
      closest_dist = dist;
      closest_ix   = i;
    }
  }

  if(closest_dist > thresh_dist) {
    XYHazard null_hazard;
    return(null_hazard);
  }

  return(m_hazards[closest_ix]);
}

//-----------------------------------------------------------
// Procedure: findHazardByPosition

XYHazard XYHazardSet::findHazardByPosition(double gx, double gy)
{
  unsigned int i, vsize = m_hazards.size();
  if(vsize == 0) {
    XYHazard null_hazard;
    return(null_hazard);
  }

  double       closest_dist = 0;
  unsigned int closest_ix;

  for(i=0; i<vsize; i++) {
    double x2 = m_hazards[i].getX();
    double y2 = m_hazards[i].getY();
    double dist = distPointToPoint(gy, gy, x2, y2);
    if((i==0) || (dist < closest_dist)) {
      closest_dist = dist;
      closest_ix   = i;
    }
  }

  return(m_hazards[closest_ix]);
}


//-----------------------------------------------------------
// Procedure: findHazardByLabel

XYHazard XYHazardSet::findHazardByLabel(string label)
{
  XYHazard return_hazard;
  
  unsigned int i, vsize = m_hazards.size();
  for(i=0; i<vsize; i++) {
    if(m_hazards[i].getLabel() == label)
      return_hazard = m_hazards[i];
  }

  return(return_hazard);
}


//-----------------------------------------------------------
// Procedure: getSpec()

string XYHazardSet::getSpec() const
{
  string str = "source=" + m_source;
  unsigned int i, vsize = m_hazards.size();
  for(i=0; i<vsize; i++) {
    string hazard_str = m_hazards[i].getSpec();
    str += "#" + hazard_str;
  }
  return(str);
}

