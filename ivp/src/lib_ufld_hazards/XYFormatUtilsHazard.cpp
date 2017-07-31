/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYFormatUtilsHazard.cpp                              */
/*    DATE: Jan 27th, 2012                                       */
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

#include <vector>
#include <cstdlib>
#include "XYFormatUtilsHazard.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: string2Hazard (Method #0)
//   Example: Create a point from a string representation. 

XYHazard string2Hazard(string str)
{
  return(stringStandard2Hazard(str));
}

//---------------------------------------------------------------
// Procedure: stringStandard2Hazard  (Method #1)
//      Note: This function is standard because it processes the 
//            string format used when a string is created from an 
//            existing XYHazard instance.
//   Example: x=4,y=2,type=hazard

XYHazard stringStandard2Hazard(string str)
{
  XYHazard new_hazard;

  str = stripBlankEnds(str);
  vector<string> mvector = parseString(str, ',');
  unsigned int i, vsize = mvector.size();

  string hazard_field;
  
  for(i=0; i<vsize; i++) {
    mvector[i] = stripBlankEnds(mvector[i]);
    string param = tolower(stripBlankEnds(biteString(mvector[i], '=')));
    string value = stripBlankEnds(mvector[i]);
    double dval  = atof(value.c_str());

    if((param == "x") && isNumber(value))
      new_hazard.setX(dval);
    else if((param == "y") && isNumber(value))
      new_hazard.setY(dval);
    else if((param == "hr") && isNumber(value))
      new_hazard.setResemblance(dval);
   else if((param == "aspect") && isNumber(value))
      new_hazard.setAspect(dval);
   else if((param == "aspect_min") && isNumber(value))
      new_hazard.setAspectRangeMin(dval);
   else if((param == "aspect_max") && isNumber(value))
      new_hazard.setAspectRangeMax(dval);
    else if(param == "type")
      new_hazard.setType(value);
    else if(param == "shape")
      new_hazard.setShape(value);
    else if(param == "source")
      new_hazard.setSource(value);
    else if(param == "color")
      new_hazard.setColor(value);
    else if(param == "width")
      new_hazard.setWidth(value);
    else if(param == "label")
      new_hazard.setLabel(value);
    else if(param == "hazard")
      hazard_field = value;
  }

  if((new_hazard.getType() == "") && (hazard_field != "")) {
    if(hazard_field == "true") 
      new_hazard.setType("hazard");
    else
      new_hazard.setType("benign");
  }

  return(new_hazard);
}









