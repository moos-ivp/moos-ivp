/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng                               */
/*    FILE: ExpEntry.cpp                                         */
/*    DATE: December 15th, 2021                                  */
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

#include <cmath>
#include "MBUtils.h"
#include "ExpEntry.h"

using namespace std;

//--------------------------------------------------------
// Constructor

ExpEntry::ExpEntry()
{
  m_xval = 0;
  
  m_yavg = 0;
  m_ymin = 0;
  m_ymax = 0;
  m_ystd = 0;

  m_yavg_set = false;
  m_ymin_set = false;
  m_ymax_set = false;
  m_ystd_set = false;
}

//--------------------------------------------------------
// Procedure: addYVal()

void ExpEntry::addYVal(double y)
{
  m_yvals.push_back(y);

  m_yavg_set = false;
  m_ymin_set = false;
  m_ymax_set = false;
  m_ystd_set = false;
}


//--------------------------------------------------------
// Procedure: getYAvg()

double ExpEntry::getYAvg()
{
  if(m_yavg_set)
    return(m_yavg);
  
  double total = 0;
  for(unsigned int i=0; i<m_yvals.size(); i++)
    total += m_yvals[i];

  double avg = total / ((double)(m_yvals.size()));

  m_yavg = avg;
  m_yavg_set = true;
  return(avg);  
}

//--------------------------------------------------------
// Procedure: getYMin()

double ExpEntry::getYMin()
{
  if(m_ymin_set)
    return(m_ymin);
  
  double minval = 0;
  for(unsigned int i=0; i<m_yvals.size(); i++) {
    if((i==0) || (m_yvals[i] < minval))
      minval = m_yvals[i];
  }

  m_ymin = minval;
  m_ymin_set = true;
  return(minval);  
}


//--------------------------------------------------------
// Procedure: getYMax()

double ExpEntry::getYMax()
{
  if(m_ymax_set)
    return(m_ymax);
  
  double maxval = 0;
  for(unsigned int i=0; i<m_yvals.size(); i++) {
    if((i==0) || (m_yvals[i] > maxval))
      maxval = m_yvals[i];
  }

  m_ymax = maxval;
  m_ymax_set = true;
  return(maxval);  
}


//--------------------------------------------------------
// Procedure: getYStd()

double ExpEntry::getYStd()
{
  if(m_ystd_set)
    return(m_ystd);

  double avg = getYAvg();
  
  double total = 0;
  for(unsigned int i=0; i<m_yvals.size(); i++) {
    double delta = avg - m_yvals[i];
    total += (delta * delta);
  }

  double variance = total / ((double)(m_yvals.size()));

  double std_deviation = sqrt(variance);

  m_ystd = std_deviation;
  m_ystd_set = true;
  return(std_deviation);  
}


