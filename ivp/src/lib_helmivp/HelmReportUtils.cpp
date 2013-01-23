/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmReport.cpp                                       */
/*    DATE: Oct0511                                              */
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

#include <cstdlib>
#include "HelmReportUtils.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: string2HelmReport(string)

HelmReport string2HelmReport(const string& str)
{
  HelmReport empty_report;
  return(string2HelmReport(str, empty_report));
}


//-----------------------------------------------------------
// Procedure: string2HelmReport(string, HelmReport)
//
//   Example: iter=231,
//            ofnum=3,
//            warnings=0,
//            solve_time=0.01,
//            create_time=0.0,    
//            loop_time=0.01,    
//            utc_time=131223429183.22,    
//            var=speed:2,var=course:124,
//            halted=false,
//            modes=MODE@ACTIVE:LOITERING
//            halt_msg=
// 
//            active_bhvs=waypt_survey$1343667403.10$100.00000$6$0.00000$n/a$1,
//            idle_bhvs=waypt_return$0.00$n/a
// 
//  Example = iter=1,utc_time=1343667403.10,ofnum=1,var=speed:2,var=course:124,
//            active_bhvs=waypt_survey$1343667403.10$100.00000$6$0.00000$n/a$1,
//            idle_bhvs=waypt_return$0.00$n/a

HelmReport string2HelmReport(const string& str, 
			     const HelmReport& prior_report)
{
  HelmReport report = prior_report;

  bool new_decision = false;

  vector<string> svector = parseStringQ(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = biteStringX(svector[i], '=');
    string right = svector[i];

    if(left == "iter") 
      report.setIteration(atoi(right.c_str()));
    else if(left == "ofnum")
      report.setOFNUM(atoi(right.c_str()));
    else if(left == "warnings")
      report.setWarningCount(atoi(right.c_str()));
    else if(left == "solve_time")
      report.setSolveTime(atof(right.c_str()));
    else if(left == "create_time")
      report.setCreateTime(atof(right.c_str()));
    else if(left == "max_create_time")
      report.setMaxCreateTime(atof(right.c_str()));
    else if(left == "max_solve_time")
      report.setMaxSolveTime(atof(right.c_str()));
    else if(left == "max_loop_time")
      report.setMaxLoopTime(atof(right.c_str()));

    else if(left == "utc_time")
      report.setTimeUTC(atof(right.c_str()));
    else if(left == "var") {
      if(!new_decision) {
	report.clearDecisions();
	new_decision = true;
      }
      string var = biteStringX(right,':');
      double val = atof(right.c_str());
      report.addDecision(var, val);
    }      
    else if(left == "halted")
      report.setHalted((right == "true"));
    else if(left == "modes")
      report.setModeSummary(right);
    else if(left == "ivpdomain") {
      right = stripQuotes(right);
      IvPDomain domain = stringToDomain(right);
      report.setIvPDomain(domain);
    }
    else if(left == "active_bhvs") {
      report.clearActiveBHVs();
      string descriptor = biteStringX(right, '$');
      string time = biteStringX(right, '$');
      string pwt  = biteStringX(right, '$');
      string pcs  = biteStringX(right, '$');
      string cpu  = biteStringX(right, '$');
      string upds = biteStringX(right, '$');
      string ipfs = right;

      double d_time = atof(time.c_str());
      double d_pwt  = atof(pwt.c_str());
      int    i_pcs  = atoi(pcs.c_str());
      double d_cpu  = atof(cpu.c_str());
      unsigned int u_ipfs = (unsigned int)(atoi(ipfs.c_str()));      
      report.addActiveBHV(descriptor, d_time, d_pwt, i_pcs, d_cpu, upds, u_ipfs);
    }
    
    else if(left == "running_bhvs") {
      report.clearRunningBHVs();
      string descriptor = biteStringX(right, '$');
      string time = biteStringX(right, '$');
      string upds = biteStringX(right, '$');
      double d_time = atof(time.c_str());
      report.addRunningBHV(descriptor, d_time, upds);
    }

    else if(left == "idle_bhvs") {
      report.clearIdleBHVs();
      string descriptor = biteStringX(right, '$');
      string time = biteStringX(right, '$');
      string upds = biteStringX(right, '$');
      double d_time = atof(time.c_str());
      report.addIdleBHV(descriptor, d_time, upds);
    }

    else if(left == "completed_bhvs") {
      if(strContains(report.getCompletedBehaviors(), "none"))
	report.clearCompletedBHVs();
      string descriptor = biteStringX(right, '$');
      string time = biteStringX(right, '$');
      string upds = biteStringX(right, '$');
      double d_time = atof(time.c_str());
      report.addCompletedBHV(descriptor, d_time, upds);
    }

    else if(left == "halt_msg")
      report.setHaltMsg(right);
  }

  return(report);
}



