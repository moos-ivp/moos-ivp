/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PartitionRecord.cpp                                  */
/*    DATE: May 19th, 2012                                       */
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

#include <iostream>
#include "PartitionRecord.h"

using namespace std;

PartitionRecord::PartitionRecord()
{
  // Init config vars
  m_high_val = 0;
  m_low_val  = 0;
  m_delta    = 0;

  m_bar_color.setColor("light_blue");

  // By default, there is only one partition (everything)
  m_partition_entry_cnt.push_back(0); 

  // Init status variables
  m_entries_out_of_range = 0;
  m_average              = 0;
}

//-------------------------------------------------------------
// Procedure: init

void PartitionRecord::init(double low, double high, unsigned int partitions)
{
  if(low > high)
    return;
  if(partitions == 0)
    return;

  m_high_val = high;
  m_low_val  = low;
  m_delta    = (m_high_val-m_low_val) / (double)(partitions);

  m_entries.clear();

  vector<unsigned int> part_cnt(partitions, 0);

  m_partition_entry_cnt = part_cnt;
}
  

//-------------------------------------------------------------
// Procedure: addValue()
//      Note: Add a new value/entry to memory, update running average

void PartitionRecord::addValue(double val)
{
  // Determin if the new entry is within the stated min/max range
  if((val > m_high_val) || (val < m_low_val)) {
    m_entries_out_of_range++;
    return;
  }

  // Update the running average  
  double old_count = (double)(m_entries.size());
  double old_total = m_average * old_count;

  double new_total = old_total + val;
  m_average = new_total / (old_count + 1);

  // Add the new entry to raw vector of entries
  m_entries.push_back(val);

  // Add the new entry to the correct partition
  unsigned int ix = (unsigned int)(val / m_delta);
  if(ix < m_partition_entry_cnt.size())
    m_partition_entry_cnt[ix]++;
}

//-------------------------------------------------------------
// Procedure: getValue()

double PartitionRecord::getValue(unsigned int ix) const
{
  if(ix < m_entries.size())
    return(m_entries[ix]);
  return(0);
}


//-------------------------------------------------------------
// Procedure: getEntryCount
//      Note: Return the number of entries contained in given partition

unsigned int PartitionRecord::getEntryCount(unsigned int ix) const
{
  if(ix < m_partition_entry_cnt.size())
    return(m_partition_entry_cnt[ix]);
  return(0);
}


//-------------------------------------------------------------
// Procedure: print()

void PartitionRecord::print() const
{
  cout << "Low: " << m_low_val  << endl;
  cout << "Hgh: " << m_high_val << endl;
  cout << "Partitions: " << m_partition_entry_cnt.size() << endl;

  for(unsigned int i=0; i<m_partition_entry_cnt.size(); i++)  {
    cout << "[" << i << "]: ";
    cout << "[" << ((double)(i)*m_delta) << "," << (double)(i+1)*m_delta << "]:";
    cout << m_partition_entry_cnt[i] << endl;
  }

  for(unsigned int j=0; j<m_entries.size(); j++) {
    if(j != 0)
      cout << ",";
    cout << m_entries[j];
  }
  cout << endl;
}









