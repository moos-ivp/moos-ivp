/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_EncounterPlot.cpp                          */
/*    DATE: Jan 12th 2016                                        */
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
#include "Populator_EncounterPlot.h"
#include "MBUtils.h"
#include "CPAEvent.h"


using namespace std;

//---------------------------------------------------------------
// Procedure: populateFromEntries

bool Populator_EncounterPlot::populateFromEntries(const vector<ALogEntry>& entries)
{
  if(entries.size() == 0)
    return(false);
  
  for(unsigned int i=0; i<entries.size(); i++) {
    string var = entries[i].getVarName();
    if(var == "EVAL_LOITER_SUMMARY") {
      double time = entries[i].getTimeStamp();
      string sval = entries[i].getStringVal();
      CPAEvent event(sval);
      m_encounter_plot.addEncounter(time, event);
    }
  }
  return(true);
}






