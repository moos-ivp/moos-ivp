/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYFormatUtilsPoint.cpp                               */
/*    DATE: Mar 16, 2008 Sunday Afternoon at Brugger's           */
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
#include "XYFormatUtilsPoint.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: string2Point (Method #0)
//   Example: Create a point from a string representation. Will 
//            call one of the string*2Point functions below. 
//   ***NOTE: This is the only function that should be called by 
//            the user. The other functions are subject to change 
//            without regard to backward compatibility.

XYPoint string2Point(const string& str)
{
  XYPoint new_point = stringStandard2Point(str);
  if(new_point.valid())
    return(new_point);
  
  // Last chance....
  return(stringAbbreviated2Point(str));
}

//---------------------------------------------------------------
// Procedure: stringStandard2Point  (Method #1)
//      Note: This function is standard because it processes the 
//            string format used when a string is created from an 
//            existing XYPoint instance.
//   Example: x=4,y=2,z=3,vertex_size=2,label_color=red,
//            vertex_color=white,soure=foobar
// 

XYPoint stringStandard2Point(const string& str)
{
  XYPoint null_point;
  XYPoint new_point;

  vector<string> mvector = parseString(str, ',');
  unsigned int i, vsize = mvector.size();
  
  string x,y,z,trans;
  for(i=0; i<vsize; i++) {
    string param = biteStringX(mvector[i], '=');
    string value = mvector[i];

    if(param == "x")
      x = value;
    else if(param == "y")
      y = value;
    else if(param == "z")
      z = value;
    else if((param == "trans") && isNumber(value))
      trans = value;
    else
      new_point.set_param(param, value);
  }
  
  if((x=="") || (y==""))
    return(null_point);

  if(trans != "")
    new_point.set_transparency(atof(trans.c_str()));
  
  new_point.set_vertex(atof(x.c_str()), atof(y.c_str()), atof(z.c_str()));
  
  return(new_point);
}

//---------------------------------------------------------------
// Procedure: stringAbbreviated2Point
//   Example: 0,0
//   Example: 4,5:label,foobar:source,bravo:msg,hello

XYPoint stringAbbreviated2Point(const string& str)
{
  XYPoint null_point;
  XYPoint new_point;

  vector<string> mvector = parseString(str, ':');
  unsigned int i, vsize = mvector.size();
  
  for(i=0; i<vsize; i++) {
    string param = biteStringX(mvector[i], ',');
    string value = mvector[i];
    bool handled = new_point.set_param(param, value);

    if(!handled) { // This component might be the vertex  x,y or x,y,z
      string xstr = param;
      string ystr = value;
      string zstr;
      if(strContains(ystr, ',')) {
	ystr = stripBlankEnds(biteString(value, ','));
	zstr = stripBlankEnds(value);
      }

      if(isNumber(xstr) && isNumber(ystr) && (isNumber(zstr)||(zstr==""))) {
	double xval = atof(xstr.c_str());
	double yval = atof(ystr.c_str());
	double zval = 0;
	if(isNumber(zstr))
	  zval = atof(zstr.c_str());
	new_point.set_vertex(xval,yval,zval);
      }
    }
  }

  return(new_point);
}










