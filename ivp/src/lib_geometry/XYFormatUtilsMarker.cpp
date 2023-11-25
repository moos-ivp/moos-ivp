/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYFormatUtilsMarker.cpp                              */
/*    DATE: Mar 13, 2011                                         */
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
#include <vector>
#include <cstdlib>
#include "XYFormatUtilsMarker.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: string2Marker (Method #0)
//   Example: Create a marker from a string representation. Will 
//            call one of the string*2Marker functions below. 
//   ***NOTE: This is the only function that should be called by 
//            the user. The other functions are subject to change 
//            without regard to backward compatibility.

XYMarker string2Marker(string str)
{
  str = stripBlankEnds(str);

  XYMarker new_marker = stringStandard2Marker(str);
  return(new_marker);
}

//---------------------------------------------------------------
// Procedure: stringStandard2Marker()
//      Note: This function is standard because it processes the 
//            string format used when a string is created from an 
//            existing XYMarker instance.
//   Example: x=4,y=2,type=square,label=base,primary_color=white
//            secondary_color=blue
// 

XYMarker stringStandard2Marker(string str)
{
  XYMarker null_marker;
  XYMarker new_marker;

  str = stripBlankEnds(str);
  vector<string> mvector = parseString(str, ',');
  unsigned int i, vsize = mvector.size();

  string x,y,transparency,range,width;
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(mvector[i], '='));
    string value = mvector[i];

    if(((param == "x") || (param == "xpos")) && isNumber(value))
      x = value;
    else if(((param == "y") || (param == "ypos")) && isNumber(value))
      y = value;
    else if((param == "fill_transparency") && isNumber(value))
      transparency = value;
    else if(((param == "width") || (param == "scale")) && isNumber(value))
      width = value;
    else if((param == "range") && isNumber(value))
      range = value;
    else if(param == "colors") {
      string primary_color = biteString(value, ':');
      string secondary_color = value;
      if(isColor(primary_color))
	new_marker.set_color("primary_color", primary_color);
      if(isColor(secondary_color))
	new_marker.set_color("secondary_color", secondary_color);
    }
    else if((param == "primary_color") || (param == "color"))
      new_marker.set_color("primary_color", value);
    else if(param == "secondary_color")
      new_marker.set_color("secondary_color", value);
    else if(param == "type")
      new_marker.set_type(value);
    else if(param == "owner")
      new_marker.set_owner(value);
    else
      new_marker.set_param(param, value);
  }

  if((x=="") || (y==""))
    return(null_marker);
  
  new_marker.set_vx(atof(x.c_str()));
  new_marker.set_vy(atof(y.c_str()));
  
  if(range != "")
    new_marker.set_range(atof(range.c_str()));
  if(width != "")
    new_marker.set_width(atof(width.c_str()));

  if(transparency != "") {
    double dval = atof(transparency.c_str());
    new_marker.set_transparency(dval);
  }

  return(new_marker);
}











