/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: DemuxUnit.cpp                                        */
/*    DATE: May 20th 2006                                        */
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
#include "DemuxUnit.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

DemuxUnit::DemuxUnit(const string& g_unit_id, int g_total_packets, 
		     double g_time_stamp)
{
  m_unit_id = g_unit_id;
  for(int i=0; i<g_total_packets; i++) {
    m_data.push_back("");
    m_flag.push_back(false);
  }
  m_time_stamp = g_time_stamp;
}
  
//----------------------------------------------------------------
// Procedure: getDemuxString
//   Purpose: If all packets have been received (all flag[]=true)
//            then return a string composed by concatenating all 
//            m_data[] elements.

string DemuxUnit::getDemuxString()
{
  unsigned int i;
  for(i=0; i<m_flag.size(); i++)
    if(m_flag[i]==false)
      return("");
  
  string full_string;
  for(i=0; i<m_data.size(); i++)
    full_string += m_data[i];
  
  return(full_string);
}

//----------------------------------------------------------------
// Procedure: unitReady()
//   Purpose: Determine if all demux packets have been received, 
//            based on the expected number of packets provided to
//            the DemuxUnit Constructor.

bool DemuxUnit::unitReady()
{
  unsigned int i;
  for(i=0; i<m_flag.size(); i++)
    if(!m_flag[i])
      return(false);
  return(true);
}
  
//----------------------------------------------------------------
// Procedure: print()

void DemuxUnit::print()
{
  unsigned int i;

  cout << "UnitID: " << m_unit_id << endl;
  for(i=0; i<m_data.size(); i++) {
    cout << "[" << i << "]: [" << m_data[i] << "]" << endl;
  }
  cout << "UnitReady(): " << unitReady() << endl;
}
  








