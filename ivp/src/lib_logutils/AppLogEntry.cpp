/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppLogEntry.cpp                                      */
/*    DATE: Oct 15th 2021                                        */
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
#include "AppLogEntry.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: getLine()

string AppLogEntry::getLine(unsigned int ix) const
{
  if(ix >= m_lines.size())
    return("");
  return(m_lines[ix]);
}

//----------------------------------------------------------------
// Procedure: getTruncLine()

string AppLogEntry::getTruncLine(unsigned int ix) const
{
  if(ix >= m_lines.size())
    return("");
  return(truncString(m_lines[ix],80));
}

//----------------------------------------------------------------
// Procedure: getTruncLines()

vector<string> AppLogEntry::getTruncLines()
{
  vector<string> trunc_lines;
  for(unsigned int i=0; i<m_lines.size(); i++) {
    string str = truncString(m_lines[i], 80);
    trunc_lines.push_back(str);
  }
  return(trunc_lines);
}

//----------------------------------------------------------------
// Procedure: getWrapLines()

vector<string> AppLogEntry::getWrapLines()
{
  vector<string> all_lines;
  for(unsigned int i=0; i<m_lines.size(); i++) {
    vector<string> wrap_strings = breakLen(m_lines[i], 70);
    all_lines = mergeVectors(all_lines, wrap_strings);
  }
  return(all_lines);
}

//----------------------------------------------------------------
// Procedure: stringtoAppLogEntry()
//   Example: APP_LOG = "iter=23,log=line!@#line!@#...!@#line"

AppLogEntry stringToAppLogEntry(string raw, bool verbose)
{
  AppLogEntry null_entry;
  AppLogEntry good_entry;
  
  while(raw != "") {
    string field = biteStringX(raw, ',');
    string param = biteStringX(field, '=');
    string value = field;
    if(param == "iter") {
      int ival = atoi(value.c_str());
      if(ival < 1) {
	if(verbose)
	  cout << "VERBOSE: bad ival:" << ival << endl;
	return(null_entry);
      }
    }
    else if(param == "log") {
      vector<string> lines = parseString(value, "!@#");
      good_entry.setAppLogLines(lines);
      if(verbose)
	cout << "VERBOSE: numlines: " << lines.size() << endl;
      raw = "";
    }
    else {
      if(verbose)
	cout << "bad param:[" << param << "]" << endl;
      return(null_entry);
    }
  }

  return(good_entry);
}

