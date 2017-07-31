/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYFormatUtilsVector.cpp                              */
/*    DATE: Oct 18, 2010 In Waterloo                             */
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
#include "XYFormatUtilsVector.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: string2Vector
//

XYVector string2Vector(string str)
{
  XYVector null_vector;
  XYVector new_vector;

  str = tolower(stripBlankEnds(str));
  vector<string> mvector = parseStringQ(str, ',');
  unsigned int i, vsize = mvector.size();
  
  // Below are the mandatory parameters - check they are set.
  double xpos     = 0;
  double ypos     = 0;
  double mag      = 0;
  double ang      = 0;
  double xdot     = 0;
  double ydot     = 0;
  bool   xpos_set = false;
  bool   ypos_set = false;

  for(i=0; i<vsize; i++) {
    string param = stripBlankEnds(biteString(mvector[i], '='));
    string value = stripBlankEnds(mvector[i]);
    double dval  = atof(value.c_str());
    if(((param == "xpos") || (param == "x")) && isNumber(value)) {
      xpos_set = true;
      xpos = dval;
    }
    else if(((param == "ypos") || (param=="y")) && isNumber(value)) {
      ypos_set = true;
      ypos = dval;
    }
    else if((param == "ang") && isNumber(value))
      ang = dval;
    else if((param == "mag") && isNumber(value))
      mag = dval;
    else if((param == "xdot") && isNumber(value))
      xdot = dval;
    else if((param == "ydot") && isNumber(value))
      ydot = dval;
    else if((param == "vertex_size") && isNumber(value))
      new_vector.set_vertex_size(dval);
    else if(param == "head_size") 
      new_vector.setHeadSize(dval);
    else
      new_vector.set_param(param, value);
  }

  if(!xpos_set || !ypos_set)
    return(null_vector);
  
  new_vector.setPosition(xpos, ypos);
  new_vector.setVectorMA(mag, ang);
  new_vector.mergeVectorXY(xdot, ydot);
  
  return(new_vector);
}









