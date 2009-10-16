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
  void         onIdleState();
  void         onIdleToRunState();
  bool         setParam(std::string, std::string);
  std::string  onSetParamComplete();

 protected:
  void         updateInfoIn();
  void         postStatusReport();

private: // Configuration Parameters

  double  m_period_lazy;
  double  m_period_busy;
  double  m_period_speed;
  double  m_zaic_basewidth;
  double  m_zaic_peakwidth;
  double  m_zaic_summit_delta;
  bool    m_reset_upon_running;

  std::string m_var_busy_count;
  std::string m_var_pending_busy;
  std::string m_var_pending_lazy;

private: // State Variables
  bool    m_mode_busy;
  bool    m_mark_needed;
  double  m_mark_time;
  int     m_busy_count;

  double  m_time_to_busy;
  double  m_time_to_lazy;
};
#endif










