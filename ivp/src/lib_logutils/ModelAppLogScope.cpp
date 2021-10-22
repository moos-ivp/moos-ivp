/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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

  m_grep           = "";
  m_wrap           = false;
  m_truncate       = false;
  m_show_separator = false;
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
// Procedure: getNowLines()

vector<string> ModelAppLogScope::getNowLines() const
{
  cout << "ModelAppLogScope::getNowLines() " << endl;
  cout << "alplot size: " << m_alplot.size() << endl;
  AppLogEntry entry = m_alplot.getEntryByTime(m_curr_time);

  vector<string> rvector = entry.getLines();
  
  return(rvector);
}


//-------------------------------------------------------------
// Procedure: getLinesUpToNow()

vector<string> ModelAppLogScope::getLinesUpToNow(bool separator) const
{
  cout << "ModelAppLogScope::getLinesUpToNow() " << endl;
  cout << "alplot size: " << m_alplot.size() << endl;
  vector<AppLogEntry> entries = m_alplot.getEntriesUpToTime(m_curr_time);

  vector<string> all_lines;
  for(unsigned int i=0; i<entries.size(); i++) {  
    if(separator)
      all_lines.push_back("===================================== " + uintToString(i));
    
    vector<string> lines;
    if(m_wrap)
      lines = entries[i].getWrapLines();
    else if(m_truncate)
      lines = entries[i].getTruncLines();
    else
      lines = entries[i].getLines();

    if(m_grep != "") {
      vector<string> grep_lines;
      for(unsigned int j=0; j<lines.size(); j++) {
	if(strContains(lines[j], m_grep))
	  grep_lines.push_back(lines[j]);
      }
      lines = grep_lines;
    }
	
    all_lines = mergeVectors(all_lines, lines);
  }
    
  return(all_lines);
}



//-------------------------------------------------------------
// Procedure: getLinesPastNow()

vector<string> ModelAppLogScope::getLinesPastNow(bool separator) const
{
  cout << "ModelAppLogScope::getLinesPastNow() " << endl;
  cout << "alplot size: " << m_alplot.size() << endl;
  vector<AppLogEntry> entries = m_alplot.getEntriesPastTime(m_curr_time);

  vector<string> all_lines;
  for(unsigned int i=0; i<entries.size(); i++) {  
    if(separator)
      all_lines.push_back("===================================== " + uintToString(i));
    vector<string> lines;
    if(m_wrap)
      lines = entries[i].getWrapLines();
    else if(m_truncate)
      lines = entries[i].getTruncLines();
    else
      lines = entries[i].getLines();
    all_lines = mergeVectors(all_lines, lines);
  }
    
  return(all_lines);
}
