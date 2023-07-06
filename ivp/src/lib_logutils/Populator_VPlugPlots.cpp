/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_VPlugPlots.cpp                             */
/*    DATE: August 9th, 2009                                     */
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

#include "Populator_VPlugPlots.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: populateFromEntries()

bool Populator_VPlugPlots::populateFromEntries(const vector<ALogEntry>& entries)
{
  char carriage_return = 13;

  int pct_prev = 0;
  int pct = 0;

  
  unsigned int vsize = entries.size();
  for(unsigned int i=0; i<vsize; i++) {

    pct = 1 + ((100 * i) / vsize);
    if(pct != pct_prev) {
      cout << "     Caching visual data: " << pct << "%";
      cout << carriage_return << flush;
      pct_prev = pct;
    }
      
    //if((i % 1000) ==0)
    //  cout << "+" << flush;
    //cout << "yyh " << entries[i].getVarName() << endl;
    m_vplug_plot.addEvent(entries[i].getVarName(), 
			  entries[i].getStringVal(), 
			  entries[i].getTimeStamp());
  }
  //cout << "yyh done" << endl;
  cout << endl;
  return(true);
}

//---------------------------------------------------------------
// Procedure: populateFromEntry()

bool Populator_VPlugPlots::populateFromEntry(const ALogEntry& entry)
{
  bool ok = m_vplug_plot.addEvent(entry.getVarName(), 
				  entry.getStringVal(), 
				  entry.getTimeStamp());
  return(ok);
}
