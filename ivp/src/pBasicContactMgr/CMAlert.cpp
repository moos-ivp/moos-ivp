/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
  m_range = 0;
  m_range_far = 0;
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
// Procedure: setAlertRangeColor

bool CMAlert::setAlertRangeColor(string str)
{
  if(!isColor(str))
    return(false);
  m_rng_color = str;
  return(true);
}

//---------------------------------------------------------------
// Procedure: setAlertRangeFarColor

bool CMAlert::setAlertRangeFarColor(string str)
{
  if(!isColor(str))
    return(false);
  m_rng_far_color = str;
  return(true);
}

//---------------------------------------------------------------
// Procedure: setAlertRegion

bool CMAlert::setAlertRegion(string str)
{
  XYPolygon poly = string2Poly(str);
  if(!poly.is_convex())
    return(false);
  
  m_region = poly;
  return(true);
}

//---------------------------------------------------------------
// Procedure: addAlertOnFlag

bool CMAlert::addAlertOnFlag(string str)
{
  string lft = biteStringX(str, '=');
  string rgt = str;

  if((lft == "") || (rgt == ""))
    return(false);

  VarDataPair pair(lft, rgt, "auto");
  m_on_flags.push_back(pair);
  return(true);    
}

//---------------------------------------------------------------
// Procedure: addAlertOffFlag

bool CMAlert::addAlertOffFlag(string str)
{
  string lft = biteStringX(str, '=');
  string rgt = str;

  if((lft == "") || (rgt == ""))
    return(false);
  
  VarDataPair pair(lft, rgt, "auto");
  m_off_flags.push_back(pair);
  return(true);    
}

//---------------------------------------------------------------
// Procedure: addMatchType()
//            If match types are configured with this alert, then
//            a contact, if it has a type, its type must be one of
//            these match types.
//  Examples: "kayak", "mokai,kayak,uuv"

bool CMAlert::addMatchType(string str)
{
  bool all_ok = true;
  
  vector<string> svector = parseString(str, ':');
  for(unsigned int i=0; i<svector.size(); i++) {
    string match_type = stripBlankEnds(svector[i]);
    if(!vectorContains(m_match_type, match_type))
      m_match_type.push_back(match_type);
    else
      all_ok = false;
  }
  return(all_ok);    
}

//---------------------------------------------------------------
// Procedure: addIgnoreType()
//            If ignore types are configured with this alert, then
//            a contact, if it has a type, its type must NOT be 
//            one of these ignore types.
//  Examples: "kayak", "mokai,kayak,uuv"

bool CMAlert::addIgnoreType(string str)
{
  bool all_ok = true;
  
  vector<string> svector = parseString(str, ':');
  for(unsigned int i=0; i<svector.size(); i++) {
    string ignore_type = stripBlankEnds(svector[i]);
    if(!vectorContains(m_ignore_type, ignore_type))
      m_ignore_type.push_back(ignore_type);
    else
      all_ok = false;
  }
  return(all_ok);    
}

//---------------------------------------------------------------
// Procedure: addMatchGroup()
//            If match groups are configured with this alert, then
//            a contact, if it has a group, its group must be one of
//            these match groups.

bool CMAlert::addMatchGroup(string str)
{
  bool all_ok = true;
  
  vector<string> svector = parseString(str, ':');
  for(unsigned int i=0; i<svector.size(); i++) {
    string match_group = stripBlankEnds(svector[i]);
    if(!vectorContains(m_match_group, match_group))
      m_match_group.push_back(match_group);
    else
      all_ok = false;
  }
  return(all_ok);    
}

//---------------------------------------------------------------
// Procedure: addIgnoreGroup()
//            If ignore groups are configured with this alert, then
//            a contact, if it has a group, its group must NOT be 
//            one of these ignore groups.

bool CMAlert::addIgnoreGroup(string str)
{
  bool all_ok = true;
  
  vector<string> svector = parseString(str, ':');
  for(unsigned int i=0; i<svector.size(); i++) {
    string ignore_group = stripBlankEnds(svector[i]);
    if(!vectorContains(m_ignore_group, ignore_group))
      m_ignore_group.push_back(ignore_group);
    else
      all_ok = false;
  }
  return(all_ok);    
}

//---------------------------------------------------------------
// Procedure: getAlertOnFlags

vector<VarDataPair> CMAlert::getAlertOnFlags() const
{
  return(m_on_flags);
}

//---------------------------------------------------------------
// Procedure: getAlertOffFlags

vector<VarDataPair> CMAlert::getAlertOffFlags() const
{
  return(m_off_flags);
}
