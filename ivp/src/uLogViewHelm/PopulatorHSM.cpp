/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PopulatorHSM.cpp                                     */
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

#include "PopulatorHSM.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

PopulatorHSM::PopulatorHSM()
{
  m_vplot_errors.setVarName("BHV_ERROR");
  m_vplot_warnings.setVarName("BHV_WARNING");
  m_vplot_engaged.setVarName("IVPHELM_ENGAGED");
}

//---------------------------------------------------------------
// Procedure: populateFromEntries

bool PopulatorHSM::populateFromEntries(const vector<ALogEntry>& entries)
{
  int i, vsize = entries.size();
  if(vsize == 0)
    return(false);
  
  for(i=0; i<vsize; i++) {
    string var  = entries[i].getVarName();
    double time = entries[i].getTimeStamp();
    string sval = entries[i].getStringVal();
    if(var == "IVPHELM_SUMMARY")
      m_helm_plot.addEntry(time, sval);
    else if(var == "BHV_ERROR")
      m_vplot_errors.addEntry(entries[i]);
    else if(var == "BHV_WARNING")
      m_vplot_warnings.addEntry(entries[i]);
    else if(var == "IVPHELM_ENGAGED")
      m_vplot_engaged.addEntry(entries[i]);
  }
  return(true);
}


//---------------------------------------------------------------
// Procedure: setLogStartUTC

void PopulatorHSM::setLogStartUTC(double utc_time)
{
  m_helm_plot.setLogStartUTC(utc_time);
  m_vplot_errors.setBaseUTC(utc_time);
  m_vplot_warnings.setBaseUTC(utc_time);
  m_vplot_engaged.setBaseUTC(utc_time);
}


//---------------------------------------------------------------
// Procedure: setVName

void PopulatorHSM::setVName(string name)
{
  m_helm_plot.setVName(name);
  m_vplot_errors.setVehiName(name);
  m_vplot_warnings.setVehiName(name);
  m_vplot_engaged.setVehiName(name);
}

