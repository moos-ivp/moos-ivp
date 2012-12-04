/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYHazardRepEval.cpp                                  */
/*    DATE: Oct 29, 2012                                         */
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
  m_penalty_false_alarm      = 0;
  m_penalty_missed_hazard    = 0;
  m_penalty_max_time_over    = 0;
  m_penalty_max_time_overage = 0;
}

  
//-----------------------------------------------------------
// Procedure: getShortSpec()

string XYHazardRepEval::getShortSpec() const
{
  string str = "vname=" + m_vname;

  str += ",total_score="    + doubleToStringX(m_total_score);
  str += ",score_missed_hazards=" + doubleToStringX(m_score_missed_hazards);
  str += ",score_false_alarms=" + doubleToStringX(m_score_false_alarms);
  str += ",score_time_overage=" + doubleToStringX(m_score_time_overage);

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
  str += ",penalty_max_time_overage=" + doubleToStringX(m_penalty_max_time_overage);
  str += ",max_time=" + doubleToStringX(m_max_time);

  return(str);
}


//-----------------------------------------------------------
// Procedure: getFullSpec()

string XYHazardRepEval::getFullSpec() const
{
  string str = getShortSpec();
  str += "object_report={" + m_object_report + "}";
  
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
    else if(param == "penalty_max_time_overage")
      m_penalty_max_time_overage = atof(value.c_str());
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
  stringstream ss;
  ss << "vname: " << m_vname   << endl;
  ss << "--------------------" << endl;
  ss << "         total_score: " << doubleToStringX(m_total_score)          << endl;
  ss << "score_missed_hazards: " << doubleToStringX(m_score_missed_hazards) << endl;
  ss << "  score_false_alarms: " << doubleToStringX(m_score_false_alarms)   << endl;
  ss << "  score_time_overage: " << doubleToStringX(m_score_time_overage)   << endl;
  ss << "--------------------- " << endl;
  ss << "       total_objects: " << m_total_objects   << endl;
  ss << "     correct_hazards: " << m_correct_hazards << endl;
  ss << "      missed_hazards: " << m_missed_hazards  << endl;
  ss << "        false_alarms: " << m_false_alarms    << endl;
  ss << "          total_time: " << doubleToString(m_total_time,2)    << endl;
  ss << "       received_time: " << doubleToString(m_received_time,2) << endl;
  ss << "          start_time: " << doubleToString(m_start_time,2)    << endl;
  ss << "--------------------- " << endl;
  ss << "         false_alarm: " << doubleToStringX(m_penalty_false_alarm)      << endl;
  ss << "       missed_hazard: " << doubleToStringX(m_penalty_missed_hazard)    << endl;
  ss << "       max_time_over: " << doubleToStringX(m_penalty_max_time_over)    << endl;
  ss << "    max_time_overage: " << doubleToStringX(m_penalty_max_time_overage) << endl;
  ss << "            max_time: " << doubleToString(m_max_time,2)                << endl;
  
  return(ss.str());
}


