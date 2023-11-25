/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYFormatUtilsRangePulse.cpp                          */
/*    DATE: Feb 5th, 2011                                        */
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

#include <vector>
#include <cstdlib>
#include "XYFormatUtilsRangePulse.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: string2RangePulse (Method #0)
//   Example: Create a point from a string representation. 

XYRangePulse string2RangePulse(string str)
{
  return(stringStandard2RangePulse(str));
}

//---------------------------------------------------------------
// Procedure: stringStandard2RangePulse  (Method #1)
//      Note: This function is standard because it processes the 
//            string format used when a string is created from an 
//            existing XYRangePulse instance.
//   Example: label=one,x=4,y=2,radius=50,duration=5,fill=0.3,
//            fill_color=yellow,edge_color=green
// 

XYRangePulse stringStandard2RangePulse(string str)
{
  XYRangePulse null_pulse;
  XYRangePulse new_pulse;

  str = stripBlankEnds(str);
  vector<string> mvector = parseString(str, ',');
  unsigned int i, vsize = mvector.size();
  
  for(i=0; i<vsize; i++) {
    mvector[i] = stripBlankEnds(mvector[i]);
    string param = tolower(biteStringX(mvector[i], '='));
    string value = mvector[i];
    double dval  = atof(value.c_str());
    
    if((param == "x") && isNumber(value))
      new_pulse.set_x(dval);
    else if((param == "y") && isNumber(value))
      new_pulse.set_y(dval);
    else if((param == "radius") && isNumber(value))
      new_pulse.set_rad(dval);
    else if((param == "duration") && isNumber(value))
      new_pulse.set_duration(dval);
    else if((param == "linger") && isNumber(value))
      new_pulse.set_linger(dval);
    else if((param == "fill") && isNumber(value))
      new_pulse.set_fill(dval);
    else if((param == "fill_invariant") && isBoolean(value))
      new_pulse.set_fill_invariant(tolower(value)=="true");
    else
      new_pulse.set_param(param, value);
  }
  
  if(!new_pulse.valid())
    return(null_pulse);
  
  return(new_pulse);
}










