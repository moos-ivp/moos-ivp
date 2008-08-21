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
  
  vsize = m_decision_var.size();
  for(i=0; i<vsize; i++) {
    report += ",var=";
    report += m_decision_var[i];
    report += ":";
    report += doubleToString(m_decision_val[i], 1);
  }

  // Now check to see if the helm did not produce a decision for one
  // of the variables in the originally declared domain for the helm
  int dsize = m_domain.size();
  for(i=0; i<dsize; i++) {
    bool found = false;
    string varname = m_domain.getVarName(i);
    for(j=0; j<vsize; j++) { 
      if(varname == m_decision_var[j])
	found = true;
    }
    if(!found)
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


