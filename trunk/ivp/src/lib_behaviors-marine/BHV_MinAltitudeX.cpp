/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_MinAltitudeX.cpp                                 */
/*    DATE: Mar 3rd, 2007                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <cmath> 
#include <cstdlib>
#include "BHV_MinAltitudeX.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ZAIC_LEQ.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_MinAltitudeX::BHV_MinAltitudeX(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  m_descriptor = "min_altitude";
  m_domain     = subDomain(m_domain, "depth");

  // Behavior Parameter Default Values:
  m_min_altitude  = 0;  // meters
  m_missing_altitude_critical = true;

  // Behavior State Variable Initial Values:
  m_curr_depth    = 0;  // meters
  m_curr_altitude = 0;  // meters

  // Declare the variables we will need from the info_buffer
  addInfoVars("NAV_DEPTH");
  addInfoVars("NAV_ALTITUDE");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_MinAltitudeX::setParam(string param, string param_val) 
{
  if(IvPBehavior::setParamCommon(param, param_val))
    return(true);

  param = tolower(param);

  if(param == "min_altitude") {
    double dval = atof(param_val.c_str());
    if((dval < 0) || (!isNumber(param_val)))
      return(false);
    m_min_altitude = dval;
    return(true);
  }
  else if(param == "missing_altitude_critical")
    return(setBooleanOnString(m_missing_altitude_critical, param_val));
  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_MinAltitudeX::onRunState() 
{
  if(!updateInfoIn())
    return(0);

  if(m_curr_altitude > 0) {

    // check here for bottom altitude and change max depth if necessary
    double curr_water_depth = m_curr_depth + m_curr_altitude;
    m_curr_max_depth  = curr_water_depth - m_min_altitude;

    if(m_curr_max_depth < 0)
      m_curr_max_depth = 0;

    ZAIC_LEQ zaic_depth(m_domain, "depth");
    zaic_depth.setSummit(m_curr_max_depth);
    zaic_depth.setBaseWidth(0);
      
    IvPFunction *ipf_depth = zaic_depth.extractOF();
    
    if(ipf_depth)
      ipf_depth->setPWT(m_priority_wt);
      
    return(ipf_depth);
  }
  return(0);
}

//-----------------------------------------------------------
// Procedure: updateInfoIn

bool BHV_MinAltitudeX::updateInfoIn() 
{
  bool   ok1, ok2;

  m_curr_depth    = getBufferDoubleVal("NAV_DEPTH", ok1);
  m_curr_altitude = getBufferDoubleVal("NAV_ALTITUDE", ok2);
  
  if(!ok1) {
    postEMessage("No ownship NAV_DEPTH info in the helm info_buffer");
    postMessage("MIN_ALT_STATUS", "Missing NAV_DEPTH info");
    return(false);
  }

  if(!ok2) {
    if(m_missing_altitude_critical)
      postEMessage("No ownship NAV_ALTITUDE info in the helm info_buffer");
    else {
      postEMessage("No ownship NAV_ALTITUDE info in the helm info_buffer");
      m_curr_altitude = 0;
    }
    postMessage("MIN_ALT_STATUS", "Missing NAV_ALTITUDE info");
    return(false);
  }

  return(true);
}


//-----------------------------------------------------------
// Procedure: postStatusReport

void BHV_MinAltitudeX::postStatusReport() 
{
  if(m_curr_max_depth == 0) 
    postWMessage("Min_Altitude allowed is zero");
  
  string info = doubleToString(m_curr_max_depth, 0);
  if(m_curr_max_depth < 10)
    info = doubleToString(m_curr_max_depth, 1);

  postMessage("MIN_ALT_STATUS", info);
}










