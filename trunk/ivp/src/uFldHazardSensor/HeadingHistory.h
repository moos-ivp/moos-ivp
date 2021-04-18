/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HeadingHistory.h                                     */
/*    DATE: Jan 19th, 2013                                       */
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

#ifndef HEADING_HISTORY_HEADER
#define HEADING_HISTORY_HEADER

#include <list>

class HeadingHistory {
 public:
  HeadingHistory();
  virtual ~HeadingHistory() {}

  bool   addHeadingTime(double heading, double time);
  double getTurnRate(double time_interval) const;

 private:
  std::list<double>  m_headings;
  std::list<double>  m_timestamps;
  
  unsigned int m_history_amt;
  unsigned int m_history_time;

};

#endif 







