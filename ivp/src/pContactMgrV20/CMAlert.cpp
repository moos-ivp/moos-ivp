/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CMAlert.cpp                                          */
/*    DATE: Oct 11th 2017                                        */
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
#include "XYFormatUtilsPoly.h"
#include "CMAlert.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

CMAlert::CMAlert()
{
  m_range     = -1;
  m_range_far = -1;
}

//---------------------------------------------------------------
// Procedure: configFilter()

bool CMAlert::configFilter(string param, string value)
{
  if(param == "match_name")
    return(m_filter_set.addMatchName(value));
  else if(param == "ignore_name")
    return(m_filter_set.addIgnoreName(value));

  else if(param == "match_group")
    return(m_filter_set.addMatchGroup(value));
  else if(param == "ignore_group")
    return(m_filter_set.addIgnoreGroup(value));

  else if(param == "match_type")
    return(m_filter_set.addMatchType(value));
  else if(param == "ignore_type")
    return(m_filter_set.addIgnoreType(value));

  else if(param == "match_region")
    return(m_filter_set.addMatchRegion(value));
  else if(param == "ignore_region")
    return(m_filter_set.addIgnoreRegion(value));

  else if(param == "strict_ignore")
    return(m_filter_set.setStrictIgnore(value));
  
  return(false);
}

//---------------------------------------------------------------
// Procedure: filterCheck()

bool CMAlert::filterCheck(NodeRecord record) const
{
  return(m_filter_set.filterCheck(record));
}

//---------------------------------------------------------------
// Procedure: filterCheck()

bool CMAlert::filterCheck(NodeRecord record, double osx, double osy) const
{
  return(m_filter_set.filterCheck(record, osx, osy));
}


//---------------------------------------------------------------
// Procedure: setAlertSource()

bool CMAlert::setAlertSource(string str)
{
  if(str == "")
    return(false);

  m_alert_source = str;
  return(true);
}

//---------------------------------------------------------------
// Procedure: setAlertRange()
//      Note: We ensure that m_range_far is always greater than or
//            equal to m_range. 

bool CMAlert::setAlertRange(string str)
{
  if(tolower(str) == "off") {
    // The -2 value indicates it has been purposely set to off
    m_range     = -2;
    m_range_far = -2;
    return(true);
  }

  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  return(setAlertRange(dval));
}

//---------------------------------------------------------------
// Procedure: setAlertRangeFar()
//      Note: We ensure that m_range_far is always greater than or
//            equal to m_range. 

bool CMAlert::setAlertRangeFar(string str)
{
  if(tolower(str) == "off") {
    // The -2 value indicates it has been purposely set to off
    m_range_far = -2;
    return(true);
  }
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());

  return(setAlertRangeFar(dval));
}

//---------------------------------------------------------------
// Procedure: setAlertRange
//      Note: We ensure that m_range_far is always greater than or
//            equal to m_range. 

bool CMAlert::setAlertRange(double dval)
{
  if(dval < 0)
    return(false);
  m_range = dval;
  if(m_range_far < m_range)
    m_range_far = m_range;

  return(true);
}

//---------------------------------------------------------------
// Procedure: setAlertRangeFar
//      Note: We ensure that m_range_far is always greater than or
//            equal to m_range. 

bool CMAlert::setAlertRangeFar(double dval)
{
  if(dval < 0)
    return(false);
  m_range_far = dval;
  if(m_range > m_range_far)
    m_range = m_range_far;

  return(true);
}


//---------------------------------------------------------------
// Procedure: addAlertOnFlag

bool CMAlert::addAlertOnFlag(string str)
{
  // Sanity check: Make sure this isn't a duplicate
  str = stripBlankEnds(str);
  if(m_on_flags_raw.count(str))
    return(true);
  m_on_flags_raw.insert(str);
  
  // Ok, create and add the VarDataPair
  string lft = biteStringX(str, '=');
  string rgt = str;

  if((lft == "") || (rgt == ""))
    return(false);

  VarDataPair pair(lft, rgt, "auto");
  m_on_flags.push_back(pair);
  return(true);    
}

#if 0
bool CMAlert::addAlertOnFlag(string str)
{
  vector<string> svector = parseString(str, '#');
  for(unsigned int i=0; i<svector.size(); i++) {
    // Sanity check: Make sure this isn't a duplicate
    string str = stripBlankEnds(svector[i]);
    if(m_on_flags_raw.count(str))
      continue;

    m_on_flags_raw.insert(str);
    
    // Ok, create and add the VarDataPair
    string lft = biteStringX(str, '=');
    string rgt = str;
    
    if((lft == "") || (rgt == ""))
      return(false);
    
    VarDataPair pair(lft, rgt, "auto");
    m_on_flags.push_back(pair);
    
  }
  return(true);  
}
#endif

//---------------------------------------------------------------
// Procedure: addAlertOffFlag

bool CMAlert::addAlertOffFlag(string str)
{
  vector<string> svector = parseString(str, '#');
  for(unsigned int i=0; i<svector.size(); i++) {
    // Sanity check: Make sure this isn't a duplicate
    string str = stripBlankEnds(svector[i]);

    if(m_off_flags_raw.count(str))
      continue;
    m_off_flags_raw.insert(str);
    
    // Ok, create and add the VarDataPair
    string lft = biteStringX(str, '=');
    string rgt = str;
    
    if((lft == "") || (rgt == ""))
      return(false);
  
    VarDataPair pair(lft, rgt, "auto");
    m_off_flags.push_back(pair);
  }
  
  return(true);    
}

//---------------------------------------------------------------
// Procedure: valid()
//      Note: For ranges, must have a non-zero range or have been
//            explicitly turned off (indicated by val of -2)
//            OK fo cpa_range to be same as range
//            Must have at least one flag. 

bool CMAlert::valid() const
{
  if(m_range == -1)
    return(false);

  if(!hasAlertOnFlag() && !hasAlertOffFlag())
    return(false);

  return(true);
}

//---------------------------------------------------------------
// Procedure: getAlertOnFlags()

vector<VarDataPair> CMAlert::getAlertOnFlags() const
{
  return(m_on_flags);
}

//---------------------------------------------------------------
// Procedure: getAlertOffFlags()

vector<VarDataPair> CMAlert::getAlertOffFlags() const
{
  return(m_off_flags);
}


//---------------------------------------------------------------
// Procedure: getSummary()
//   Example: on_flag = SAY_MOOS = hello
//            match_type = usv
//            ignore_region = pts={}


vector<string> CMAlert::getSummary() const
{
  vector<string> rvector;

  // Part 1: Get the Alert Ranges
  string alert_range_str = "off";
  if(m_range == -1)
    alert_range_str = "unspecified";
  if(m_range >=0)
    alert_range_str = doubleToStringX(m_range,2);

  
  string alert_range_far_str = "off";
  if(m_range_far == -1)
    alert_range_far_str = "unspecified";
  if(m_range_far >=0)
    alert_range_far_str = doubleToStringX(m_range_far, 2);

  string alert_str = "Alert Ranges: " + alert_range_str;
  alert_str += "/" + alert_range_far_str;
  rvector.push_back(alert_str);

  string source = "--";
  if(m_alert_source != "")
    source = m_alert_source;
  rvector.push_back("Source: " + source); 
  
   // Part 2: Get the On/Off Flags
  for(unsigned int i=0; i<m_on_flags.size(); i++) {
    string msg = "OnFlag: " + m_on_flags[i].getPrintable();
    rvector.push_back(msg);
  }
  for(unsigned int i=0; i<m_off_flags.size(); i++) {
    string msg = "OnFlag: " + m_off_flags[i].getPrintable();
    rvector.push_back(msg);
  }

   // Part 3: Get the Filter Summary
  vector<string> filter_summary = m_filter_set.getSummaryX();
  rvector = mergeVectors(rvector, filter_summary);
  
  return(rvector);
}

