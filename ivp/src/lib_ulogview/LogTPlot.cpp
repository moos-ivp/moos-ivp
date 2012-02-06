/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogTPlot.cpp                                         */
/*    DATE: May3105, Sep2811                                     */
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

#include <list>
#include <iostream>
#include "LogTPlot.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

LogTPlot::LogTPlot() : TimePlot()
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


bool LogTPlot::setValue(double gtime, double gval, bool recalc_median)
{
  unsigned int tsize = m_time.size();

  if((tsize == 0) || (m_time[tsize-1] <= gtime)) {
    m_time.push_back(gtime);
    m_value.push_back(gval);

    if((tsize == 0) || (gval > m_max_val))
      m_max_val = gval;
    if((tsize == 0) || (gval < m_min_val))
      m_min_val = gval;

    if(!recalc_median)
      m_median_set = false;
    else
      resetMedian();

    return(true);
  }
  else
    return(false);
}

//---------------------------------------------------------------
// Procedure: resetMedian
//   Purpose: Force a reset of the median calculation regardless of
//            whether there is presently a cached value.

void LogTPlot::resetMedian()
{
  m_median = getMedian();
  m_median_set = true;
}
     
//---------------------------------------------------------------
// Procedure: getMedian
//   Purpose: Calculate the median value of all points added so far.
//            Perhaps use cached value if it exists and if a forced
//            reset has not been requested.

double LogTPlot::getMedian() const
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

  return(m_median);
}
     
//---------------------------------------------------------------
// Procedure: getValueByIndex

double LogTPlot::getValueByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_value[index]);
  else
    return(0);
}
     
//---------------------------------------------------------------
// Procedure: getValueByTime
//      Note: If the argument, interp, is true, the function will
//            return an interpolated value if the given value falls
//            between two data points. The default is interp=false.

double LogTPlot::getValueByTime(double gtime, bool interp) const
{
  unsigned int vsize = m_time.size();

  if(vsize == 0)
    return(0);

  if(gtime >= m_time[vsize-1])
    return(m_value[vsize-1]);

  if(gtime <= m_time[0])
    return(m_value[0]);

  unsigned int index = getIndexByTime(gtime);
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
// Procedure: getValueByTimeUTC

double LogTPlot::getValueByTimeUTC(double utc_time, bool interp) const
{
  double local_time = utc_time - m_logstart_utc;
  return(getValueByTime(local_time, interp));
}
     

//---------------------------------------------------------------
// Procedure: print

void LogTPlot::print() const
{
  unsigned int i;
  cout << "LogTPlot::print()" << endl;
  cout << " Variable Name: " << m_varname << endl;
  cout << " Vehicle  Name: " << m_vname << endl;
  for(i=0; i<m_time.size(); i++) {
    cout << "time:" << m_time[i] << "  val:" << m_value[i] << endl;
  }
}







