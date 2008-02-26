/*****************************************************************/
/*    NAME: Henrik Schmidt                                       */
/*    ORGN: MIT Cambridge MA               */
/*    FILE: BHV_ConstantAltitude.cpp                                 */
/*    DATE: Mar. 3, 2007                                         */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <math.h> 
#include "BHV_ConstantAltitude.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_ConstantAltitude::BHV_ConstantAltitude(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_altitude");

  // Behavior Parameter Default Values:
  depth_local      = 0;   // meters
  altitude = 0;

  // Behavior State Variable Initial Values:

  m_domain = subDomain(m_domain, "depth");

  // Declare the variables we will need from the info_buffer
  addInfoVars("NAV_DEPTH");
  addInfoVars("NAV_SPEED");
  addInfoVars("NAV_ALTITUDE");

}

//-----------------------------------------------------------
// Procedure: onIdleState
//      Note: This function overrides the onIdleState() virtual
//            function defined for the IvPBehavior superclass
//            This function will be executed by the helm each
//            time the behavior FAILS to meet its run conditions.

void BHV_ConstantAltitude::onIdleState()
{
  // Do your thing here
   }


//-----------------------------------------------------------
// Procedure: setParam

bool BHV_ConstantAltitude::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParamCommon(g_param, g_val))
    return(true);

  g_val = stripBlankEnds(g_val);

  if(g_param == "altitude") 
    {
      double dval = atof(g_val.c_str());
      if((dval < 0) || (!isNumber(g_val)))
	return(false);
      altitude = dval;
      return(true);
    }
  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_ConstantAltitude::onRunState() 
{
  double curr_ang;
  double curr_time = getBufferCurrTime();
 
  bool   ok,ok1,ok2;
  double curr_depth = getBufferDoubleVal("NAV_DEPTH", ok);
  double curr_altitude = getBufferDoubleVal("NAV_ALTITUDE", ok1);
  double curr_speed = getBufferDoubleVal("NAV_SPEED", ok2);
  // check here for bottom altitude and change max depth if necessary
  double curr_water_depth = curr_depth + curr_altitude;
  depth_local = curr_water_depth-altitude;

  ZAIC_PEAK zaic_depth(m_domain, "depth");
  zaic_depth.setSummit(depth_local);
  zaic_depth.setPeakWidth(0.1*altitude);
  zaic_depth.setBaseWidth(0.0);
  zaic_depth.setSummitDelta(100.0);
  zaic_depth.setMinMaxUtil(0, 100);

  IvPFunction *ipf_depth = zaic_depth.extractOF();

  ipf_depth->setPWT(m_priority_wt);

  return(ipf_depth);
}

