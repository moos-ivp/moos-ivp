/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_Hysteresis.h                                     */
/*    DATE: August 4th 2008 (aboard the Alliance)                */
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
 
#ifndef BHV_HYSTERESIS_HEADER
#define BHV_HYSTERESIS_HEADER

#include <list>
#include "IvPBehavior.h"

class BHV_Hysteresis : public IvPBehavior {
public:
  BHV_Hysteresis(IvPDomain);
  ~BHV_Hysteresis() {}
  
  IvPFunction* onRunState();
  void         onIdleState();
  bool         setParam(std::string, std::string);

protected:
  std::string updateHeadingAvg();
  void        addHeading(double, double);
  double      getHeadingAverage();
  double      getHeadingVariance(double);


private: // Configuration Parameters

  double m_memory_time;
  double m_turn_range;

private: // State Variables

  std::list<double> m_heading_val;
  std::list<double> m_heading_time;

  double m_current_heading;
  double m_current_speed;

  double m_min_heading_window;
  double m_max_heading_window;
  double m_max_window_utility;

  //double m_peakwidth;
  //double m_basewidth;
};
#endif



















