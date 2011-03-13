/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Alog2Range.cpp                                       */
/*    DATE: Feb 17th, 2011                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "MBUtils.h"
#include "Alog2Range.h"
#include "LogUtils.h"
#include "ColorParse.h"

using namespace std;

//--------------------------------------------------------
// Constructor

Alog2Range::Alog2Range()
{
  // Default configuration parameters
  m_report_type = "table";
  m_delim       = "padded";
  m_header      = true;
  m_post_nav    = false;
  m_verbose     = false;

  // Initial values for state variables
  m_longest_moos_time = 0;
  m_longest_utc_time  = 0;
  m_longest_id        = 0;
  m_longest_vname     = 0;
  m_longest_range     = 0;

  m_lp_navx.set_varname("NAV_X");
  m_lp_navy.set_varname("NAV_Y");
  m_lp_navh.set_varname("NAV_HEADING");
}

//--------------------------------------------------------
// Procedure: handle
//     Notes: 

bool Alog2Range::handle(const string& alogfile)
{
  if(alogfile == "") {
    cout << termColor("red");
    cout << "No alog file was specified. Exiting now." << endl;
    cout << termColor();
    return(false);
  }

  FILE *file_ptr = fopen(alogfile.c_str(), "r");
  if(!file_ptr) {
    cout << termColor("red");
    cout << "Alog file not found or unable to open - exiting" << endl;
    cout << termColor();
    return(false);
  }
  

  unsigned int line_count    = 0;
  bool done = false;
  while(!done) {
    line_count++;
    if(m_verbose) {
      if((line_count % 10000) == 0)
	cout << "+" << flush;
      if((line_count % 100000) == 0)
	cout << " (" << uintToCommaString(line_count) << ") lines" << endl;
    }

    string line_raw = getNextRawLine(file_ptr);

    bool line_is_comment = false;
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      line_is_comment = true;

    if(line_raw == "eof") 
      done = true;
    
    if(!done && !line_is_comment) {
      string varname = getVarName(line_raw);
      if(varname == "RANGE_REPORT") {
	string stime = getTimeStamp(line_raw);
	string sval  = getDataEntry(line_raw);
	m_range_reports.push_back(sval);
	m_time_stamps.push_back(stime);
      }
      else if((varname == "NAV_X") || (varname == "NAV_Y") ||
	      (varname == "NAV_HEADING"))  {
	string stime = getTimeStamp(line_raw);
	string sval  = getDataEntry(line_raw);
	double dtime = atof(stime.c_str());
	double dval  = atof(sval.c_str());
	if(varname == "NAV_X")
	  m_lp_navx.set_value(dtime, dval);
	else if(varname == "NAV_Y")
	  m_lp_navy.set_value(dtime, dval);
	else if(varname == "NAV_HEADING")
	  m_lp_navh.set_value(dtime, dval);
      }
    }
  }
  if(m_verbose) {
    cout << termColor("magenta") << endl;
    cout << uintToCommaString(line_count) << " lines total." << endl;
    cout << m_range_reports.size() << " range reports." << endl;
    cout << termColor();
  }

  if(file_ptr)
    fclose(file_ptr);

  determineMaxLens();
  determineUniqueBeaconIDs();
  determineUniqueTimeStamps();

  return(true);
}

//--------------------------------------------------------
// Procedure: setReportType
//     Notes: 

bool Alog2Range::setReportType(string rtype)
{
  rtype = tolower(rtype);
  if((rtype == "default") || (rtype == "flat") || (rtype == "table")) {
    m_report_type = rtype;
    return(true);
  }
  return(false);
}

//--------------------------------------------------------
// Procedure: setDelim
//     Notes: 

bool Alog2Range::setDelim(string delim)
{
  delim = tolower(delim);
  if((delim == "padded") || (delim == "tab") || (delim == "space")) {
    m_delim = delim;
    return(true);
  }
  return(false);
}

//--------------------------------------------------------
// Procedure: printReport()
//     Notes: 

void Alog2Range::printReport()
{
  if((m_report_type == "default") || (m_report_type == "flat"))
    printReportFlat();
  else 
    printReportTable();
}

//--------------------------------------------------------
// Procedure: printReportFlat
//     Notes: 

void Alog2Range::printReportFlat()
{
  char delim = 32;
  if(m_delim == "tab")
    delim = 9;

  string hdr1 = "Time";
  string hdr2 = "ID";
  string hdr3 = "Range";

  // Handler the Header Line
  if(m_header) {
    cout << endl;
    if(m_delim == "padded") {
      cout << padString("Time", m_longest_moos_time+1, false);
      cout << padString("ID",   m_longest_id+1, false);
      cout << padString("Range", m_longest_range, false) << endl;
    }  
    else
      cout << hdr1 << delim << hdr2 << delim << hdr3 << endl;
  }

  // Handler the body of the report
  if(m_report_type == "default") {
    unsigned int i, vsize = m_range_reports.size();
    for(i=0; i<vsize; i++) {
      string a = m_time_stamps[i];
      string b = getID(m_range_reports[i]);
      string c = getRange(m_range_reports[i]);
      if(m_delim == "padded") {
	cout << padString(a, m_longest_moos_time+1, false);
	cout << padString(b, m_longest_id+1, false);
	cout << padString(c, m_longest_range, false);
	cout  << endl;
      }
      else 
	cout << a << delim << b << delim << c << endl;
    }
  }
}


//--------------------------------------------------------
// Procedure: printReportTable
//     Notes: 

void Alog2Range::printReportTable()
{
  char delim = 32;
  if(m_delim == "tab")
    delim = 9;

  unsigned int hdr1 = m_longest_moos_time + 4;
  unsigned int hdrn = m_longest_range + 4;

  // Handler the Header Line
  if(m_header) {
    cout << endl;
    cout << padString("Time", hdr1, false);
    unsigned int i, vsize = m_unique_beacon_ids.size();
    for(i=0; i<vsize; i++) {
      if(m_delim == "padded") 
	cout << padString(m_unique_beacon_ids[i], hdrn, false);
      else
	cout << m_unique_beacon_ids[i] << delim;
    }
    if(!m_post_nav)
      cout << endl;
    else
      cout << "NAV_X      NAV_Y      NAV_HDG" << endl;
  }

  // Handler the body of the report
  unsigned int i, vsize = m_unique_time_stamps.size();
  for(i=0; i<vsize; i++) {
    // Generate the timestamp
    string tstamp = m_unique_time_stamps[i];
    if(m_delim == "padded")
      cout << padString(tstamp, hdr1, false);
    else
      cout << tstamp << delim;
    // Generate the range data columns
    unsigned int j, jsize = m_unique_beacon_ids.size();
    for(j=0; j<jsize; j++) {
      string id = m_unique_beacon_ids[j];
      string range = getRangeFromTimeID(tstamp, id);
      if(range == "")
	range = "-";
      if(m_delim == "padded")
	cout << padString(range, hdrn, false);
      else
	cout << range << delim;
    }
    // Conditionally generate the navigation columns
    if(!m_post_nav)
      cout << endl;
    else {
      double df_timestamp = atof(tstamp.c_str());
      double navx = m_lp_navx.get_value_by_time(df_timestamp);
      double navy = m_lp_navy.get_value_by_time(df_timestamp);
      double navh = m_lp_navh.get_value_by_time(df_timestamp);
      string s_navx = doubleToString(navx, 3);
      string s_navy = doubleToString(navy, 3);
      string s_navh = doubleToString(navh, 3);
      cout << padString(s_navx, 11, false);
      cout << padString(s_navy, 11, false);
      cout << padString(s_navh, 11, false);
      cout << endl;
    }
  }
}


//--------------------------------------------------------
// Procedure: determineMaxLens
//   Purpose: Determine the max lengths of things that might end
//            up as columns. Used for padding evenly.

void Alog2Range::determineMaxLens()
{
  unsigned int i, vsize = m_range_reports.size();
  for(i=0; i<vsize; i++) {
    string id        = getID(m_range_reports[i]);
    string vname     = getVName(m_range_reports[i]);
    string range     = getRange(m_range_reports[i]);
    string utc_time  = getUTC(m_range_reports[i]);
    string moos_time = m_time_stamps[i];

    if(id.length() > m_longest_id)
      m_longest_id = id.length();

    if(vname.length() > m_longest_vname)
      m_longest_vname = vname.length();

    if(range.length() > m_longest_range)
      m_longest_range = range.length();

    if(utc_time.length() > m_longest_utc_time)
      m_longest_utc_time = utc_time.length();

    if(moos_time.length() > m_longest_moos_time)
      m_longest_moos_time = moos_time.length();
  }
}


//--------------------------------------------------------
// Procedure: getVName()
// Procedure: getRange()
// Procedure: getID()
// Procedure: getUTC()

string Alog2Range::getVName(string report)
{
  return(tokStringParse(report, "vname", ',', '='));
}

string Alog2Range::getRange(string report)
{
  return(tokStringParse(report, "range", ',', '='));
}

string Alog2Range::getID(string report)
{
  return(tokStringParse(report, "id", ',', '='));
}

string Alog2Range::getUTC(string report)
{
  return(tokStringParse(report, "time", ',', '='));
}

//--------------------------------------------------------
// Procedure: determineUniqueBeaconIDs

void Alog2Range::determineUniqueBeaconIDs()
{
  unsigned int i, vsize = m_range_reports.size();
  for(i=0; i<vsize; i++) {
    string id = getID(m_range_reports[i]);
    if(!vectorContains(m_unique_beacon_ids, id))
      m_unique_beacon_ids.push_back(id);
  }
}

//--------------------------------------------------------
// Procedure: determineUniqueTimeStamps

void Alog2Range::determineUniqueTimeStamps()
{
  unsigned int i, vsize = m_range_reports.size();
  for(i=0; i<vsize; i++) {
    string ts  = m_time_stamps[i];
    double dts = atof(ts.c_str());
    bool   found = false;
    unsigned int j, jsize = m_unique_time_stamps.size();
    for(j=0; j<jsize; j++) {
      string jts = m_unique_time_stamps[j];
      double djts = atof(jts.c_str());
      double delta = (dts-djts)>= 0 ? (dts-djts) : (djts-dts);
      if(delta < 0.01)
	found = true;
    }
    if(!found)
      m_unique_time_stamps.push_back(ts);
  }
}

//--------------------------------------------------------
// Procedure: getRangeFromTimeID()

string Alog2Range::getRangeFromTimeID(string time_stamp, string id)
{
  unsigned int i, vsize = m_time_stamps.size();
  for(i=0; i<vsize; i++) {
    double a = atof(time_stamp.c_str());
    double b = atof(m_time_stamps[i].c_str());
    double delta = (a-b)>0 ? (a-b) : (b-a);
    if(delta < 0.01) {
      string this_id = getID(m_range_reports[i]);
      if(this_id == id) {
	string range = getRange(m_range_reports[i]);
	return(range);
      }
    }
  }
  return("");
}
