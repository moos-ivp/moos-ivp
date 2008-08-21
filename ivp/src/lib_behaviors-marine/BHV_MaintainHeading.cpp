/*****************************************************************/
/*    NAME: Michael Benjamin and Henrik Schmidt                  */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_MaintainHeading.cpp                              */
/*    DATE: August 7th 2008                                      */
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

#include <iostream>
#include <math.h> 
#include <stdlib.h>
#include "BHV_MaintainHeading.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_MaintainHeading::BHV_MaintainHeading(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "maintain-heading");

  m_domain = subDomain(m_domain, "course");

  m_desired_heading = 0;
  m_peakwidth       = 30;
  m_basewidth       = 150;
  m_summit_delta    = 80;

  m_running         = false;
  m_heading_on_active = true;

  // The default duration at the IvPBehavior level is "-1", which
  // indicates no duration applied to the behavior by default. By
  // setting to zero here, we force the user to provide a duration
  // value otherwise it will timeout immediately.
  m_duration        = 0;

  addInfoVars("NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_MaintainHeading::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  if(param == "heading") {
    if(tolower(val) == "on_active") {
      m_heading_on_active = true;
      return(true);
    }
    if(!isNumber(val))
      return(false);
    double dval = atof(val.c_str());
    if((dval < -180) || (dval >=360)) 
      postWMessage("Dubious initial heading provided:"+val);
    dval = angle360(dval);
    m_desired_heading = dval;
    return(true);
  }
  if(param == "peakwidth") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_peakwidth = dval;
    return(true);
  }
  if(param == "basewidth") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_basewidth = dval;
    return(true);
  }
  if(param == "summitdelta") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_summit_delta = dval;
    return(true);
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: onIdleState
//

void BHV_MaintainHeading::onIdleState() 
{
  m_running = false;

  // The below is a IvPBehavior variable that when set to false
  // effectively resets the timer
  m_started = false;
}


//-----------------------------------------------------------
// Procedure: onRunState
//

IvPFunction *BHV_MaintainHeading::onRunState() 
{
  bool ok;
  double current_heading = getBufferDoubleVal("NAV_HEADING", ok);
  if(!ok) {
    postWMessage("No Ownship NAV_HEADING in info_buffer");
    return(0);
  }
  
  if(m_running == false) {
    m_running = true;
    if(m_heading_on_active)
      m_desired_heading = angle360(current_heading);
  }
  
  ZAIC_PEAK zaic(m_domain, "course");
  zaic.setSummit(m_desired_heading);
  zaic.setBaseWidth(m_basewidth);
  zaic.setPeakWidth(m_peakwidth);
  zaic.setSummitDelta(m_summit_delta);
  zaic.setValueWrap(true);
  
  IvPFunction *ipf = zaic.extractOF();
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}










