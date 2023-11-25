/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYFormatUtilsCircle.cpp                              */
/*    DATE: Jun 21st, 2009                                       */
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

#include <cstdlib>
#include "MBUtils.h"
#include "XYFormatUtilsCircle.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: string2Circle
//
// circle = x=20, y=-30, rad=40, 
//          label=val, source=val, type=val, time=val, active=val, 
//          edge_color=val, edge_size=val, vertex_color=val, 
//          vertex_size=val, label_color=val, fill_color=white

XYCircle string2Circle(string str)
{
  XYCircle null_circle;
  XYCircle new_circle;

  str = stripBlankEnds(str);

  vector<string> kvector = parseStringQ(str, ',');
  unsigned int k, ksize  = kvector.size();

  for(k=0; k<ksize; k++) {
    string left  = stripBlankEnds(biteString(kvector[k], '='));
    string right = stripBlankEnds(kvector[k]);
    double dval  = atof(right.c_str());
    
    if((left == "x") && isNumber(right))
      new_circle.setX(dval);
    else if((left == "y") && isNumber(right))
      new_circle.setY(dval);
    else if((left == "radius") && isNumber(right))
      new_circle.setRad(dval);
    else if((left == "duration") && isNumber(right))
      new_circle.setDuration(dval);

    else if((left == "time") && isNumber(right))
      new_circle.set_time(dval);
    else if((left == "vertices") && isNumber(right) && (dval > 0))
      new_circle.setDrawVertices(atoi(right.c_str()));
    else if(left == "label") 
      new_circle.set_label(right);
    else if(left == "label_color") 
      new_circle.set_color("label", right);
    else if(left == "msg") 
      new_circle.set_msg(right);
    else if(left == "vertex_color") 
      new_circle.set_color("vertex", right);
    else if((left == "vertex_size") && isNumber(right))
      new_circle.set_vertex_size(dval);
    else if(left == "edge_color")
      new_circle.set_color("edge", right);
    else if(left == "fill_color")
      new_circle.set_color("fill", right);
    else if(left == "fill_transparency")
      new_circle.set_param("fill_transparency", right);
    else if((left == "edge_size") && isNumber(right))
      new_circle.set_edge_size(dval);
    else if(left == "active") 
      new_circle.set_active(tolower(right)=="true");
  }

  if(new_circle.valid())
    return(new_circle);
  else
    return(null_circle);
}










