/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VarTPlot.cpp                                         */
/*    DATE: Oct1211                                              */
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

#include <iostream>
#include "VarTPlotSet.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

VarTPlotSet::VarTPlotSet()
{
  m_null_entry.setStatus("null");
}

//---------------------------------------------------------------
// Procedure: addVarPlot
//      Note: filter=true indicates the variable will be included
//            in calls to getEntriesByTimeUTC()

void VarTPlotSet::addVarPlot(const VarTPlot& vplot, bool filter)
{
  m_vplots.push_back(vplot);
  m_filter.push_back(filter);
}


//---------------------------------------------------------------
// Procedure: initCache

void VarTPlotSet::initCacheTimeUTC(double gtime)
{
  m_cache_index.clear();
  m_cache_entry.clear();

  unsigned int i, vsize = m_vplots.size();
  for(i=0; i<vsize; i++) {
    if(m_filter[i] == true) {
      unsigned int index = m_vplots[i].getIndexByTimeUTC(gtime);
      const ALogEntry entry = m_vplots[i].getEntryByIndex(index);
      m_cache_index.push_back(index);
      m_cache_entry.push_back(entry);
    }
    else {
      m_cache_index.push_back(0);
      m_cache_entry.push_back(m_null_entry);
    }
  }
}
     
//---------------------------------------------------------------
// Procedure: getEntriesByTimeUTC()

vector<ALogEntry> VarTPlotSet::getEntriesByTimeUTC(double gtime,
						   unsigned int amt)
{
  initCacheTimeUTC(gtime);
  vector<ALogEntry> return_vector;

  unsigned int i = 0;
  for(i=0; i<amt; i++) {
    const ALogEntry& entry = getNextEntry();
    if(!entry.isNull())
      return_vector.push_back(entry);
  }
  return(return_vector);
}

//---------------------------------------------------------------
// Procedure: getNextEntry()

ALogEntry VarTPlotSet::getNextEntry()
{
  unsigned int i, vsize = m_cache_entry.size();
  if(vsize == 0)
    return(m_null_entry);

  // Find entry from set of vplots that has with oldest timestamp
  unsigned int chosen_ix = 0;
  double       latest_ts = 0;
  bool         found_non_null = false;
  for(i=0; i<vsize; i++) {
    if(!m_cache_entry[i].isNull()) {
      if(!found_non_null || (m_cache_entry[i].time() > latest_ts)) {
	chosen_ix = i;
	latest_ts = m_cache_entry[i].time();
	found_non_null = true;
      }
    }    
  }

  // If all VPlots are empty just return null entry
  if(!found_non_null)
    return(m_null_entry);

  // Store the return entry saving it for returning
  ALogEntry return_entry = m_cache_entry[chosen_ix];

  // ================== Begin update the cache ================
  // If the winning vplot is already down to its last index(0) 
  // then just replace the cache entry with the null_entry.
  if(m_cache_index[chosen_ix] == 0) 
    m_cache_entry[chosen_ix] = m_null_entry;
  // Otherwise reduce the index cached for the winning vplot
  // and update the cached entry for the winnning vplot
  else {
    unsigned int newix = m_cache_index[chosen_ix] - 1;
    m_cache_index[chosen_ix] = newix;
    m_cache_entry[chosen_ix] = m_vplots[chosen_ix].getEntryByIndex(newix);
  }
  // ================== End update the cache ================

  return(return_entry);
}
     
//---------------------------------------------------------------
// Procedure: print

void VarTPlotSet::print() const
{
  cout << "VarTPlotSet::print()" << endl;
  
  unsigned int i, vsize = m_vplots.size();
  for(i=0; i<vsize; i++) {
    cout << "VarName [" << i << "]: " << m_vplots[i].getVarName() << endl;
  }
}

