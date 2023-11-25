/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_PeriodicSurface.h                                */
/*    DATE: Feb 21st 2007                                        */
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
 
#ifndef BHV_PERIODIC_SURFACE_ADVANCED_HEADER
#define BHV_PERIODIC_SURFACE_ADVANCED_HEADER

#include <string>
#include "IvPBehavior.h"

class IvPDomain;
class BHV_PeriodicSurface : public IvPBehavior {
public:
  BHV_PeriodicSurface(IvPDomain);
  ~BHV_PeriodicSurface() {}
  
  bool         setParam(std::string, std::string);
  IvPFunction* onRunState();
  void         onIdleState();

protected:
  bool         updateInfoIn();  
  void         noteTimeAtSurface();
  double       setDesiredSpeed();
  bool         checkForMarking();
  void         checkForAcommsMarking();

private: // Behavior Parameters
  std::string m_pending_status_var;
  std::string m_atsurface_status_var;
  std::string m_mark_variable;
  std::string m_acomms_mark_variable;
  double      m_acomms_mark_interval;
  double      m_acomms_mark_max_time;
  double      m_period;
  double      m_zero_speed_depth;
  double      m_max_time_at_surface;
  double      m_time_at_surface;
  double      m_ascent_speed;
  std::string m_ascent_grade;

private: // Behavior State Variables

  // On the first iteration we pretend as if we just received a 
  // mark message. If there was no actual mark message posted at
  // the outset, the m_first_marking_xxx variables remain true so
  // the first mark message eventually received is not required 
  // to be "unique" to be counted as a new marking.
  bool        m_first_iteration;        
  bool        m_first_marking_string;   
  bool        m_first_marking_double;

  // The noted depth at the moment we transition from a non-ascent
  // state into an ascent state.
  double      m_depth_at_ascent_begin;

  // The noted speed at the moment we transition from a non-ascent
  // state into an ascent state.
  double      m_speed_at_ascent_begin;  

  // The time at which the last unique mark message (GPS) received.
  double      m_mark_time;

  // The value of the last mark message. Stored to detect that an
  // incoming mark message is new. Store string and double since
  // we don't know a priori the mark message type.
  std::string m_curr_mark_string_val;
  double      m_curr_mark_double_val;

  std::string m_curr_amark_string_val;
  double      m_amark_extension_time;

  // The variable name posted (var==true) when first entering 
  // the ascending state
  std::string m_ascending_flag;

  // Time at which the vehicle reached the surface, zero_speed_depth
  double      m_surface_mark_time;
 
  bool        m_at_surface;
 
  double      m_curr_depth;
  double      m_curr_speed;
  double      m_curr_time;

  std::string m_state;


};
#endif













