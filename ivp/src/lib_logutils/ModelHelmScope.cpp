/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ModelHelmScope.cpp                                   */
/*    DATE: Oct 4th, 2011                                        */
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
#include <cstdio>
#include <cstdlib>
#include "ModelHelmScope.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ACTable.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

ModelHelmScope::ModelHelmScope()
{
  m_curr_time = 0;
  m_curr_iter = 0;
  m_utc_start = 0;

  m_headers_bhv = true;
}


//-------------------------------------------------------------
// Procedure: setDataBroker()

void ModelHelmScope::setDataBroker(ALogDataBroker dbroker, string vname)
{
  m_dbroker = dbroker;
  m_vname   = vname;

  unsigned int aix = m_dbroker.getAixFromVName(vname);
  m_helm_plot = m_dbroker.getHelmPlot(aix);
  m_utc_start = m_dbroker.getLogStartFromAix(aix);

  unsigned int mix1 = m_dbroker.getMixFromVNameVarName(vname, "IVPHELM_STATE");
  unsigned int mix2 = m_dbroker.getMixFromVNameVarName(vname, "IVPHELM_MODESET");
  unsigned int mix3 = m_dbroker.getMixFromVNameVarName(vname, "MODE");
  unsigned int mix4 = m_dbroker.getMixFromVNameVarName(vname, "BHV_WARNING");
  unsigned int mix5 = m_dbroker.getMixFromVNameVarName(vname, "BHV_ERROR");
  unsigned int mix6 = m_dbroker.getMixFromVNameVarName(vname, "IVPHELM_LIFE_EVENT");

  m_vplot_helm_state   = m_dbroker.getVarPlot(mix1);
  m_vplot_helm_modeset = m_dbroker.getVarPlot(mix2);
  m_vplot_helm_mode    = m_dbroker.getVarPlot(mix3);
  m_vplot_bhv_warning  = m_dbroker.getVarPlot(mix4, true); // true:incSourceInfo
  m_vplot_bhv_error    = m_dbroker.getVarPlot(mix5, true); // true:incSourceInfo
  m_vplot_life_event   = m_dbroker.getVarPlot(mix6);
}

//-------------------------------------------------------------
// Procedure: incrementIter

void ModelHelmScope::incrementIter(int amt)
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

  setTimeFromIter(new_iter);
}

//-------------------------------------------------------------
// Procedure: setTime()

void ModelHelmScope::setTime(double gtime)
{
  m_curr_time = gtime;
  setIterFromTime(gtime);
}

//-------------------------------------------------------------
// Procedure: getVPlotSize()

unsigned int ModelHelmScope::getVPlotSize(string ptype) const
{
  if(ptype == "bhv_error")
    return(m_vplot_bhv_error.size());
  else if(ptype == "bhv_warning")
    return(m_vplot_bhv_warning.size());
  else if(ptype == "helm_state")
    return(m_vplot_helm_state.size());
  else if(ptype == "helm_mode")
    return(m_vplot_helm_mode.size());
  else if(ptype == "helm_modeset")
    return(m_vplot_helm_modeset.size());
  else if(ptype == "life_event")
    return(m_vplot_life_event.size());
  
  return(0);
}
  
//-------------------------------------------------------------
// Procedure: getCurrMode()

string ModelHelmScope::getCurrMode() const
{
  string mode = m_helm_plot.getValueByTime("mode", m_curr_time);
  return(mode);

}
  
//-------------------------------------------------------------
// Procedure: getCurrIter()

string ModelHelmScope::getCurrIter() const
{
  return(uintToString(m_curr_iter));
}
  
//-------------------------------------------------------------
// Procedure: getCurrDecision()
//      Note: Example incoming str:
//            ",var=course:118,var=speed:1.2"

string ModelHelmScope::getCurrDecision() const
{
  string str = m_helm_plot.getValueByTime("decision", m_curr_time);
  
  str = findReplace(str, ",var=", " ");
  str = findReplace(str, ":", "=");
  str = stripBlankEnds(str);
  str = findReplace(str, " ", ", ");
  return(str);
}
  

//-------------------------------------------------------------
// Procedure: getCurrHelmState()

string ModelHelmScope::getCurrHelmState() const
{
  string str = m_vplot_helm_state.getEntryByTime(m_curr_time);
  return(str);
}
  

//-------------------------------------------------------------
// Procedure: getActiveList()

vector<string> ModelHelmScope::getActiveList() const
{
  // active_bhvs=loiter$42743622595.08$100.00000$9$0.00000$0/0$1
  // loiter$
  // 42743622595.08$
  // 100.00000$
  // 9$
  // 0.00000$
  // 0/0$
  // 1
  string raw = m_helm_plot.getValueByTime("active", m_curr_time);

  ACTable actab(7,2);
  if(m_headers_bhv) {
    actab << "Behavior" << "Time" << "Pwt" << "Pcs" << "CPU" << "UPD" << "IPFs";
    actab.addHeaderLines();
  }

  vector<string> svector = parseString(raw, ':');
  for(unsigned int i=0; i<svector.size(); i++) {
    string bhv = biteString(svector[i], '$');
    if(bhv != "none") {
      string utc = biteString(svector[i], '$');
      string pwt = biteString(svector[i], '$');
      string pcs = biteString(svector[i], '$');
      string cpu = biteString(svector[i], '$');
      string upd = biteString(svector[i], '$');
      string ipf = biteString(svector[i], '$');
      
      string elapsed = convertTimeUTC2TimeElapsed(utc);
      
      pwt = dstringCompact(pwt);
      cpu = dstringCompact(cpu);
      actab << bhv << elapsed << pwt << pcs << cpu << upd << ipf;
    }
  }

  vector<string> rvector = actab.getTableOutput();
  return(rvector);
}

//-------------------------------------------------------------
// Procedure: getNonActiveList()

vector<string> ModelHelmScope::getNonActiveList(string bhv_type) const
{
  vector<string> rvector;
  if(!m_helm_plot.containsTime(m_curr_time))
    return(rvector);

  string raw = m_helm_plot.getValueByTime(bhv_type, m_curr_time);
  
  ACTable actab(3,2);
  if(m_headers_bhv) {
    actab << "Behavior" << "Time" << "UPD";
    actab.addHeaderLines();
  }

  vector<string> svector = parseString(raw, ':');
  for(unsigned int i=0; i<svector.size(); i++) {
    string bhv = biteStringX(svector[i], '$');
    if(bhv != "none") {
      string tim = biteStringX(svector[i], '$');
      string upd = svector[i];
      string time_elapsed = convertTimeUTC2TimeElapsed(tim);
      actab << bhv << time_elapsed << upd;
    }
  }
  
  rvector = actab.getTableOutput();
  return(rvector);
}


//-------------------------------------------------------------
// Procedure: getWarnings

vector<string> ModelHelmScope::getWarnings() const
{
  return(getErrWarnings(m_vplot_bhv_warning));
}

//-------------------------------------------------------------
// Procedure: getErrors

vector<string> ModelHelmScope::getErrors() const
{
  return(getErrWarnings(m_vplot_bhv_error));
}

//-------------------------------------------------------------
// Procedure: getErrWarnings

vector<string> ModelHelmScope::getErrWarnings(const VarPlot& vplot) const
{
  vector<string> valvector = vplot.getEntriesUpToTime(m_curr_time);
  vector<string> srcvector = vplot.getSourcesUpToTime(m_curr_time);
  vector<double> timvector = vplot.getTStampsUpToTime(m_curr_time);

  ACTable actab(4,2);
  if(m_headers_bhv) {
    actab << "Iter" << "Time" << "behavior" << "Message";
    actab.addHeaderLines(15);
  }

  // Example: val = "loiter: Test warning message: 85"
  // Example: src = "pHelmIvP:1180:loiter"
  for(unsigned int i=0; i<valvector.size(); i++) {
    string val = valvector[valvector.size()-1-i];
    string src = srcvector[srcvector.size()-1-i];
    string tim = doubleToString(timvector[timvector.size()-1-i], 3);
    string bhv = biteStringX(val, ':');
    string msg = val;
    string itr = "unlogged";
    if(src != "") {
      biteStringX(src, ':');
      itr = biteStringX(src, ':');
    }
    actab << itr << tim << bhv << msg;
  }
  
  vector<string> rvector = actab.getTableOutput();
  return(rvector);
}


//-------------------------------------------------------------
// Procedure: getModes()

vector<string> ModelHelmScope::getModes() const
{
  vector<string> valvector = m_vplot_helm_mode.getEntriesUpToTime(m_curr_time);
  vector<double> timvector = m_vplot_helm_mode.getTStampsUpToTime(m_curr_time);

  ACTable actab(2,3);
  if(m_headers_bhv) {
    actab << "Time" << "Mode";
    actab.addHeaderLines();
  }

  for(unsigned int i=0; i<valvector.size(); i++) {
    string val = valvector[valvector.size()-1-i];
    string tim = doubleToString(timvector[timvector.size()-1-i], 3);
    actab << tim << val;
  }
  
  vector<string> rvector = actab.getTableOutput();
  return(rvector);
}


//-------------------------------------------------------------
// Procedure: getLifeEveents
//  Examples: iter=1, bname=loiter, btype=BHV_Loiter, event=spawn, seed=helm_startup 
//            iter=479, bname=avdcollision_avd_henry, btype=BHV_AvoidCollision, 
//                      event=spawn, seed=name=avd_henry # contact=henry 

vector<string> ModelHelmScope::getLifeEvents() const
{
  vector<string> valvector = m_vplot_life_event.getEntriesUpToTime(m_curr_time);
  vector<double> timvector = m_vplot_life_event.getTStampsUpToTime(m_curr_time);

  ACTable actab(6,2);
  if(m_headers_bhv) {
    actab << "Iter" << "Time" << "bhv-name" << "bhv-type" << "Event" << "Seed";
    actab.addHeaderLines();
  }

  // Example: val = iter=479, bname=avdcollision_avd_henry, btype=BHV_AvoidCollision, 
  //                event=spawn, seed=name=avd_henry # contact=henry 
  // Example: tim = 324.981
  for(unsigned int i=0; i<valvector.size(); i++) {
    string val = valvector[valvector.size()-1-i];
    string tim = doubleToString(timvector[timvector.size()-1-i], 3);
    string itr, bname, btype, event, seed="n/a";
    
    vector<string> svector = parseString(val, ',');
    for(unsigned int j=0; j<svector.size(); j++) {
      string param = biteStringX(svector[j], '='); 
      string value = svector[j]; 
      if(param == "iter")
	itr = value;
      else if(param == "bname")
	bname = value;
      else if(param == "btype") {
	if(strBegins(value, "BHV_"))
	  value = value.substr(4);
	btype = value;
      }
      else if(param == "event")
	event = value;
      else if((param == "seed") && (value != ""))
	seed = value;
    }
    actab << itr << tim << bname << btype << event << seed;
  }
  
  vector<string> rvector = actab.getTableOutput();
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

string ModelHelmScope::convertTimeUTC2TimeElapsed(string utc_time) const
{
  
  double d_utc_time = atof(utc_time.c_str());
  string time_in_state = "-"; // Means always been in this state
  if(d_utc_time > 0) {
    double val = m_curr_time - (d_utc_time - m_utc_start);
    if(val >= 0)
      time_in_state = doubleToString(val,1);
    else
      time_in_state = "n/a";
  }
  return(time_in_state);
}


//-------------------------------------------------------------
// Procedure: setIterFromTime

void ModelHelmScope::setIterFromTime(double time)
{
  if(m_helm_plot.containsTime(time))
    m_curr_iter = m_helm_plot.getIterByTime(m_curr_time);
  else
    m_curr_iter = 0;
}


//-------------------------------------------------------------
// Procedure: setTimeFromIter

void ModelHelmScope::setTimeFromIter(unsigned int new_iter)
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
  m_curr_time = m_helm_plot.getTimeByIndex(index);
  m_curr_iter = new_iter;
}









