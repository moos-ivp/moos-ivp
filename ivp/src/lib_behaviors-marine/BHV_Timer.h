/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_Timer.h                                          */
/*    DATE: July 21st 2005                                       */
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
 
#ifndef BHV_TIMER_HEADER
#define BHV_TIMER_HEADER

#include "IvPBehavior.h"

class BHV_Timer : public IvPBehavior {
public:
  BHV_Timer(IvPDomain);
  ~BHV_Timer() {}
  
  IvPFunction* onRunState();
  void         onIdleState();
  bool         setParam(std::string, std::string);

 protected:
  std::string  m_var_status_idle;
  std::string  m_var_status_running;
  std::string  m_var_status_suffix;
};

#endif




