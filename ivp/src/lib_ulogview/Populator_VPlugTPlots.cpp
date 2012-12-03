/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_VPlugPlots.cpp                             */
/*    DATE: Aug0909, Sep2811                                     */
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

#include "Populator_VPlugTPlots.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: populateFromEntries

bool Populator_VPlugTPlots::populateFromEntries(const vector<ALogEntry>& entries)
{
  unsigned int i, vsize = entries.size();
  for(i=0; i<vsize; i++)
    m_vplug_plot.addEvent(entries[i].getVarName(), 
			  entries[i].getStringVal(), 
			  entries[i].getTimeStamp());
  m_vplug_plot.setLogStartUTC(m_logstart_utc);
  return(true);
}



