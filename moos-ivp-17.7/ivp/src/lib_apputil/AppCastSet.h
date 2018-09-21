/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppCastSet.h                                         */
/*    DATE: June 12th 2012                                       */
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

#ifndef APPCAST_SET_HEADER
#define APPCAST_SET_HEADER

#include <string>
#include <map>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCast.h"

class AppCastSet
{
 public:
  AppCastSet();
  ~AppCastSet() {}

  bool         addAppCast(const AppCast& appcast);
  AppCast      getAppCast(const std::string& proc) const;

  std::string  getProcNameFromID(const std::string& id)    const;
  bool         hasProc(const std::string& proc)            const;
  unsigned int getAppCastCount(const std::string& proc)    const;
  unsigned int getCfgWarningCount(const std::string& proc) const;
  unsigned int getRunWarningCount(const std::string& proc) const;

  
  unsigned int getTotalCfgWarningCount() const;
  unsigned int getTotalRunWarningCount() const;
  unsigned int getTotalWarningCount() const;
  unsigned int getTotalAppCastCount() const {return(m_total_appcasts);}
  unsigned int getTotalProcCount()    const {return(m_map_appcasts.size());}

  std::vector<std::string> getIDs()   const;
  std::vector<std::string> getProcs() const {return(m_procs);}

 private:
  // Map from ID to proc name
  std::map<std::string, std::string>  m_map_id_src;

  // Maps from proc name to various info
  std::map<std::string, AppCast>      m_map_appcasts;
  std::map<std::string, unsigned int> m_map_appcast_cnt;

  // total number of id's used so far
  unsigned int m_proc_id_count;

  // Max string length of all process names
  unsigned int m_max_proc_name_len;

  unsigned int m_total_appcasts;

  // Keep a separate vector of proc names so that when proc name vector is
  // retrieved by caller, the earlier items stay in the same order. The 
  // alternative, iterating through the map, means order may shift as map grows.
  std::vector<std::string> m_procs;
};

#endif 








