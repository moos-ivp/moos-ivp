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
// Procedure: populateFromEntries

bool Populator_VPlugPlots::populateFromEntries(const vector<ALogEntry>& entries)
{
  int i, vsize = entries.size();
  for(i=0; i<vsize; i++)
    m_vplug_plot.addEvent(entries[i].getVarName(), 
			  entries[i].getStringVal(), 
			  entries[i].getTimeStamp());
  return(true);
}




