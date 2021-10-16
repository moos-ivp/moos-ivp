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
// Procedure: setAppLogEntry()
//   Example: APP_LOG = "iter=23,log=line!@#line!@#...!@#line"

AppLogEntry stringToAppLogEntry(string raw)
{
  AppLogEntry null_entry;
  AppLogEntry good_entry;
  
  while(raw != "") {
    string field = biteStringX(raw, ',');
    string param = biteStringX(field, '=');
    string value = field;
    if(param == "iter") {
      int ival = atoi(value.c_str());
      if(ival < 1)
	return(null_entry);
    }
    else if(param == "log") {
      vector<string> lines = parseString(value, "!@#");
      good_entry.setAppLogLines(lines);
    }
    else
      return(null_entry);
  }

  return(good_entry);
}




