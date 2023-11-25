/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WatchCluster.h                                       */
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

#ifndef WATCH_CLUSTER_HEADER
#define WATCH_CLUSTER_HEADER

#include <string>
#include <map>
#include "WatchCast.h"
#include "InfoCastSettings.h"

class WatchCluster
{
 public:
  WatchCluster();
  ~WatchCluster() {}

  void setKey(std::string s)    {m_key=s;}
  bool addVar(std::string s);

  bool addWatchCast(const WatchCast&);

  std::string getKey() const   {return(m_key);}

  std::vector<std::string> getVars() const   {return(m_cluster_vars);}
  std::string getVarsAsString() const;
  
  std::vector<std::string> getReport(std::string var, InfoCastSettings ics);

  unsigned int getVarCount(std::string var) const;
  unsigned int getWCastCount() const {return(m_wcast_cnt);}

 protected:
  std::vector<std::string> getReportSingle(std::string, InfoCastSettings) const;
  std::vector<std::string> getReportMulti() const;
  
 private: 
  std::string m_key;

  unsigned int m_report_cnt;
  unsigned int m_wcast_cnt;
  
  std::vector<std::string> m_cluster_vars;

  // outer key is the node. Inner key is the varname.
  // m_map_info[node][varname] = watchast
  
  std::map<std::string, std::map<std::string, WatchCast> > m_map_info;

  // key is varname, count is number of watchcast updates for this var
  std::map<std::string, unsigned int> m_map_varcount;
};

#endif 

