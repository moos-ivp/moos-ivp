/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmScopeModel.cpp                                   */
/*    DATE: Oct0411                                              */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "HelmScopeModel.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "MBTimer.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

HelmScopeModel::HelmScopeModel()
{
  m_curr_time = 0;
  m_curr_iter = 0;

  m_headers_bhv = true;
}


//-------------------------------------------------------------
// Procedure: setHelmPlot
//      Note: A local copy of the given Helm_TPlot is created here.

void HelmScopeModel::setHelmPlot(const HelmTPlot& helm_plot)
{
  m_helm_plot = helm_plot;
}

//-------------------------------------------------------------
// Procedure: setVarPlot
//      Note: A local copy of the given VarTPlot is created here.

void HelmScopeModel::addVarPlot(const VarTPlot& var_plot)
{
  if(var_plot.getVarName() == "IVPHELM_ENGAGED")
    m_vplot_engaged = var_plot;
  else
    m_vplotset.addVarPlot(var_plot);
}


//-------------------------------------------------------------
// Procedure: incrementIter

void HelmScopeModel::incrementIter(int amt)
{
  unsigned int hp_size = m_helm_plot.size();
  if(hp_size == 0)
    return;
  unsigned int min_iter = m_helm_plot.getIterByIndex(0);
  unsigned int max_iter = m_helm_plot.getIterByIndex(hp_size-1);

  unsigned int new_iter = m_curr_iter;
  if((amt > 0) && ((m_curr_iter + amt) <= max_iter))
    new_iter += (unsigned int)(amt);
  if((amt < 0) && ((m_curr_iter - amt) >=  min_iter))
    new_iter += (unsigned int)(amt);

  if(new_iter == m_curr_iter)
    return;

  double prev_time = m_curr_time;
  setTimeUTCFromIteration(new_iter);

  if(m_curr_time != prev_time)
    m_pbuffer.pushPending("ALOGVIEW_TIME", m_curr_time);
}



//-------------------------------------------------------------
// Procedure: setTime()
//      Note: If the time is specified by helm iteration, it takes precedent 

void HelmScopeModel::setTime(const ULV_TimeStamp& timestamp)
{
  string curr_vname = m_helm_plot.getVName();
  if(timestamp.vname() == curr_vname)
    setTimeUTCFromIteration(timestamp.iter());
  else {
    m_curr_time = timestamp.time();
    setIterationFromTimeUTC(timestamp.time());
  }
}

//-------------------------------------------------------------
// Procedure: setTime()

void HelmScopeModel::setTime(double time_utc)
{
  m_curr_time = time_utc;
  setIterationFromTimeUTC(time_utc);
}

//-------------------------------------------------------------
// Procedure: getCurrTime()

double HelmScopeModel::getCurrTime() const
{
  double logstart_utc = m_helm_plot.getLogStartUTC();
  return(m_curr_time - logstart_utc);
}

//-------------------------------------------------------------
// Procedure: getCurrTimeUTC()

double HelmScopeModel::getCurrTimeUTC() const
{
  return(m_curr_time);
}

//-------------------------------------------------------------
// Procedure: getPostingBuffer

PostingBuffer HelmScopeModel::getPostingBuffer(bool clear)
{
  PostingBuffer return_buffer = m_pbuffer;
  if(clear)
    m_pbuffer.clear();
  return(return_buffer);
}


//-------------------------------------------------------------
// Procedure: getCurrMode()

string HelmScopeModel::getCurrMode() const
{
  unsigned int index = m_helm_plot.getIndexByIter(m_curr_iter);
  return(m_helm_plot.getValueByIndex("mode", index));
}
  
//-------------------------------------------------------------
// Procedure: getCurrIter()

string HelmScopeModel::getCurrIter() const
{
  return(uintToString(m_curr_iter));
}
  
//-------------------------------------------------------------
// Procedure: getCurrDecision()
//      Note: Example incoming str:
//            ",var=course:118,var=speed:1.2"

string HelmScopeModel::getCurrDecision() const
{
  unsigned int index = m_helm_plot.getIndexByIter(m_curr_iter);
  string str = m_helm_plot.getValueByIndex("decision", index);
  
  str = findReplace(str, ",var=", " ");
  str = findReplace(str, ":", "=");
  str = stripBlankEnds(str);
  str = findReplace(str, " ", ", ");
  return(str);
}
  

//-------------------------------------------------------------
// Procedure: getCurrEngaged()

string HelmScopeModel::getCurrEngaged() const
{
  if(!m_vplot_engaged.containsTimeUTC(m_curr_time))
    return("n/a");

  unsigned int index = m_vplot_engaged.getIndexByTimeUTC(m_curr_time);
  ALogEntry entry = m_vplot_engaged.getEntryByIndex(index);
  return(entry.getStringVal());
}
  

//-------------------------------------------------------------
// Procedure: getActiveList()

vector<string> HelmScopeModel::getActiveList() const
{
  vector<string> rvector;
  if(!m_helm_plot.containsTimeUTC(m_curr_time))
    return(rvector);

  unsigned int index = m_helm_plot.getIndexByIter(m_curr_iter);
  string raw = m_helm_plot.getValueByIndex("active", index);

  vector<string> vbhv;
  vector<string> vtim;
  vector<string> vpwt;
  vector<string> vpcs;
  vector<string> vcpu;
  vector<string> vupd;
  vector<string> vipf;
  vector<string> vfrs;
  
  unsigned int lbhv = 8;  // "Behavior"
  unsigned int ltim = 4;  // "Time"
  unsigned int lpwt = 3;  // "Pwt"
  unsigned int lpcs = 3;  // "Pcs"
  unsigned int lcpu = 3;  // "CPU"
  unsigned int lupd = 3;  // "UPD"
  unsigned int lipf = 4;  // "IPFs"
  unsigned int lfrs = 3;  // "New"

  // First parse the entries putting them into temporary vectors
  // and taking note of the max length for each field.
  vector<string> svector = parseString(raw, ':');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    vbhv.push_back(biteString(svector[i], '$'));
    string utc_time = biteString(svector[i], '$');
    string time_elapsed = convertTimeUTC2TimeElapsed(utc_time);
    vtim.push_back(time_elapsed);
    vpwt.push_back(biteString(svector[i], '$'));
    vpcs.push_back(biteString(svector[i], '$'));
    vcpu.push_back(biteString(svector[i], '$'));
    vupd.push_back(biteString(svector[i], '$'));
    vipf.push_back(biteString(svector[i], '$'));
    vfrs.push_back(biteString(svector[i], '$'));

    lbhv = (vbhv[i].length() > lbhv) ? vbhv[i].length() : lbhv;
    ltim = (vtim[i].length() > ltim) ? vtim[i].length() : ltim;
    lpwt = (vpwt[i].length() > lpwt) ? vpwt[i].length() : lpwt;
    lpcs = (vpcs[i].length() > lpcs) ? vpcs[i].length() : lpcs;
    lcpu = (vcpu[i].length() > lcpu) ? vcpu[i].length() : lcpu;
    lupd = (vupd[i].length() > lupd) ? vupd[i].length() : lupd;
    lipf = (vipf[i].length() > lipf) ? vipf[i].length() : lipf;
    lfrs = (vfrs[i].length() > lfrs) ? vfrs[i].length() : lfrs;
  }

  string sep = "  ";

  if(m_headers_bhv) {
    string header = padString("Behavior", lbhv, false) + sep;
    header += padString("Time", ltim, true) + sep;
    header += padString("Pwt", lpwt, true) + sep;
    header += padString("Pcs", lpcs, true) + sep;
    header += padString("CPU", lcpu, true) + sep;
    header += padString("UPD", lupd, true) + sep;
    header += padString("IPFs", lipf, true) + sep;
    header += padString("New", lfrs, true);
    rvector.push_back(header);
    
    string sline = padString("--------", lbhv, false) + sep;
    sline += padString("----", ltim, true) + sep;
    sline += padString("---", lpwt, true) + sep;
    sline += padString("---", lpcs, true) + sep;
    sline += padString("---", lcpu, true) + sep;
    sline += padString("---", lupd, true) + sep;
    sline += padString("----", lipf, true) + sep;
    sline += padString("---", lfrs, true);
    rvector.push_back(sline);
  }

  // Then build the entries using the proper padding
  for(i=0; i<vsize; i++) {
    string entry = padString(vbhv[i], lbhv, false) + sep;
    entry += padString(vtim[i], ltim, true) + sep;
    entry += padString(vpwt[i], lpwt, true) + sep;
    entry += padString(vpcs[i], lpcs, true) + sep;
    entry += padString(vcpu[i], lcpu, true) + sep;
    entry += padString(vupd[i], lupd, true) + sep;
    entry += padString(vipf[i], lipf, true) + sep;
    entry += padString(vfrs[i], lfrs, true);
    rvector.push_back(entry);
  }

  return(rvector);
}

//-------------------------------------------------------------
// Procedure: getNonActiveList()

vector<string> HelmScopeModel::getNonActiveList(string bhv_type) const
{
  vector<string> rvector;
  if(!m_helm_plot.containsTimeUTC(m_curr_time))
    return(rvector);

  unsigned int index = m_helm_plot.getIndexByIter(m_curr_iter);
  string raw = m_helm_plot.getValueByIndex(bhv_type, index);

  return(parseBehaviors(raw));
}


//-------------------------------------------------------------
// Procedure: parseBehaviors
//      Note: Example string: "loiter$15809867923.1$2/3
//            Field [0]: behavior name
//            Field [1]: UTC time when entered this state
//            Field [2]: Successful updates / Attempted updates

vector<string> HelmScopeModel::parseBehaviors(string raw) const
{
  vector<string> rvector;

  vector<string> vbhv;
  vector<string> vtim;
  vector<string> vupd;

  unsigned int lbhv = 8;  // "Behavior"
  unsigned int ltim = 4;  // "Time"
  unsigned int lupd = 3;  // "UPD"

  bool empty = false;
  if(raw == "none") {
    empty = true;
    raw = "";
  }

  // First parse the entries putting them into temporary vectors
  // and taking note of the max length for each field.
  vector<string> svector = parseString(raw, ':');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    vbhv.push_back(biteString(svector[i], '$'));
    string utc_time = biteString(svector[i], '$');
    string time_elapsed = convertTimeUTC2TimeElapsed(utc_time);
    vtim.push_back(time_elapsed);
    vupd.push_back(biteString(svector[i], '$'));

    lbhv = (vbhv[i].length() > lbhv) ? vbhv[i].length() : lbhv;
    ltim = (vtim[i].length() > ltim) ? vtim[i].length() : ltim;
    lupd = (vupd[i].length() > lupd) ? vupd[i].length() : lupd;
  }
  
  string sep = "  ";

  if(m_headers_bhv) {
    string header = padString("Behavior", lbhv, false) + sep;
    header += padString("Time", ltim, true) + sep;
    header += padString("UPD", lupd, true) + sep;
    rvector.push_back(header);
    
    string sline = padString("--------", lbhv, false) + sep;
    sline += padString("----", ltim, true) + sep;
    sline += padString("---", lupd, true) + sep;
    rvector.push_back(sline);
  }

  // Then build the entries using the proper padding
  for(i=0; i<vsize; i++) {
    string entry = padString(vbhv[i], lbhv, false) + sep;
    entry += padString(vtim[i], ltim, true) + sep;
    entry += padString(vupd[i], lupd, true) + sep;
    rvector.push_back(entry);
  }

  return(rvector);
}



//-------------------------------------------------------------
// Procedure: getWarningsList

vector<string> HelmScopeModel::getWarningsList()
{
  vector<string> rvector;

  //if(!m_vplot_warnings.containsTimeUTC(m_curr_time))
  //  return(rvector);
  
  vector<ALogEntry> entries;
  entries = m_vplotset.getEntriesByTimeUTC(m_curr_time, 40);
  unsigned int i, vsize = entries.size();
  for(i=0; i<vsize; i++) {
    double time = entries[i].time();
    string var = entries[i].getVarName();
    string src  = entries[i].getSource();
    string line = doubleToString(time,2) + "  " + var + "  " + src + "  ";
    if(entries[i].isNumerical())
      line += doubleToStringX(entries[i].getDoubleVal());
    else
      line += entries[i].getStringVal();
    rvector.push_back(line);
  }

  return(rvector);
}

//-------------------------------------------------------------
// Procedure: convertTimeUTC2TimeElapsed
//   Purpose: convert a string time representation expected to be
//            in UTC format and convert it to a string represented
//            by the time elapsed SINCE that given UTC time based
//            on the current time value given by m_curr_time
//     Notes: Three possible values:
//            (1) A positive number meaning # secs in the state
//            (2) "-" meaning its been in this state since helm startup
//            (3) "n/a" curr_time is outside the time window for which
//                anything is known about this behavior. Should not 
//                ever return this value if caught elsewhere.

string HelmScopeModel::convertTimeUTC2TimeElapsed(const string& str) const
{
  
  double utc_time_v = atof(str.c_str());
  string time_in_state = "-"; // Means always been in this state
  if(utc_time_v > 0) {
    double val = m_curr_time - utc_time_v;
    if(val >= 0)
      time_in_state = doubleToString(val,1);
    else
      time_in_state = "n/a";
  }
  return(time_in_state);
}


//-------------------------------------------------------------
// Procedure: setIterationFromTimeUTC

void HelmScopeModel::setIterationFromTimeUTC(double utc_time)
{
  if(m_helm_plot.containsTimeUTC(utc_time))
    m_curr_iter = m_helm_plot.getIterByTimeUTC(m_curr_time);
  else
    m_curr_iter = 0;
}


//-------------------------------------------------------------
// Procedure: setTimeUTCFromIteration

void HelmScopeModel::setTimeUTCFromIteration(unsigned int new_iter)
{
  unsigned int hp_size = m_helm_plot.size();
  if(hp_size == 0)
    return;
  unsigned int min_iter = m_helm_plot.getIterByIndex(0);
  unsigned int max_iter = m_helm_plot.getIterByIndex(hp_size-1);

  if(new_iter < min_iter)
    new_iter = min_iter;
  if(new_iter > max_iter)
    new_iter = max_iter;

  unsigned int index = m_helm_plot.getIndexByIter(new_iter);
  m_curr_time = m_helm_plot.getTimeUTCByIndex(index);
  m_curr_iter = new_iter;
}
