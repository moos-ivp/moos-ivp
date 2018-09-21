/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_ConstantHeading.cpp                              */
/*    DATE: Jul 21st 2005                                        */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <cmath> 
#include <cstdlib>
#include "BHV_ConstantHeading.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_ConstantHeading::BHV_ConstantHeading(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "bhv_constantHeading");

  m_domain = subDomain(m_domain, "course");

  m_desired_heading = 0;
  m_peakwidth       = 10;
  m_basewidth       = 170;
  m_summitdelta     = 25;
  m_os_heading      = 0;

  // The complete threshold represents an absolute discrepancy between 
  // the desired and actual heading, below which the behavior will 
  // complete and post an endflag. By default, by setting to -1, it will
  // not factor in to the performance of the behavior.
  m_complete_thresh = -1;  

  // The default duration at the IvPBehavior level is "-1", which
  // indicates no duration applied to the behavior by default. By
  // setting to zero here, we force the user to provide a duration
  // value otherwise it will timeout immediately.
  m_duration        = 0;
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_ConstantHeading::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);
  
  double dval = atof(val.c_str());

  if((param == "heading") && isNumber(val)) {
   double   angle_360 = angle360(dval);
   unsigned int index = m_domain.getDiscreteVal(0, angle_360, 2);

   double new_desired_heading = 0;
   bool ok = m_domain.getVal(0, index, new_desired_heading);
   if(ok) {
     m_desired_heading = new_desired_heading;
     return(true);
   }
  }
  else if((param == "peakwidth") && isNumber(val)) {
    m_peakwidth = vclip_min(dval, 0);
    return(true);
  }
  else if((param == "basewidth") && isNumber(val)) {
    m_basewidth = vclip_min(dval, 0);
    return(true);
  }
  else if((param == "summitdelta") && isNumber(val)) {
    m_summitdelta = vclip(dval, 0, 100);
    return(true);
  }
  else if((param == "complete_thresh") && isNumber(val)) {
    m_complete_thresh = dval;
    return(true);
  }
  else if((param == "heading_mismatch_var") && !strContainsWhite(val)) {
    m_heading_mismatch_var = val;
    return(true);
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState
//

IvPFunction *BHV_ConstantHeading::onRunState() 
{
  updateInfoIn();
  if(!m_domain.hasDomain("course")) {
    postEMessage("No 'heading/course' variable in the helm domain");
    return(0);
  }

  if(m_heading_delta < m_complete_thresh) {
    setComplete();
    return(0);
  }


  ZAIC_PEAK zaic(m_domain, "course");
  zaic.setSummit(m_desired_heading);
  zaic.setBaseWidth(m_basewidth);
  zaic.setPeakWidth(m_peakwidth);
  zaic.setSummitDelta(m_summitdelta);
  zaic.setValueWrap(true);
  
  IvPFunction *ipf = zaic.extractIvPFunction();
  if(ipf)
    ipf->setPWT(m_priority_wt);
  else 
    postEMessage("Unable to generate constant-heading IvP function");

  string zaic_warnings = zaic.getWarnings();
  if(zaic_warnings != "")
    postWMessage(zaic_warnings);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: updateInfoIn()
//   Purpose: Update relevant to the behavior from the info_buffer.
//            Warning messages may be posted if info is missing.
//   Returns: true if no relevant info is missing from the info_buffer.
//            false otherwise.

bool BHV_ConstantHeading::updateInfoIn()
{
  bool ok;
  m_os_heading = getBufferDoubleVal("NAV_HEADING", ok);

  // Should get ownship information from the InfoBuffer
  if(!ok) {
    postWMessage("No ownship HEADING info in info_buffer.");  
    return(false);
  }
  
  m_heading_delta = angle180(m_os_heading - m_desired_heading);
  if(m_heading_delta < 0)
    m_heading_delta *= -1; 

  if(m_heading_mismatch_var != "")
    postMessage(m_heading_mismatch_var, m_heading_delta);
  
  return(true);
}








