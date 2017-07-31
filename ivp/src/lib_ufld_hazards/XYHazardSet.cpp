/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYHazardSet.cpp                                      */
/*    DATE: Mar 12, 2012                                         */
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

  m_xpath = 0;
  m_ypath = 0;
  m_xpath_set = false;
  m_ypath_set = false;
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
  unsigned int closest_ix = 0;

  for(i=0; i<vsize; i++) {
    double x2 = m_hazards[i].getX();
    double y2 = m_hazards[i].getY();
    double dist = distPointToPoint(gx, gy, x2, y2);
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
// Procedure: getHazardCnt(XYPolygon)

unsigned int XYHazardSet::getHazardCnt(const XYPolygon& poly) const
{
  if(!poly.is_convex())
    return(0);

  unsigned int count = 0;
  unsigned int i, vsize = m_hazards.size();
  for(i=0; i<vsize; i++) {
    if(m_hazards[i].getType() == "hazard") {
      double x = m_hazards[i].getX();
      double y = m_hazards[i].getY();
      if(poly.contains(x, y))
	count++;
    }
  }
  return(count);
}

//-----------------------------------------------------------
// Procedure: findMinXPath()

unsigned int XYHazardSet::findMinXPath(double path_width)
{
  if(path_width == 0)
    return(33);

  if(m_search_region.is_convex() == false)
    return(22);

  double xmin = m_search_region.get_min_x();
  double xmax = m_search_region.get_max_x();
  double ymin = m_search_region.get_min_y();
  double ymax = m_search_region.get_max_y();

  double beg_x = xmin + path_width;
  double end_x = xmax - path_width;
  if(beg_x > end_x)
    return(11);

  unsigned int min_count = 0;
  double       min_xpath = beg_x;

  for(double x=beg_x; x<end_x; x++) {
    XYPolygon test_poly;
    test_poly.add_vertex(x-path_width, ymin);
    test_poly.add_vertex(x-path_width, ymax);
    test_poly.add_vertex(x+path_width, ymax);
    test_poly.add_vertex(x+path_width, ymin);
    unsigned int count = getHazardCnt(test_poly);
    if((x==beg_x) || (count < min_count)) {
      min_count = count;
      min_xpath = x;
    }
  }
  
  m_xpath = min_xpath;
  m_xpath_set = true;

  return(min_count);
}



//-----------------------------------------------------------
// Procedure: getSpec()

string XYHazardSet::getSpec(string report_style) const
{
  string noshow = "";
  if(report_style == "final_report")
    noshow = "hr,type,width,color,search_region";

  string str = "source=" + m_source;
  if(m_name != "")
    str += "#name=" + m_name;
  if(m_xpath_set)
    str += "#xpath=" + doubleToStringX(m_xpath,1);
  if(m_ypath_set)
    str += "#ypath=" + doubleToStringX(m_ypath,1);
  if(m_search_region.is_convex() && (report_style != "final_report"))
    str += "#search_region=" + m_search_region.get_spec_pts();
  
  unsigned int i, vsize = m_hazards.size();
  for(i=0; i<vsize; i++) {
    string hazard_str = m_hazards[i].getSpec(noshow);
    str += "#" + hazard_str;
  }
  return(str);
}







