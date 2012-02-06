/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: TimePlot.h                                           */
/*    DATE: Sep 27th, 2011                                       */
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

#ifndef TIME_PLOT_HEADER
#define TIME_PLOT_HEADER

#include <vector>

class TimePlot
{
 public:
  TimePlot() {m_logstart_utc=0;};
  ~TimePlot() {};
  
 public: // Initialization
  void         setVName(std::string s)   {m_vname = s;};
  void         setLogStartUTC(double v)  {m_logstart_utc=v;};

 public: // Querying
  double       getTimeByIndex(unsigned int index) const;
  double       getTimeUTCByIndex(unsigned int index) const;

  unsigned int getIndexByTime(double time) const;
  unsigned int getIndexByTimeUTC(double time) const;

  double       getMinTime() const;
  double       getMinTimeUTC() const;

  double       getMaxTime() const;
  double       getMaxTimeUTC() const;

  double       getLogStartUTC() const {return(m_logstart_utc);};
  std::string  getVName() const       {return(m_vname);};
  unsigned int size() const           {return(m_time.size());};

  bool         containsTime(double) const;
  bool         containsTimeUTC(double) const;

protected:
  std::string         m_vname;       // Name of the platform
  std::vector<double> m_time;        // Times stored localDB time

  double              m_logstart_utc;
};
#endif 
