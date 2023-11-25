/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RealmCastSet.h                                       */
/*    DATE: Dec 4th 2020                                         */
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

#ifndef REALMCAST_SET_HEADER
#define REALMCAST_SET_HEADER

#include <vector>
#include <string>
#include <map>
#include "RealmCast.h"
#include "RealmSummary.h"

class RealmCastSet
{
 public:
  RealmCastSet();
  ~RealmCastSet() {}

  bool         addRealmSummary(RealmSummary, std::string onstart_proc);
  
  bool         addRealmCast(const RealmCast& relcast);
  RealmCast    getRealmCast(const std::string& proc) const;

  std::string  getProcNameFromID(const std::string& id) const;
  bool         hasProc(const std::string& proc) const;
  unsigned int getRealmCastCount(const std::string& proc) const;
  
  unsigned int getTotalRealmCastCount() const {return(m_total_relcasts);}
  unsigned int getTotalProcCount() const      {return(m_map_relcasts.size());}

  std::vector<std::string> getIDs() const;
  std::vector<std::string> getProcs() const {return(m_procs);}

  bool        setCurrentProc(std::string);
  std::string getCurrentProc() const {return(m_current_proc);}
  
  std::string report() const;
  
 protected:
  void setNewProcID(std::string node);
  
 private:
  // Map from ID to proc name
  std::map<std::string, std::string>  m_map_id_src;

  // Maps from proc name to various info
  std::map<std::string, RealmCast>    m_map_relcasts;
  std::map<std::string, unsigned int> m_map_relcast_cnt;

  // total number of id's used so far
  unsigned int m_proc_id_count;

  // Max string length of all process names
  unsigned int m_max_proc_name_len;

  unsigned int m_total_relcasts;

  std::string m_current_proc;
  
  // Keep a separate vector of proc names so that when proc name
  // vector is retrieved by caller, the earlier items stay in the same
  // order. The alternative, iterating through the map, means order
  // may shift as map grows.
  std::vector<std::string> m_procs;

  

  
};

#endif 



