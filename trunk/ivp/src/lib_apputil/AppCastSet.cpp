/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppCastSet.cpp                                       */
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

#include "AppCastSet.h"
using namespace std;

//---------------------------------------------------------
// Constructor

AppCastSet::AppCastSet()
{
  m_proc_id_count     = 0;
  m_max_proc_name_len = 0;
  m_total_appcasts    = 0;
}

//---------------------------------------------------------
// Procedure: addAppCast
//   Returns: true if appcast added proc never seen before.

bool AppCastSet::addAppCast(const AppCast& appcast)
{
  m_total_appcasts++;

  string proc = appcast.getProcName();
  if(proc == "")
    proc = "unknown_proc";

  if(appcast.size() == 0)
    return(false);

  bool new_proc = false;
  if(m_map_appcasts.count(proc) == 0)
    new_proc = true;
  
  m_map_appcast_cnt[proc]++;
  m_map_appcasts[proc] = appcast;

  if(!new_proc)
    return(false);
  else
    m_procs.push_back(proc);

  if(proc == "uFldShoreBroker")
    m_map_id_src["9"] = proc;
  else if(proc == "uFldNodeBroker")
    m_map_id_src["1"] = proc;
  else if(proc == "pHostInfo")
    m_map_id_src["3"] = proc;
  else if(proc == "pBasicContactMgr")
    m_map_id_src["2"] = proc;
  else if(proc == "pHelmIvP")
    m_map_id_src["0"] = proc;
  else {
    unsigned char c = 97 + (int)(m_proc_id_count);  // 'a' + cnt
    m_proc_id_count++;

    if((c=='e') || (c=='h') || (c=='n')) {
      c++;
      m_proc_id_count++;
    }
    else if((c=='p') || (c=='q') || (c=='r') || (c=='s')) {
      c += 4;
      m_proc_id_count += 4;
    }

    // If exceeded the {a-z}, then start using {A-Z}
    if((c > 122) && (c <= 126))
      c = c - 58;
    
    string id(1,c);
    m_map_id_src[id] = proc;
  }

  if(proc.length() > m_max_proc_name_len)
    m_max_proc_name_len = proc.length();

  return(true);
}

//---------------------------------------------------------
// Procedure: getProcNameFromID(id)
//   Returns: The process name associated with the given ID, or
//            the empty string if the process name is not found.

string AppCastSet::getProcNameFromID(const string& id) const
{
  map<string, string>::const_iterator p = m_map_id_src.find(id);
  if(p==m_map_id_src.end())
    return("");
  else
    return(p->second);
}

//---------------------------------------------------------
// Procedure: hasProc
//   Returns: true if an appcast has been received to-date with
//            the given process name.

bool AppCastSet::hasProc(const string& proc) const
{
  return(m_map_appcasts.count(proc) != 0);
}

//---------------------------------------------------------
// Procedure: getAppCastCount(proc)
//   Returns: The total number of appcasts received from the named
//            process.

unsigned int AppCastSet::getAppCastCount(const string& proc) const
{
  map<string, unsigned int>::const_iterator p = m_map_appcast_cnt.find(proc);
  if(p==m_map_appcast_cnt.end()) 
    return(0);
  else
    return(p->second);
}

//---------------------------------------------------------
// Procedure: getCfgWarningCount(proc)
//   Returns: Total number of configuration warnings in the latest
//            appcast received from the named process.

unsigned int AppCastSet::getCfgWarningCount(const string& proc) const
{
  map<string, AppCast>::const_iterator p = m_map_appcasts.find(proc);
  if(p==m_map_appcasts.end()) 
    return(0);
  else
    return(p->second.getCfgWarningCount());
}

//---------------------------------------------------------
// Procedure: getRunWarningCount(proc)
//   Returns: Total number of run warnings in the latest appcast
//            received from the named process.

unsigned int AppCastSet::getRunWarningCount(const string& proc) const
{
  map<string, AppCast>::const_iterator p = m_map_appcasts.find(proc);
  if(p==m_map_appcasts.end()) 
    return(0);
  else
    return(p->second.getRunWarningCount());
}

//---------------------------------------------------------
// Procedure: getTotalCfgWarningCount()
//   Returns: Total number of config warnings for ALL processes.

unsigned int AppCastSet::getTotalCfgWarningCount() const
{
  unsigned int total = 0;

  map<string, AppCast>::const_iterator p;
  for(p=m_map_appcasts.begin(); p!=m_map_appcasts.end(); p++)
    total += p->second.getCfgWarningCount();

  return(total);
}

//---------------------------------------------------------
// Procedure: getTotalRunWarningCount()
//   Returns: Total number of run warnings for ALL processes.

unsigned int AppCastSet::getTotalRunWarningCount() const
{
  unsigned int total = 0;

  map<string, AppCast>::const_iterator p;
  for(p=m_map_appcasts.begin(); p!=m_map_appcasts.end(); p++)
    total += p->second.getRunWarningCount();

  return(total);
}

//---------------------------------------------------------
// Procedure: getTotalWarningCount()
//   Returns: Total number of config + run warnings for ALL procs.

unsigned int AppCastSet::getTotalWarningCount() const
{
  return(getTotalCfgWarningCount() + getTotalRunWarningCount());
}

//---------------------------------------------------------
// Procedure: getIDs
//   Returns: vector of all ids [0]="a" [1]="b" [2]="1" [3]="2"

vector<string> AppCastSet::getIDs() const
{
  vector<string> rvector;

  map<string, string>::const_iterator p;
  for(p=m_map_id_src.begin(); p!=m_map_id_src.end(); p++)
    rvector.push_back(p->first);

  return(rvector);
}

//---------------------------------------------------------
// Procedure: getAppCast(proc)
//   Returns: A copy of the latest appcast for the named process

AppCast AppCastSet::getAppCast(const string& proc) const
{
  map<string, AppCast>::const_iterator p = m_map_appcasts.find(proc);
  if(p==m_map_appcasts.end()) {
    AppCast null_appcast;
    return(null_appcast);
  }
  else
    return(p->second);
}









