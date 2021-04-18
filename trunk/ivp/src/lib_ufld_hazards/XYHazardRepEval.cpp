/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYHazardRepEval.cpp                                  */
/*    DATE: Oct 29, 2012                                         */
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

#include <sstream>
#include <cstdlib>
#include "XYHazardRepEval.h"
#include "MBUtils.h"
#include "GeomUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

XYHazardRepEval::XYHazardRepEval()
{
  m_total_score          = 0;
  m_norm_score           = 0;
  m_score_missed_hazards = 0;
  m_score_false_alarms   = 0;
  m_score_time_overage   = 0;

  m_total_objects   = 0;
  m_missed_hazards  = 0;
  m_correct_hazards = 0;
  m_false_alarms    = 0;
  m_total_time      = 0;
  m_received_time   = 0;
  m_start_time      = 0;

  m_max_time = 0;
  m_penalty_false_alarm   = 0;
  m_penalty_missed_hazard = 0;
  m_penalty_max_time_over = 0;
  m_penalty_max_time_rate = 0;
}

  
//-----------------------------------------------------------
// Procedure: getShortSpec()

string XYHazardRepEval::getShortSpec() const
{
  string str = "vname=" + m_vname;

  if(m_report_name != "")
    str += ",report_name=" + m_report_name;

  str += ",total_score="    + doubleToStringX(m_total_score,2);
  str += ",norm_score="    + doubleToStringX(m_norm_score,2);
  str += ",score_missed_hazards=" + doubleToStringX(m_score_missed_hazards);
  str += ",score_false_alarms=" + doubleToStringX(m_score_false_alarms);
  str += ",score_time_overage=" + doubleToStringX(m_score_time_overage,2);

  str += ",total_objects="  + uintToString(m_total_objects);
  str += ",total_time="     + doubleToStringX(m_total_time);
  str += ",received_time="  + doubleToStringX(m_received_time);
  str += ",start_time="     + doubleToStringX(m_start_time);
  str += ",missed_hazards=" + uintToString(m_missed_hazards);
  str += ",correct_hazards=" + uintToString(m_correct_hazards);
  str += ",false_alarms="   + uintToString(m_false_alarms);

  str += ",penalty_false_alarm="   + doubleToStringX(m_penalty_false_alarm);
  str += ",penalty_missed_hazard=" + doubleToStringX(m_penalty_missed_hazard);
  str += ",penalty_max_time_over=" + doubleToStringX(m_penalty_max_time_over);
  str += ",penalty_max_time_rate=" + doubleToStringX(m_penalty_max_time_rate);
  str += ",max_time=" + doubleToStringX(m_max_time);

  return(str);
}


//-----------------------------------------------------------
// Procedure: getFullSpec()

string XYHazardRepEval::getFullSpec() const
{
  string str = getShortSpec();
  str += ",object_report={" + m_object_report + "}";
  
  return(str);
}


//-----------------------------------------------------------
// Procedure: buildFromSpec()

bool XYHazardRepEval::buildFromSpec(const string& str) 
{
  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();

  bool all_handled = true;
  for(i=0; i<vsize; i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    
    if(param == "vname")
      m_vname = value;
    else if(param == "total_score")
      m_total_score = atof(value.c_str());
    else if(param == "norm_score")
      m_norm_score = atof(value.c_str());
    else if(param == "score_missed_hazards")
      m_score_missed_hazards = atof(value.c_str());
    else if(param == "score_false_alarms")
      m_score_false_alarms = atof(value.c_str());
    else if(param == "score_time_overage")
      m_score_time_overage = atof(value.c_str());

    else if(param == "total_objects")
      m_total_objects = atoi(value.c_str());
    else if(param == "total_time")
      m_total_time = atof(value.c_str());
    else if(param == "received_time")
      m_received_time = atof(value.c_str());
    else if(param == "start_time")
      m_start_time = atof(value.c_str());
    else if(param == "missed_hazards")
      m_missed_hazards = atoi(value.c_str());
    else if(param == "correct_hazards")
      m_correct_hazards = atoi(value.c_str());
    else if(param == "false_alarms")
      m_false_alarms = atoi(value.c_str());

    else if(param == "penalty_false_alarm")
      m_penalty_false_alarm = atof(value.c_str());
    else if(param == "penalty_missed_hazard")
      m_penalty_missed_hazard = atof(value.c_str());
    else if(param == "penalty_max_time_over")
      m_penalty_max_time_over = atof(value.c_str());
    else if(param == "penalty_max_time_rate")
      m_penalty_max_time_rate = atof(value.c_str());
    else if(param == "max_time")
      m_max_time = atof(value.c_str());
    else
      all_handled = false;
  }

  return(all_handled);
}


//-----------------------------------------------------------
// Procedure: getFormattedString

string XYHazardRepEval::getFormattedString() const
{
  string score_tot = doubleToStringX(m_total_score,2);
  string paren_nrm = "(" + doubleToStringX(m_norm_score,2) + ")";

  string score_mh = doubleToStringX(m_score_missed_hazards);
  string paren_mh = "(" + uintToString(m_missed_hazards) + ")";
  string score_fa = doubleToStringX(m_score_false_alarms);
  string paren_fa = "(" + uintToString(m_false_alarms) + ")";

  double overage = m_total_time - m_max_time;
  if(overage < 0)
    overage = 0;
  string score_to = doubleToStringX(m_score_time_overage,2);
  string paren_to = "(" + doubleToStringX(overage,2) + ")";

  unsigned int total_hazards = m_missed_hazards + m_correct_hazards;
  string of_hazards = " (of " + uintToString(total_hazards) + ")";

  stringstream ss;
  ss << "          total_score: " << score_tot << " " << paren_nrm  << endl;
  ss << " score_missed_hazards: " << score_mh << " " << paren_mh    << endl;
  ss << "   score_false_alarms: " << score_fa << " " << paren_fa    << endl;
  ss << "   score_time_overage: " << score_to << " " << paren_to    << endl;
  ss << "--------------------- " << endl;
  ss << "     objects reported: " << m_total_objects   << endl;
  ss << "      correct_hazards: " << m_correct_hazards << of_hazards << endl;

  
  return(ss.str());
}









