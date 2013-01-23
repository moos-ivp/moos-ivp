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
// Constructor

XYHazardSet::XYHazardSet()
{
  m_hazard_count = 0;
  m_benign_count = 0;
}

//-----------------------------------------------------------
// Procedure: addHazard
//      Note: No check is made to determine if this set already has a 
//            hazard with the same label (in some cases one may want this?)
//            If the caller wants uniqueness, they should check first
//            before adding.

void XYHazardSet::addHazard(const XYHazard& new_hazard)
{
  m_hazards.push_back(new_hazard);
  if(new_hazard.getType() == "hazard")
    m_hazard_count++;
  else
    m_benign_count++;
}

//-----------------------------------------------------------
// Procedure: setHazard

bool XYHazardSet::setHazard(unsigned int ix, const XYHazard& new_hazard)
{
  if(ix >= m_hazards.size())
    return(false);
  
  // Step 1: Decrement the appropriate counter for the outgoing hazard.
  if(m_hazards[ix].getType() == "hazard")
    m_hazard_count--;
  else
    m_benign_count--;

  // Step 2: Replace the old with the new.
  m_hazards[ix] = new_hazard;

  // Step 3: Increment the appropriate counter for the new hazard.
  if(new_hazard.getType() == "hazard")
    m_hazard_count++;
  else
    m_benign_count++;

  return(true);
}

//-----------------------------------------------------------
// Procedure: getHazard

XYHazard XYHazardSet::getHazard(unsigned int index) const
{
  if(index >= m_hazards.size()) {
    XYHazard null_hazard;
    return(null_hazard);
  }
  
  return(m_hazards[index]);
}

//-----------------------------------------------------------
// Procedure: findHazard

int XYHazardSet::findHazard(double gx, double gy) const
{
  unsigned int i, vsize = m_hazards.size();
  if(vsize == 0) 
    return(-1);

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

  return(closest_ix);
}


//-----------------------------------------------------------
// Procedure: findHazard

int XYHazardSet::findHazard(const string& label) const
{
  unsigned int i, vsize = m_hazards.size();
  for(i=0; i<vsize; i++) {
    if(m_hazards[i].getLabel() == label)
      return((int)(i));
  }
  
  return(-1);
}

//-----------------------------------------------------------
// Procedure: hasHazard

bool XYHazardSet::hasHazard(const string& label) const
{
  unsigned int i, vsize = m_hazards.size();
  for(i=0; i<vsize; i++) {
    if(m_hazards[i].getLabel() == label)
      return(true);
  }

  return(false);
}


//-----------------------------------------------------------
// Procedure: isValidLaydown()
//      Note: A valid laydown consists of a non-empty source, and,
//            for each hazard, a set x,y position and type. 
//            The source is typically just the filename.

bool XYHazardSet::isValidLaydown() const
{
  unsigned int i, vsize = m_hazards.size();
  for(i=0; i<vsize; i++) {
    if(!m_hazards[i].isSetX() ||
       !m_hazards[i].isSetY() ||
       !m_hazards[i].isSetType())
      return(false);
  }
  return(true);
}

//-----------------------------------------------------------
// Procedure: isValidReport()
//      Note: A valid report consists of a non-empty source, and,
//            for each hazard, a set x,y position.

bool XYHazardSet::isValidReport() const
{
  unsigned int i, vsize = m_hazards.size();
  for(i=0; i<vsize; i++) {
    if(!m_hazards[i].isSetX() ||
       !m_hazards[i].isSetY())
      return(false);
  }
  return(true);
}


//-----------------------------------------------------------
// Procedure: getSpec()

string XYHazardSet::getSpec(string report_style) const
{
  string noshow = "";
  if(report_style == "final_report")
    noshow = "hr,type,width,color";

  string str = "source=" + m_source;
  if(m_name != "")
    str += "#name=" + m_name;
  unsigned int i, vsize = m_hazards.size();
  for(i=0; i<vsize; i++) {
    string hazard_str = m_hazards[i].getSpec(noshow);
    str += "#" + hazard_str;
  }
  return(str);
}
