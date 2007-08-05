/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_AvoidObstacles.cpp                               */
/*    DATE: Aug 2nd, 2006                                        */
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
 
#include <math.h>
#include <string>
#include "AOF_AvoidObstacles.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_AvoidObstacles::AOF_AvoidObstacles(IvPDomain gdomain) : AOF(gdomain)
{
  crs_ix = gdomain.getIndex("course");

  os_x_set = false;
  os_y_set = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_AvoidObstacles::setParam(const string& param, double param_val)
{
  if(param == "os_y") {
    os_y = param_val;
    os_y_set = true;
    return(true);
  }
  if(param == "os_x") {
    os_x = param_val;
    os_x_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

void AOF_AvoidObstacles::addObstacle(XYPolygon poly, 
				     double out_dist, double in_dist)
{
  if((out_dist < 0) || (in_dist < 0))
    return;
  if(out_dist < in_dist)
    return;

  obstacles.push_back(poly);
  inner_dist.push_back(in_dist);
  outer_dist.push_back(out_dist);
}



//----------------------------------------------------------------
// Procedure: initialize

bool AOF_AvoidObstacles::initialize()
{
  if((crs_ix==-1))
    return(false);
  if((!os_x_set) || (!os_y_set))
    return(false);

  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_AvoidObstacles::evalBox(const IvPBox *b) const
{
}








