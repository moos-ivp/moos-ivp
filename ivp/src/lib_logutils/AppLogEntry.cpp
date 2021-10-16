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


bool AppLogEntry::setAppLogEntry(string raw)
{
  bool ok = true;
  
  while(raw != "") {
    string field = biteStringX(raw, ',');
    string param = biteStringX(field, '=');
    string value = field;
    if(param == "iter") {
      int ival = atoi(value.c_str());
      if(ival < 1) {
	ok = false;
	break;
      }
    }
    else if(param == "log") 
      m_lines = parseString(value, "!@#");
    else
      ok = false;
  }

  if(!ok) {
    m_iteration = 0;
    m_lines.clear();
    return(false);
  }

  return(true);
}




