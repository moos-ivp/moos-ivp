/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ModeSet.cpp                                          */
/*    DATE: Dec 26th, 2008                                       */
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
#include "ModeSet.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------------
// Procedure: addEntry

bool ModeSet::addEntry(const string& str)
{
  ModeEntry new_entry;
  bool ok = new_entry.setEntry(str);
  if(ok)
    m_entries.push_back(new_entry);
  return(ok);
}

//------------------------------------------------------------------
// Procedure: print()

void ModeSet::print()
{
  unsigned int i, vsize = m_entries.size();
  for(i=0; i<vsize; i++) {
    cout << "Mode Entry # " << i << ": " << endl;
    m_entries[i].print();
  }

  vector<string> con_vars = getConditionVars();
  vsize = con_vars.size();
  cout << "MOOS variables (" << vsize << "): " << endl;

  for(i=0; i<vsize; i++) {
    cout << con_vars[i];
    if(i < (vsize-1))
      cout << ", "; 
    else
      cout << endl;
  }
}


//------------------------------------------------------------------
// Procedure: getConditionVars

vector<string> ModeSet::getConditionVars()
{
  vector<string> union_vector;
  
  unsigned int i, vsize = m_entries.size();
  for(i=0; i<vsize; i++) {
    vector<string> svector = m_entries[i].getConditionVars();
    union_vector = mergeVectors(union_vector, svector);
  }

  union_vector = removeDuplicates(union_vector);
  return(union_vector);
}
