/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_HeadingChange.h                                  */
/*    DATE: Feb 11th 2009                                        */
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
 
#ifndef BHV_HEADING_CHANGE_HEADER
#define BHV_HEADING_CHANGE_HEADER

#include "IvPBehavior.h"

class BHV_HeadingChange : public IvPBehavior {
public:
  BHV_HeadingChange(IvPDomain);
  ~BHV_HeadingChange() {}
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onIdleState();

protected:
  void         postTrend();
  bool         checkForTurnCompletion();

 protected: // Configuration parameters
  double       m_heading_delta;
  double       m_turn_speed;
  std::string  m_turn_type;

 protected: // state variables
  double       m_prev_heading;
  double       m_start_heading;
  double       m_start_xpos;
  double       m_start_ypos;
  double       m_start_des_speed;
  double       m_heading_goal;
  std::string  m_state;
  bool         m_sign_positive;

  double       m_curr_time;
  double       m_curr_heading;
  double       m_curr_speed;
  double       m_curr_des_speed;
  double       m_curr_xpos;
  double       m_curr_ypos;

  int          m_completions;

  double       m_turn_time_commenced;

  std::vector<double> m_acc_turn_time;
  std::vector<double> m_acc_heading_delta;
  
  std::vector<double> m_turn_xpos;
  std::vector<double> m_turn_ypos;

};
#endif









