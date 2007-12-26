/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmReport.cpp                                       */
/*    DATE: Sep 26th, 2006                                       */
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

#include "HelmReport.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

HelmReport::HelmReport() 
{ 
  m_iteration       = 0;
  m_ofnum           = 0;
  m_create_time     = 0;
  m_solve_time      = 0;
  m_loop_time       = 0;
  m_halted          = false;
}

//-----------------------------------------------------------
// Procedure: addDecision()

void HelmReport::addDecision(const string& var, double val) 
{
  m_decision_var.push_back(var);
  m_decision_val.push_back(val);
}

//-----------------------------------------------------------
// Procedure: getDecision()

double HelmReport::getDecision(const string& var) 
{
  int vsize = m_decision_var.size();
  for(int i=vsize-1; i>=0; i--)
    if(m_decision_var[i] == var)
      return(m_decision_val[i]);
  
  return(0);
}

//-----------------------------------------------------------
// Procedure: hasDecision()

bool HelmReport::hasDecision(const string& var) 
{
  int vsize = m_decision_var.size();
  for(int i=vsize-1; i>=0; i--)
    if(m_decision_var[i] == var)
      return(true);
  
  return(false);
}


//-----------------------------------------------------------
// Procedure: addRunningBHV

void HelmReport::addRunningBHV(const std::string& descriptor)
{
  if(m_running_bhvs == "") {
    m_running_bhvs += "#";
    m_running_bhvs += intToString(m_iteration);
  }
  
  m_running_bhvs += ",";
  m_running_bhvs += descriptor;
}


//-----------------------------------------------------------
// Procedure: addActiveBHV

//   143:bhv_waypoint:100,bhv_avoid:200,bhv_opregion:100

void HelmReport::addActiveBHV(const std::string& descriptor, 
			      double pwt)
{
  if(m_active_bhvs == "") {
    m_active_bhvs += "#";
    m_active_bhvs += intToString(m_iteration);
  }
  
  m_active_bhvs += ",";
  m_active_bhvs += descriptor;
  m_active_bhvs += ":" + doubleToString(pwt, 2);
}


