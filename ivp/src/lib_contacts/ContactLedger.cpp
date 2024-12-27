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

#include <cmath>
#include <iostream>
#include "ContactLedger.h"
#include "LinearExtrapolator.h"
#include "NodeRecordUtils.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

ContactLedger::ContactLedger(unsigned int history_size)
{
  // Config vars
  m_stale_thresh = -1;
  m_history_size = history_size;

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
  
  m_total_reports_valid++;

  // ========================================================
  // Part 1: Do some "Type-Fixing".
  // Known types: kayak, auv, glider, ship, wamv, etc.
  // ========================================================
  string vtype = tolower(record.getType());
  if(vtype == "uuv")
    vtype = "auv";
  else if(!isKnownVehicleType(vtype))
    vtype = "ship";

  if((vtype == "auv") || (vtype == "glider")) { 
    if(!record.valid("depth")) {
      whynot = "underwater vehicle type with no depth reported";
      return("");
    }
  }
  record.setType(vtype);
  
  // ========================================================
  // Part 2: Add defaults lengths if field is missing.
  // ========================================================
  double vlen = record.getLength();
  if(!record.valid("length") || (vlen <= 0)) {
    if((vtype == "auv") || (vtype == "kayak") || (vtype == "heron"))
      vlen = 3.0; // meters
    else if(vtype == "glider")
      vlen = 2.0; // meters
    else if((vtype == "cray")  || (vtype == "bcray") ||
	    (vtype == "crayx") || (vtype == "bcrayx"))
      vlen = 1.5; // meters
    else if(vtype == "ship")
      vlen = 12; // meters
    else if(vtype == "longship")
      vlen = 15; // meters
    else if(vtype == "wamv")
      vlen = 6; // meters
  }
  record.setLength(vlen);
  

  // ========================================================
  // Part 3: Receive the node reccord
  // ========================================================
  m_map_records_rep[vname] = record;
  m_map_records_ext[vname] = record;
  m_map_records_utc[vname] = m_curr_utc;

  if(m_active_vname == "")
    m_active_vname = vname;
  
  // ========================================================
  // Part 4: Update the vehicle position history
  // ========================================================
  if(m_history_size > 0) {
    double pos_x = record.getX();
    double pos_y = record.getY();
    
    ColoredPoint point(pos_x, pos_y);
    map<string,CPList>::iterator p2;
    p2 = m_map_hist.find(vname);
    if(p2 != m_map_hist.end()) {
      p2->second.push_back(point);
      if(p2->second.size() > m_history_size)
	p2->second.pop_front();
    }
    else {
      list<ColoredPoint> newlist;
      newlist.push_back(point);
      m_map_hist[vname] = newlist;
    }
  }

  // ========================================================
  // Part 5: Determine skew and update stats
  // ========================================================
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
  m_map_records_utc.erase(vname);

  m_map_records_total.erase(vname);
  m_map_skew_min.erase(vname);
  m_map_skew_max.erase(vname);
  m_map_skew_cnt.erase(vname);
  m_map_skew_avg.erase(vname);

  m_map_hist.erase(vname);
}

//---------------------------------------------------------------
// Procedure: clearAllNodes()

void ContactLedger::clearAllNodes()
{
  m_map_records_rep.clear();
  m_map_records_ext.clear();
  m_map_records_utc.clear();

  m_map_records_total.clear();
  m_map_skew_min.clear();
  m_map_skew_max.clear();
  m_map_skew_cnt.clear();
  m_map_skew_avg.clear();

  m_map_hist.clear();
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
// Procedure: setActiveVName()

void ContactLedger::setActiveVName(string vstr)
{
  vstr = toupper(vstr);
  // If given arg matches a known vname, then just set it
  if(m_map_records_rep.count(vstr)) {
    m_active_vname = vstr;
    return;
  }

  // If the request is to cycle, then find the active vname and ++
  if(vstr == "CYCLE_ACTIVE") {
    map<string, NodeRecord>::iterator p;
    for(p=m_map_records_rep.begin(); p!=m_map_records_rep.end(); p++) {
      string vname = p->first;
      if(m_active_vname == "") 
	m_active_vname = vname;
      else if(tolower(vname) == tolower(m_active_vname)) {
	p++;
	if(p == m_map_records_rep.end())
	  p = m_map_records_rep.begin();
	m_active_vname = p->first;
      }
    }
  }

  // Otherwise just take no action
}

//---------------------------------------------------------------
// Procedure: extrapolate()

void ContactLedger::extrapolate(string vname)
{
  vname = toupper(vname);
  
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

  hdg = angle360(hdg);
  
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
  vname = toupper(vname);
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
  vname = toupper(vname);
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
// Procedure: getUTC()
//   Purpose: Return the UTC timestamp of the most recent report
//            for the given vname. Note this is NOT the age of
//            report, which is relative to current time. It is 
//            NOT the time the report was received, which is 
//            typically at higher number timestamp due to latency
//            (but could be lower due to a clock discrepancy
//            between vehiclels).
//   Returns: UTC time in seconds if contact is known.
//            0 otherwise.

double ContactLedger::getUTC(string vname) const
{
  vname = toupper(vname);
  map<string,NodeRecord>::const_iterator p;
  p = m_map_records_rep.find(vname);
  if(p == m_map_records_rep.end())
    return(0);

  return(p->second.getTimeStamp());
}

//---------------------------------------------------------------
// Procedure: getUTCAge()
//   Purpose: Return age of the most recent report for the given
//            vname BASED ON TIMESTAMP IN THE REPORT.
  
double ContactLedger::getUTCAge(string vname) const
{
  double utc = getUTC(vname);
  if(utc == 0)
    return(-1);
  
  return(m_curr_utc - utc);
}

//---------------------------------------------------------------
// Procedure: getUTCReceived()
//   Purpose: Return the UTC timestamp of the most recent report
//            for the given vname based on the local time when
//            added to the ledger. (Not the timestamp contained
//            within the record message itself)
  
double ContactLedger::getUTCReceived(string vname) const
{
  vname = toupper(vname);
  map<string,double>::const_iterator p;
  p = m_map_records_utc.find(vname);
  if(p == m_map_records_utc.end())
    return(0);

  return(p->second);
}

//---------------------------------------------------------------
// Procedure: getUTCAgeReceived()
//   Purpose: Return age of the most recent report for the given
//            vname based on the local time when added to the 
//            ledger.
  
double ContactLedger::getUTCAgeReceived(string vname) const
{
  double utc = getUTCReceived(vname);
  if(utc == 0)
    return(-1);
  
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
  vname = toupper(vname);

  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname);
  return(record.getX());
}


//---------------------------------------------------------------
// Procedure: getY()

double ContactLedger::getY(string vname, bool extrap) const
{
  vname = toupper(vname);
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname);
  return(record.getY());
}


//---------------------------------------------------------------
// Procedure: getSpeed()

double ContactLedger::getSpeed(string vname) const
{
  vname = toupper(vname);
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getSpeed());
}

//---------------------------------------------------------------
// Procedure: getHeading()

double ContactLedger::getHeading(string vname) const
{
  vname = toupper(vname);
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getHeading());
}


//---------------------------------------------------------------
// Procedure: getDepth()

double ContactLedger::getDepth(string vname) const
{
  vname = toupper(vname);
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getDepth());
}


//---------------------------------------------------------------
// Procedure: getLat()

double ContactLedger::getLat(string vname) const
{
  vname = toupper(vname);
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getLat());
}


//---------------------------------------------------------------
// Procedure: getLon()

double ContactLedger::getLon(string vname) const
{
  vname = toupper(vname);
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getLon());
}

//---------------------------------------------------------------
// Procedure: getGroup()

string ContactLedger::getGroup(string vname) const
{
  vname = toupper(vname);
  if(!hasVName(vname))
    return("");

  NodeRecord record = getRecord(vname, false);
  return(record.getGroup());
}

//---------------------------------------------------------------
// Procedure: getType()

string ContactLedger::getType(string vname) const
{
  vname = toupper(vname);
  if(!hasVName(vname))
    return("");

  NodeRecord record = getRecord(vname, false);
  return(record.getType());
}

//---------------------------------------------------------------
// Procedure: getSpec()

string ContactLedger::getSpec(string vname) const
{
  vname = toupper(vname);
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
// Procedure: getVHist()

CPList ContactLedger::getVHist(string vname) const
{
  vname = toupper(vname);
  map<string,CPList>::const_iterator p=m_map_hist.find(vname);
  if(p==m_map_hist.end()) {
    CPList empty_cplist;
    return(empty_cplist);
  }
  else
    return(p->second);
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


//-------------------------------------------------------------
// Procedure: getWeightedCenter()

bool ContactLedger::getWeightedCenter(double& x, double& y) const
{
  double total_x = 0;
  double total_y = 0;

  unsigned int msize = m_map_records_rep.size();
  if(msize == 0) {
    x = 0;
    y = 0;
    return(false);
  }
  
  map<string, NodeRecord>::const_iterator p;
  for(p=m_map_records_rep.begin();
      p!=m_map_records_rep.end(); p++) {
    total_x += p->second.getX();
    total_y += p->second.getY();
  }

  x = total_x / (double)(msize);
  y = total_y / (double)(msize);

  return(true);
}

//-------------------------------------------------------------
// Procedure: getClosestVehicle()

string ContactLedger::getClosestVehicle(double mx, double my) const
{
  if(m_map_records_rep.size() == 0)
    return("");

  double closest_range = -1;
  string closest_vname;
  
  map<string, NodeRecord>::const_iterator p;
  for(p=m_map_records_rep.begin(); p!=m_map_records_rep.end(); p++) {
    double vx = p->second.getX();
    double vy = p->second.getY();
    double range = hypot(vx-mx, vy-my);
    if((closest_range == -1) || (range < closest_range)) {
      closest_range = range;
      closest_vname = p->second.getName();
    }      
  }

  return(closest_vname);
}

