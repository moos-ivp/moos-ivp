/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_HelmPlots.cpp                              */
/*    DATE: July 15th, 2009                                      */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
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
      m_helm_plot.add_entry(time, sval);
    }
  }
  return(true);
}


