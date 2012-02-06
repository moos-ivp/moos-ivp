/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_HelmTPlots.cpp                             */
/*    DATE: Jul1509, Sep2811                                     */
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

#include "Populator_HelmTPlots.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: populateFromEntries

bool Populator_HelmTPlots::populateFromEntries(const vector<ALogEntry>& entries)
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
      m_helm_plotx.addEntry(time, sval);
    }
  }
  return(true);
}


//---------------------------------------------------------------
// Procedure: setLogStartUTC

void Populator_HelmTPlots::setLogStartUTC(double utc_time)
{
  m_logstart_utc =  utc_time;
  m_helm_plot.setLogStartUTC(utc_time);
  m_helm_plotx.setLogStartUTC(utc_time);
}


//---------------------------------------------------------------
// Procedure: setVName

void Populator_HelmTPlots::setVName(string name)
{
  m_vname = name;
  m_helm_plot.setVName(name);
  m_helm_plotx.setVName(name);
}

