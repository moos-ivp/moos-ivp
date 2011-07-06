/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: USM_Model.h                                          */
/*    DATE: Nov 19th 2006 (as separate class under MVC paradigm) */
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

#ifndef USM_MODEL_HEADER
#define USM_MODEL_HEADER

#include <string>
#include "NodeRecord.h"
#include "MBTimer.h"
#include "SimEngine.h"
#include "ThrustMap.h"

class USM_Model
{
public:
  USM_Model();
  virtual  ~USM_Model() {};

  bool   propagate(double time);
  void   resetTime(double time);
  
  // Setters
  bool   setParam(std::string, double);

  void   setRudder(double v)        {m_rudder = v;};
  void   setThrust(double v)        {m_thrust = v;};
  void   setElevator(double v)      {m_elevator = v;};
  void   setDualState(bool v)       {m_dual_state = v;}; 
  void   setForceFresh(bool v)      {m_force_fresh = v;}; 
  void   setPaused(bool); 
  void   setThrustFactor(double);
  void   setThrustReflect(bool);

  void   setForceVector(std::string, bool add=false);
  void   magForceVector(double);

  bool   initPosition(const std::string&);
  bool   addThrustMapping(double, double);

  // Getters
  double     getForceX()       {return(m_force_x);};
  double     getForceY()       {return(m_force_y);};
  bool       usingDualState()  {return(m_dual_state);}
  bool       isForceFresh()    {return(m_force_fresh);}; 
  NodeRecord getNodeRecord()   {return(m_record);};
  NodeRecord getNodeRecordGT() {return(m_record_gt);};

  std::string getForceSummary();

 protected:
  void   propagateNodeRecord(NodeRecord&, double delta_time, bool);

 protected:
  bool       m_paused;
  double     m_rudder;
  double     m_thrust;
  double     m_elevator;

  double     m_turn_rate;
  double     m_torque_theta;
  double     m_force_x;           // meters per sec
  double     m_force_y;           // meters per sec
  double     m_buoyancy_rate;
  double     m_max_depth_rate;
  double     m_max_depth_rate_speed;
  double     m_max_deceleration;
  double     m_max_acceleration;

  NodeRecord m_record;       // NAV_X, NAV_Y           
  NodeRecord m_record_gt;    // NAV_GT_X, NAV_GT_Y   

  bool       m_dual_state;   
  bool       m_force_fresh;

  MBTimer    m_pause_timer;
  SimEngine  m_sim_engine;

  ThrustMap  m_thrust_map;
};
#endif

