/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYHazardRepEval.h                                    */
/*    DATE: Oct 29th, 2012                                       */
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

#ifndef XY_HAZARD_REP_EVAL_HEADER
#define XY_HAZARD_REP_EVAL_HEADER

#include <string>
#include <vector>
#include "XYHazard.h"

class XYHazardRepEval
{
 public:
  XYHazardRepEval();
  virtual ~XYHazardRepEval() {};

  // Setters (Score components)
  void  setVName(std::string s)           {m_vname=s;};
  void  setTotalScore(double v)           {m_total_score=v;};
  void  setScoreMissedHazards(double v)   {m_score_missed_hazards=v;};
  void  setScoreFalseAlarms(double v)     {m_score_false_alarms=v;};
  void  setScoreTimeOverage(double v)     {m_score_time_overage=v;};

  // Setters (Report Stats)
  void  setTotalObjects(unsigned int v)   {m_total_objects=v;};
  void  setCorrectHazards(unsigned int v) {m_correct_hazards=v;};
  void  setMissedHazards(unsigned int v)  {m_missed_hazards=v;};
  void  setFalseAlarms(unsigned int v)    {m_false_alarms=v;};
  void  setTotalTime(double v)            {m_total_time=v;};
  void  setReceivedTime(double v)         {m_received_time=v;};
  void  setStartTime(double v)            {m_start_time=v;};
  
  // Setters (Scoring Criteria)
  void  setPenaltyFalseAlarm(double v)    {m_penalty_false_alarm=v;};
  void  setPenaltyMissedHazard(double v)  {m_penalty_missed_hazard=v;};
  void  setPenaltyMaxTimeOver(double v)   {m_penalty_max_time_over=v;};
  void  setPenaltyMaxTimeOverage(double v) {m_penalty_max_time_overage=v;};
  void  setMaxTime(double v)              {m_max_time=v;};

  // Setters (other)
  void  setObjectReport(std::string s)   {m_object_report=s;};
  bool  buildFromSpec(const std::string&);

  // Getters
  std::string getShortSpec() const;
  std::string getFullSpec() const;
  std::string getFormattedString() const;

  double getTotalScore() const   {return(m_total_score);};
  double getTotalTime() const    {return(m_total_time);};
  double getReceivedTime() const {return(m_received_time);};

 protected: // Key properties
  std::string  m_vname;
  double       m_total_score;
  double       m_score_missed_hazards;
  double       m_score_false_alarms;
  double       m_score_time_overage;
  
  unsigned int m_total_objects;
  unsigned int m_missed_hazards;
  unsigned int m_correct_hazards;
  unsigned int m_false_alarms;
  double       m_total_time;
  double       m_received_time;
  double       m_start_time;

  double       m_penalty_false_alarm;
  double       m_penalty_missed_hazard;
  double       m_max_time;
  double       m_penalty_max_time_over;
  double       m_penalty_max_time_overage;

  std::string  m_object_report;
};

#endif 


