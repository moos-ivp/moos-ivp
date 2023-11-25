/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Populator_AppLogPlot.cpp                             */
/*    DATE: Oct 16th, 2021                                       */
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

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "Populator_AppLogPlot.h"
#include "AppLogEntry.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: populateFromEntries()

bool Populator_AppLogPlot::populateFromEntries(const vector<ALogEntry>& entries)
{
  if(entries.size() == 0)
    return(false);

  AppLogPlot alplot;
  
  for(unsigned int i=0; i<entries.size(); i++) {
    string var = entries[i].getVarName();
    if(var == "APP_LOG") {
      double time = entries[i].getTimeStamp();
      string src  = entries[i].getSource();

      
      if(alplot.getAppName() == "")
	alplot.setAppName(src);
      else if(alplot.getAppName() != src) {
	cout << "APP_LOG appname mismatch!!" << endl;
	cout << "alplot.getAppName():[" << alplot.getAppName() << "]" << endl;
	cout << "src:[" << src << "]" << endl;
	return(false);
      }

      string sval = entries[i].getStringVal();
      AppLogEntry entry = stringToAppLogEntry(sval);
      if(entry.valid())
	alplot.addAppLogEntry(time, entry);
      else {
	cout << "Populator_AppLogPlot warning: invalid entry!" << endl;
	cout << "Entry:[" << sval << "]" << endl;
	entry =  stringToAppLogEntry(sval, true);
      }
    }
  }

  m_alplot = alplot;
  return(true);
}











