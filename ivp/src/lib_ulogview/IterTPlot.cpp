/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IterTPlot.cpp                                        */
/*    DATE: Oct0211                                              */
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
#include "IterTPlot.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: setValue
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.


bool IterTPlot::setValue(double gtime, unsigned int new_iter)
{
  unsigned int tsize = m_time.size();

  if((tsize == 0) || (m_iter[tsize-1] < new_iter)) {
    m_time.push_back(gtime);
    m_iter.push_back(new_iter);
    return(true);
  }
  else
    return(false);
}


//---------------------------------------------------------------
// Procedure: getIndexByIter
//   Purpose: Given an iter, determine the highest index that has a
//            iter less than or equal to the given iter.

//  2 5 9 13 14 19 23 28 31 35 43 57
//             ^                 ^
//            25                 56

unsigned int IterTPlot::getIndexByIter(unsigned int iter) const
{
  unsigned int vsize = m_iter.size();
  if(vsize == 0)
    return(0);

  // Handle special cases
  if(iter <= m_iter[0])
    return(0);
  if(iter >= m_iter[vsize-1])
    return(vsize-1);

  int jump  = vsize / 2;
  int index = vsize / 2;
  bool done = false;
  while(!done) {
    //cout << "[" << index << "," << jump << "]" << flush;
    if(jump > 1)
      jump = jump / 2;
    if(m_iter[index] <= iter) {
      if(m_iter[index+1] > iter)
	done = true;
      else
	index += jump;
    }
    else
      index -= jump;
  }
  return(index);
}


//---------------------------------------------------------------
// Procedure: getIterByIndex

unsigned int IterTPlot::getIterByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_iter[index]);
  else
    return(0);
}
     
//---------------------------------------------------------------
// Procedure: getIterByTime

unsigned int IterTPlot::getIterByTime(double gtime) const
{
  unsigned int vsize = m_time.size();
  if(vsize == 0)
    return(0);

  if(gtime >= m_time[vsize-1])
    return(m_iter[vsize-1]);
  if(gtime <= m_time[0])
    return(m_iter[0]);

  unsigned int index = getIndexByTime(gtime);
  return(m_iter[index]);
}
     
//---------------------------------------------------------------
// Procedure: getIterByTimeUTC

unsigned int IterTPlot::getIterByTimeUTC(double utc_time) const
{
  double local_time = utc_time - m_logstart_utc;
  return(getIterByTime(local_time));
}
     

//---------------------------------------------------------------
// Procedure: getMinIter

unsigned int IterTPlot::getMinIter() const
{
  unsigned int vsize = m_time.size();
  if(vsize == 0)
    return(0);
  return(m_iter[0]);
}
     
//---------------------------------------------------------------
// Procedure: getMaxIter

unsigned int IterTPlot::getMaxIter() const
{
  unsigned int vsize = m_time.size();
  if(vsize == 0)
    return(0);
  return(m_iter[vsize-1]);
}
     
//---------------------------------------------------------------
// Procedure: containsIter

bool IterTPlot::containsIter(unsigned int iter) const
{
  // First check if non-empty
  unsigned int vsize = m_time.size();
  if(vsize == 0)
    return(false);

  // Then check if iteration satisfies extreme bounds
  if(iter > m_iter[vsize-1])
    return(false);
  if(iter < m_iter[0])
    return(false);

  return(true);
}
     

//---------------------------------------------------------------
// Procedure: print

void IterTPlot::print() const
{
  unsigned int i, vsize = m_time.size();
  cout << "Helm Iteration Plot - Size: " << vsize << endl;

  for(i=0; i<vsize; i++) 
    cout << "t[" << m_time[i] << "]: " << m_iter[i] << endl;
}
     

