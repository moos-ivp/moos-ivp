/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TStamp.h                                             */
/*    DATE: March 18th, 2020                                     */
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

#ifndef TSTAMP_HEADER
#define TSTAMP_HEADER

#include <string>

class TStamp {

public:
  TStamp();
  ~TStamp() {};

  bool  setTime(unsigned int hour, unsigned int min, double sec);
  bool  setTimeStr(std::string);

  std::string getTimeStr() const;
  bool  valid() const;

 protected:
  unsigned int   m_hour;    // Hour in 24-hour clock (valid is 0 to 23)
  unsigned int   m_min;     // Minute must be 0 to 59
  double         m_sec;     // Second must be > 0.0 and < 60.0
};
#endif

