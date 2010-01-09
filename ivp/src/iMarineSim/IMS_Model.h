/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IMS_Model.h                                          */
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

#ifndef IMS_MODEL_HEADER
#define IMS_MODEL_HEADER

#include <string>
#include "VState.h"
#include "MBTimer.h"
#include "SimEngine.h"
#include "AngleUtils.h"

class IMS_Model
{
public:
  IMS_Model();
  virtual ~IMS_Model() {};

  bool   propagate(double time);
  void   reset(double);

  double getPositionX() {return(m_vstate.m_dfX);};
  double getPositionY() {return(m_vstate.m_dfY);};
  double getForceX()    {return(m_force_x);};
  double getForceY()    {return(m_force_y);};
  double getSpeed()     {return(m_vstate.m_dfSpeed);};
  double getDepth()     {return(m_vstate.m_dfDepth);};
  double getHeading();
  double getYaw();

  // Get the total force factor in terms of direction and magnitude
  double getForceMagnitude();
  double getForceAngle();

  void   setForceVector(std::string, bool add=false);

  void   setRudder(double v)        {m_rudder = v;};
  void   setThrust(double v)        {m_thrust = v;};
  void   setElevator(double v)      {m_elevator = v;};
  void   setForceX(double v)        {m_force_x = v;};
  void   setForceY(double v)        {m_force_y = v;};
  void   addForceX(double v)        {m_force_x += v;};
  void   addForceY(double v)        {m_force_y += v;};
  void   setTorqueTheta(double v)   {m_torque_theta = v;};
  void   setFloatRate(double v)     {m_float_rate = v;};
  void   setDeceleration(double v)  {m_deceleration = v;};
  void   setPaused(bool); 
  void   setThrustFactor(double v)  {m_thrust_factor = v;};

  void   setPositionX(double v)     {m_vstate.m_dfX = v;};
  void   setPositionY(double v)     {m_vstate.m_dfY = v;};
  void   setHeadingRAD(double v)    {m_vstate.m_dfHeading = v;};
  void   setHeadingDEG(double v)   
    {m_vstate.m_dfHeading = degToRadians(v);};
  void   setSpeed(double v)         {m_vstate.m_dfSpeed = v;};
  void   setDepth(double v)         {m_vstate.m_dfDepth = v;};
  void   setPosition(std::string);
  void   setPositionPairs(std::string);

  void   printSummary();

protected:
  bool      m_paused;
  double    m_rudder;
  double    m_thrust;
  double    m_elevator;
  double    m_force_x;           // meters per sec
  double    m_force_y;           // meters per sec
  double    m_torque_theta;
  double    m_float_rate;
  double    m_deceleration;
  double    m_thrust_factor;
  VState    m_vstate; // with no noise
  MBTimer   m_pause_timer;
  SimEngine m_sim_engine;
};
#endif
