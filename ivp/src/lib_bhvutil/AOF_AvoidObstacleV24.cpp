/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng, MIT Cambridge MA             */
/*    FILE: AOF_AvoidObstacleV24.cpp                             */
/*    DATE: Sep 11, 2019                                         */
/*    DATE: Aug 05, 2023 +TurnModel                              */
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
#include <string>
#include "AOF_AvoidObstacleV24.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//----------------------------------------------------------
// Constructor()

AOF_AvoidObstacleV24::AOF_AvoidObstacleV24(IvPDomain gdomain) :
  AOF(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");
}

//----------------------------------------------------------------
// Procedure: setParam()

bool AOF_AvoidObstacleV24::setParam(const string& param, double val)
{
  return(false);
}

//----------------------------------------------------------------
// Procedure: setParam()

bool AOF_AvoidObstacleV24::setParam(const string& param,
				    const string& value)
{
  return(false);
}

//----------------------------------------------------------------
// Procedure: initialize()

bool AOF_AvoidObstacleV24::initialize()
{
  // Part 1: Sanity Checks
  if(m_crs_ix == -1) 
    return(postMsgAOF("crs_ix is not set"));
  if(m_spd_ix == -1) 
    return(postMsgAOF("spd_ix is not set"));
  if(!m_obship_model.paramIsSet("osx")) 
    return(postMsgAOF("osx is not set"));
  if(!m_obship_model.paramIsSet("osy")) 
    return(postMsgAOF("osy is not set"));
  if(!m_obship_model.paramIsSet("osh")) 
    return(postMsgAOF("osh is not set"));
  if(!m_obship_model.paramIsSet("min_util_cpa"))
    return(postMsgAOF("min_util_cpa is not set"));
  if(!m_obship_model.paramIsSet("max_util_cpa")) 
    return(postMsgAOF("max_util_cpa is not set"));
  if(!m_obship_model.paramIsSet("allowable_ttc")) 
    return(postMsgAOF("allowable_ttc is not set"));
  if(!m_obship_model.getGutPoly().is_convex()) 
    return(postMsgAOF("m_obstacle is not convex"));
  if(m_obship_model.ownshipInGutPoly())
    return(postMsgAOF("m_obstacle contains osx,osy"));

  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_AvoidObstacleV24::evalBox(const IvPBox *b) const
{
  //===============================================================
  // Part 1: Declare utility range and get the eval crs and speed
  //===============================================================
  //double min_util = getKnownMin();
  //double max_util = getKnownMax();
  double eval_crs = 0;
  double eval_spd = 0;
  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);

  return(m_obship_model.evalHdgSpd(eval_crs, eval_spd));
}
