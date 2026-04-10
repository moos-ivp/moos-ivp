/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PMGen.cpp                                            */
/*    DATE: Jan 1st, 2024                                        */
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

#include "AngleUtils.h"
#include "PMGen.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: setHdgHist()

void PMGen::setHdgHist(std::list<double> hdgs, std::list<double> utcs)
{
  m_recent_val_osh=hdgs;
  m_recent_utc_osh=utcs;  
}


//----------------------------------------------------------------
// Procedure: getHdgAvg()

double PMGen::getHdgAvg(double time_window)
{
  list<double> hdgs = m_recent_val_osh;
  list<double> utcs = m_recent_utc_osh;

  if(hdgs.size() != utcs.size())
    return(-1);

  if(hdgs.size() == 0)
    return(-1);

  double prev_hdg = hdgs.front();
  double most_recent_utc = utcs.front();
  hdgs.pop_front();
  utcs.pop_front();

  double total_delta_hdg = 0;
  unsigned int count = 0;

  bool done = false;
  while(!done) {
    if(hdgs.size() == 0)
      break;

    double hdg = hdgs.front();
    double utc = utcs.front();
    double elapsed = most_recent_utc - utc;
    if(elapsed > time_window)
      done = true;
    else {
      // Will return smaller of two deltas given possible ang wrap
      double delta_hdg = angleDiff(hdg, prev_hdg);
      // If port turn then delta is negative, otherwise stays positive
      if(portTurn(hdg, prev_hdg))
	delta_hdg = -delta_hdg;
      
      total_delta_hdg += delta_hdg;
      prev_hdg = hdg;
      count++;
    }
      
    hdgs.pop_front();
    utcs.pop_front();
  }

  if(count == 0)
    return(0);

  double avg = total_delta_hdg / (double)(count);
  return(avg);
}
