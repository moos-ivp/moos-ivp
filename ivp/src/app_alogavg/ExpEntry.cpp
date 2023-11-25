/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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

#include <iostream>
#include <cmath>
#include "MBUtils.h"
#include "ExpEntry.h"

using namespace std;

//--------------------------------------------------------
// Constructor()

ExpEntry::ExpEntry()
{
  m_xval = 0;
  m_yavg = 0;
  m_ymin = 0;
  m_ymax = 0;
  m_ystd = 0;
  m_processed = false;
}

//--------------------------------------------------------
// Procedure: addYVal()

void ExpEntry::addYVal(double y)
{
  m_yvals.push_back(y);
  m_processed = false;
}

//--------------------------------------------------------
// Procedure: process()

void ExpEntry::process()
{
  if((m_processed) || (m_yvals.size() == 0))
    return;
  setYAvg();
  setYMin();
  setYMax();
  setYStd();
  m_processed = true;
}

//--------------------------------------------------------
// Procedure: setYAvg()

void ExpEntry::setYAvg()
{
  double total = 0;
  for(unsigned int i=0; i<m_yvals.size(); i++)
    total += m_yvals[i];
  m_yavg = total / ((double)(m_yvals.size()));
}

//--------------------------------------------------------
// Procedure: setYMin()

void ExpEntry::setYMin()
{
  for(unsigned int i=0; i<m_yvals.size(); i++) {
    if((i==0) || (m_yvals[i] < m_ymin))
      m_ymin = m_yvals[i];
  }
}

//--------------------------------------------------------
// Procedure: setYMax()

void ExpEntry::setYMax()
{
  for(unsigned int i=0; i<m_yvals.size(); i++) {
    if((i==0) || (m_yvals[i] > m_ymax))
      m_ymax = m_yvals[i];
  }
}

//--------------------------------------------------------
// Procedure: setYStd()

void ExpEntry::setYStd()
{
  double avg = m_yavg;
  double total = 0;
  for(unsigned int i=0; i<m_yvals.size(); i++) {
    double delta = avg - m_yvals[i];
    total += (delta * delta);
  }

  double variance = total / ((double)(m_yvals.size()));
  double std_deviation = sqrt(variance);

  m_ystd = std_deviation;
}

