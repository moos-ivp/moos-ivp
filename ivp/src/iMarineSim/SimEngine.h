/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SimEngine.h                                          */
/*    DATE: Mar 8th, 2005 just another day at CSAIL              */
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

#ifndef SIM_ENGINE_HEADER
#define SIM_ENGINE_HEADER

#include "VState.h"

class SimEngine
{
public:
  SimEngine(double=0, double=0, double=0);
  virtual ~SimEngine() {};
  
public:
  void   propagate(VState&, double velocity, double rudder_angle, 
		   double delta_time, double elevator_angle=0);
  bool   setParam(const std::string& param, double value);

protected:
  double m_force_x;        // An external translational force
  double m_force_y;        // An external translational force

  double m_torque_theta;   // An external rotational force
  double m_top_turn_speed;
  double m_deceleration;   // meters/sec^2
  double m_float_rate;
};

#endif
