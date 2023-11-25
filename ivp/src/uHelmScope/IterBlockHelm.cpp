/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: IterBlockHelm.cpp                                    */
/*    DATE: Apr 12th 2008                                        */
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
#include <cstdlib>
#include "MBUtils.h"
#include "IterBlockHelm.h"
#include "BehaviorRecord.h"

using namespace std;

//------------------------------------------------------------
// Constructor

IterBlockHelm::IterBlockHelm()
{
  m_iteration   = 0;
  m_utc_time    = 0;
  m_solve_time  = 0;
  m_create_time = 0;
  m_loop_time   = 0;
  m_halted      = false;

  m_count_ipf   = 0;
  m_warning_cnt = 0;
  m_error_cnt   = 0;
  m_posting_cnt = 0;
  m_infomsg_cnt = 0;
}

//------------------------------------------------------------
// Procedure: initialize()

void IterBlockHelm::initialize(const IterBlockHelm& block)
{
  m_active_bhv    = block.m_active_bhv;
  m_running_bhv   = block.m_running_bhv;
  m_idle_bhv      = block.m_idle_bhv;
  m_completed_bhv = block.m_completed_bhv;

  m_decvar        = block.m_decvar;
  m_decval        = block.m_decval;
  m_decchg        = block.m_decchg;

  // Mark all decision vars "unchanged" until proven otherwise
  unsigned int i, vsize = m_decchg.size();
  for(i=0; i<vsize; i++)
    m_decchg[i] = false;

  m_modes         = block.m_modes;

  m_solve_time    = block.m_solve_time;
  m_create_time   = block.m_create_time;
  m_loop_time     = block.m_loop_time;
  m_halted        = block.m_halted;

  m_count_ipf     = block.m_count_ipf;
  m_warning_cnt   = block.m_warning_cnt;
  m_error_cnt     = block.m_error_cnt;
  m_posting_cnt   = block.m_posting_cnt;
  m_infomsg_cnt   = block.m_infomsg_cnt;
}

//------------------------------------------------------------
// Procedure: setActiveBHVs
// Ex:  "waypoint$100: loiter$98.2: return$45.1"

void IterBlockHelm::setActiveBHVs(const string& str)
{
  vector<string> svector = parseString(str, ':');
  unsigned int i, vsize = svector.size();
  
  m_active_bhv.clear();

  for(i=0; i<vsize; i++) {
    BehaviorRecord record;
    svector[i] = stripBlankEnds(svector[i]);
    vector<string> ivector = parseString(svector[i], '$');
    unsigned int j, isize = ivector.size();
    for(j=0; j<isize; j++)
      ivector[j] = stripBlankEnds(ivector[j]);

    record.setName(ivector[0]);
    if(isize >= 2)
      record.setTimeStamp(atof(ivector[1].c_str()));
    if(isize >= 3)
      record.setPriority(ivector[2]);
    if(isize >= 4)
      record.setPieces(ivector[3]);
    if(isize >= 5)
      record.setTimeCPU(ivector[4]);
    if(isize >= 6)
      if(ivector[5] != "n/a")
	record.setUpdates(ivector[5]);
    if(isize >= 7)
      record.setIPFCount(ivector[6]);
    if(isize >= 8)
      record.setOriginal(ivector[7]);
 

   m_active_bhv.push_back(record);
  }
}

//------------------------------------------------------------
// Procedure: setRunningBHVs
//
// Example "Loiter$23.4$n/a" - has three fields separated by '$'
//    (1) "Loiter" is the name of the behavior
//    (2) "23.4" is the number of second it has been in this state
//    (3) "n/a" means the update feature off for this behavior

void IterBlockHelm::setRunningBHVs(const string& str)
{
  vector<string> svector = parseString(str, ':');
  unsigned int i, vsize = svector.size();

  m_running_bhv.clear();

  for(i=0; i<vsize; i++) {
    BehaviorRecord record;
    vector<string> ivector = parseString(svector[i], '$'); 
    unsigned int j, isize = ivector.size();
    for(j=0; j<isize; j++)
      ivector[j] = stripBlankEnds(ivector[j]);
    record.setName(ivector[0]);
    if(isize >= 2)
      record.setTimeStamp(atof(ivector[1].c_str()));
    if(isize >= 3)
      if(ivector[2] != "n/a")
	record.setUpdates(ivector[2]);
    m_running_bhv.push_back(record);
  }
}

//------------------------------------------------------------
// Procedure: setIdleBHVs
//
// Example "Loiter$23.4$4/5" - has three fields separated by '$'
//    (1) "Loiter" is the name of the behavior
//    (2) "23.4" is the number of second it has been in this state
//    (3) "4/5" means 4 of 5 updates attempts were legal/accepted

void IterBlockHelm::setIdleBHVs(const string& str)
{
  vector<string> jvector = parseString(str, ':');
  unsigned int j, jsize = jvector.size();
  
  m_idle_bhv.clear();

  for(j=0; j<jsize; j++) {
    BehaviorRecord record;
    vector<string> kvector = parseString(jvector[j], '$'); 
    unsigned int k, ksize = kvector.size();
    for(k=0; k<ksize; k++)
      kvector[k] = stripBlankEnds(kvector[k]);
    record.setName(kvector[0]);
    if(ksize >= 2)
      record.setTimeStamp(atof(kvector[1].c_str()));
    if(ksize >= 3)
      if(kvector[2] != "n/a")
	record.setUpdates(kvector[2]);
    m_idle_bhv.push_back(record);
  }
}

//------------------------------------------------------------
// Procedure: setCompletedBHVs
//
// Example "Loiter$23.4$4/5" - has three fields separated by '$'
//    (1) "Loiter" is the name of the behavior
//    (2) "23.4" is the number of second it has been in this state
//    (3) "4/5" means 4 of 5 updates attempts were legal/accepted

void IterBlockHelm::setCompletedBHVs(const string& str)
{
  vector<string> svector = parseString(str, ':');
  unsigned int i, vsize = svector.size();
  
  m_completed_bhv.clear();

  for(i=0; i<vsize; i++) {
    BehaviorRecord record;
    vector<string> ivector = parseString(svector[i], '$'); 
    unsigned int j, isize = ivector.size();
    for(j=0; j<isize; j++)
      ivector[j] = stripBlankEnds(ivector[j]);
    record.setName(ivector[0]);
    if(isize >= 2)
      record.setTimeStamp(atof(ivector[1].c_str()));
    if(isize >= 3)
      if(ivector[2] != "n/a")
	record.setUpdates(ivector[2]);
    m_completed_bhv.push_back(record);
  }
}

//------------------------------------------------------------
// Procedure: getActiveBHV()

vector<string> IterBlockHelm::getActiveBHV(double utc_time) const
{
  vector<string> rvector;

  unsigned int k, ksize = m_active_bhv.size();
  for(k=0; k<ksize; k++)
    rvector.push_back(m_active_bhv[k].getSummary(utc_time));
  
  return(rvector);
}

//------------------------------------------------------------
// Procedure: getRunningBHV()

vector<string> IterBlockHelm::getRunningBHV(double utc_time) const
{
  vector<string> rvector;

  unsigned int k, ksize = m_running_bhv.size();
  for(k=0; k<ksize; k++)
      rvector.push_back(m_running_bhv[k].getSummary(utc_time));

  return(rvector);
}

//------------------------------------------------------------
// Procedure: getIdleBHV()

vector<string> IterBlockHelm::getIdleBHV(double utc_time, 
					 bool concise) const
{
  vector<string> rvector;

  unsigned int k, ksize = m_idle_bhv.size();
  for(k=0; k<ksize; k++) {
    if(concise)
      rvector.push_back(m_idle_bhv[k].getName());
    else
      rvector.push_back(m_idle_bhv[k].getSummary(utc_time));
  }

  return(rvector);
}

//------------------------------------------------------------
// Procedure: getCompletedBHV()

vector<string> IterBlockHelm::getCompletedBHV(double utc_time, 
					      bool concise) const
{
  vector<string> rvector;
  
  unsigned int k, ksize = m_completed_bhv.size();
  for(k=0; k<ksize; k++) {
    if(concise) 
      rvector.push_back(m_completed_bhv[k].getName());
    else
      rvector.push_back(m_completed_bhv[k].getSummary(utc_time));
  }
  
  return(rvector);
}


//------------------------------------------------------------
// Procedure: addDecVarVal

void IterBlockHelm::addDecVarVal(const string& varname, const string& value)
{
  unsigned int i, vsize = m_decvar.size();
  for(i=0; i<vsize; i++) {
    if(varname == m_decvar[i]) {
      if(value != m_decval[i]) {
	m_decchg[i] = true;
	m_decval[i] = value;
      }
      return;
    }
  }

  m_decvar.push_back(varname);
  m_decval.push_back(value);
  m_decchg.push_back(true);
}
    

//------------------------------------------------------------
// Procedure: setModeString

void IterBlockHelm::setModeString(string str)
{
  m_modes = "";

  vector<string> svector = parseString(str, '#');
  unsigned int i, vsize = svector.size();
    
  for(i=0; i<vsize; i++) {
    if(i>0)
      m_modes += ", ";
    string mode_name  = biteString(svector[i], '@');
    string mode_value = stripBlankEnds(svector[i]);
    if(vsize > 1) 
      m_modes += (mode_name + "=");
    m_modes += mode_value;
  }

}
    
//------------------------------------------------------------
// Procedure: getDecVar(int)

string IterBlockHelm::getDecVar(unsigned int ix) const
{
  if(ix < m_decvar.size())
    return(m_decvar[ix]);
  else
    return("");
}

//------------------------------------------------------------
// Procedure: getDecVal(int)

string IterBlockHelm::getDecVal(unsigned int ix) const
{
  if(ix < m_decvar.size())
    return(m_decval[ix]);
  else
    return(0);
}

//------------------------------------------------------------
// Procedure: getDecChg(int)

bool IterBlockHelm::getDecChg(unsigned int ix) const
{
  if(ix < m_decchg.size())
    return(m_decchg[ix]);
  else
    return(false);
}

//------------------------------------------------------------
// Procedure: print()

void IterBlockHelm::print(int iter) const
{
  cout << "==================================(" << iter << ")" << endl;
  cout << "m_iteration:    " << m_iteration << endl;
  cout << "m_utc_time:     " << doubleToString(m_utc_time,2) << endl;
  cout << "m_solve_time:   " << m_solve_time << endl;
  cout << "m_create_time:  " << m_create_time << endl;
  cout << "m_loop_time:    " << m_loop_time << endl;
  cout << "m_halted:       " << m_halted << endl;
  cout << "m_count_ipf:    " << m_count_ipf << endl;
  cout << "m_warning_count:" << m_warning_cnt << endl;
  cout << "m_error_cnt:    " << m_error_cnt << endl;
  cout << "m_posting_cnt:  " << m_posting_cnt << endl;
  cout << "m_infomsg_cnt:  " << m_infomsg_cnt << endl;
  cout << endl;
  cout << "m_active_bhv.size(): " << m_active_bhv.size() << endl;
  cout << "m_running_bhv.size(): " << m_running_bhv.size() << endl;
  cout << "m_idle_bhv.size(): " << m_idle_bhv.size() << endl;
  cout << "m_completed_bhv.size(): " << m_completed_bhv.size() << endl;
}










