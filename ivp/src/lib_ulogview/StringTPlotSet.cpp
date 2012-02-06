/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: StringTPlot.cpp                                      */
/*    DATE: Oct0711                                              */
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
#include "StringTPlot.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

StringTPlot::StringTPlot() : TimePlot()
{
  m_max_strlen = 0;
}


//---------------------------------------------------------------
// Procedure: setValue
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.

bool StringTPlot::setValue(double gtime, const string& gval)
{
  unsigned int tsize = m_time.size();

  if((tsize == 0) || (m_time[tsize-1] <= gtime)) {
    m_time.push_back(gtime);
    m_value.push_back(gval);

    unsigned int len = gval.length();
    if(len > m_max_strlen)
      m_max_strlen = len;

    return(true);
  }
  else
    return(false);
}


//---------------------------------------------------------------
// Procedure: getValueByIndex

string StringTPlot::getValueByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_value[index]);
  else
    return("");
}
     
//---------------------------------------------------------------
// Procedure: getValueByTime

string StringTPlot::getValueByTime(double gtime) const
{
  unsigned int vsize = m_time.size();
  if(vsize == 0)
    return("");

  unsigned int index = getIndexByTime(gtime);
  return(m_value[index]);
}
     
//---------------------------------------------------------------
// Procedure: getValueByTimeUTC

string StringTPlot::getValueByTimeUTC(double utc_time) const
{
  double local_time = utc_time - m_logstart_utc;
  return(getValueByTime(local_time));
}
     

//---------------------------------------------------------------
// Procedure: print

void StringTPlot::print() const
{
  unsigned int i;
  cout << "StringTPlot::print()" << endl;
  cout << " Vehicle  Name: " << m_vname << endl;
  cout << " Variable Name: " << m_varname << endl;
  cout << " Max Length:    " << m_max_strlen << endl;
  for(i=0; i<m_time.size(); i++) {
    cout << "time:" << m_time[i] << "  val:" << m_value[i] << endl;
  }
}







