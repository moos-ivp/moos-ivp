/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ExFilterSet.cpp                                      */
/*    DATE: July 12th, 2020                                      */
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

#include <cmath>
#include "MBUtils.h"
#include "ExFilterSet.h"
#include "XYFormatUtilsPoly.h"
 
using namespace std;

// ----------------------------------------------------------
// Constructor

ExFilterSet::ExFilterSet()
{
  m_strict_ignore = true;
}


// ----------------------------------------------------------
// Procedure: configFilter()

string ExFilterSet::configFilter(string str)
{
  string unhandled_params;
  
  vector<string> svector = parseStringZ(str, ',', "{");
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    bool ok = false;
    if(param == "ignore_group")
      ok = addIgnoreGroup(value);
    else if(param == "match_group")
      ok = addMatchGroup(value);
    else if(param == "ignore_type")
      ok = addIgnoreType(value);
    else if(param == "match_type")
      ok = addMatchType(value);
    else if(param == "ignore_region")
      ok = addIgnoreRegion(value);
    else if(param == "match_region")
      ok = addMatchRegion(value);
    else if(param == "strict_ignore")
      ok = setStrictIgnore(value);

    if(!ok) {
      if(unhandled_params != "")
	unhandled_params += ",";
      unhandled_params += param + "=" + value;
    }
  }
  
  return(unhandled_params);
}
  
// ----------------------------------------------------------
// Procedure: setStrictIgnore()

bool ExFilterSet::setStrictIgnore(string str)
{
  return(setBooleanOnString(m_strict_ignore, str));
}

// ----------------------------------------------------------
// Procedure: addIgnoreName()
//   Example: "hal" or "hal,abe" or "{hal,abe}"
//   Returns: false if a given name is already a match name

bool ExFilterSet::addIgnoreName(string str)
{
  // All group names are treated case insensitive
  str = stripBraces(stripBlankEnds(tolower(str)));

  bool all_ok = true;

  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string vname = svector[i];
    if(m_match_names.count(vname)) 
      all_ok = false;
    else 
      m_ignore_names.insert(vname);
  }
  return(all_ok);
}


// ----------------------------------------------------------
// Procedure: addMatchName()
//   Example: "hal" or "hal,abe" or "{hal,abe}"
//   Returns: false if a given name is already an ignore name

bool ExFilterSet::addMatchName(string str)
{
  // All group names are treated case insensitive
  str = stripBraces(stripBlankEnds(tolower(str)));
  
  bool all_ok = true;

  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string vname = svector[i];
    if(m_ignore_names.count(vname))
      all_ok = false;
    else
      m_match_names.insert(vname);
  }
  return(all_ok);
}


// ----------------------------------------------------------
// Procedure: addIgnoreGroup()
//   Example: "blue" or "red,blue" or "{red,blue}"
//   Returns: false if a given group is already a match group

bool ExFilterSet::addIgnoreGroup(string str)
{
  // All group names are treated case insensitive
  str = stripBraces(stripBlankEnds(tolower(str)));

  bool all_ok = true;

  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string group = svector[i];
    if(m_match_groups.count(group)) 
      all_ok = false;
    else 
      m_ignore_groups.insert(group);
  }
  return(all_ok);
}


// ----------------------------------------------------------
// Procedure: addMatchGroup()
//   Example: "blue" or "red,blue" or "{red,blue}"
//   Returns: false if a given group is already an ignore group

bool ExFilterSet::addMatchGroup(string str)
{
  // All group names are treated case insensitive
  str = stripBraces(stripBlankEnds(tolower(str)));
  
  bool all_ok = true;

  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string group = svector[i];
    if(m_ignore_groups.count(group))
      all_ok = false;
    else
      m_match_groups.insert(group);
  }
  return(all_ok);
}


// ----------------------------------------------------------
// Procedure: addIgnoreType()
//   Example: "uuv" or "uuv,usv" or "{uuv,usv}"
//   Returns: false if a given type is already a match type

bool ExFilterSet::addIgnoreType(string str)
{
  // All platform type names are treated case insensitive
  str = stripBraces(stripBlankEnds(tolower(str)));

  bool all_ok = true;

  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string vtype = svector[i];
    if(m_match_types.count(vtype))
      all_ok = false;
    else
      m_ignore_types.insert(vtype);
  }
  return(all_ok);
}

// ----------------------------------------------------------
// Procedure: addMatchType()
//   Example: "uuv" or "uuv,usv" or "{uuv,usv}"
//   Returns: false if a given type is already an ignore type

bool ExFilterSet::addMatchType(string str)
{
  // All platform type names are treated case insensitive
  str = stripBraces(stripBlankEnds(tolower(str)));
  
  bool all_ok = true;

  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string vtype = svector[i];
    if(m_ignore_types.count(vtype))
      all_ok = false;
    else
      m_match_types.insert(vtype);
  }
  return(all_ok);
}

// ----------------------------------------------------------
// Procedure: addIgnoreRegion()

bool ExFilterSet::addIgnoreRegion(string str)
{
 XYPolygon poly = string2Poly(str);
  if(!poly.is_convex())
    return(false);

  m_ignore_regions.push_back(poly);
  return(true);
}

// ----------------------------------------------------------
// Procedure: addMatchRegion()

bool ExFilterSet::addMatchRegion(string str)
{
 XYPolygon poly = string2Poly(str);
  if(!poly.is_convex())
    return(false);

  m_match_regions.push_back(poly);
  return(true);
}

// ----------------------------------------------------------
// Procedure: addIgnoreRegion()

bool ExFilterSet::addIgnoreRegion(XYPolygon poly)
{
  if(!poly.is_convex())
    return(false);

  m_ignore_regions.push_back(poly);
  return(true);
}

// ----------------------------------------------------------
// Procedure: addMatchRegion()

bool ExFilterSet::addMatchRegion(XYPolygon poly)
{
  if(!poly.is_convex())
    return(false);

  m_match_regions.push_back(poly);
  return(true);
}

// ----------------------------------------------------------
// Procedure: setOwnshipGroup()

bool ExFilterSet::setOwnshipGroup(string group_name)
{
  return(setNonWhiteVarOnString(m_os_group, group_name));
}

// ----------------------------------------------------------
// Procedure: setOwnshipType()

bool ExFilterSet::setOwnshipType(string type_name)
{
  return(setNonWhiteVarOnString(m_os_type, type_name));
}

// ----------------------------------------------------------
// Procedure: filterCheck()
//   Returns: true if it passes all filters
//      Note: This filter check will perform additional filter checks
//            for which ownship position needs to be considered.

bool ExFilterSet::filterCheck(NodeRecord record, double osx, double osy) const
{
  // Check the general set of filters
  if(!filterCheck(record))
    return(false);

  return(true);
}


// ----------------------------------------------------------
// Procedure: filterCheck()
//   Returns: true if it passes all filters

bool ExFilterSet::filterCheck(NodeRecord record) const
{
  string vname = tolower(record.getName());
  string group = record.getGroup();
  string vtype = record.getType();
  double cnx = record.getX();
  double cny = record.getY();

  if(!filterCheckGroup(group))
    return(false);
  if(!filterCheckVName(vname))
    return(false);
  if(!filterCheckVType(vtype))
    return(false);
  if(!filterCheckRegion(cnx, cny))
    return(false);

  return(true);
}

// ----------------------------------------------------------
// Procedure: filterCheckGroup()

bool ExFilterSet::filterCheckGroup(string group) const
{
  // Part 1: Apply Match Groups
  if(m_match_groups.size() > 0) {
    if(m_match_groups.count(group) == 0)
      return(false);
  }
  // Part 2: Apply Ignore Groups
  if(m_ignore_groups.size() > 0) {
    if(m_strict_ignore && (group == ""))
      return(false);
    if(m_ignore_groups.count(group))
      return(false);
  }
  return(true);
}

// ----------------------------------------------------------
// Procedure: filterCheckVType()

bool ExFilterSet::filterCheckVType(string vtype) const
{
  // Part 1: Apply Match Types
  if(m_match_types.size() > 0) {
    if(m_match_types.count(vtype) == 0)
      return(false);
  }
  // Part 2: Apply Ignore Type
  if(m_ignore_types.size() > 0) {
    if(m_strict_ignore && (vtype == ""))
      return(false);   
    if(m_ignore_types.count(vtype))
      return(false);
  }

  return(true);
}

// ----------------------------------------------------------
// Procedure: filterCheckVName()

bool ExFilterSet::filterCheckVName(string vname) const
{
  // Part 1: Apply Match Names
  if(m_match_names.size() > 0) {
    if(m_match_names.count(vname) == 0)
      return(false);
  }
  // Part 2: Apply Ignore Names
  if(m_ignore_names.size() > 0) {
    if(m_strict_ignore && (vname == ""))
      return(false);   
    if(m_ignore_names.count(vname))
      return(false);
  }
  return(true);
}

// ----------------------------------------------------------
// Procedure: filterCheckRegion()

bool ExFilterSet::filterCheckRegion(double cnx, double cny) const
{
  // Part 1: Apply Match Regions
  if(m_match_regions.size() > 0) {
    bool in_at_least_one = false;
    for(unsigned int i=0; i<m_match_regions.size(); i++) {
      if(m_match_regions[i].contains(cnx, cny))
	in_at_least_one = true;
    }
    if(!in_at_least_one)
      return(false);
  }
  // Part 2: Apply Ignore Regions
  if(m_ignore_regions.size() > 0) {
    for(unsigned int i=0; i<m_ignore_regions.size(); i++) {
      if(m_ignore_regions[i].contains(cnx, cny))
	return(false);
    }
  }
  
  return(true);
}

// ----------------------------------------------------------
// Procedure: getSummary()
//   Returns: serialized full description of the filter set

string ExFilterSet::getSummary() const
{
  string str; 

  vector<string> svector = getSummaryX();
  for(unsigned int i=0; i<svector.size(); i++) {
    if(i != 0)
      str += ", ";
    str += svector[i];
  }

  return(str);
}

// ----------------------------------------------------------
// Procedure: getSummaryX()
//   Returns: serialized full description of the filter set

vector<string> ExFilterSet::getSummaryX() const
{
  vector<string> summary; 

  set<string>::iterator p;
  
  for(p=m_ignore_groups.begin(); p!=m_ignore_groups.end(); p++)
    summary.push_back("ignore_group=" + *p);
  for(p=m_match_groups.begin(); p!=m_match_groups.end(); p++)
    summary.push_back("match_group=" + *p);

  for(p=m_ignore_names.begin(); p!=m_ignore_names.end(); p++)
    summary.push_back("ignore_name=" + *p);
  for(p=m_match_names.begin(); p!=m_match_names.end(); p++)
    summary.push_back("match_name=" + *p);

  for(p=m_ignore_types.begin(); p!=m_ignore_types.end(); p++)
    summary.push_back("ignore_type=" + *p);
  for(p=m_match_types.begin(); p!=m_match_types.end(); p++)
    summary.push_back("match_type=" + *p);

  for(unsigned int i=0; i<m_ignore_regions.size(); i++) 
    summary.push_back("ignore_region=" + m_ignore_regions[i].get_spec_pts());
  for(unsigned int i=0; i<m_match_regions.size(); i++) 
    summary.push_back("match_region=" + m_match_regions[i].get_spec_pts());

  if((m_ignore_types.size() > 0) || (m_ignore_groups.size() > 0))
    summary.push_back("strict_ignore=" + boolToString(m_strict_ignore));
  
  return(summary);
}

