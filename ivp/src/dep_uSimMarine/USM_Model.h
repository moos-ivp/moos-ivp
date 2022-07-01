/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: USM_Model.h                                          */
/*    DATE: Nov 19th 2006 (as separate class under MVC paradigm) */
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
  virtual  ~USM_Model() {}

  bool   propagate(double time);
  void   resetTime(double time);
  
  // Setters
  bool   setParam(std::string, double);

  void   setRudder(double v)          {m_rudder = v; m_thrust_mode="normal";}
  void   setRudder(double, double);
  void   setThrust(double v)          {m_thrust = v; m_thrust_mode="normal";}
  void   setElevator(double v)        {m_elevator = v;}
  void   setDualState(bool v)         {m_dual_state = v;} 
  void   setDriftFresh(bool v)        {m_drift_fresh = v;} 
  void   setThrustModeReverse(bool v) {m_thrust_mode_reverse=v;}
  void   setThrustModeDiff(std::string s) {m_thrust_mode=s;}
  bool   setMaxRudderDegreesPerSec(double);
  void   setPaused(bool); 
  void   setThrustFactor(double);
  void   setThrustReflect(bool);

  void   setThrustLeft(double);
  void   setThrustRight(double);

  bool   setDriftVector(std::string, bool add=false);
  void   magDriftVector(double);

  bool   initPosition(const std::string&);
  bool   addThrustMapping(double, double);

  // Getters
  double     getThrust() const       {return(m_thrust);}
  double     getThrustLeft() const   {return(m_thrust_lft);}
  double     getThrustRight() const  {return(m_thrust_rgt);}
  double     getDriftX() const       {return(m_drift_x);}
  double     getDriftY() const       {return(m_drift_y);}
  double     getDriftMag() const;
  double     getDriftAng() const;
  double     getRotateSpd() const    {return(m_rotate_speed);}
  double     getWaterDepth() const   {return(m_water_depth);}
  bool       usingDualState() const  {return(m_dual_state);}
  bool       isDriftFresh() const    {return(m_drift_fresh);} 
  NodeRecord getNodeRecord() const   {return(m_record);}
  NodeRecord getNodeRecordGT() const {return(m_record_gt);}
  double     getBuoyancyRate() const {return(m_buoyancy_rate);}
  bool       usingThrustFactor() const;
  double     getThrustFactor() const;
  double     getMaxDepthRate() const    {return(m_max_depth_rate);}
  double     getMaxDepthRateSpd() const {return(m_max_depth_rate_speed);}
  double     getMaxAcceleration() const {return(m_max_acceleration);}
  double     getMaxDeceleration() const {return(m_max_deceleration);}

  bool       getThrustModeReverse() const {return(m_thrust_mode_reverse);}

  std::string getThrustMapPos() const {return(m_thrust_map.getMapPos());}
  std::string getThrustMapNeg() const {return(m_thrust_map.getMapNeg());}
  std::string getDriftSummary();
  std::string getThrustModeDiff() const {return(m_thrust_mode);}

 protected:
  void   propagateNodeRecord(NodeRecord&, double delta_time, bool);

 protected:
  double     m_rudder;
  double     m_rudder_prev;
  double     m_rudder_tstamp;
  double     m_max_rudder_degs_per_sec;

  bool       m_paused;
  double     m_thrust;
  double     m_elevator;

  double     m_water_depth;

  double     m_thrust_lft;
  double     m_thrust_rgt;

  double     m_turn_rate;         // turning characteristic of the vehicle
  double     m_rotate_speed;      // External rotational drift force
  double     m_drift_x;           // meters per sec
  double     m_drift_y;           // meters per sec
  double     m_buoyancy_rate;
  double     m_max_depth_rate;
  double     m_max_depth_rate_speed;
  double     m_max_deceleration;
  double     m_max_acceleration;

  NodeRecord m_record;       // NAV_X, NAV_Y           
  NodeRecord m_record_gt;    // NAV_GT_X, NAV_GT_Y   

  bool       m_dual_state;   
  bool       m_drift_fresh;

  bool       m_thrust_mode_reverse;

  MBTimer    m_pause_timer;
  SimEngine  m_sim_engine;

  ThrustMap  m_thrust_map;

  std::string m_thrust_mode;

};
#endif




