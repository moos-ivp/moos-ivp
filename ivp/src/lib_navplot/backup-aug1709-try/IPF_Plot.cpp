/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IPF_Plot.cpp                                         */
/*    DATE: Feb 24th, 2007                                       */
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
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "IPF_Plot.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------------
// Destructor

IPF_Plot::~IPF_Plot()
{
  unsigned int i, vsize = m_ivp_function.size();
  for(i=0; i<vsize; i++)
    delete(m_ivp_function[i]);
}

//---------------------------------------------------------------
// Procedure: addEntry
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.

bool IPF_Plot::addEntry(IvPFunction* ipf, double gtime, unsigned int iter)
{
  if(!ipf)
    return(false);

  unsigned int tsize = m_time_stamp.size();
  if((tsize != 0) && (m_time_stamp[tsize-1] > gtime))
    return(false);

  unsigned int isize = m_helm_iteration.size();
  if((isize != 0) && (iter != 0) && (m_helm_iteration[isize-1] > iter))
    return(false);

  m_ivp_function.push_back(ipf);
  m_time_stamp.push_back(gtime);
  m_helm_iteration.push_back(iter);
  return(true);
}

//---------------------------------------------------------------
// Procedure: getTimeByIndex

double IPF_Plot::getTimeByIndex(unsigned int index) const
{
  if(m_time_stamp.size() == 0)
    return(0);

  if(index >= m_time_stamp.size())
    return(m_time_stamp[m_time_stamp.size()-1]);
  else
    return(m_time_stamp[index]);
}
     
//---------------------------------------------------------------
// Procedure: getIPFByIndex

IvPFunction *IPF_Plot::getIPFByIndex(unsigned int index) const
{
  if(m_ivp_function.size() == 0)
    return(0);

  if(index >= m_ivp_function.size())
    return(0);
  else
    return(m_ivp_function[index]);
}
     
//---------------------------------------------------------------
// Procedure: getIPFByTime
//   Purpose: Find the ipf for a given point in time.
//            If the point in time happens between two elements
//            in the time array, choose the lower indexed.

IvPFunction *IPF_Plot::getIPFByTime(double qtime) const
{
  // Special case: if the IPF_Plot instance is "empty"
  int vsize = m_time_stamp.size();
  if(vsize == 0)
    return(0);

  // Special case: if the query time is outside the IPF_Plot 
  // time range, return the extreme value.
  if(qtime >=  m_time_stamp[vsize-1])
    return(0);
  if(qtime <= m_time_stamp[0])
    return(0);

  // Determine highest index w/ time <= qtime
  int index = getIndexByTime(qtime);

  return(m_ivp_function[index]);
}
 
//---------------------------------------------------------------
// Procedure: getIPFByHelmIteration
//   Purpose: Find the ipf for a given point in time.
//            If the point in time happens between two elements
//            in the time array, choose the lower indexed.

IvPFunction *IPF_Plot::getIPFByHelmIteration(unsigned int iter) const
{
  // Special case: if the IPF_Plot instance is "empty"
  int vsize = m_helm_iteration.size();
  if(vsize == 0)
    return(0);

  // Determine the index for the given helm iteration
  int index = getIndexByHelmIter(iter);
  
  if(index == -1)
    return(0);
  else
    return(m_ivp_function[index]);
}
 
//---------------------------------------------------------------
// Procedure: nullHeadingSpeedIPF
//   Purpose:

string IPF_Plot::nullHeadingSpeedIPF() const
{
  string str = "H,14,46:bhv_waypt_b,2,1,1,100,D,course;0;359;360:speed;0;4;41,G,359,40,F,0,359,0,40,0,0,0";
  return(str);
}
    
//---------------------------------------------------------------
// Procedure: getIndexByTime
//   Purpose: Given a query time, determine the highest index that 
//            has a time less than or equal to the query time.
//            Search is log(n)

int IPF_Plot::getIndexByTime(double qtime) const
{
  int vsize = m_time_stamp.size();

  // Special case: if the query time is outside the IPF_Plot 
  // time range, return the extreme value.
  if(qtime <= m_time_stamp[0])
    return(0);
  if(qtime >= m_time_stamp[vsize-1])
    return(vsize-1);
  
  // Handle general case
  int jump  = vsize / 2;
  int index = vsize / 2;
  bool done = false;
  while(!done) {
    if(jump > 1)
      jump = jump / 2;
    if(m_time_stamp[index] <= qtime) {
      if(m_time_stamp[index+1] > qtime)
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
// Procedure: getIndexByHelmIter
//   Purpose: Given a query iteration#, determine the index that 
//            contains the iteration.
//            Returns -1 if the given helm iteration doesn't exist

int IPF_Plot::getIndexByHelmIter(unsigned int iter) const
{
  int vsize = m_helm_iteration.size();

  // Special case: if the query iter is outside the IPF_Plot 
  // iter range, then return -1;
  if(iter < m_helm_iteration[0])
    return(-1);
  if(iter > m_helm_iteration[vsize-1])
    return(-1);
  
  // Handle general case
  int jump  = vsize / 2;
  int index = vsize / 2;
  bool done = false;
  while(!done) {
    if(jump > 1)
      jump = jump / 2;
    if(m_helm_iteration[index] == iter)
      done = true;
    else if(m_helm_iteration[index] < iter) {
      if((index == vsize-1) || (m_time_stamp[index+1] > iter))
	done = true;
      else
	index += jump;
    }
    else if(m_helm_iteration[index] > iter) {
      if((index==0) || (m_time_stamp[index-1] < iter))
	done = true;
      else
	index -= jump;
    }
  }
  return(index);
}
     
//---------------------------------------------------------------
// Procedure: getMinTime

double IPF_Plot::getMinTime() const
{
  if(m_time_stamp.size() > 0)
    return(m_time_stamp[0]);
  else
    return(0);
}

//---------------------------------------------------------------
// Procedure: getMaxTime

double IPF_Plot::getMaxTime() const
{
  if(m_time_stamp.size() > 0)
    return(m_time_stamp[m_time_stamp.size()-1]);
  else
    return(0);
}
