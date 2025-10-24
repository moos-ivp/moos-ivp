/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AppCastRepo.cpp                                      */
/*    DATE: Aug 14th 2012                                        */
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

#include <algorithm>
#include <iostream>
#include <iterator>
#include "MBUtils.h"
#include "AppCastRepo.h"

using namespace std;

//---------------------------------------------------------
// Constructor

AppCastRepo::AppCastRepo(bool strip_color)
{
  m_current_node = "";
  m_current_proc = "";
  m_refresh_mode = "events";

  m_strip_color = strip_color;
}

//---------------------------------------------------------
// Procedure: addAppCast
//   Returns: true if first time hearing from this node

bool AppCastRepo::addAppCast(const string& appcast_str)
{
  string str = appcast_str;

  if(m_strip_color) {
    str = findReplace(appcast_str, "\33[7;32m", "");
    str = findReplace(str, "\33[7;31m", "");
    str = findReplace(str, "\33[0m", "");
  }

  AppCast appcast = string2AppCast(str);
  return(addAppCast(appcast));
}

//---------------------------------------------------------
// Procedure: addAppCast
//   Returns: true if first time hearing from this node

bool AppCastRepo::addAppCast(const AppCast& appcast)
{
  string node = appcast.getNodeName();
  string proc = appcast.getProcName();
  if(m_current_node == "")
    m_current_node = node;
  if(m_current_proc == "")
    m_current_proc = proc;

  // If no proc currently tied to current node, tie this proc
  if(m_map_node_proc.count(node) == 0)
    m_map_node_proc[node] = proc;
 
  return(m_appcast_tree.addAppCast(appcast));
}


//---------------------------------------------------------
// Procedure: removeNode

bool AppCastRepo::removeNode(const string& nodename)
{
  if(!m_appcast_tree.removeNode(nodename))
    return(false);

  m_map_node_proc.erase(nodename);

  vector<string> nodes = m_appcast_tree.getNodes();
  if(nodes.size() != 0) {
    setCurrentNode(nodes[0]);
    setCurrentNode(m_map_node_proc[nodes[0]]);
  }

  return(true);
}


//---------------------------------------------------------
// Procedure: setCurrentNode
//   Returns: true if node is valid 

bool AppCastRepo::setCurrentNode(const string& str)
{
  string new_current_node;

  if(m_appcast_tree.hasNode(str))
    new_current_node = str;
  else {
    string nodename_from_id = m_appcast_tree.getNodeNameFromID(str);
    if(m_appcast_tree.hasNode(nodename_from_id))
      new_current_node = nodename_from_id;
  }
  
  if(new_current_node == "")
    return(false);
  else
    m_current_node = new_current_node;
  
  string related_proc = m_map_node_proc[m_current_node];
  if(m_appcast_tree.hasNodeProc(m_current_node, related_proc))
    m_current_proc = related_proc;

  return(true);
}

//---------------------------------------------------------
// Procedure: setRefreshMode
//   Returns: true if the given mode is a valid mode name

bool AppCastRepo::setRefreshMode(const string& given_mode)
{
  string mode = tolower(given_mode);
  if((mode == "streaming") || (mode == "events") || (mode == "paused")) {
    m_refresh_mode = mode;
    return(true);
  }
  return(false);
}

//---------------------------------------------------------
// Procedure: setCurrentProc
//   Returns: true if given str names an proc known to this node, or if it
//            names an ID mapped to a proc known to this node, 
//      Note: The given key may be an Proc name, or an ID mapped to an Proc

bool AppCastRepo::setCurrentProc(const string& str)
{
  string new_current_proc;

  if(m_appcast_tree.hasNodeProc(m_current_node, str))
    new_current_proc = str;
  else {
    string procname_from_id = m_appcast_tree.getProcNameFromID(m_current_node, str);
    if(m_appcast_tree.hasNodeProc(m_current_node, procname_from_id))
      new_current_proc = procname_from_id;
  }

  if(new_current_proc == "")
    return(false);
  else
    m_current_proc = new_current_proc;

  m_map_node_proc[m_current_node] = m_current_proc;
  return(true);
}

//---------------------------------------------------------
// Procedure: getCurrentNodes()

vector<string> AppCastRepo::getCurrentNodes() const
{
  vector<string> rvector;
  rvector = m_appcast_tree.getNodes();
  if(rvector.size() == 0)
    return(rvector);

  std::sort(rvector.begin(), rvector.end());

  if(rvector.back() == "shoreside") {
    vector<string> rvector2;
    rvector2.push_back("shoreside");
    for(unsigned int i=0; i<rvector.size()-1; i++)
      rvector2.push_back(rvector[i]);
    rvector = rvector2;
  }

  return(rvector);
}

//---------------------------------------------------------
// Procedure: getCurrentProcs()

vector<string> AppCastRepo::getCurrentProcs() const
{
  vector<string> procs = m_appcast_tree.getProcs(m_current_node);

  vector<string> sorted_procs;
  
  set<string> set_procs;
  for(unsigned int i=0; i<procs.size(); i++) 
    set_procs.insert(procs[i]);

  string checks = "pHelmIvP,pNodeReporter,pContactMgrV20,uFldNodeBroker";
  checks += ",pObstacleMgr,uTimerScript,pMarinePIDV22,uSimMarineV22";
  checks += ",pMissionEval,uFldMessageHandler,uLoadWatch,uProcessWatch";
  checks += ",uXMS,pDeadManPost,pHostInfo,pRealm,uMemWatch";

  while(checks != "") {
    string check = biteStringX(checks, ',');
    if(set_procs.count(check)) {
      sorted_procs.push_back(check);
      set_procs.erase(check);
    }
  }
  for(set<string>::iterator p=set_procs.begin(); p!=set_procs.end(); p++)
    sorted_procs.push_back(*p);

  return(sorted_procs);
}
  
//  return(m_appcast_tree.getProcs(m_current_node));


//---------------------------------------------------------
// Procedure: getNodeCount

unsigned int AppCastRepo::getNodeCount() const
{
  return(m_appcast_tree.getTreeNodeCount());
}

//---------------------------------------------------------
// Procedure: getProcCount

unsigned int AppCastRepo::getProcCount() const
{
  return(m_appcast_tree.getTreeProcCount());
}

//---------------------------------------------------------
// Procedure: getProcCount

unsigned int AppCastRepo::getProcCount(string node) const
{
  return(m_appcast_tree.getNodeTotalProcCount(node));
}

//---------------------------------------------------------
// Procedure: getAppCastCount

unsigned int AppCastRepo::getAppCastCount(string node, string proc) const
{
  return(m_appcast_tree.getProcAppCastCount(node, proc));
}

//---------------------------------------------------------
// Procedure: getAppCastCount

unsigned int AppCastRepo::getAppCastCount(string node) const
{
  return(m_appcast_tree.getNodeAppCastCount(node));
}










