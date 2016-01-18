/************************************************************/
/*    NAME: Mike Benjamin, based on an earlier version by   */
/*          Kyle Woerner                                    */
/*    ORGN: MIT CSAIL, Dept of Mechanical Engineering       */
/*    FILE: CPAMonitor.cpp                                  */
/*    DATE: Dec 20th 2015                                   */
/************************************************************/

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "MBUtils.h"
#include "CPAMonitor.h"
#include "NodeRecordUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

CPAMonitor::CPAMonitor()
{
  m_ignore_range = 100;     // meters
  m_report_range = 50;      // meters
  m_swing_range  = 1;       // meters
  m_verbose      = false;
}

//---------------------------------------------------------
// Procedure: setIgnoreRange
//      Note: Enforce report_range <= ignore_range

void CPAMonitor::setIgnoreRange(double val)
{
  m_ignore_range = val;
  if(m_report_range > m_ignore_range)
    m_report_range = m_ignore_range;
}


//---------------------------------------------------------
// Procedure: setReportRange
//      Note: Enforce ignore_range >= reportRange

void CPAMonitor::setReportRange(double val)
{
  m_report_range = val;
  if(m_ignore_range < m_report_range)
    m_ignore_range = m_report_range;
}


//---------------------------------------------------------
// Procedure: setSwingRange
//      Note: The amount of noted changed needed before declaring
//            a swing from opening to closing mode. To avoid
//            spurious events due to jitter in range measurements.

void CPAMonitor::setSwingRange(double val)
{
  m_swing_range = val;
  if(m_swing_range < 0)
    m_swing_range = 0;
}


//---------------------------------------------------------
// Procedure: getEvent()

CPAEvent CPAMonitor::getEvent(unsigned int ix) const
{
  CPAEvent null_event;
  if(ix >= m_events.size())
    return(null_event);
  return(m_events[ix]);
}

//---------------------------------------------------------
// Procedure: handleNodeReport()
//   Purpose: Handle a single node report, presumably within a
//            round of multiple incoming updated reports.

bool CPAMonitor::handleNodeReport(string node_report)
{
  NodeRecord record = string2NodeRecord(node_report);
  if(!record.valid())
    return(false);
  string vname = record.getName();

  // Part 1: Update the node record list for this vehicle
  m_map_vrecords[vname].push_front(record);
  if(m_map_vrecords[vname].size() > 2)
    m_map_vrecords[vname].pop_back();
  m_map_updated[vname] = true;

  return(true);
}

//---------------------------------------------------------
// Procedure: examineAndReport()

bool CPAMonitor::examineAndReport()
{
  map<string, bool>::iterator p;
  for(p=m_map_updated.begin(); p!=m_map_updated.end(); p++) {
    string vname = p->first;
    if(m_map_updated[vname]) {
      examineAndReport(vname);
    }
  }
      
  return(true);
}

//---------------------------------------------------------
// Procedure: examineAndReport(vname)

bool CPAMonitor::examineAndReport(string vname)
{
  map<string, bool>::iterator p;
  for(p=m_map_updated.begin(); p!=m_map_updated.end(); p++) {
    string contact = p->first;
    if(vname != contact) {
      examineAndReport(vname, contact);
    }
  }
      
  return(true);
}

//---------------------------------------------------------
// Procedure: examineAndReport(vname, contact)

bool CPAMonitor::examineAndReport(string vname, string contact)
{
  if(!m_map_vrecords.count(vname) || !m_map_vrecords.count(contact))
    return(false);

  string tag = pairTag(vname, contact);

  cout << "Examining: " << vname << " and " << contact <<
    "(" << tag << ")" << "  [" << m_iteration << "]" << endl;

  if(m_map_pair_examined[tag])
    return(true);

  
  double prev_dist    = m_map_pair_dist[tag];
  bool   prev_closing = m_map_pair_closing[tag];
  bool   prev_valid   = m_map_pair_valid[tag];

  updatePairRangeAndRate(vname, contact);

  bool now_closing = m_map_pair_closing[tag];
  bool now_valid   = m_map_pair_valid[tag];

  if(m_verbose) {

    cout << "  prev_dist:    " << prev_dist << endl;
    cout << "  prev_closing: " << boolToString(prev_closing) << endl;
    cout << "  prev_valid:   " << boolToString(prev_valid) << endl;  
    cout << "  NOW closing: " << now_closing << endl;
    cout << "  NOW valid:   " << boolToString(now_valid)   << endl;
  }
  
  if(!prev_valid || !now_valid)
    return(true);

  // If transitioned from closing to opening
  if(prev_closing && !now_closing) {
    if(m_verbose)
      cout << " *********** POSTING ************* " << endl;
    double cpa_dist = m_map_pair_min_dist_running[tag];
    if(cpa_dist <= m_report_range) {
      CPAEvent event(vname, contact, cpa_dist);
      double midx = m_map_pair_midx[tag];
      double midy = m_map_pair_midy[tag];
      event.setX(midx);
      event.setY(midy);
      m_events.push_back(event);
    }
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: updatePairRangeAndRate(vname, contact)

bool CPAMonitor::updatePairRangeAndRate(string vname, string contact)
{
  // Sanity check - make sure we have node records for each
  if(!m_map_vrecords.count(vname) || !m_map_vrecords.count(contact))
    return(false);

  // Part 1: get the vehicle info and calculate range
  NodeRecord os_record = m_map_vrecords[vname].front();
  NodeRecord cn_record = m_map_vrecords[contact].front();
  double osx  = os_record.getX();
  double osy  = os_record.getY();
  double cnx  = cn_record.getX();
  double cny  = cn_record.getY();
  double midx = (osx + ((cnx-osx)/2));
  double midy = (osy + ((cny-osy)/2));
  double dist = hypot(osx-cnx, osy-cny);
  string tag  = pairTag(vname, contact);

  // Note that this pair has been examined on this round. This is cleared
  // for all pairs at the end of a round.
  m_map_pair_examined[tag] = true; 
  
  // If the distance is really large (greater than the ignore_range) then
  // remove all data for this tag and return. 
  if(dist > m_ignore_range) {
    m_map_pair_dist.erase(tag);
    m_map_pair_closing[tag] = false;
    m_map_pair_valid[tag] = false;
    m_map_pair_midx[tag] = 0;
    m_map_pair_midy[tag] = 0;
    return(true);
  }
  
  // Handle case where this is the first distance noted for this pair
  if(m_map_pair_dist.count(tag) == 0) {
    m_map_pair_dist[tag] = dist;
    m_map_pair_closing[tag] = false;
    m_map_pair_valid[tag] = false;
    return(true);
  }
  
  double dist_prev = m_map_pair_dist[tag];
  bool   closing_prev = m_map_pair_closing[tag];
  
  // Simple case: If distance hasn't changed, then no updates to the 
  // closing and valid flags either. 
  if(dist_prev == dist) 
    return(true);

  // Handle the case where we are technically closing
  if(dist_prev > dist) {
    // Handle case where may be transitioning from opening to closing
    if(!closing_prev) {
      // Check if range has "swung" sufficiently to warrant a change
      if((m_map_pair_max_dist_running[tag] - dist) > m_swing_range) {
	m_map_pair_closing[tag] = true;
      }
    }
    m_map_pair_min_dist_running[tag] = dist;
  }
  else {  // Handle case where we are technically opening
    // Handle case where may be transitioning from closing to opening 
    if(closing_prev) {
      // Check if range has "swung" sufficiently to warrant a change
      if((dist - m_map_pair_min_dist_running[tag]) > m_swing_range) {
	m_map_pair_closing[tag] = false;
      }
    }
    m_map_pair_max_dist_running[tag] = dist;
  }    

  m_map_pair_dist[tag] = dist;
  m_map_pair_valid[tag] = true;
  m_map_pair_midx[tag] = midx;
  m_map_pair_midy[tag] = midy;
  
  return(true);
}

//---------------------------------------------------------
// Procedure: clear()
//   Purpose: At the end of a round, clear data.

void CPAMonitor::clear()
{
  map<string, bool>::iterator p;
  for(p=m_map_updated.begin(); p!=m_map_updated.end(); p++)
    p->second = false;
  
  for(p=m_map_pair_examined.begin(); p!=m_map_pair_examined.end(); p++)
    p->second = false;

  m_events.clear();
}

//---------------------------------------------------------
// Procedure: pairTag()

string CPAMonitor::pairTag(string a, string b)
{
  if(a < b)
    return(a + "#" + b);
  return(b + "#" + a);
}
