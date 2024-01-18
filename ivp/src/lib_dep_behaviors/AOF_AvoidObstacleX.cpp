/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_AvoidObstacleX.cpp                               */
/*    DATE: Sep 11, 2019                                         */
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
#include "AOF_AvoidObstacleX.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//----------------------------------------------------------
// Constructor()

AOF_AvoidObstacleX::AOF_AvoidObstacleX(IvPDomain gdomain) : AOF(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");
}

//----------------------------------------------------------------
// Procedure: setParam()

bool AOF_AvoidObstacleX::setParam(const string& param, double param_val)
{
  return(false);
}

//----------------------------------------------------------------
// Procedure: setParam()

bool AOF_AvoidObstacleX::setParam(const string& param, const string& value)
{
  return(false);
}

//----------------------------------------------------------------
// Procedure: initialize()

bool AOF_AvoidObstacleX::initialize()
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
  if(!m_obship_model.getObstacle().is_convex()) 
    return(postMsgAOF("m_obstacle is not convex"));
  if(m_obship_model.ownshipInObstacle())
    return(postMsgAOF("m_obstacle contains osx,osy"));

  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_AvoidObstacleX::evalBox(const IvPBox *b) const
{
  //===============================================================
  // Part 1: Declare utility range and get the eval crs and speed
  //===============================================================
  double min_util = getKnownMin();
  double max_util = getKnownMax();
  double eval_crs = 0;
  double eval_spd = 0;
  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);
  
  if(m_obship_model.osHdgInBasinMajor(eval_crs)) {
    if(((eval_crs >= 32) && (eval_crs <= 48)) &&
       ((eval_spd > 2.0) && (eval_spd < 5.0))) {
      cout << "eval_crs:" << eval_crs << ", eval_spd:" << eval_spd << endl;
      cout << " eval: " << min_util + 20 << endl;
      m_obship_model.osHdgInBasinMajor(eval_crs, true);
      cout << "******";
    }
    return(min_util+20);
  }
  if(m_obship_model.osHdgInPlatMajor(eval_crs))
    return(max_util);
  if(m_obship_model.osHdgSpdInBasinMinor(eval_crs, eval_spd))
    return(min_util+20);
  if(m_obship_model.osSpdInPlatMinor(eval_spd))
    return(max_util);

  double osx = m_obship_model.getOSX();
  double osy = m_obship_model.getOSY();

  XYPolygon obstacle = m_obship_model.getObstacle();
  XYPolygon obstacle_buff_min = m_obship_model.getObstacleBuffMin();
  XYPolygon obstacle_buff_max = m_obship_model.getObstacleBuffMax();
  
  double ix, iy;
  double cpa = polyRayCPA(osx, osy, eval_crs,
			  obstacle, ix, iy);

  // If CPA is greater than max_util_cpa, then just return the
  // max utility value. These cases are v. likely to be caught
  // by the "hdgInPlatMajor" case above. And if the refinery
  // is active, v. likely this crs/spd pair would never even
  // make it to this function call.
  if(cpa > m_obship_model.getMaxUtilCPA())
    return(max_util);

  double dist_to_poly = obstacle_buff_max.dist_to_poly(osx, osy);
  double time_to_dist = dist_to_poly / eval_spd;
  if(time_to_dist > m_obship_model.getAllowableTTC())
    return(max_util);

  // If the ttc does not allow us to ignore this maneuver, and
  // the cpa is less than min utility cpa, just return min util
  if(cpa <= m_obship_model.getMinUtilCPA())
    return(min_util);
  
  double util_range = (m_obship_model.getMaxUtilCPA() - 
		       m_obship_model.getMinUtilCPA());
  // Sanity check - the util_range, if it were indeed zero, would
  // have resulted in one of the other return conditions, but we
  // check here anyway just to ensure no division by zero.
  if(util_range <= 0)
    return(min_util);
  
  double delta = cpa - m_obship_model.getMinUtilCPA();
  double pct = delta / util_range;

  // Sanity check - pct should be in [0,1], due to above logic
  // but sanity check anyway
  if(pct < 0)
    pct = 0;
  else if(pct > 1)
    pct = 1;

  return(pct * (max_util - min_util));
}

