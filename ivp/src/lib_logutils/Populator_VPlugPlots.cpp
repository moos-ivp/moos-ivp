/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
//      Note: Likely called from ALogDataBroker::getVPlugPlot()

bool Populator_VPlugPlots::populateFromEntries(const vector<ALogEntry>& entries)
{
  // ===============================================================
  // Part 1: Determine bin value. The VPlugPlot will create a bin of
  // visuals for each unique timestamp, carrying over the visuals from
  // the previous bin, for each new timestamp. When the bin value is
  // non-zero, the bins hold a range of times. The larger the bin, the
  // more efficient/fast the creation of the VPlugPlot. Larger bins
  // also will appear less accurate or more "choppy" in playback. So a
  // compromise is sought based on the number of entries. The below
  // represents the policy for ever larger bin value, with larger sets
  // of entries.
  // ===============================================================
  unsigned esize = entries.size();
  double binval = 0;
  if(esize > 50000)
    binval = 0.001;
  if(esize > 100000)
    binval = 0.002;
  if(esize > 200000)
    binval = 0.005;
  if(esize > 400000)
    binval = 0.01;
  if(esize > 800000)
    binval = 0.05;
  if(esize > 1200000)
    binval = 0.1;
  if(esize > 2000000)
    binval = 0.15;
  if(esize > 4000000)
    binval = 0.2;
  if(esize > 8000000)
    binval = 0.5;

  // Adjust binval from the above policy based on an adjustment
  // factor chosen at launch time. "med" means just use binval as-is.
  if(m_vqual == "max")
    binval = 0;
  else if(m_vqual == "low")
    binval = binval * 2;
  else if(m_vqual == "vlow")
    binval = binval * 4;
  else if(m_vqual == "vvlow")
    binval = binval * 10;
  else if(m_vqual == "high")
    binval = binval / 2;

  m_vplug_plot.setBinVal(binval);

  cout << "     Visual entries: " << uintToCommaString(esize); 
  cout << ", binval: " << doubleToStringX(binval,2) << endl; 
   
  // ===============================================================
  // Part 2: Handle each entry in the vector of entries
  // ===============================================================
  char carriage_return = 13;
  int  pct_prev = 0;
  int  pct = 0;

  unsigned int vsize = entries.size();
  for(unsigned int i=0; i<vsize; i++) {

    pct = 1 + ((100 * i) / vsize);
    if(pct != pct_prev) {
      cout << "     Caching visual data: " << pct << "%";
      cout << carriage_return << flush;
      pct_prev = pct;
    }

    m_vplug_plot.addEvent(entries[i].getVarName(), 
			  entries[i].getStringVal(), 
			  entries[i].getTimeStamp());
  }
  unsigned int total_entries = m_vplug_plot.size();
  string str_entries = uintToCommaString(total_entries);
  cout << "Done. Total Plot Entries: " << str_entries << endl;
  cout << endl;
  return(true);
}

//---------------------------------------------------------------
// Procedure: populateFromEntry()
//      Note: Likely called from ALogDataBroker::getVPlugPlot()

bool Populator_VPlugPlots::populateFromEntry(const ALogEntry& entry)
{
  bool ok = m_vplug_plot.addEvent(entry.getVarName(), 
				  entry.getStringVal(), 
				  entry.getTimeStamp());
  return(ok);
}


