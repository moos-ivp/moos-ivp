/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_HeadingBias.cpp                                  */
/*    DATE: Mar 26th 2009                                        */
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
#include "BHV_HeadingBias.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_HeadingBias::BHV_HeadingBias(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "bhv_headingbias");

  m_domain = subDomain(m_domain, "course");

  m_bias_right = true;
  m_os_heading = 0;
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_HeadingBias::setParam(string param, string value) 
{
  param = tolower(param);
  value = tolower(value);
  
  if(param == "bias") {
    if((value == "right") || (value == "left")) {
      m_bias_right = (value == "right");
      return(true);
    }
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState
//

IvPFunction *BHV_HeadingBias::onRunState() 
{
  if(!m_domain.hasDomain("course")) {
    postWMessage("No 'heading/course' variable in the helm domain");
    return(0);
  }
  bool ok = updateInfoIn();
  if(!ok) 
    return(0);

  double curr_heading_plus = m_os_heading + 90;
  if(curr_heading_plus >= 360)
    curr_heading_plus -= 360;

  ZAIC_PEAK zaic(m_domain, "course");
  zaic.setSummit(curr_heading_plus);
  zaic.setPeakWidth(180);
  zaic.setBaseWidth(0);
  zaic.setValueWrap(true);
  
  IvPFunction *ipf = zaic.extractIvPFunction();
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: updateInfoIn()
//   Purpose: Update info need by the behavior from the info_buffer.
//            Error or warning messages can be posted.
//   Returns: true if no vital info is missing from the info_buffer.
//            false otherwise.
//      Note: By posting an WMessage, this sets the state_ok member
//            variable to false which will communicate the gravity
//            of the situation to the helm.

bool BHV_HeadingBias::updateInfoIn()
{
  bool ok;
  m_os_heading = getBufferDoubleVal("NAV_HEADING", ok);
  
  // Must get ownship heading from InfoBuffer
  if(!ok) {
    postWMessage("No ownship HEADING info in info_buffer.");
    return(false);
  }

  return(true);
}











