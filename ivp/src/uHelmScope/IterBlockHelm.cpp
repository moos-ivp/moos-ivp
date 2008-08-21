/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IterBlockHelm.cpp                                    */
/*    DATE: Apr 12th 2008                                        */
/*****************************************************************/

#include "IterBlockHelm.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------
// Constructor

IterBlockHelm::IterBlockHelm()
{
  m_iteration   = 0;
  m_count_ipf   = 0;
  m_solve_time  = 0;
  m_create_time = 0;
  m_loop_time   = 0;
  m_halted      = false;
  m_warning_cnt = -1;
}

//------------------------------------------------------------
// Procedure: addActiveBHVs
// Ex:  "waypoint$100: loiter$98.2: return$45.1"

void IterBlockHelm::addActiveBHVs(const string& str)
{
  vector<string> svector = parseString(str, ':');
  int vsize = svector.size();
  
  for(int i=0; i<vsize; i++) {
    string entry;
    svector[i] = stripBlankEnds(svector[i]);
    vector<string> ivector = parseString(svector[i], '$');
    int isize = ivector.size();
    for(int j=0; j<isize; j++)
      ivector[j] = stripBlankEnds(ivector[j]);
    entry += ivector[0];
    if(ivector.size() >= 2)
      entry += " (" + ivector[1] + ")";
    if(ivector.size() >= 3)
      entry += " (pwt=" + ivector[2] + ")";
    if(ivector.size() >= 4)
      entry += " (pcs=" + ivector[3] + ")";
    if(ivector.size() >= 5)
      entry += " (cpu=" + ivector[4] + ")";
    if(ivector.size() >= 6)
      if(ivector[5] != "n/a")
	entry += " (upd=" + ivector[5] + ")";
    m_active_bhv.push_back(entry);
  }
}

//------------------------------------------------------------
// Procedure: addRunningBHVs
//
// Example "Loiter$23.4$n/a" - has three fields separated by '$'
//    (1) "Loiter" is the name of the behavior
//    (2) "23.4" is the number of second it has been in this state
//    (3) "n/a" means the update feature off for this behavior

void IterBlockHelm::addRunningBHVs(const string& str)
{
  vector<string> svector = parseString(str, ':');
  int vsize = svector.size();

  for(int i=0; i<vsize; i++) {
    string entry;
    vector<string> ivector = parseString(svector[i], '$'); 
    int isize = ivector.size();
    for(int j=0; j<isize; j++)
      ivector[j] = stripBlankEnds(ivector[j]);
    entry += ivector[0];
    if(isize >= 2)
      entry += " (" + ivector[1] + ")";
    if(isize >= 3)
      if(ivector[2] != "n/a")
	entry += " (upd=" + ivector[2] + ")";
    m_running_bhv.push_back(entry);
  }
}

//------------------------------------------------------------
// Procedure: addIdleBHVs
//
// Example "Loiter$23.4$4/5" - has three fields separated by '$'
//    (1) "Loiter" is the name of the behavior
//    (2) "23.4" is the number of second it has been in this state
//    (3) "4/5" means 4 of 5 updates attempts were legal/accepted

void IterBlockHelm::addIdleBHVs(const string& str)
{
  vector<string> svector = parseString(str, ':');
  int vsize = svector.size();
  
  for(int i=0; i<vsize; i++) {
    string entry;
    vector<string> ivector = parseString(svector[i], '$'); 
    int isize = ivector.size();
    for(int j=0; j<isize; j++)
      ivector[j] = stripBlankEnds(ivector[j]);
    entry += ivector[0];
    if(isize >= 2)
      entry += " (" + ivector[1] + ")";
    if(isize >= 3)
      if(ivector[2] != "n/a")
	entry += " (upd=" + ivector[2] + ")";
    m_idle_bhv.push_back(entry);
  }
}

//------------------------------------------------------------
// Procedure: addCompletedBHVs
//
// Example "Loiter$23.4$4/5" - has three fields separated by '$'
//    (1) "Loiter" is the name of the behavior
//    (2) "23.4" is the number of second it has been in this state
//    (3) "4/5" means 4 of 5 updates attempts were legal/accepted

void IterBlockHelm::addCompletedBHVs(const string& str)
{
  vector<string> svector = parseString(str, ':');
  int vsize = svector.size();
  
  for(int i=0; i<vsize; i++) {
    string entry;
    vector<string> ivector = parseString(svector[i], '$'); 
    int isize = ivector.size();
    for(int j=0; j<isize; j++)
      ivector[j] = stripBlankEnds(ivector[j]);
    entry += ivector[0];
    if(isize >= 2)
      entry += " (" + ivector[1] + ")";
    if(isize >= 3)
      if(ivector[2] != "n/a")
	entry += " (upd=" + ivector[2] + ")";
    m_completed_bhv.push_back(entry);
  }
}

//------------------------------------------------------------
// Procedure: getActiveBHV()

vector<string> IterBlockHelm::getActiveBHV() const
{
  return(m_active_bhv);
}

//------------------------------------------------------------
// Procedure: getRunningBHV()

vector<string> IterBlockHelm::getRunningBHV() const
{
  return(m_running_bhv);
}

//------------------------------------------------------------
// Procedure: getIdleBHV()

vector<string> IterBlockHelm::getIdleBHV(bool concise) const
{
  if(!concise)
    return(m_idle_bhv);
  
  vector<string> rvector;
  int vsize = m_idle_bhv.size();
  for(int i=0; i<vsize; i++) {
    vector<string> ivector = chompString(m_idle_bhv[i], ' ');
    rvector.push_back(ivector[0]);
  }
  return(rvector);
}

//------------------------------------------------------------
// Procedure: getCompletedBHV()

vector<string> IterBlockHelm::getCompletedBHV(bool concise) const
{
  if(!concise)
    return(m_completed_bhv);
  
  vector<string> rvector;
  int vsize = m_completed_bhv.size();
  for(int i=0; i<vsize; i++) {
    vector<string> ivector = chompString(m_completed_bhv[i], ' ');
    rvector.push_back(ivector[0]);
  }
  return(rvector);
}


//------------------------------------------------------------
// Procedure: addDecVarVal

void IterBlockHelm::addDecVarVal(const string& varname, const string& value)
{
  if(vectorContains(m_decvar, varname))
    return;
  m_decvar.push_back(varname);
  m_decval.push_back(value);
}
    

//------------------------------------------------------------
// Procedure: getDecVar(int)

string IterBlockHelm::getDecVar(unsigned int ix) const
{
  if((ix >= 0) && (ix < m_decvar.size()))
    return(m_decvar[ix]);
  else
    return("");
}

//------------------------------------------------------------
// Procedure: getDecVal(int)

string IterBlockHelm::getDecVal(unsigned int ix) const
{
  if((ix >= 0) && (ix < m_decvar.size()))
    return(m_decval[ix]);
  else
    return(0);
}

