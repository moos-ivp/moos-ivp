/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_PeriodicSpeed.h                                  */
/*    DATE: June 9th 2006                                        */
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
 
#ifndef BHV_PERIODIC_SPEED_HEADER
#define BHV_PERIODIC_SPEED_HEADER

#include <string>
#include "IvPBehavior.h"

class IvPDomain;
class BHV_PeriodicSpeed : public IvPBehavior {
public:
  BHV_PeriodicSpeed(IvPDomain);
  ~BHV_PeriodicSpeed() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);


private: // Configuration Parameters

  double  m_period_gap;
  double  m_period_length;
  double  m_period_speed;
  double  m_period_basewidth;
  double  m_period_peakwidth;

  std::string m_period_start_flag;

  std::string m_var_pending_active;
  std::string m_var_pending_inactive;


private: // State Variables
  bool    m_state_active;
  bool    m_first_iteration;
  double  m_mark_time;

  std::string m_period_start_value;

};
#endif










