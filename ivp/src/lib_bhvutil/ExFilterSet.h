/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ExFilterSet.h                                        */
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

#ifndef EX_FILTER_SET_HEADER
#define EX_FILTER_SET_HEADER

#include <set>
#include <vector>
#include "XYPolygon.h"
#include "NodeRecord.h"

class ExFilterSet
{
 public:
  ExFilterSet();
  ~ExFilterSet() {};

 public: // setters

  bool addIgnoreName(std::string);
  bool addMatchName(std::string);
  bool addIgnoreGroup(std::string);
  bool addMatchGroup(std::string);
  bool addIgnoreType(std::string);
  bool addMatchType(std::string);
  bool addIgnoreRegion(std::string);
  bool addMatchRegion(std::string);

  bool setOwnshipGroup(std::string);
  bool setOwnshipType(std::string);
  
  bool setStrictIgnore(std::string);
  
  bool addIgnoreRegion(XYPolygon);
  bool addMatchRegion(XYPolygon);
  void setStrictIgnore(bool v) {m_strict_ignore=v;}

 public: // Analysis
  bool filterCheck(NodeRecord) const;
  bool filterCheck(NodeRecord, double osx, double osy) const;
  bool filterCheckGroup(std::string group) const;
  bool filterCheckVType(std::string vtype) const;
  bool filterCheckVName(std::string vtype) const;
  bool filterCheckRegion(double cnx, double cny) const;
  
 public: // Serialization
  std::string configFilter(std::string);
  std::string getSummary() const;

  std::vector<std::string> getSummaryX() const;
  
 private: 
  bool     m_strict_ignore;

  std::string m_os_group;
  std::string m_os_type;
  
  std::set<std::string> m_ignore_names;
  std::set<std::string> m_match_names;

  std::set<std::string> m_ignore_groups;
  std::set<std::string> m_match_groups;

  std::set<std::string> m_ignore_types;
  std::set<std::string> m_match_types;

  std::vector<XYPolygon>   m_ignore_regions;
  std::vector<XYPolygon>   m_match_regions;
};

#endif 

