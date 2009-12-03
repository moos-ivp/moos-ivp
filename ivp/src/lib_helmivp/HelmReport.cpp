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
  m_iteration     = 0;
  m_ofnum         = 0;
  m_time_utc      = 0;
  m_warning_count = 0;
  m_create_time   = 0;
  m_solve_time    = 0;
  m_loop_time     = 0;
  m_halted        = false;
}

//-----------------------------------------------------------
// Procedure: addDecision()

void HelmReport::addDecision(const string& var, double val) 
{
  m_decisions[var] = val;
}

//-----------------------------------------------------------
// Procedure: setDecision()

void HelmReport::setDecision(const string& var, double val) 
{
  m_decisions[var] = val;
}

//-----------------------------------------------------------
// Procedure: getDecision()

double HelmReport::getDecision(const string& var) 
{
  map<string, double>::iterator p;
  p = m_decisions.find(var);
  double result = 0;
  if(p != m_decisions.end())
    result = p->second;
  return(result);
}

//-----------------------------------------------------------
// Procedure: hasDecision()

bool HelmReport::hasDecision(const string& var) 
{
  map<string, double>::iterator p = m_decisions.find(var);
  if(p != m_decisions.end())
    return(true);
  else
    return(false);
}

//-----------------------------------------------------------
// Procedure: delDecision()

void HelmReport::delDecision(const string& var) 
{
  m_decisions.erase(var);
}


//-----------------------------------------------------------
// Procedure: addRunningBHV
//            addIdleBHV
//            addCompletedBHV

void HelmReport::addRunningBHV(const string& descriptor, double time, 
			       const string& update_summary)
{
  if(m_running_bhvs != "")
    m_running_bhvs += ":";
  m_running_bhvs += descriptor;
  m_running_bhvs += "$" + doubleToString(time, 1);
  m_running_bhvs += "$" + update_summary;
}

void HelmReport::addIdleBHV(const string& descriptor, double time,
			    const string& update_summary)
{
  if(m_idle_bhvs != "")
    m_idle_bhvs += ":";
  m_idle_bhvs += descriptor;
  m_idle_bhvs += "$" + doubleToString(time, 1);
  m_idle_bhvs += "$" + update_summary;
}

void HelmReport::addCompletedBHV(const string& descriptor, double time,
				 const string& update_summary)
{
  if(m_completed_bhvs != "")
    m_completed_bhvs += ":";
  m_completed_bhvs += descriptor;
  m_completed_bhvs += "$" + doubleToString(time, 1);
  m_completed_bhvs += "$" + update_summary;
}

//-----------------------------------------------------------
// Procedure: addActiveBHV

//   bhv_waypoint:100,bhv_avoid:200,bhv_opregion:100

void HelmReport::addActiveBHV(const string& descriptor, 
			      double time, double pwt, 
			      int pcs, double cpu_time, 
			      const string& update_summary)
{
  if(m_active_bhvs != "")
    m_active_bhvs += ":";
  
  m_active_bhvs += descriptor;
  m_active_bhvs += "$" + doubleToString(time, 1);
  m_active_bhvs += "$" + doubleToString(pwt, 2);
  m_active_bhvs += "$" + intToString(pcs);
  m_active_bhvs += "$" + doubleToString(cpu_time,2);
  m_active_bhvs += "$" + update_summary;
}


//-----------------------------------------------------------
// Procedure: getRunningBehaviors()

string HelmReport::getRunningBehaviors()
{
  string result = "#";
  result += intToString(m_iteration);
  result += ":";
  result += m_running_bhvs;

  return(result);
}

//-----------------------------------------------------------
// Procedure: getActiveBehaviors()

string HelmReport::getActiveBehaviors()
{
  string result = "#";
  result += intToString(m_iteration);
  result += ":";
  result += m_active_bhvs;
  
  return(result);
}

//-----------------------------------------------------------
// Procedure: getReportAsString()

string HelmReport::getReportAsString()
{
  int i, j, vsize;
  string report;

  report += ("iter=" + intToString(m_iteration));
  report += (",ofnum=" + intToString(m_ofnum));
  report += (",warnings=" + intToString(m_warning_count));
  report += (",utc_time=" + doubleToString(m_time_utc, 2));
  report += (",solve_time=" + doubleToString(m_solve_time, 2));
  report += (",create_time=" + doubleToString(m_create_time, 2));
  report += (",loop_time=" + doubleToString(m_loop_time, 2));
  
  map<string, double>::iterator p;
  for(p=m_decisions.begin(); p!=m_decisions.end(); p++) {
    string var = p->first;
    double val = p->second;
    report += ",var=" + var + ":";
    report += doubleToString(val, 1);
  }

  // Now check to see if the helm did not produce a decision for one
  // of the variables in the originally declared domain for the helm
  int dsize = m_domain.size();
  for(i=0; i<dsize; i++) {
    string varname = m_domain.getVarName(i);
    if(!hasDecision(varname))
      report += (",var=" + varname + ":varbalk");
  }

  if(m_halted)
    report += ",halted=true";
  else
    report += ",halted=false";

  report += ",running_bhvs=";
  if(m_running_bhvs == "")
    report += "none";
  else
    report += m_running_bhvs;

  report += ",modes=";
  if(m_modes == "")
    report += "none";
  else
    report += m_modes;

  report += ",active_bhvs=";
  if(m_active_bhvs == "")
    report += "none";
  else
    report += m_active_bhvs;

  report += ",idle_bhvs=";
  if(m_idle_bhvs == "")
    report += "none";
  else
    report += m_idle_bhvs;
  
  report += ",completed_bhvs=";
  if(m_completed_bhvs == "")
    report += "none";
  else
    report += m_completed_bhvs;

  return(report);
}


