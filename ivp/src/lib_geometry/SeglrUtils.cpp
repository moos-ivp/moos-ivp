/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: SeglrUtils.cpp                                       */
/*    DATE: Apr 6th, 2015                                        */
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
 
#include <cmath>
#include "SeglrUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: seglrToString()

string seglrToString(Seglr seglr)
{
  string result;
  for(unsigned int i=0; i<seglr.size(); i++) {
    if(i!=0)
      result += ":";
    result += doubleToString(seglr.getVX(i),2) + ",";
    result += doubleToString(seglr.getVY(i),2);
  }
  result += "#" + doubleToString(seglr.getRayAngle(),1);
  return(result);
}

//----------------------------------------------------------------
// Procedure: rotateSeglr

Seglr rotateSeglr(Seglr seglr, double degrees)
{
  if(seglr.size() == 0)
    return(seglr);

  Seglr new_seglr = seglr;

  double base_x = seglr.getVX(0);
  double base_y = seglr.getVY(0);

  for(unsigned int i=1; i<seglr.size(); i++) {
    double curr_x = seglr.getVX(i);
    double curr_y = seglr.getVY(i);
    double curr_angle = relAng(base_x, base_y, curr_x, curr_y);
    double curr_range = hypot(curr_x-base_x, curr_y-base_y);
    double new_angle = angle360(curr_angle + degrees);
    double new_x = 0;
    double new_y = 0;
    projectPoint(new_angle, curr_range, base_x, base_y, new_x, new_y);
    new_seglr.setVertex(new_x, new_y, i);
  }

  double curr_seglr_angle = seglr.getRayAngle();
  double new_seglr_angle = angle360(curr_seglr_angle + degrees);
  new_seglr.setRayAngle(new_seglr_angle);

  return(new_seglr);
}






