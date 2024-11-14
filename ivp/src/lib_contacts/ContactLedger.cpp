/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ContactLedger.cpp                                    */
/*    DATE: Aug 31st 2022                                        */
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

#include <iostream>
#include "ContactLedger.h"
#include "LinearExtrapolator.h"
#include "NodeRecordUtils.h"
#include "MBUtils.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

ContactLedger::ContactLedger()
{
  // Config vars
  m_stale_thresh = -1;

  // If a record has been received as recently as this value
  // don't bother calculating an extrapolated position.
  m_extrap_thresh = 0.2; 
  
  // State vars
  m_curr_utc = 0;
  m_geodesy_init  = false;

  m_total_reports = 0;
  m_total_reports_valid = 0;
}

//---------------------------------------------------------------
// Procedure: setGeodesy()

void ContactLedger::setGeodesy(CMOOSGeodesy geodesy)
{
  m_geodesy = geodesy;
  m_geodesy_init = true;  
  m_geodesy_updates++;

  updateLocalCoords();
}

//---------------------------------------------------------------
// Procedure: setGeodesy()

bool ContactLedger::setGeodesy(double dlat, double dlon)
{
  bool ok_init = m_geodesy.Initialise(dlat, dlon);
  if(!ok_init)
    return(false);
  
  m_geodesy_init = true;
  m_geodesy_updates++;

  updateLocalCoords();

  return(true);
}

//---------------------------------------------------------------
// Procedure: addIgnoreVName()

bool ContactLedger::addIgnoreVName(string vname)
{
  if(vname == "")
    return(false);

  m_ignore_vnames.insert(toupper(vname));
  return(true);
}

//---------------------------------------------------------------
// Procedure: addIgnoreGroup()

bool ContactLedger::addIgnoreGroup(string group)
{
  if(group == "")
    return(false);
  
  m_ignore_groups.insert(toupper(group));
  return(true);
}

//---------------------------------------------------------------
// Procedure: processNodeReport()
//   Returns: If successful, returns name of vehicle just added
//            Otherwise empty string

string ContactLedger::processNodeReport(string report,
					string& whynot)
{
  NodeRecord record = string2NodeRecord(report);

  bool ok = preCheckNodeRecord(record, whynot);
  if(!ok)
    return("");
  
  return(processNodeRecord(record, whynot));
}

//---------------------------------------------------------------
// Procedure: precheckNodeReport()
//      Note: Utiity for users of ContactLedger class that wish
//            to (a) convert a node report msg to a record, (b)
//            apply some criteria of their own before deciding
//            to add it to the ledger, e.g., range/bng, before
//            (c) finally adding it to the ledger. 

NodeRecord ContactLedger::preCheckNodeReport(string report,
					     string& whynot)
{
  NodeRecord record = string2NodeRecord(report);

  if(!preCheckNodeRecord(record, whynot)) {
    NodeRecord empty_record;
    return(empty_record);
  }

  return(record);
}

//---------------------------------------------------------------
// Procedure: preCheckNodeRecord()
//   Purpose: (a) Sanity checks for required fields, (b) cross fill
//            the coordinates if necessary.
//      Note: This function modifies the incoming record.
//            All records will have both Lat/Lon and X/Y at end.

bool ContactLedger::preCheckNodeRecord(NodeRecord& record,
				       string& whynot)
{
  // Check 1: Must have a name
  if(record.getName() == "") {
    whynot = "Missing vname";    
    return(false);
  }
  
  // Check 2: Must have a timestamp
  if(!record.isSetTimeStamp()) {
    whynot = "Missing timestamp";
    return(false);
  }

  // Check 3: Must have either Lat/Lon or X/Y
  if(!record.isSetLatLon() && !record.isSetXY()) {
    whynot = "Missing both x/y and lat/lon";
    return(false);
  }

  // Cross-fill coords if one or the other is missing
  if(record.isSetLatLon() && !record.isSetXY())
    updateLocalCoords(record);
  else if(!record.isSetLatLon() && record.isSetXY())
    updateGlobalCoords(record);

  return(true);
}

//---------------------------------------------------------------
// Procedure: processNodeRecord()

string ContactLedger::processNodeRecord(NodeRecord record,
					string& whynot)
{
  m_total_reports++;

  // Check 1: Check record min reqs, and cross-fill coords
  if(!preCheckNodeRecord(record, whynot))
    return("");
  
  string vname = toupper(record.getName());
  string group = toupper(record.getGroup());

  // Check 2: Ignore vnames on ignore list, (ownship somtimes)
  if(m_ignore_vnames.count(vname))
    return("");

  // Check 3: Ignore groups on ignore list, (red/blue team)
  if((group != "") && m_ignore_groups.count(group))
    return("");
  
  m_total_reports_valid++;
  
  // Part 2: Receive the node reccord
  m_map_records_rep[vname] = record;
  m_map_records_ext[vname] = record;
  m_map_records_utc[vname] = m_curr_utc;

  // Part 3: Determine skew and update stats
  double skew = m_curr_utc - record.getTimeStamp();

  m_map_skew_cnt[vname]++;

  if(!m_map_skew_min.count(vname))
    m_map_skew_min[vname] = skew;
  else if(skew < m_map_skew_min[vname])
    m_map_skew_min[vname] = skew;

  if(!m_map_skew_max.count(vname))
    m_map_skew_max[vname] = skew;
  else if(skew > m_map_skew_max[vname])
    m_map_skew_max[vname] = skew;
    
  m_map_records_total[vname] += skew;

  if(m_map_skew_cnt[vname] > 0) {
    double total = m_map_records_total[vname];
    m_map_skew_avg[vname] = total / (double)m_map_skew_cnt[vname];
  }
    
  return(vname);
}

//---------------------------------------------------------------
// Procedure: clearNode()

void ContactLedger::clearNode(string vname)
{
  m_map_records_rep.erase(vname);
  m_map_records_ext.erase(vname);
}

//---------------------------------------------------------------
// Procedure: extrapolate()

void ContactLedger::extrapolate(double utc)
{
  if(utc != 0)
    m_curr_utc = utc;
  
  map<string,NodeRecord>::iterator p;
  for(p=m_map_records_rep.begin(); p!=m_map_records_rep.end(); p++) {
    string vname = p->first;
    extrapolate(vname);
  }
}

//---------------------------------------------------------------
// Procedure: extrapolate()

void ContactLedger::extrapolate(string vname)
{
  // Sanity check 1: Ledger UTC current time must be set
  if(m_curr_utc <= 0)
    return;
  // Sanity check 2: ensure record exists for this vehicle
  if(!hasVName(vname))
    return;

  NodeRecord record_rep = m_map_records_rep[vname];

  // Sanity check 3: Record must have nav info
  if(!record_rep.isSetXY() && !record_rep.isSetLatLon())
    return;
  
  // Sanity check 4: Record must timestamp
  if(!record_rep.isSetTimeStamp())
    return;

  // Fixable check: Record has Lat/Lon but not x/y 
  if(!record_rep.isSetXY()) {
    updateLocalCoords(m_map_records_rep[vname]);
    record_rep = m_map_records_rep[vname];
  }
  
  // By default, extrap record is set to reported record
  m_map_records_ext[vname] = record_rep;
  
  // If reported record is very recent, then we're done
  double record_utc = record_rep.getTimeStamp();
  double elapsed = m_curr_utc - record_utc;
  if(elapsed < m_extrap_thresh)
    return;

  // Else reported record is not very recent, extrapolate
  double x = record_rep.getX();
  double y = record_rep.getY();
  double hdg = record_rep.getHeading();
  double spd = record_rep.getSpeed();
  
  LinearExtrapolator extrapolator;
  extrapolator.setPosition(x, y, spd, hdg, record_utc);

  double new_x, new_y;
  bool ok = extrapolator.getPosition(new_x, new_y, m_curr_utc);

  if(!ok) {
    string failure_reason = extrapolator.getFailureReason();
    cout << "Extrap Fail: " << failure_reason << endl;
  }
  else {
    m_map_records_ext[vname].setX(new_x);
    m_map_records_ext[vname].setY(new_y);
    updateGlobalCoords(m_map_records_ext[vname]);
  }
}

//---------------------------------------------------------------
// Procedure: isValid()

bool ContactLedger::isValid(string vname) const
{
  map<string,NodeRecord>::const_iterator p;
  p = m_map_records_rep.find(vname);
  if(p != m_map_records_rep.end())
    return(p->second.valid("name,x,y,lat,lon,time"));

  return(false);
}

//---------------------------------------------------------------
// Procedure: isStale()
//      Note: If thresh < 0, treat as disabled, nothing is stale

bool ContactLedger::isStale(string vname, double thresh) const
{
  if(thresh < 0)
    return(false);

  map<string,double>::const_iterator p;
  p = m_map_records_utc.find(vname);
  if(p == m_map_records_utc.end())
    return(false);

  double rcvd_utc = p->second;
  double elapsed  = m_curr_utc - rcvd_utc;

  if(elapsed < thresh)
    return(false);
  
  return(true);
}

//---------------------------------------------------------------
// Procedure: getAge()
//   Purpose: Return age of the most recent report for the given
//            vname BASED ON TIMESTAMP IN THE REPORT. So it may
//            reflect discrepancy based on latency and differenc
//            in clocks between the two vehicles.
  
double ContactLedger::getAge(string vname) const
{
  map<string,NodeRecord>::const_iterator p;
  p = m_map_records_rep.find(vname);
  if(p == m_map_records_rep.end())
    return(-1);

  return(p->second.getTimeStamp());
}

//---------------------------------------------------------------
// Procedure: getAgeReceived()
//   Purpose: Return age of the most recent report for the given
//            vname based on the local time when addaed to the 
//            ledger.
  
double ContactLedger::getAgeReceived(string vname) const
{
  map<string,double>::const_iterator p;
  p = m_map_records_utc.find(vname);
  if(p == m_map_records_utc.end())
    return(-1);

  double utc = p->second;
  return(m_curr_utc - utc);
}

//---------------------------------------------------------------
// Procedure: hasVName()

bool ContactLedger::hasVName(string vname) const
{
  vname = toupper(vname);
  
  if(m_map_records_rep.count(vname) == 0)
    return(false);

  return(true);
}

//---------------------------------------------------------------
// Procedure: hasVNameValid()

bool ContactLedger::hasVNameValid(string vname) const
{
  vname = toupper(vname);
  
  map<string,NodeRecord>::const_iterator q;
  q = m_map_records_rep.find(vname);
  if(q==m_map_records_rep.end())
    return(false);

  return(q->second.valid());
}

//---------------------------------------------------------------
// Procedure: hasVNameValidNotStale()

bool ContactLedger::hasVNameValidNotStale(string vname,
					  double thresh) const
{
  vname = toupper(vname);
  
  map<string,NodeRecord>::const_iterator q;
  q = m_map_records_rep.find(vname);
  if(q==m_map_records_rep.end())
    return(false);

  NodeRecord record = q->second;

  if(!record.valid())
    return(false);
    
  double record_utc = record.getTimeStamp();
  double elapsed = m_curr_utc - record_utc;

  if(thresh > 0) {
    if(elapsed > thresh)
      return(false);
  }
  else {
    if((m_stale_thresh > 0) && (elapsed > m_stale_thresh))
      return(false);
  }

  return(true);
}

//---------------------------------------------------------------
// Procedure: getX()

double ContactLedger::getX(string vname, bool extrap) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname);
  return(record.getX());
}


//---------------------------------------------------------------
// Procedure: getY()

double ContactLedger::getY(string vname, bool extrap) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname);
  return(record.getY());
}


//---------------------------------------------------------------
// Procedure: getSpeed()

double ContactLedger::getSpeed(string vname) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getSpeed());
}

//---------------------------------------------------------------
// Procedure: getHeading()

double ContactLedger::getHeading(string vname) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getHeading());
}


//---------------------------------------------------------------
// Procedure: getDepth()

double ContactLedger::getDepth(string vname) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getDepth());
}


//---------------------------------------------------------------
// Procedure: getLat()

double ContactLedger::getLat(string vname) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getLat());
}


//---------------------------------------------------------------
// Procedure: getLon()

double ContactLedger::getLon(string vname) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getLon());
}

//---------------------------------------------------------------
// Procedure: getGroup()

string ContactLedger::getGroup(string vname) const
{
  if(!hasVName(vname))
    return("");

  NodeRecord record = getRecord(vname, false);
  return(record.getGroup());
}

//---------------------------------------------------------------
// Procedure: getType()

string ContactLedger::getType(string vname) const
{
  if(!hasVName(vname))
    return("");

  NodeRecord record = getRecord(vname, false);
  return(record.getType());
}

//---------------------------------------------------------------
// Procedure: getSpec()

string ContactLedger::getSpec(string vname) const
{
  map<string,NodeRecord>::const_iterator p;

  p = m_map_records_ext.find(vname);
  if(p != m_map_records_ext.end())
    return(p->second.getSpec());

  p = m_map_records_rep.find(vname);
  if(p != m_map_records_rep.end()) 
    return(p->second.getSpec());
  
  return("");
}

//---------------------------------------------------------------
// Procedure: getIgnoreVNames()

string ContactLedger::getIgnoreVNames() const
{
  return(stringSetToString(m_ignore_vnames));
}

//---------------------------------------------------------------
// Procedure: getIgnoreGroups()

string ContactLedger::getIgnoreGroups() const
{
  return(stringSetToString(m_ignore_groups));
}

//---------------------------------------------------------------
// Procedure: getVNamesByGroup()

set<string> ContactLedger::getVNamesByGroup(string group) const
{
  set<string> rset;
  
  map<string,NodeRecord>::const_iterator p;
  for(p=m_map_records_rep.begin(); p!=m_map_records_rep.end(); p++) {
    string vname = p->first;
    string vgroup = p->second.getGroup();
    if(tolower(group) == tolower(vgroup))
      rset.insert(vname);
    else if(tolower(group) == tolower("all"))
      rset.insert(vname);
  }

  return(rset);
}

//---------------------------------------------------------------
// Procedure: getVNames()

vector<string> ContactLedger::getVNames() const
{
  vector<string> svector;
  
  map<string,NodeRecord>::const_iterator p;
  for(p=m_map_records_rep.begin(); p!=m_map_records_rep.end(); p++)
    svector.push_back(p->first);

  return(svector);
}

//---------------------------------------------------------------
// Procedure: getVNamesStale()
//      Note: If thresh < 0, treat as disabled, nothing is stale

vector<string> ContactLedger::getVNamesStale(double thresh) const
{
  vector<string> svector; 
  if(thresh < 0)
    return(svector);
  
  vector<string> vnames = getVNames();;
  for(unsigned int i=0; i<vnames.size(); i++) {
    string vname = vnames[i];
    if(isStale(vname, thresh))
      svector.push_back(vname);
  }
  
  return(svector);
}

//---------------------------------------------------------------
// Procedure: totalReports(vname)

unsigned int ContactLedger::totalReports(string vname) const
{
  vname = toupper(vname);

  map<string, unsigned int>::const_iterator p;
  p = m_map_skew_cnt.find(vname);
  if(p != m_map_skew_cnt.end())
    return(p->second);

  return(0);
}

//---------------------------------------------------------------
// Procedure: groupMatch()

bool ContactLedger::groupMatch(string vname1, string vname2) const
{
  if(getGroup(vname1) != getGroup(vname2))
    return(false);
  return(true);
}


//---------------------------------------------------------------
// Procedure: getReportLags()

vector<string> ContactLedger::getReportLags() const
{
  ACTable actab(3);
  
  map<string,double>::const_iterator p;
  for(p=m_map_records_utc.begin(); p!=m_map_records_utc.end(); p++) {
    string vname = p->first;
    double utc   = p->second;
    double diff  = m_curr_utc - utc;
    string sdiff = doubleToStringX(diff,1); 
    
    string entry = vname + "(" + sdiff + ")";
    actab << entry;
  }
  vector<string> svector = actab.getTableOutput();
  return(svector);
}


//---------------------------------------------------------------
// Procedure: getRecord()

NodeRecord ContactLedger::getRecord(string vname, bool extrap) const
{
  vname = toupper(vname);
  
  if(extrap) {
    map<string,NodeRecord>::const_iterator q=m_map_records_ext.find(vname);
    if(q!=m_map_records_ext.end())
      return(q->second);
  }
  else {
    map<string,NodeRecord>::const_iterator q=m_map_records_rep.find(vname);
    if(q!=m_map_records_rep.end())
      return(q->second);  
  }
  
  NodeRecord null_record;
  return(null_record);
}


//---------------------------------------------------------------
// Procedure: updateLocalCoords()
//   Purpose: After a datum update, all X/Y values stored with any
//            contact are updated to be related to the new datum.

void ContactLedger::updateLocalCoords()
{
  map<string,NodeRecord>::iterator p;
  for(p=m_map_records_rep.begin(); p!=m_map_records_rep.end(); p++)
    updateLocalCoords(p->second);
  for(p=m_map_records_ext.begin(); p!=m_map_records_ext.end(); p++)
    updateLocalCoords(p->second);
}

//---------------------------------------------------------------
// Procedure: updateLocalCoords()

void ContactLedger::updateLocalCoords(NodeRecord& record)
{
  double nav_lat = record.getLat();
  double nav_lon = record.getLon();
  double nav_x, nav_y;

#ifdef USE_UTM
  m_geodesy.LatLong2LocalUTM(nav_lat, nav_lon, nav_y, nav_x);
#else
  m_geodesy.LatLong2LocalGrid(nav_lat, nav_lon, nav_y, nav_x);
#endif      

  record.setX(nav_x);
  record.setY(nav_y);
}    

//---------------------------------------------------------------
// Procedure: updateGlobalCoords()
//      Note: For determining lat/lon based on x/y
//            WE REALLY DONT WANT TO BE DOING THIS IN GENERAL
//            (1) for backward compatibility where node reports
//            are being shared only with x/y, we support this.
//            Node reports with x/y will be deprecated eventually.
//            (2) For updating lat/lon after extrapolating in x/y

void ContactLedger::updateGlobalCoords(NodeRecord& record)
{
  double nav_x = record.getX();
  double nav_y = record.getY();
  double nav_lat, nav_lon;

#ifdef USE_UTM
  m_geodesy.UTM2LatLong(nav_x, nav_y, nav_lat, nav_lon);
#else
  m_geodesy.LocalGrid2LatLong(nav_x, nav_y, nav_lat, nav_lon);
#endif      

  record.setLat(nav_lat);
  record.setLon(nav_lon);  
}    
