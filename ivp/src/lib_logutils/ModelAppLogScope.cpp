/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ModelAppLogScope.cpp                                 */
/*    DATE: Oct 16th 2021                                        */
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
#include "ModelAppLogScope.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

ModelAppLogScope::ModelAppLogScope()
{
  m_curr_time = 0;

  m_grep1          = "size";
  m_grep2          = "";
  m_wrap           = false;
  m_future         = false;
  m_truncate       = false;
  m_show_separator = true;
  m_grep_apply1    = false;
  m_grep_apply2    = false;

  m_entries_upto_now_cnt = 0;
}

//-------------------------------------------------------------
// procedure: setAppLogPlot()

void ModelAppLogScope::setAppLogPlot(const AppLogPlot& alplot)
{
  m_alplot = alplot;
}

//-------------------------------------------------------------
// Procedure: setTime()

void ModelAppLogScope::setTime(double gtime)
{
  m_curr_time = gtime;
}

//-------------------------------------------------------------
// Procedure: getLinesUpToNow()

vector<string> ModelAppLogScope::getLinesUpToNow()
{
  vector<AppLogEntry> entries = m_alplot.getEntriesUpToTime(m_curr_time);
    
  vector<string> all_lines = processLines(entries);

  return(all_lines);
}



//-------------------------------------------------------------
// Procedure: getLinesPastNow()

vector<string> ModelAppLogScope::getLinesPastNow() const
{
  vector<AppLogEntry> entries = m_alplot.getEntriesPastTime(m_curr_time);

  vector<string> all_lines = processLines(entries, m_entries_upto_now_cnt);

  return(all_lines);
}



//-------------------------------------------------------------
// Procedure: processLines()
//   Purpose: Do the work of converting all AppLogEntrie into a set of
//            user consumable lines. Applying:
//              a) separator lines if activated
//              b) truncation and/or wrapping if activated
//              c) Grep1 and Grep2 patterns if activated

vector<string> ModelAppLogScope::processLines(vector<AppLogEntry> entries,
					      unsigned int start_ctr) const
{
  vector<string> all_lines;
  for(unsigned int i=0; i<entries.size(); i++) {  
    unsigned int ctr = entries[i].getIteration();
    if(m_show_separator)
      all_lines.push_back("----------------- App Iteration ----- " + uintToString(ctr));

    // Get the set of lines from this entry, possibly truncating
    // and possibly wrapping if these features are activated.
    vector<string> lines;    
    if(m_wrap)
      lines = entries[i].getWrapLines();
    else if(m_truncate)
      lines = entries[i].getTruncLines();
    else
      lines = entries[i].getLines();
    
    // Apply Grep 1 if activated
    if((m_grep1 != "") && m_grep_apply1) {
      vector<string> grep_lines;
      for(unsigned int j=0; j<lines.size(); j++) {
	if(strContains(lines[j], m_grep1))
	  grep_lines.push_back(lines[j]);
      }
      lines = grep_lines;
    }
	
    // Apply Grep 2 if activated
    if((m_grep2 != "") && m_grep_apply2) {
      vector<string> grep_lines;
      for(unsigned int j=0; j<lines.size(); j++) {
	if(strContains(lines[j], m_grep2))
	  grep_lines.push_back(lines[j]);
      }
      lines = grep_lines;
    }
    
    
    all_lines = mergeVectors(all_lines, lines);
  }
    
  return(all_lines);
}

