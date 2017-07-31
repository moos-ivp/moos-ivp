/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_ConstantDepth.cpp                                */
/*    DATE: Jul 3rd 2005                                         */
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
#include "BHV_ConstantDepth.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "ZAIC_PEAK.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_ConstantDepth::BHV_ConstantDepth(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "bhv_constantdepth");

  m_domain = subDomain(m_domain, "depth");

  m_desired_depth = 0;

  // Default values changed by HS 110530
  m_peakwidth     = 3;
  m_basewidth     = 100;
  m_summitdelta   = 50;
  m_osd           = 0;

  // The default duration at the IvPBehavior level is "-1", which
  // indicates no duration applied to the behavior by default. By
  // setting to zero here, we force the user to provide a duration
  // value otherwise it will timeout immediately.
  m_duration      = 0;
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_ConstantDepth::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  double dval = atof(val.c_str());

  if((param == "depth") && isNumber(val)) {
    m_desired_depth = vclip_min(dval, 0);
    return(true);
  }

  else if((param == "peakwidth") && isNumber(val)) {
    m_peakwidth = vclip_min(dval, 0);
    return(true);
  }
  
  else if((param == "summitdelta") && isNumber(val)) {
    m_summitdelta = vclip(dval, 0, 100);
    return(true);
  }
  
  else if((param == "basewidth") && isNumber(val)) {
    m_basewidth = vclip_min(dval, 0);
    return(true);
  }

  else if((param == "depth_mismatch_var") && !strContainsWhite(val)) {
    m_depth_mismatch_var = val;
    return(true);
  }
  
  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_ConstantDepth::onRunState() 
{
  updateInfoIn();
  if(!m_domain.hasDomain("depth")) {
    postEMessage("No 'depth' variable in the helm domain");
    return(0);
  }

  ZAIC_PEAK zaic(m_domain, "depth");
  zaic.setSummit(m_desired_depth);
  zaic.setBaseWidth(m_basewidth);
  zaic.setPeakWidth(m_peakwidth);
  zaic.setSummitDelta(m_summitdelta);

  IvPFunction *ipf = zaic.extractIvPFunction();
  if(ipf)
    ipf->setPWT(m_priority_wt);
  else 
    postEMessage("Unable to generate constant-depth IvP function");

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

bool BHV_ConstantDepth::updateInfoIn()
{
  bool ok;
  m_osd = getBufferDoubleVal("NAV_DEPTH", ok);

  // Should get ownship information from the InfoBuffer
  if(!ok) {
    postWMessage("No ownship DEPTH info in info_buffer.");  
    return(false);
  }
  
  double delta = m_osd - m_desired_depth;
  if(delta < 0)
    delta *= -1; 

  if(m_depth_mismatch_var != "")
    postMessage(m_depth_mismatch_var, delta);
  
  return(true);
}








