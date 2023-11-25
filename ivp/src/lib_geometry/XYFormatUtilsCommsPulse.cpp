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
#include "XYFormatUtilsCommsPulse.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: string2CommsPulse (Method #0)
//   Example: Create a point from a string representation. 

XYCommsPulse string2CommsPulse(string str)
{
  return(stringStandard2CommsPulse(str));
}

//---------------------------------------------------------------
// Procedure: stringStandard2CommsPulse  (Method #1)
//      Note: This function is standard because it processes the 
//            string format used when a string is created from an 
//            existing XYCommsPulse instance.
//   Example: label=one,sx=4,sy=2,tx=44,ty=55,beam_width=10,duration=5,
//            fill=0.3,fill_color=yellow,edge_color=green
// 

XYCommsPulse stringStandard2CommsPulse(string str)
{
  XYCommsPulse null_pulse;
  XYCommsPulse new_pulse;

  str = stripBlankEnds(str);
  vector<string> mvector = parseString(str, ',');
  unsigned int i, vsize = mvector.size();
  
  for(i=0; i<vsize; i++) {
    mvector[i] = stripBlankEnds(mvector[i]);
    string param = tolower(stripBlankEnds(biteString(mvector[i], '=')));
    string value = stripBlankEnds(mvector[i]);
    double dval  = atof(value.c_str());

    if((param == "sx") && isNumber(value))
      new_pulse.set_sx(dval);
    else if((param == "sy") && isNumber(value))
      new_pulse.set_sy(dval);
    else if((param == "tx") && isNumber(value))
      new_pulse.set_tx(dval);
    else if((param == "ty") && isNumber(value))
      new_pulse.set_ty(dval);
    else if((param == "beam_width") && isNumber(value))
      new_pulse.set_beam_width(dval);
    else if((param == "duration") && isNumber(value))
      new_pulse.set_duration(dval);
    else if((param == "fill") && isNumber(value))
      new_pulse.set_fill(dval);
    else if(param == "ptype")
      new_pulse.set_pulse_type(value);
    else
      new_pulse.set_param(param, value);
  }
  
  if(!new_pulse.valid())
    return(null_pulse);
  
  return(new_pulse);
}










