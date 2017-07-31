/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_MaxDepth.cpp                                     */
/*    DATE: Apr 17th 2014                                        */
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

#include <iostream>
#include <cmath> 
#include <cstdlib>
#include "BHV_MaxDepth.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "ZAIC_LEQ.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_MaxDepth::BHV_MaxDepth(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "maxdepth");

  m_domain = subDomain(m_domain, "depth");

  m_max_depth = 0;
  m_basewidth = 100;
  m_osd       = 0;
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_MaxDepth::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  double dval = atof(val.c_str());

  if((param == "max_depth") && isNumber(val)) {
    m_max_depth = vclip_min(dval, 0);
    return(true);
  }

  else if((param == "basewidth") && isNumber(val)) {
    m_basewidth = vclip_min(dval, 0);
    return(true);
  }

  else if((param == "tolerance") && isNumber(val)) {
    m_basewidth = vclip_min(dval, 0);
    return(true);
  }

  else if((param == "depth_slack_var") && !strContainsWhite(val)) {
    m_depth_slack_var = val;
    return(true);
  }
  
  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_MaxDepth::onRunState() 
{
  updateInfoIn();
  if(!m_domain.hasDomain("depth")) {
    postEMessage("No 'depth' variable in the helm domain");
    return(0);
  }

  ZAIC_LEQ zaic(m_domain, "depth");
  zaic.setSummit(m_max_depth);
  zaic.setBaseWidth(m_basewidth);

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

bool BHV_MaxDepth::updateInfoIn()
{
  bool ok;
  m_osd = getBufferDoubleVal("NAV_DEPTH", ok);

  // Should get ownship information from the InfoBuffer
  if(!ok) {
    postWMessage("No ownship DEPTH info in info_buffer.");  
    return(false);
  }
  
  double slack = m_max_depth - m_osd;

  if(m_depth_slack_var != "")
    postMessage(m_depth_slack_var, slack);
  
  return(true);
}




