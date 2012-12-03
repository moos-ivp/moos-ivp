/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LifeEventHistory.cpp                                 */
/*    DATE: Mar 9th 2010                                         */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#include <cstdlib>
#include "LifeEventHistory.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

LifeEventHistory::LifeEventHistory()
{
  m_max_len_time  = 4;   // Minimallly as long as "Time"
  m_max_len_iter  = 4;   // Minimallly as long as "Iter"
  m_max_len_bname = 8;   // Minimallly as long as "Behavior"
  m_max_len_btype = 13;  // Minimallly as long as "Behavior Type"
  m_max_len_event = 5;   // Minimallly as long as "Event"
  m_max_len_seed  = 15;  // Minimallly as long as "Spawning String"

  m_stale_report  = true;

  m_banner_active = true;
  m_color_active  = true;
  m_seed_active   = true;
}

//-----------------------------------------------------------
// Procedure: addLifeEvent

void LifeEventHistory::addLifeEvent(const LifeEvent& event)
{
  m_life_events.push_back(event);
  m_stale_report = true;
}

//-----------------------------------------------------------
// Procedure: addLifeEvent

void LifeEventHistory::addLifeEvent(const string& str)
{
  LifeEvent event;
  
  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = tolower(stripBlankEnds(biteString(svector[i], '=')));
    string right = stripBlankEnds(svector[i]);
    unsigned int rlen = right.length();

    if(left == "time") {
      event.setTimeStamp(atof(right.c_str()));
      if(rlen > m_max_len_time)
	m_max_len_time = rlen;
    }

    else if(left == "iter") {
      event.setIteration(atoi(right.c_str()));
      if(rlen > m_max_len_iter)
	m_max_len_iter = rlen;
    }

    else if(left == "bname") {
      event.setBehaviorName(right);
      if(rlen > m_max_len_bname)
	m_max_len_bname = rlen;
    }

    else if(left == "btype") {
      event.setBehaviorType(right);
      if(rlen > m_max_len_btype)
	m_max_len_btype = rlen;
    }

    else if(left == "event") {
      event.setEventType(right);
      if(rlen > m_max_len_event)
	m_max_len_event = rlen;
    }

    else if(left == "seed") {
      event.setSpawnString(right);
      if(rlen > m_max_len_seed)
	m_max_len_seed = rlen;
    }
  }

  m_life_events.push_back(event);
  m_stale_report = true;
}

//-----------------------------------------------------------
// Procedure: getReport

vector<string> LifeEventHistory::getReport(string mode)
{
  if(m_stale_report == false)
    return(m_history_report);

  // Preliminaries: clear the old report, set the column pad
  m_history_report.clear();
  unsigned int column_pad_spaces = 2;
  string cpad(column_pad_spaces, ' ');

  // Possibly includea banner in the report
  if(m_banner_active) {
    string s1, s2, s3, blank_line;
    s1 = "      ***************************************************";
    s2 = "      *        Summary of Behavior Life Events          *";
    s3 = "      ***************************************************";
    if(m_color_active) {
      s1 = termColor("magenta") + s1 + termColor();
      s2 = termColor("magenta") + s2 + termColor();
      s3 = termColor("magenta") + s3 + termColor();
    }
    m_history_report.push_back(s1);
    m_history_report.push_back(s2);
    m_history_report.push_back(s3);
    m_history_report.push_back(blank_line);
  }

  // Part 1 Make the header string
  string header = padString("Time", m_max_len_time, false) + cpad;
  header += padString("Iter", m_max_len_iter, false) + cpad;
  header += padString("Event", m_max_len_event, false) + cpad;
  header += padString("Behavior", m_max_len_bname, false) + cpad;
  header += padString("Behavior Type", m_max_len_btype, false) + cpad;
  if(m_seed_active)
    header += padString("Spawning Seed", m_max_len_seed, false);
  m_history_report.push_back(header);

  // Part 2 Make the divider string
  string str, divider;
  divider += str.assign(m_max_len_time, '-')  + cpad;
  divider += str.assign(m_max_len_iter, '-')  + cpad;
  divider += str.assign(m_max_len_event, '-') + cpad;
  divider += str.assign(m_max_len_bname, '-') + cpad;
  divider += str.assign(m_max_len_btype, '-') + cpad;
  if(m_seed_active)
    divider += str.assign(m_max_len_seed, '-');
  m_history_report.push_back(divider);

  // Part 3 Fill out the body of the report
  unsigned int i, vsize = m_life_events.size();
  for(i=0; i<vsize; i++) {
    string line;

    double timestamp = m_life_events[i].getTimeStamp();
    line += padString(doubleToString(timestamp,2), m_max_len_time) + cpad;
    
    unsigned int iter = m_life_events[i].getIteration();
    line += padString(intToString(iter), m_max_len_iter) + cpad;
    
    string event = m_life_events[i].getEventType();
    line += padString(event, m_max_len_iter, false) + cpad;

    string bname = m_life_events[i].getBehaviorName();
    line += padString(bname, m_max_len_bname, false) + cpad;

    string btype = m_life_events[i].getBehaviorType();
    line += padString(btype, m_max_len_btype, false) + cpad;

    string seed = m_life_events[i].getSpawnString();
    if(m_seed_active)
      line += padString(seed, m_max_len_seed, false);

    if(m_color_active) {
      if(event == "spawn") {
	if(seed == "helm startup")
	  line = termColor("blue") + line + termColor();
	else
	  line = termColor("green") + line + termColor();
      }
      else if(event == "abort")
	line = termColor("red") + line + termColor();
    }
    m_history_report.push_back(line);
  }

  m_stale_report = false;
  return(m_history_report);
}


