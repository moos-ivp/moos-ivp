/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Timer.cpp                                        */
/*    DATE: Jul 22nd 2005                                        */
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

#include "BHV_Timer.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Timer::BHV_Timer(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  this->setParam("descriptor", "bhv_timer");

  m_var_status_idle    = "TIMER_IDLE";
  m_var_status_running = "TIMER_RUNNING";
  m_var_status_suffix  = "";
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Timer::setParam(string param, string val) 
{
  if((param == "var_status_idle") && !strContainsWhite(val))
    m_var_status_idle = val;

  else if((param == "var_status_running") && !strContainsWhite(val))
    m_var_status_running = val;

  else if((param == "status_suffix") && !strContainsWhite(val)) {
    if(val.at(0) != '_')
      val = "_" + val;
    m_var_status_suffix  = val;
  }
  else
    return(false);

  return(true);      
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_Timer::onIdleState() 
{
  string ivar = m_var_status_idle + m_var_status_suffix;
  postMessage(ivar, m_duration_idle_time);

  string rvar = m_var_status_running + m_var_status_suffix;
  postMessage(rvar, m_duration_running_time);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Timer::onRunState() 
{
  string ivar = m_var_status_idle + m_var_status_suffix;
  postMessage(ivar, m_duration_idle_time);

  string rvar = m_var_status_running + m_var_status_suffix;
  postMessage(rvar, m_duration_running_time);
  
  return(0);
}








