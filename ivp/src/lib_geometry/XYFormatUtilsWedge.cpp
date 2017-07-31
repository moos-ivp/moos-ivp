/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYFormatUtilsCircle.cpp                              */
/*    DATE: Sep 17th, 2015                                       */
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
#include "XYFormatUtilsWedge.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: string2Wedge
//
// wedge = x=20, y=-30, rad_low=40, rad_hgh, ang_low, ang_hgh
//        label=val, active=true, fill_transparency=0.4
//        edge_color=val, edge_size=val, vertex_color=val, 
//        vertex_size=val, label_color=val, fill_color=white


XYWedge string2Wedge(string str)
{
  XYWedge null_wedge;
  XYWedge new_wedge;

  vector<string> kvector = parseStringQ(str, ',');
  for(unsigned int k=0; k<kvector.size(); k++) {
    string left  = biteStringX(kvector[k], '=');
    string right = kvector[k];
    double dval  = atof(right.c_str());
    
    if((left == "x") && isNumber(right))
      new_wedge.setX(dval);
    else if((left == "y") && isNumber(right))
      new_wedge.setY(dval);
    else if((left == "rad_low") && isNumber(right))
      new_wedge.setRadLow(dval);
    else if((left == "rad_hgh") && isNumber(right))
      new_wedge.setRadHigh(dval);
    else if((left == "ang_low") && isNumber(right))
      new_wedge.setLangle(dval);
    else if((left == "ang_hgh") && isNumber(right))
      new_wedge.setHangle(dval);

    else if(left == "label") 
      new_wedge.set_label(right);
    else if((left == "label_color") && isColor(right))
      new_wedge.set_color("label", right);
    else if((left == "vertex_color") && isColor(right))
      new_wedge.set_color("vertex", right);
    else if((left == "vertex_size") && isNumber(right))
      new_wedge.set_vertex_size(dval);
    else if((left == "edge_color") && isColor(right))
      new_wedge.set_color("edge", right);
    else if((left == "fill_color") && isColor(right))
      new_wedge.set_color("fill", right);
    else if((left == "fill_transparency") && isNumber(right))
      new_wedge.set_param("fill_transparency", right);
    else if((left == "edge_size") && isNumber(right))
      new_wedge.set_edge_size(dval);
    else if(left == "active") 
      new_wedge.set_active(tolower(right)=="true");
  }

  if(new_wedge.valid())
    return(new_wedge);
  else
    return(null_wedge);
}









