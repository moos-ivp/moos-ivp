/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmReport.cpp                                       */
/*    DATE: Oct0511                                              */
/*****************************************************************/

#include <cstdlib>
#include "HelmReportUtils.h"
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
    
HelmReport string2HelmReport(const string& str, 
			     const HelmReport& prior_report)
{
  HelmReport report = prior_report;

  bool new_decision = false;

  vector<string> svector = parseString(str, ',');
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
    else if(left == "active_bhvs")
      report.setActiveBHVs(right);
    else if(left == "running_bhvs")
      report.setRunningBHVs(right);
    else if(left == "idle_bhvs")
      report.setIdleBHVs(right);
    else if(left == "completed_bhvs")
      report.setCompletedBHVs(right);
    else if(left == "halt_msg")
      report.setHaltMsg(right);
  }

  return(report);
}



