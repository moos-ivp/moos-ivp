/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_TimeOut.cpp                                      */
/*    DATE: Jul 27 2007 (pulled out from BHV_OpRegion)           */
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
#include <iostream>
#include <math.h> 
#include <stdlib.h>
#include "BHV_TimeOut.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_TimeOut::BHV_TimeOut(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_timeout");
  m_max_time = 0;

  // Keep a flag indicating whether this is the first time the
  // behavior is invoked.
  m_first_time   = true;

  // Time stamps for calculating how long the vehicle has been
  // inside or out of the polygon.
  m_previous_time = 0;
  m_current_time  = 0;
  m_elapsed_time  = 0;
  m_start_time    = 0;
  m_delta_time    = 0;
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_TimeOut::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  else if(param == "max_time") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_max_time = dval;
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState
//     Notes: Always returns NULL, never returns an IvPFunction*
//     Notes: Sets state_ok = false and posts an error message if
//            the max-time condition is violated.

IvPFunction *BHV_TimeOut::onRunState() 
{
  if(m_elapsed_time > m_max_time) {
    string emsg = "OpRegion timeout failure: MaxTime:";
    emsg += doubleToString(m_max_time);
    emsg += "  Elapsed Time: ";
    emsg += doubleToString(m_elapsed_time);
    postEMessage(emsg);
  }

  return(0);
}

//-----------------------------------------------------------
// Procedure: setTimeStamps()

void BHV_TimeOut::setTimeStamps()
{
  // Grab current time from Info Buffer
  m_current_time = getBufferCurrTime();
  
  //cout << "Current Time -    " << delta_time << endl;
  //cout << "Previous Time -    " << delta_time << endl;

  // Calculate the Delta time since this behavior was invoked.
  // The delta time is 0 on first invocation.
  if(m_first_time) {
    m_start_time = m_current_time;
    m_delta_time = 0;
    m_first_time = false;
  }
  else
    m_delta_time = m_current_time - m_previous_time;

  // No longer need to access previous time. Set it now for
  // access on the next invocation of this behavior.
  m_previous_time = m_current_time;

  m_elapsed_time = m_current_time - m_start_time;
}


