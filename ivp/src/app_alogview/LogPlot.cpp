/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogPlot.cpp                                          */
/*    DATE: May 31st, 2005                                       */
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

#include <list>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "LogPlot.h"
#include "LogUtils.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

LogPlot::LogPlot()
{
  m_min_val  = 0; 
  m_max_val  = 0; 
  m_median   = 0;
  m_median_set = false;
}

//---------------------------------------------------------------
// Procedure: setValue
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.

bool LogPlot::setValue(double gtime, double gval)
{
  unsigned int tsize = m_time.size();

  if((tsize == 0) || (m_time[tsize-1] <= gtime)) {
    m_time.push_back(gtime);
    m_value.push_back(gval);
    if((tsize == 0) || (gval > m_max_val))
      m_max_val = gval;
    if((tsize == 0) || (gval < m_min_val))
      m_min_val = gval;
    m_median_set = false;
    return(true);
  }
  else
    return(false);
}

//---------------------------------------------------------------
// Procedure: applySkew

void LogPlot::applySkew(double skew)
{
  for(unsigned int i=0; i<m_time.size(); i++)
    m_time[i] += skew;
}
     
//---------------------------------------------------------------
// Procedure: getValueByIndex

double LogPlot::getValueByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_value[index]);
  return(0);
}
     
//---------------------------------------------------------------
// Procedure: getTimeByIndex

double LogPlot::getTimeByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_time[index]);
  return(0);
}
     
//---------------------------------------------------------------
// Procedure: getValueByTime
//      Note: If the argument, interp, is true, the function will
//            return an interpolated value if the given value falls
//            between two data points. The default is interp=false.

double LogPlot::getValueByTime(double gtime, bool interp) const
{
  unsigned int vsize = m_time.size();

  if(vsize == 0)
    return(0);

  if(gtime >= m_time[vsize-1])
    return(m_value[vsize-1]);

  if(gtime <= m_time[0])
    return(m_value[0]);

  unsigned int index = getIndexByTime(m_time, gtime);
  if((gtime == m_time[index]) || !interp)
    return(m_value[index]);
  
  double val1 = m_value[index];
  double val2 = m_value[index+1];
  
  double val_range  = val2 - val1;
  double time_range = m_time[index+1] - m_time[index];

  if(time_range <= 0)
    return(m_value[index]);

  double pct_time = (gtime - m_time[index]) / time_range;

  double rval = (pct_time * val_range) + val1;

  return(rval);
}
     
//---------------------------------------------------------------
// Procedure: getMedian
//   Purpose: Calculate the median value of all points added so far.

double LogPlot::getMedian() 
{
  // The median value is stored locally - only calculated it if it
  // has not been calculated already. Subsequent additions to the 
  // log will clear the median value;
  if(m_median_set)
    return(m_median);

  list<double> vlist;

  // First put all the log values in the list.
  unsigned int k, ksize = m_value.size();
  for(k=0; k<ksize; k++)
    vlist.push_back(m_value[k]);

  // Then sort them - ascending or descending does not matter.
  vlist.sort();

  // Increment an interator half way through the list and get
  // the value at that point as the median.
  unsigned int lsize = vlist.size();
  list<double>::iterator p = vlist.begin();
  for(k=0; k<(lsize/2); k++)
    p++;

  m_median     = *p;
  m_median_set = true;

  return(m_median);
}
     
//---------------------------------------------------------------
// Procedure: getMinTime

double LogPlot::getMinTime() const
{
  if(m_time.size() > 0)
    return(m_time[0]);
  return(0);
}

//---------------------------------------------------------------
// Procedure: getMaxTime

double LogPlot::getMaxTime() const
{
  if(m_time.size() > 0)
    return(m_time[m_time.size()-1]);
  return(0);
}

//---------------------------------------------------------------
// Procedure: print

void LogPlot::print() const
{
  unsigned int i;
  cout << "LogPlot::print()" << endl;
  cout << " Variable Name: " << m_varname << endl;
  for(i=0; i<m_time.size(); i++) {
    cout << "time:" << m_time[i] << "  val:" << m_value[i] << endl;
  }
}
