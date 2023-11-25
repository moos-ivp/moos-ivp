/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RealmCastSet.cpp                                     */
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

#include <iostream>
#include "RealmCastSet.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

RealmCastSet::RealmCastSet()
{
  m_proc_id_count     = 0;
  m_max_proc_name_len = 0;
  m_total_relcasts    = 0;
}

//---------------------------------------------------------
// Procedure: report()

string RealmCastSet::report() const
{
  string str;
  map<string, unsigned int>::const_iterator p;
  for(p=m_map_relcast_cnt.begin(); p!=m_map_relcast_cnt.end(); p++) {
    string proc = p->first;
    string scnt = uintToString(p->second);
    if(str != "")
      str += ",";
    str += "proc=" + proc + "[" + scnt + "]";
  }
  return(str);
}

//---------------------------------------------------------
// Procedure: addRealmSummary()

bool RealmCastSet::addRealmSummary(RealmSummary summary,
				   string onstart_proc)
{
  if(!summary.valid())
    return(false);

  vector<string> procs = summary.getProcs();
  for(unsigned int i=0; i<procs.size(); i++) {
    string proc = procs[i];
    
    if(m_map_relcasts.count(proc) == 0) {
      RealmCast empty_realmcast;
      m_map_relcasts[proc] = empty_realmcast;
      m_map_relcast_cnt[proc] = 0;
    }

    if(!vectorContains(m_procs, proc)) {
      m_procs.push_back(proc);
      setNewProcID(proc);
    }

    if(m_current_proc == "")
      m_current_proc = proc;
    if((onstart_proc != "") && (proc == onstart_proc))
      m_current_proc = proc;
    
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: addRealmCast
//   Returns: true if relcast added proc never seen before.

bool RealmCastSet::addRealmCast(const RealmCast& relcast)
{
  m_total_relcasts++;

  string proc = relcast.getProcName();
  if(proc == "")
    proc = "unknown_proc";

  if(relcast.size() == 0)
    return(false);

  bool new_proc = false;
  if(m_map_relcasts.count(proc) == 0)
    new_proc = true;

  // Update the count for this channel/proc and store the info
  // in the realmcast itself.
  m_map_relcast_cnt[proc]++;
  m_map_relcasts[proc] = relcast;

  m_map_relcasts[proc].setCount(m_map_relcast_cnt[proc]);

  if(!new_proc)
    return(false);

  return(true);
}

//---------------------------------------------------------
// Procedure: setNewProcID()
//      Note: Terminal based apps will need to use single chars
//            to select a proc. Likely not used in GUI apps.

void RealmCastSet::setNewProcID(string proc)
{
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
}

//---------------------------------------------------------
// Procedure: getProcNameFromID(id)
//   Returns: The process name associated with the given ID, or
//            the empty string if the process name is not found.

string RealmCastSet::getProcNameFromID(const string& id) const
{
  map<string, string>::const_iterator p = m_map_id_src.find(id);
  if(p==m_map_id_src.end())
    return("");
  else
    return(p->second);
}

//---------------------------------------------------------
// Procedure: hasProc
//   Returns: true if an relcast has been received to-date with
//            the given process name.

bool RealmCastSet::hasProc(const string& proc) const
{
  return(m_map_relcasts.count(proc) != 0);
}

//---------------------------------------------------------
// Procedure: getRealmCastCount(proc)
//   Returns: The total number of relcasts received from the named
//            process.

unsigned int RealmCastSet::getRealmCastCount(const string& proc) const
{
  map<string, unsigned int>::const_iterator p = m_map_relcast_cnt.find(proc);
  if(p==m_map_relcast_cnt.end()) 
    return(0);
  else
    return(p->second);
}

//---------------------------------------------------------
// Procedure: getIDs
//   Returns: vector of all ids [0]="a" [1]="b" [2]="1" [3]="2"

vector<string> RealmCastSet::getIDs() const
{
  vector<string> rvector;

  map<string, string>::const_iterator p;
  for(p=m_map_id_src.begin(); p!=m_map_id_src.end(); p++)
    rvector.push_back(p->first);

  return(rvector);
}

//---------------------------------------------------------
// Procedure: getRealmCast(proc)
//   Returns: A copy of the latest relcast for the named process

RealmCast RealmCastSet::getRealmCast(const string& proc) const
{
  map<string, RealmCast>::const_iterator p = m_map_relcasts.find(proc);
  if(p==m_map_relcasts.end()) {
    RealmCast null_relcast;
    return(null_relcast);
  }
  else
    return(p->second);
}

//---------------------------------------------------------
// Procedure: setCurrentProc()

bool RealmCastSet::setCurrentProc(string proc)
{
  if(!vectorContains(m_procs, proc))
    return(false);
  
  m_current_proc = proc;
  return(true);
}










