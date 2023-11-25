/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TStamp.cpp                                           */
/*    DATE: March 20th, 2020                                     */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <cstdlib>
#include "MBUtils.h"
#include "TStamp.h"

using namespace std;

//---------------------------------------------------------
// Constructor 

TStamp::TStamp()
{
  m_hour = 0;
  m_min  = 0;
  m_sec  = 0.0;
}

//---------------------------------------------------------
// Procedure: setTime()

bool TStamp::setTime(unsigned int hour, unsigned int min, double sec)
{
  if((hour > 23) || (min > 59) || (sec >= 60))
    return(false);

  m_hour = hour;
  m_min  = min;
  m_sec  = sec;
  return(true);
}

//---------------------------------------------------------
// Procedure: setTimeStr()
//     Notes: hhmmss
//            hhmmss.
//            hhmmss.s
//            hhmmss.ssssss

bool TStamp::setTimeStr(string sval)
{
  // Part 1: Sanity checks
  if(sval.length() < 6)
    return(false);
  if((sval.length() > 6) && (sval[6] != '.'))
    return(false);

  // Part 2: Handle the whole number part
  string hr  = sval.substr(0,2);
  string min = sval.substr(2,2);
  string sec = sval.substr(4,2);
  if(!isNumber(hr) || !isNumber(min) || !isNumber(sec))
    return(false);

  int    int_hr  = atoi(hr.c_str());
  int    int_min = atoi(min.c_str());
  double dbl_sec = atof(sec.c_str());
  if((int_hr < 0) || (int_min < 0) || (dbl_sec < 0))
    return(false);
  
  // Part 3: Handle the partial seconds part
  double dbl_fsecs = 0;
  if(sval.length() > 7) {
    string fsecs = "0." + sval.substr(7);
    if(!isNumber(fsecs))
      return(false);
    dbl_fsecs = atof(fsecs.c_str());
  }
  dbl_sec += dbl_fsecs;

  bool ok = setTime(int_hr, int_min, dbl_sec);
  
  return(ok);
}

//---------------------------------------------------------
// Procedure: getTimeStr()

string TStamp::getTimeStr() const
{
  string str_hr = uintToString(m_hour);
  if(m_hour < 10)
    str_hr = "0" + str_hr;
  
  string str_min = uintToString(m_min);
  if(m_min < 10)
    str_min = "0" + str_min;

  string str_sec = doubleToStringX(m_sec,3);
  if(m_sec < 10)
    str_sec = "0" + str_sec;
  
  string str = str_hr + str_min + str_sec;

  return(str);
}

//---------------------------------------------------------
// Procedure: valid()

bool TStamp::valid() const
{
  if((m_hour == 0) && (m_min == 0) && (m_sec == 0))
    return(false);

  return(true);
}

