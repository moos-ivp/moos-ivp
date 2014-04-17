/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IPF_TPlot.cpp                                        */
/*    DATE: Feb2407, Sep2811                                     */
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
#include <cstdio>
#include <cstdlib>
#include "IPF_TPlot.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

IPF_TPlot::IPF_TPlot() : TimePlot() 
{
  m_vname = "name-unknown";
  m_source = "src-unknown";
}


//---------------------------------------------------------------
// Procedure: addEntry
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.

bool IPF_TPlot::addEntry(double timestamp, 
			const string& ipf_str, 
			unsigned int helm_iteration, 
			unsigned int piece_count,
			double priority,
			IvPDomain ivp_domain)
{
  unsigned int tsize = m_time.size();
  if((tsize != 0) && (m_time[tsize-1] > timestamp))
    return(false);
  
  unsigned int isize = m_helm_iteration.size();
  if((isize != 0) && (helm_iteration != 0) && 
     (m_helm_iteration[isize-1] > helm_iteration))
    return(false);
  
  m_time.push_back(timestamp);
  m_ipf_string.push_back(ipf_str);
  m_helm_iteration.push_back(helm_iteration);
  m_piece_count.push_back(piece_count);
  m_priority.push_back(priority);
  m_ivp_domain_iter.push_back(ivp_domain);

  return(true);
}

//---------------------------------------------------------------
// Procedure: getTimeUTCByIndex

double IPF_TPlot::getTimeUTCByIndex(unsigned int index) const
{
  if(m_time.size() == 0)
    return(0);

  if(index >= m_time.size())
    return(m_time[m_time.size()-1] + m_logstart_utc);
  else
    return(m_time[index] + m_logstart_utc);
}

     
//---------------------------------------------------------------
// Procedure: getIPFByIndex

string IPF_TPlot::getIPFByIndex(unsigned int index) const
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
// Procedure: getIPFByTimeUTC

string IPF_TPlot::getIPFByTimeUTC(double timestamp) const
{
  // Special case: if the IPF_TPlot instance is "empty"
  unsigned int vsize = m_time.size();
  if(vsize == 0)
    return("");

  // Check if query time is outside the IPF_TPlot time range
  if(timestamp >=  m_time[vsize-1] + m_logstart_utc)
    return("");
  if(timestamp <= m_time[0] + m_logstart_utc)
    return("");

  // Determine highest index w/ time <= timestamp
  unsigned int index = getIndexByTimeUTC(timestamp);
  return(m_ipf_string[index]);
}

//---------------------------------------------------------------
// Procedure: getIPFByHelmIteration
//   Purpose: Find the ipf for a given point in time.
//            If the point in time happens between two elements
//            in the time array, choose the lower indexed.

string IPF_TPlot::getIPFByHelmIteration(unsigned int iter) const
{
  // Special case: if the IPF_TPlot instance is "empty"
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

unsigned int IPF_TPlot::getPcsByHelmIteration(unsigned int iter) const
{
  // Special case: if the IPF_TPlot instance is "empty"
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

double IPF_TPlot::getPwtByHelmIteration(unsigned int iter) const
{
  // Special case: if the IPF_TPlot instance is "empty"
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

IvPDomain IPF_TPlot::getDomainByHelmIter(unsigned int iter) const
{
  // Special case: if the IPF_TPlot instance is "empty"
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
// Procedure: getHelmIterByTimeUTC
//   Purpose:

unsigned int IPF_TPlot::getHelmIterByTimeUTC(double timestamp) const
{
  unsigned int index = getIndexByTimeUTC(timestamp);
  return(m_helm_iteration[index]);
}
 
//---------------------------------------------------------------
// Procedure: getTimeUTCByHelmIter
//   Purpose:

double IPF_TPlot::getTimeUTCByHelmIter(unsigned int iter) const
{
  int index = getIndexByHelmIter(iter);
  if(index == -1)
    return(0);

  return(m_time[index] + m_logstart_utc);
}
 
//---------------------------------------------------------------
// Procedure: getIndexByHelmIter
//   Purpose: Given a query iteration#, determine the index that 
//            contains the ipf_string associated with that iteration
//            Returns -1 if the given helm iteration doesn't exist

int IPF_TPlot::getIndexByHelmIter(unsigned int iter) const
{
  int vsize = m_helm_iteration.size();

  // Special case: if the query iter is outside the IPF_TPlot 
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
// Procedure: getMinHelmIter()
//      Note: Assumes time series is strictly monotonic

unsigned int IPF_TPlot::getMinHelmIter() const
{
  unsigned int vsize = m_helm_iteration.size();
  if(vsize == 0)
    return(0);
  return(m_helm_iteration[0]);
}  

//---------------------------------------------------------------
// Procedure: getMaxHelmIter()
//      Note: Assumes time series is strictly monotonic

unsigned int IPF_TPlot::getMaxHelmIter() const
{
  unsigned int vsize = m_helm_iteration.size();
  if(vsize == 0)
    return(0);
  return(m_helm_iteration[vsize-1]);
}  



//---------------------------------------------------------------
// Procedure: print

void IPF_TPlot::print() const
{
  unsigned int i;
  cout << "IPF_Plot::print()" << endl;
  for(i=0; i<m_time.size(); i++)
    cout << "time:" << m_time[i] << 
      "  ipf:" << m_ipf_string[i] << endl;
}




