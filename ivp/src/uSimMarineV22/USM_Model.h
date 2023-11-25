/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include <set>
#include "NodeRecord.h"
#include "MBTimer.h"
#include "SimEngine.h"
#include "WindModel.h"
#include "PolarPlot.h"
#include "TurnSpeedMap.h"
#include "ThrustMap.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"

class USM_Model
{
public:
  USM_Model();
  virtual  ~USM_Model() {}

  bool   propagate(double time);
  void   resetTime(double time);
  
  // Setters
  bool   setParam(std::string, double);
  bool   setParam(std::string, std::string);

  void   informX(double);
  void   informY(double);
  void   informHeading(double);
  
  void   setRudder(double v);
  void   setRudder(double, double);
  void   setThrust(double v);
  void   setElevator(double v)        {m_elevator = v;}
  void   setDriftFresh(bool v)        {m_drift_fresh = v;} 

  bool   setTSMapFullRate(std::string);
  bool   setTSMapNullRate(std::string);
  bool   setTSMapFullSpeed(std::string);
  bool   setTSMapNullSpeed(std::string);
  
  bool   setThrustModeReverse(std::string);
  bool   setThrustModeDiff(std::string);
  bool   setDualState(std::string);
  bool   setMaxRudderDegreesPerSec(double);
  bool   setPaused(std::string); 
  bool   setThrustReflect(std::string);
  void   setThrustFactor(double);
  bool   setTurnSpdLoss(double);

  void   setThrustLeft(double);
  void   setThrustRight(double);

  bool   setDriftX(double val, std::string src);
  bool   setDriftY(double val, std::string src);
  bool   setDriftVector(std::string, std::string, bool add=false);
  void   magDriftVector(double, std::string src);

  bool   initPosition(std::string);
  bool   addThrustMapping(double, double);
  bool   handleFullThrustMapping(std::string);
  
  void   setGeodesy(CMOOSGeodesy);
  void   setObstacleHit(bool v=true) {m_obstacle_hit=v;}
  
  // Getters
  double     getTurnRate() const     {return(m_turn_rate);}
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
  bool       geoOK() const {return(m_geo_ok);}

  unsigned int getResetCount() const {return(m_reset_count);}
  bool       getThrustModeReverse() const {return(m_thrust_mode_reverse);}
  void       cacheStartingInfo();
  
  std::string getThrustMapPos() const {return(m_thrust_map.getMapPos());}
  std::string getThrustMapNeg() const {return(m_thrust_map.getMapNeg());}
  std::string getDriftSummary();
  std::string getThrustModeDiff() const {return(m_thrust_mode);}

  std::string getDriftSources() const;

  bool        sailingEnabled() const;
  std::string getWindArrowSpec() const;
  std::string getWindModelSpec() const;
  std::string getPolarPlotSpec() const;

  
  // Getters for Start Info Cache
  std::string getStartNavX() const     {return(m_start_nav_x);}
  std::string getStartNavY() const     {return(m_start_nav_y);}
  std::string getStartNavHdg() const   {return(m_start_nav_hdg);}
  std::string getStartNavSpd() const   {return(m_start_nav_spd);}
  std::string getStartNavDep() const   {return(m_start_nav_dep);}
  std::string getStartNavAlt() const   {return(m_start_nav_alt);}
  std::string getStartNavLat() const   {return(m_start_nav_lat);}
  std::string getStartNavLon() const   {return(m_start_nav_lon);}
  std::string getStartBuoyancy() const {return(m_start_buoyrate);}
  std::string getStartDriftX() const   {return(m_start_drift_x);}
  std::string getStartDriftY() const   {return(m_start_drift_y);}
  std::string getStartDriftMag() const {return(m_start_drift_mag);}
  std::string getStartDriftAng() const {return(m_start_drift_ang);}
  std::string getStartRotateSpd() const {return(m_start_rotate_spd);}
  std::string getStartDatumLat() const {return(m_start_datum_lat);}
  std::string getStartDatumLon() const {return(m_start_datum_lon);}

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

  std::set<std::string> m_drift_sources;
  
  bool       m_thrust_mode_reverse;
  unsigned int m_reset_count;

  MBTimer    m_pause_timer;
  SimEngine  m_sim_engine;

  ThrustMap  m_thrust_map;

  std::string m_thrust_mode;
  
  CMOOSGeodesy m_geodesy;
  bool         m_geo_ok;
  bool         m_obstacle_hit;
  
  // Support for simulated sailing
  WindModel  m_wind_model;
  PolarPlot  m_polar_plot;
  bool       m_sailing;
  
  // A cache of starting info to facilitate generation of reports.
  std::string m_start_nav_x;
  std::string m_start_nav_y;
  std::string m_start_nav_hdg;
  std::string m_start_nav_spd;
  std::string m_start_nav_dep;
  std::string m_start_nav_alt;
  std::string m_start_nav_lat;
  std::string m_start_nav_lon;
  std::string m_start_buoyrate;
  std::string m_start_drift_x;
  std::string m_start_drift_y;
  std::string m_start_drift_mag;
  std::string m_start_drift_ang;
  std::string m_start_rotate_spd;
  std::string m_start_datum_lat;
  std::string m_start_datum_lon;
  
  TurnSpeedMap m_turn_speed_map;
};
#endif










