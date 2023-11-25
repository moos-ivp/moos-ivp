/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ModelVarScope.cpp                                    */
/*    DATE: Feb 28th 2015                                        */
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
#include "ModelVarScope.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

ModelVarScope::ModelVarScope()
{
  m_curr_time = 0;

  m_max_len_varname = 0;
  m_max_len_source  = 0;
  m_max_len_srcaux  = 0;
  m_max_len_vname   = 0;

  m_show_vname   = false;
  m_show_varname = true;
  m_show_source  = true;
  m_show_srcaux  = false;
}

//-------------------------------------------------------------
// procedure: setDataBroker()

void ModelVarScope::setDataBroker(ALogDataBroker dbroker, unsigned int mix)
{
  m_dbroker = dbroker;
  setVarPlot(mix);
}

//-------------------------------------------------------------
// procedure: setVarPlot

void ModelVarScope::setVarPlot(unsigned int mix)
{
  m_prev_entries.clear();
  m_soon_entries.clear();
  addVarPlot(mix);
}

//-------------------------------------------------------------
// procedure: addVarPlot

void ModelVarScope::addVarPlot(unsigned int mix)
{
  VarPlot vplot = m_dbroker.getVarPlot(mix, true); // true means get source info

  // Part 1: Get the possibly new max lengths given the new plot (for formatting)
  unsigned int len_vname = m_dbroker.getVNameFromMix(mix).length();
  unsigned int len_varname = vplot.getVarName().length();
  unsigned int len_source = vplot.getMaxLenSource();
  unsigned int len_srcaux = vplot.getMaxLenSrcAux();

  // Part 1B: Determine if re-formatting is neeed (if one of the column lengths
  // is greater than previous maxlen for a column)
  bool reformat_needed = false;
  if(len_vname > m_max_len_vname) {
    m_max_len_vname = len_vname;
    reformat_needed = true;
  }
  if(len_varname > m_max_len_varname) {
    m_max_len_varname = len_varname;
    reformat_needed = true;
  }
  if(len_source > m_max_len_source) {
    m_max_len_source = len_source;
    reformat_needed = true;
  }
  if(len_srcaux > m_max_len_srcaux) {
    m_max_len_srcaux = len_srcaux;
    reformat_needed = true;
  }

  // If reformatting needed, reformat the pre-existing entries
  // (Otherwise we will only "re" format the new entries below)
  if(reformat_needed) 
    reformat();

  // Part 2: Merge and format the PREV entries
  list<VarPlotEntry> prev_entries = vplot.getVarPlotEntriesUpToTime(m_curr_time);
  reformat(prev_entries);
  m_prev_entries = mergePlots(m_prev_entries, prev_entries);

  // Part 3: Merge the SOON entries
  list<VarPlotEntry> soon_entries = vplot.getVarPlotEntriesPastTime(m_curr_time);
  reformat(soon_entries);
  m_soon_entries = mergePlots(m_soon_entries, soon_entries);
}


//-------------------------------------------------------------
// procedure: delVarPlot

void ModelVarScope::delVarPlot(unsigned int mix)
{
  string vname = m_dbroker.getVNameFromMix(mix);
  string varname = m_dbroker.getVarNameFromMix(mix);

  bool item_removed = false;

  // Part 1: Remove all etries matching the node and variable
  list<VarPlotEntry>::iterator p;
  for(p=m_prev_entries.begin(); p!=m_prev_entries.end(); ) {
    if((p->getVarName() == varname) && (p->getVName() == vname)) {
      p = m_prev_entries.erase(p);
      item_removed = true;
    }
    else
      ++p;
  }

  for(p=m_soon_entries.begin(); p!=m_soon_entries.end(); ) {
    if((p->getVarName() == varname) && (p->getVName() == vname)) {
      p = m_soon_entries.erase(p);
      item_removed = true;
    }
    else
      ++p;
  }

  if(!item_removed)
    return;

  // Part 2: Possibly reformat if we need to shrink a column.
  unsigned int new_max_len_varname = 0;
  unsigned int new_max_len_vname   = 0;
  unsigned int new_max_len_source  = 0;
  unsigned int new_max_len_srcaux  = 0;

  for(p=m_prev_entries.begin(); p!=m_prev_entries.end(); p++) {
    unsigned int len_varname = p->getVarName().length();
    unsigned int len_vname   = p->getVName().length();
    unsigned int len_source  = p->getSource().length();
    unsigned int len_srcaux  = p->getVarSrcAux().length();
    if(len_varname > new_max_len_varname)
      new_max_len_varname = len_varname;
    if(len_vname > new_max_len_vname)
      new_max_len_vname = len_vname;
    if(len_source > new_max_len_source)
      new_max_len_source = len_source;
    if(len_varname > new_max_len_srcaux)
      new_max_len_srcaux = len_srcaux;
  }

  if((new_max_len_varname < m_max_len_varname) ||
     (new_max_len_vname  < m_max_len_vname)  ||
     (new_max_len_source < m_max_len_source) ||
     (new_max_len_srcaux < m_max_len_srcaux)) {
    m_max_len_varname = new_max_len_varname;
    m_max_len_vname = new_max_len_vname;
    m_max_len_source = new_max_len_source;
    m_max_len_srcaux = new_max_len_srcaux;
    reformat();
  }
}


//-------------------------------------------------------------
// procedure: reformat

void ModelVarScope::reformat()
{
  reformat(m_prev_entries);
  reformat(m_soon_entries);
}

//-------------------------------------------------------------
// procedure: reformat

void ModelVarScope::reformat(list<VarPlotEntry>& entries)
{
  list<VarPlotEntry>::iterator p;
  for(p=entries.begin(); p!=entries.end(); p++) {
    p->clearFormat();
    p->format(2, m_max_len_varname, m_max_len_vname, 
	      m_max_len_source, m_max_len_srcaux,
	      m_show_vname, m_show_varname, m_show_source, m_show_srcaux);
  }
}

//-------------------------------------------------------------
// Procedure: mergePlots

list<VarPlotEntry> ModelVarScope::mergePlots(list<VarPlotEntry> plota, 
					     list<VarPlotEntry> plotb) const
{
  list<VarPlotEntry> result;

  while(1) {
    if((plota.size() == 0) && (plotb.size() == 0))
      break;
    
    if((plota.size() == 0) && (plotb.size() != 0)) {
      result.push_back(plotb.front());
      plotb.pop_front();
    }
    else if((plota.size() != 0) && (plotb.size() == 0)) {
      result.push_back(plota.front());
      plota.pop_front();
    }
    else if(plota.front().getTStamp() < plotb.front().getTStamp()) {
      result.push_back(plota.front());
      plota.pop_front();
    }
    else {
      result.push_back(plotb.front());
      plotb.pop_front();
    }
  }
  return(result);
}

//-------------------------------------------------------------
// Procedure: setTime()

void ModelVarScope::setTime(double gtime)
{
  if(gtime > m_curr_time) {
    while((m_soon_entries.size() != 0) &&
	  (m_soon_entries.front().getTStamp() <= gtime)) {
	m_prev_entries.push_back(m_soon_entries.front());
	m_soon_entries.pop_front();
    }
  }

  if(gtime < m_curr_time) {
    while((m_prev_entries.size() != 0) &&
	  (m_prev_entries.back().getTStamp() > gtime)) {
	m_soon_entries.push_front(m_prev_entries.back());
	m_prev_entries.pop_back();
    }
  }

  m_curr_time = gtime;
}

//-------------------------------------------------------------
// Procedure: getPastEntries()

vector<string> ModelVarScope::getPastEntries() const
{
  return(getEntries(m_prev_entries));
}

//-------------------------------------------------------------
// Procedure: getSoonEntries()

vector<string> ModelVarScope::getSoonEntries() const
{
  return(getEntries(m_soon_entries));
}

//-------------------------------------------------------------
// Procedure: getEntries()

vector<string> ModelVarScope::getEntries(const list<VarPlotEntry>& entries) const
{
  vector<string> rvector;

  list<VarPlotEntry>::const_iterator p;
  for(p=entries.begin(); p!=entries.end(); p++) {
#if 1
    string entry = p->getFormatted();
#endif
#if 0
    VarPlotEntry vpentry = *p;
    string entry = doubleToString(vpentry.getTStamp(), 3) + "  ";   // FIxME
    entry += vpentry.getVarName() + "  ";
    entry += vpentry.getSource() + "  ";
    entry += vpentry.getVarVal();
#endif
    rvector.push_back(entry);
  }

  return(rvector);
}





