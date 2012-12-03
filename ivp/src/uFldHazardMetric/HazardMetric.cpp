/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardMetric.cpp                                     */
/*    DATE: March 12th, 2012                                     */
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

#include <iterator>
#include "MBUtils.h"
#include "ColorParse.h"
#include "FileBuffer.h"
#include "HazardMetric.h"
#include "XYHazardRepEval.h"
#include "XYFormatUtilsHazard.h"
#include "XYFormatUtilsHazardSet.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor

HazardMetric::HazardMetric()
{
  // Initialized configuration variables
  // Default Reward Structure
  m_penalty_false_alarm      = 10;
  m_penalty_missed_hazard    = 100; 
  m_penalty_max_time_over    = 0;
  m_penalty_max_time_overage = 0;

  // A max_time=0 means there is no mission time limit 
  m_max_time = 0;

  m_search_start_time = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool HazardMetric::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;  
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
    
    if(key == "HAZARD_REPORT")
      addHazardReport(sval);
    else if(key == "HAZARD_SEARCH_START")
      m_search_start_time = MOOSTime() - m_start_time;
    else 
      reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool HazardMetric::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool HazardMetric::Iterate()
{
  AppCastingMOOSApp::Iterate();
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool HazardMetric::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());
  
  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;
    
    bool handled = false;
    if((param == "PENALTY_MISSED_HAZARD") && isNumber(value)) {
      m_penalty_missed_hazard = atof(value.c_str());
      handled = true;
    }
    else if((param == "PENALTY_FALSE_ALARM") && isNumber(value)) {
      m_penalty_false_alarm = atof(value.c_str());
      handled = true;
    }
    else if((param == "PENALTY_MAX_TIME_OVER") && isNumber(value)) {
      m_penalty_max_time_over = atof(value.c_str());
      handled = true;
    }
    else if((param == "PENALTY_MAX_TIME_OVERAGE") && isNumber(value)) {
      m_penalty_max_time_overage = atof(value.c_str());
      handled = true;
    }
    else if((param == "MAX_TIME") && isNumber(value)) {
      m_max_time = atof(value.c_str());
      handled = true;
    }
    else if(param == "HAZARD_FILE")
      handled = processHazardFile(value);
    
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void HazardMetric::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  m_Comms.Register("HAZARD_REPORT", 0);
  m_Comms.Register("HAZARD_SEARCH_START", 0);
}


//------------------------------------------------------------
// Procedure: processHazardFile

bool HazardMetric::processHazardFile(string filename)
{
  string msg = "Reading " + filename + ": ";

  vector<string> lines = fileBuffer(filename);
  unsigned int i, vsize = lines.size();
  if(vsize == 0) {
    reportRunWarning(msg + "Empty or File Not found.");
    return(false);
  }
  else
    m_hazard_file = filename;

  XYHazardSet hazard_set;

  for(i=0; i<vsize; i++) {
    string orig  = lines[i];
    lines[i] = stripBlankEnds(stripComment(lines[i], "//"));
    string left  = biteStringX(lines[i], '=');
    string right = lines[i];
    if(left == "source")
      hazard_set.setSource(right);
    else if(left == "hazard") {
      XYHazard new_hazard = string2Hazard(right);
      if(new_hazard.valid()) 
	hazard_set.addHazard(new_hazard);
      else
	reportConfigWarning(msg + "bad hazard file entry: " + orig);
    }
    else if(left != "")
      reportConfigWarning("strange hazard file entry: " + orig);
    
  }
  
  m_hazards = hazard_set;
  reportEvent(msg + "Objects read: " + uintToString(m_hazards.size()));

  if(m_hazards.getSource() == "")
    m_hazards.setSource(filename);
  
  return(true);
}


//------------------------------------------------------------
// Procedure: addHazardReport

bool HazardMetric::addHazardReport(string report_str)
{
  XYHazardSet hazard_set = string2HazardSet(report_str);
  
  string src = hazard_set.getSource();
  
  if(src == "") {
    reportRunWarning("Rejected HAZARD_REPORT: Missing Source info");
    return(false);
  }
  
  if(src == "actual") {
    reportRunWarning("Rejected HAZARD_REPORT: Source claims to be actual");
    return(false);
  }

  if(hazard_set.size() == 0) {
    reportRunWarning("Warning: HAZARD_REPORT of size ZERO received");
    return(false);
  }

  m_map_reports[src] = hazard_set;
  m_map_report_amt[src]++;

  m_most_recent_source = src;
  
  reportEvent("Received valid report from: " + src);

  evaluateReport(hazard_set);
  return(true);
}

//------------------------------------------------------------
// Procedure: evaluateReport

void HazardMetric::evaluateReport(const XYHazardSet& src_hset)
{
  string source = src_hset.getSource();

  double total_score = 0;
  string object_report;

  // Part 1: Tally up the correct hazards, missed hazards and false alarms
  unsigned int correct_hazards = 0;
  unsigned int missed_hazards  = 0;
  unsigned int false_alarms    = 0;
  unsigned int i, vsize = m_hazards.size();
  for(i=0; i<vsize; i++) {
    XYHazard ihaz   = m_hazards.getHazard(i);
    string   ilabel = ihaz.getLabel();
    string   itype  = ihaz.getType();

    string   detailed_rep = "label=" + ilabel + ",truth=" + itype;
    bool     haz_reported = src_hset.hasHazard(ilabel);

    // Ground truth hazard: not reported as hazard (missed_hazard)
    if(!haz_reported) {
      detailed_rep += ",report=nothing";
      if(itype == "hazard") {
	missed_hazards++;
	detailed_rep += ",penalty=" + doubleToStringX(m_penalty_missed_hazard,2);
      }
    }
    // Ground truth non-hazard: reported as hazard (false alarm)
    else {
      detailed_rep += ",report=hazard";
      if(itype != "hazard") {
	false_alarms++;
	detailed_rep += ",penalty=" + doubleToStringX(m_penalty_false_alarm,2);
      }
      else
	correct_hazards++;
    }

    if(object_report != "")
      object_report += "#";
    object_report += detailed_rep;
  }

  // Part 2: Tally up the penalties for missed hazards and false alarms.
  double score_missed_hazards = (missed_hazards * m_penalty_missed_hazard);
  double score_false_alarms   = (false_alarms   * m_penalty_false_alarm);
  total_score += score_missed_hazards;
  total_score += score_false_alarms;

  // Part 3: Tally up penalties (if any) based on search time.
  double elapsed_time = m_curr_time - m_search_start_time;
  // If search start time was not marked, no elapsed time.
  if(m_search_start_time == 0)
    elapsed_time = 0;
  double overage = elapsed_time - m_max_time;

  // If no max time set, no overage
  if(m_max_time == 0)
    overage = 0;

  double score_time_overage = 0;
  if(overage > 0) {
    score_time_overage += m_penalty_max_time_over;
    score_time_overage += (m_penalty_max_time_overage * overage);
  }
  total_score += score_time_overage;

  // Part 4: Fill in the report eval data structure
  XYHazardRepEval eval;
  eval.setVName(source);
  eval.setTotalScore(total_score);
  eval.setScoreMissedHazards(score_missed_hazards);
  eval.setScoreFalseAlarms(score_false_alarms);
  eval.setScoreTimeOverage(score_time_overage);

  eval.setTotalObjects(src_hset.size());
  eval.setCorrectHazards(correct_hazards);
  eval.setMissedHazards(missed_hazards);
  eval.setFalseAlarms(false_alarms);
  eval.setTotalTime(elapsed_time);
  eval.setReceivedTime(m_curr_time - m_start_time);
  eval.setStartTime(m_search_start_time);

  eval.setPenaltyFalseAlarm(m_penalty_false_alarm);
  eval.setPenaltyMissedHazard(m_penalty_missed_hazard);
  eval.setPenaltyMaxTimeOver(m_penalty_max_time_over);
  eval.setPenaltyMaxTimeOverage(m_penalty_max_time_overage);
  eval.setMaxTime(m_max_time);
  eval.setObjectReport(object_report);

  m_map_evals[source] = eval;

  m_Comms.Notify("REPORT_FULL_EVAL", eval.getFullSpec());
  m_Comms.Notify("REPORT_FULL_EVAL_"+toupper(source), eval.getFullSpec());

  m_Comms.Notify("REPORT_EVAL", eval.getShortSpec());
  m_Comms.Notify("REPORT_EVAL_"+toupper(source), eval.getShortSpec());
}



//------------------------------------------------------------
// Procedure: buildReport()

bool HazardMetric::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "Hazard File: (" + m_hazard_file + ")"         << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "     Hazard: " + m_hazards.getHazardCnt() << endl;
  m_msgs << "     Benign: " + m_hazards.getBenignCnt() << endl;
  m_msgs << endl;  
  m_msgs << "==========================================" << endl;
  m_msgs << "Received Reports:"                          << endl;
  m_msgs << "==========================================" << endl;;

  ACTable actab(5);
  actab << "Vehicle | Total   |       | Time | Time   ";
  actab << "Name    | Reports | Score | Recd | Elapsed";
  actab.addHeaderLines();
  map<string, XYHazardRepEval>::iterator p;
  for(p=m_map_evals.begin(); p!=m_map_evals.end(); p++) {
    string vname = p->first;
    XYHazardRepEval eval = p->second;
    string amt   = uintToString(m_map_report_amt[vname]);

    string score      = doubleToStringX(eval.getTotalScore());
    string time_reced = doubleToString(eval.getReceivedTime(),2);
    string time_total = doubleToString(eval.getTotalTime(),2);
    actab << vname << amt << score << time_reced << time_total;
  }
  m_msgs << actab.getFormattedString();

  if(m_map_evals.count(m_most_recent_source)) {
    XYHazardRepEval eval = m_map_evals[m_most_recent_source];
    m_msgs << endl;
    m_msgs << "=========================================="         << endl;
    m_msgs << "Most Recent Report: (" + m_most_recent_source + ")" << endl;
    m_msgs << "=========================================="         << endl;
    m_msgs << eval.getFormattedString();
  }

  return(true);
}


