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
  void   handleCommand(char);
  void   reset(double);

  double getPositionX() {return(m_vstate.m_dfX);};
  double getPositionY() {return(m_vstate.m_dfY);};
  double getHeading();
  double getSpeed()     {return(m_vstate.m_dfSpeed);};
  double getDepth()     {return(m_vstate.m_dfDepth);};
  double getYaw();

  void   setRudder(double v)      {m_rudder = v;};
  void   setThrust(double v)      {m_thrust = v;};
  void   setElevator(double v)    {m_elevator = v;};
  void   setPushX(double v)       {m_push_x = v;};
  void   setPushY(double v)       {m_push_y = v;};
  void   setPushTheta(double v)   {m_push_theta = v;};
  void   setFloatRate(double v)   {m_float_rate = v;};
  void   setPaused(bool);

  void   setPositionX(double v)   {m_vstate.m_dfX = v;};
  void   setPositionY(double v)   {m_vstate.m_dfY = v;};
  void   setHeadingRAD(double v)  {m_vstate.m_dfHeading = v;};
  void   setHeadingDEG(double v)  
    {m_vstate.m_dfHeading = degToRadians(v);};
  void   setSpeed(double v)       {m_vstate.m_dfSpeed = v;};
  void   setDepth(double v)       {m_vstate.m_dfDepth = v;};
  void   setPosition(std::string);

protected:
  bool      m_paused;
  double    m_rudder;
  double    m_thrust;
  double    m_elevator;
  double    m_push_x;
  double    m_push_y;
  double    m_push_theta;
  double    m_float_rate;
  VState    m_vstate; // with no noise
  MBTimer   m_pause_timer;
  SimEngine m_sim_engine;
};
#endif


