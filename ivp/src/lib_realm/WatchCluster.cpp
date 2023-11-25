/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WatchCluster.cpp                                     */
/*    DATE: Dec 22nd 2020                                        */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "ACTable.h"
#include "WatchCluster.h"

using namespace std;

//---------------------------------------------------------
// Constructor

WatchCluster::WatchCluster()
{
  m_report_cnt = 0;
  m_wcast_cnt = 0;
}

//---------------------------------------------------------
// Procedure: addVar()

bool WatchCluster::addVar(string var)
{
  if(vectorContains(m_cluster_vars, var))
    return(false);

  m_cluster_vars.push_back(var);
  return(true);
}

//---------------------------------------------------------
// Procedure: addWatchCast()

bool WatchCluster::addWatchCast(const WatchCast& wcast)
{
  string node    = wcast.getNode();
  string varname = wcast.getVarName();

  if((node == "") || (varname == ""))
    return(false);

  if(!vectorContains(m_cluster_vars, varname))
    return(false);
  
  m_map_info[node][varname] = wcast;

  m_map_varcount[varname]++;

  m_wcast_cnt++;
  
  return(true);
}


//---------------------------------------------------------
// Procedure: getVarsAsString()
//   Example: DEPLOY:RETURN:STATION

string WatchCluster::getVarsAsString() const
{
  string str;
  for(unsigned int i=0; i<m_cluster_vars.size(); i++) {
    if(i != 0)
      str += ":";
    str += m_cluster_vars[i];
  }
  return(str);
}
 
//---------------------------------------------------------
// Procedure: getVarCount()

unsigned int WatchCluster::getVarCount(string varname) const
{
  map<string, unsigned int>::const_iterator p=m_map_varcount.find(varname);
  if(p!=m_map_varcount.end())
    return(p->second);

  return(0);
}
 

//---------------------------------------------------------
// Procedure: getReport()

vector<string> WatchCluster::getReport(string var, InfoCastSettings ics)
{
  vector<string> report;

  // Sanity checks
  if(m_cluster_vars.size() == 0)
    return(report);
  if(m_map_info.size() == 0)
    return(report);

  m_report_cnt++;

  string header = m_key;
  string hdrcnt = "(" + uintToString(m_report_cnt) + ")";
  int pad_len = 48 - (hdrcnt.length());
  header = padString(header, pad_len, false) + hdrcnt;
  
  report.push_back("================================================");
  report.push_back(header);
  report.push_back("================================================");
    
  // Part 1: Determine which kind of report to generate. Either a
  // single-var report or a multi-var report. Both kinds of reports
  // will show the node/vehicle for each row. Single-var reports will
  // show the source and time for each node/vehicle. The multi-var
  // report will create a column for each variable, only showing the
  // the variable value.

  vector<string> sub_report;
  if(m_cluster_vars.size() == 1)
    sub_report = getReportSingle(m_cluster_vars[0], ics);
  else if((var != "") && vectorContains(m_cluster_vars, var))
    sub_report = getReportSingle(var, ics);
  else
    sub_report = getReportMulti();
    
  report = mergeVectors(report, sub_report);
  return(report);
}


//---------------------------------------------------------
// Procedure: getReportSingle()
//
// Example:
// 
//  Node   Var     Source 	      Time   Value
//  -----  ------  -------------  -----  ----
//  Abe    DEPLOY  pHelmIvP       23.12  false
//  Ben    DEPLOY  pMarineViewer  23.12  false
//  Cal    DEPLOY  pMarineViewer  23.12  true
//  Deb    DEPLOY  pMarineViewer  23.12  true
//  Eve    DEPLOY  pMarineViewer  23.12  true
//  Fin    DEPLOY  pMarineViewer  23.12  true

vector<string> WatchCluster::getReportSingle(string varname,
					     InfoCastSettings ics) const
{
  vector<string> report;

  bool show_community = ics.getShowRealmCastCommunity();
  bool show_source = ics.getShowRealmCastSource();
  bool wrap_content = ics.getWrapRealmCastContent();
  bool trunc_content = ics.getTruncRealmCastContent();

  unsigned int columns = 6;
  if(!show_community)
    columns--;
  if(!show_source)
    columns--;
  string header = "Node | Var | Time ";
  if(show_source)
    header += "| Source ";
  if(show_community)
    header += "| Comm ";
  header += "| Value";
  
  ACTable actab(columns);
  actab << header;
  actab.addHeaderLines();
  
  map<string, map<string, WatchCast> >::const_iterator p;
  for(p=m_map_info.begin(); p!=m_map_info.end(); p++) {
    string node = p->first;
    map<string, WatchCast> wmap = p->second;
    string stime = "-";
    string source = "-";
    string community = "-";
    string value = "-";
    map<string, WatchCast>::const_iterator q=wmap.find(varname);
    if(q!=wmap.end()) {
      WatchCast wcast = q->second;
      source = wcast.getSource();
      community = wcast.getCommunity();
      double time = wcast.getLocTime();

      if(ics.getRealmCastTimeFormatUTC())
	time = wcast.getUtcTime();

      stime = doubleToString(time,2);
      value = wcast.getSVal();
      if(wcast.isDouble())
	value = doubleToStringX(wcast.getDVal(),3);

      if(trunc_content && (value.length() > 90))
	value = value.substr(0,90);
    }

    if(!show_source)
      source = "_ignore_";
    if(!show_community)
      community = "_ignore_";

    if(!wrap_content)  
      actab << node << varname << stime << source << community << value;
    else {
      vector<string> svector = breakLen(value, 90);
      for(unsigned int i=0; i<svector.size(); i++)  {
	if(i == 0)
	  actab << node << varname << source << stime << community << svector[i];
	else {
	  if(show_source)
	    source = "";
	  if(show_community)
	    community = "";
	  actab << "" << "" << source << "" << community << svector[i];
	}
      }      
    }
  }

  report = actab.getTableOutput();
  return(report);
}
 

//---------------------------------------------------------
// Procedure: getReportMulti()
//
// Example:
// 
//  Node   DEPLOY  RETURN  STATION  MODE
//  -----  ------  ------  -------  ------------ 
//  Abe    true    false   true     station-keep
//  Ben    false   false   false    inactice 
//  Cal    true    true    false    returning
//  Deb    true    true    false    returning


vector<string> WatchCluster::getReportMulti() const
{
  vector<string> report;

  // Sanity Check
  if(m_cluster_vars.size() <= 1)
    return(report);
  
  unsigned int varcount = m_cluster_vars.size();
  string header_str = "node ";
  for(unsigned int i=0; i<m_cluster_vars.size(); i++)
    header_str += "| " + m_cluster_vars[i];
  
  ACTable actab(varcount+1);
  actab << header_str;
  actab.addHeaderLines();
  
  map<string, map<string, WatchCast> >::const_iterator p;
  for(p=m_map_info.begin(); p!=m_map_info.end(); p++) {
    string node = p->first;
    map<string, WatchCast> wmap = p->second;

    actab << node;
    for(unsigned int i=0; i<m_cluster_vars.size(); i++) {
      string varname = m_cluster_vars[i];

      string value = "-";    
      map<string, WatchCast>::const_iterator q=wmap.find(varname);
      if(q!=wmap.end()) {
	WatchCast wcast = q->second;
	value = wcast.getSVal();
	if(wcast.isDouble())
	  value = doubleToStringX(wcast.getDVal(),3);
	if(value.length() > 40)
	  value = value.substr(0,40);

      }
      actab << value;
    }
  }
      
  report = actab.getTableOutput();
  return(report);
}
 

