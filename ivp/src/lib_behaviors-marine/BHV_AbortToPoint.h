/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_AbortToPoint.h                                   */
/*    DATE: August 6th 2008                                      */
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
 
#ifndef BHV_ABORT_TO_POINT_HEADER
#define BHV_ABORT_TO_POINT_HEADER

#include <string>
#include "IvPBehavior.h"
#include "WaypointEngine.h"

class BHV_AbortToPoint : public IvPBehavior {
public:
  BHV_AbortToPoint(IvPDomain);
  ~BHV_AbortToPoint() {}
  
  bool         setParam(std::string, std::string);
  void         onIdleState();
  IvPFunction* onRunState();

protected:
  bool         updateInfoIn();
  IvPFunction* buildOF(std::string);
  void         updateInfoOut();

protected: // configuration parameters
  double      m_cruise_speed;
  std::string m_ipf_type;

protected: // intermediate or object global variables.
  double    m_osv;  // Ownship velocity
  double    m_osx;  // Ownship x position
  double    m_osy;  // Ownship y position
  double    m_abort_x;
  double    m_abort_y;
  
  double    m_capture_radius;
  double    m_nm_radius;

  std::string m_status_var;
  std::string m_state;
  bool        m_sent_msg_inactive;
  bool        m_sent_msg_completed;
  bool        m_sent_msg_abandoned;
  
};
#endif




















