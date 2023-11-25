/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Populator_HelmPlots.cpp                              */
/*    DATE: July 15th, 2009                                      */
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
#include "Populator_HelmPlots.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: populateFromEntries

bool Populator_HelmPlots::populateFromEntries(const vector<ALogEntry>& entries)
{
  int i, vsize = entries.size();
  if(vsize == 0)
    return(false);
  
  for(i=0; i<vsize; i++) {
    string var = entries[i].getVarName();
    if(var == "IVPHELM_SUMMARY") {
      double time = entries[i].getTimeStamp();
      string sval = entries[i].getStringVal();
      m_helm_plot.addEntry(time, sval);
    }
  }
  return(true);
}











