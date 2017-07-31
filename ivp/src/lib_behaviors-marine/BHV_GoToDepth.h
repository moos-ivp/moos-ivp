/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_GoToDepth.h                                      */
/*    DATE: Aug 1 2006                                           */
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
 
#ifndef BHV_GOTODEPTH_HEADER
#define BHV_GOTODEPTH_HEADER

#include <string>
#include <vector>
#include "IvPBehavior.h"

class BHV_GoToDepth : public IvPBehavior {
public:
  BHV_GoToDepth(IvPDomain);
  ~BHV_GoToDepth() {}
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

protected:
  bool setNextLevelDepth();
  bool incrementLevelDepth();

protected:  // configuration parameters
  int       m_repeat;
  double    m_basewidth;
  double    m_peakwidth;
  double    m_arrival_delta;

  std::string         m_arrival_flag;
  std::vector<double> m_level_depths;
  std::vector<double> m_level_times;

protected:  // local state variables
  unsigned int  m_curr_index;
  unsigned int  m_arrivals;

  double    m_prior_vehicle_depth;
  double    m_plateau_start_time;
  bool      m_plateau_mode;
  bool      m_first_iteration;
};
#endif












