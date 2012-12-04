/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYFormatUtilsHazardSet.cpp                           */
/*    DATE: Mar 12th, 2012                                       */
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

#include <vector>
#include <cstdlib>
#include "XYFormatUtilsHazardSet.h"
#include "XYFormatUtilsHazard.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: stringStandard2HazardSet
//   Example: source=alpha # x=4,y=2,type=hazard,label=01 #
//            x=2,y=3,type=benign # x=11,y=1113,type=benign

XYHazardSet string2HazardSet(string str)
{
  XYHazardSet new_hazard_set;

  str = stripBlankEnds(str);
  vector<string> svector = parseString(str, '#');
  unsigned int i, vsize = svector.size();
  
  for(i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    if(strBegins(svector[i], "source")) {
      string src_tag = biteStringX(svector[i], '=');
      string src_val = svector[i];
      new_hazard_set.setSource(src_val);
    }
    else {
      XYHazard hazard = string2Hazard(svector[i]);
      new_hazard_set.addHazard(hazard);
    }
  }
  return(new_hazard_set);
}


