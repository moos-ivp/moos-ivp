/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include <iostream>
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
    if(var == "ENCOUNTER_SUMMARY") {
      double time = entries[i].getTimeStamp();
      string sval = entries[i].getStringVal();
      CPAEvent event(sval);
      m_encounter_plot.addEncounter(time, event);
    }
    else if(var == "COLLISION_DETECT_PARAMS") {
      vector<string> svector = parseString(entries[i].getStringVal(), ',');
      for(unsigned int j=0; j<svector.size(); j++) {
	string param = biteStringX(svector[j], '=');
	string value = svector[j];
	cout << "   param:[" << param << "] value:[" << value << "]" << endl;
	if(param == "collision_range")
	  m_encounter_plot.setCollisionRange(atof(value.c_str()));
	else if(param == "near_miss_range")
	  m_encounter_plot.setNearMissRange(atof(value.c_str()));
	else if(param == "encounter_range")
	  m_encounter_plot.setEncounterRange(atof(value.c_str()));
      }
    }
  }
  return(true);
}

