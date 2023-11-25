/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: IPF_Plot.cpp                                         */
/*    DATE: Feb 24th, 2007                                       */
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
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include "IPF_Plot.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: addEntry
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.

bool IPF_Plot::addEntry(double timestamp, 
			const string& ipf_str, 
			unsigned int helm_iteration, 
			unsigned int piece_count,
			double priority,
			IvPDomain ivp_domain)
{
  unsigned int tsize = m_time_stamp.size();
  if((tsize != 0) && (m_time_stamp[tsize-1] > timestamp))
    return(false);
  
  unsigned int isize = m_helm_iteration.size();
  if((isize != 0) && (helm_iteration != 0) && 
     (m_helm_iteration[isize-1] > helm_iteration))
    return(false);
  
  m_time_stamp.push_back(timestamp);
  m_ipf_string.push_back(ipf_str);
  m_helm_iteration.push_back(helm_iteration);
  m_piece_count.push_back(piece_count);
  m_priority.push_back(priority);
  m_ivp_domain_iter.push_back(ivp_domain);

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

string IPF_Plot::getIPFByIndex(unsigned int index) const
{
  if(m_ipf_string.size() == 0)
    return("");

  //if(index < 0)
  //  return(m_ipf_string[0]);
  if(index >= m_ipf_string.size())
    return(m_ipf_string[m_ipf_string.size()-1]);
  else
    return(m_ipf_string[index]);
}
     
//---------------------------------------------------------------
// Procedure: getIPFByTime
//   Purpose: Find the ipf for a given point in time.
//            If the point in time happens between two elements
//            in the time array, choose the lower indexed.

string IPF_Plot::getIPFByTime(double timestamp) const
{
  // Special case: if the IPF_Plot instance is "empty"
  unsigned int vsize = m_time_stamp.size();
  if(vsize == 0)
    return("");

  // Special case: if the query time is outside the IPF_Plot 
  // time range, return the extreme value.
  if(timestamp >=  m_time_stamp[vsize-1])
    return("");
  if(timestamp <= m_time_stamp[0])
    return("");

  // Determine highest index w/ time <= timestamp
  unsigned int index = getIndexByTime(timestamp);

  return(m_ipf_string[index]);
}
 
//---------------------------------------------------------------
// Procedure: getIPFByHelmIteration
//   Purpose: Find the ipf for a given point in time.
//            If the point in time happens between two elements
//            in the time array, choose the lower indexed.

string IPF_Plot::getIPFByHelmIteration(unsigned int iter) const
{
  // Special case: if the IPF_Plot instance is "empty"
  int vsize = m_helm_iteration.size();
  if(vsize == 0) {
    return("");
  }

  // Determine the index for the given helm iteration
  int index = getIndexByHelmIter(iter);
  
  if(index == -1)
    return("");
  else
    return(m_ipf_string[index]);
}
 
//---------------------------------------------------------------
// Procedure: getPcsByHelmIteration

unsigned int IPF_Plot::getPcsByHelmIteration(unsigned int iter) const
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
    return(m_piece_count[index]);
}
 
//---------------------------------------------------------------
// Procedure: getPwtByHelmIteration

double IPF_Plot::getPwtByHelmIteration(unsigned int iter) const
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
    return(m_priority[index]);
}
 
//---------------------------------------------------------------
// Procedure: getDomainByHelmIter

IvPDomain IPF_Plot::getDomainByHelmIter(unsigned int iter) const
{
  // Special case: if the IPF_Plot instance is "empty"
  IvPDomain empty_domain;
  if(m_helm_iteration.size() == 0)
    return(empty_domain);

  // Determine the index for the given helm iteration
  int index = getIndexByHelmIter(iter);
  
  if(index == -1)
    return(empty_domain);
  else
    return(m_ivp_domain_iter[index]);
}
 
//---------------------------------------------------------------
// Procedure: getHelmIterByTime
//   Purpose:

unsigned int IPF_Plot::getHelmIterByTime(double timestamp) const
{
  unsigned int index = getIndexByTime(timestamp);
  return(m_helm_iteration[index]);
}
 
//---------------------------------------------------------------
// Procedure: getTimeByHelmIter
//   Purpose:

double IPF_Plot::getTimeByHelmIter(unsigned int iter) const
{
  int index = getIndexByHelmIter(iter);
  if(index == -1)
    return(0);

  return(m_time_stamp[index]);
}
 
//---------------------------------------------------------------
// Procedure: getIndexByTime
//   Purpose: Given a query time, determine the highest index that 
//            has a time less than or equal to the query time.
//            Search is log(n)

unsigned int IPF_Plot::getIndexByTime(double timestamp) const
{
  unsigned int vsize = m_time_stamp.size();

  // Special case: if the query time is outside the IPF_Plot 
  // time range, return the extreme value.
  if(timestamp <= m_time_stamp[0])
    return(0);
  if(timestamp >= m_time_stamp[vsize-1])
    return(vsize-1);
  
  // Handle general case
  unsigned int jump  = vsize / 2;
  unsigned int index = vsize / 2;
  bool done = false;
  while(!done) {
    if(jump > 1)
      jump = jump / 2;
    if(m_time_stamp[index] <= timestamp) {
      if(m_time_stamp[index+1] > timestamp)
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
//            contains the ipf_string associated with that iteration
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
      if((index == vsize-1) || (m_helm_iteration[index+1] > iter)) {
	index = -1;
	done  = true;
      }
      else
	index += jump;
    }
    else if(m_helm_iteration[index] > iter) {
      if((index==0) || (m_helm_iteration[index-1] < iter)) {
	index = -1;
	done = true;
      }
      else
	index -= jump;
    }
  }
  return(index);
}
     

//---------------------------------------------------------------
// Procedure: applySkew

void IPF_Plot::applySkew(double skew)
{
  for(unsigned int i=0; i<m_time_stamp.size(); i++)
    m_time_stamp[i] += skew;
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

//---------------------------------------------------------------
// Procedure: print

void IPF_Plot::print() const
{
  unsigned int i;
  cout << "IPF_Plot::print()" << endl;
  for(i=0; i<m_time_stamp.size(); i++)
    cout << "time:" << m_time_stamp[i] << 
      "  ipf:" << m_ipf_string[i] << endl;
}





