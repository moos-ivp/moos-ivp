/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_HeadingHysteresis.h                              */
/*    DATE: July 30th 2009                                       */
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
 
#ifndef BHV_HEADING_HYSTERESIS_HEADER
#define BHV_HEADING_HYSTERESIS_HEADER

#include <list>
#include "IvPBehavior.h"

class BHV_HeadingHysteresis : public IvPBehavior {
 public:
  BHV_HeadingHysteresis(IvPDomain);
  ~BHV_HeadingHysteresis() {}
  
  IvPFunction* onRunState();
  void         onIdleState();
  bool         setParam(std::string, std::string);
  
protected:
  std::string updateHeadingHistory();
  void        addHeadingEntry(double value, double time);
  double      getHeadingAverage();
  double      getHeadingVariance(double);

private: // Configuration Parameters
  double       m_memory_time;
  std::string  m_variable_name;

  //double       m_peakwidth;
  //double       m_basewidth;


private: // State Variables
  std::list<double> m_heading_val;
  std::list<double> m_heading_time;

  double m_current_heading;
};
#endif




















