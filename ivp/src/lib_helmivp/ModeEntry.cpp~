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
// Procedure: print()

void ModeSet::print()
{
  int i, vsize = m_moos_vars.size();
  cout << "MOOS variables as input: (" << vsize << "): " << endl;
  for(i=0; i<vsize; i++) {
    cout << "  " << m_moos_vars[i] << endl;
  }

  vsize = m_mode_vars.size();
  cout << "MOOS variables (" << vsize << "): " << endl;
  for(i=0; i<vsize; i++) {
    cout << "   " << m_mode_vars[i];
    cout << " = " << m_mode_vals[i] << endl;
  }
}
