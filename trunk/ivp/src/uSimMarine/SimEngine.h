/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SimEngine.h                                          */
/*    DATE: Mar 8th, 2005 just another day at CSAIL              */
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

#ifndef SIM_ENGINE_HEADER
#define SIM_ENGINE_HEADER

#include "NodeRecord.h"
#include "ThrustMap.h"

class SimEngine
{
public:
  SimEngine() {}
  ~SimEngine() {}
  
 public:
  void setThrustModeReverse(bool v) {m_thrust_mode_reverse=v;}

public:
  void propagate(NodeRecord&, double delta_time, double prior_heading,
		 double prior_speed, double drift_x, double drift_y);
  
  void propagateDepth(NodeRecord&, double delta_time, 
		      double elevator_angle, double buoyancy_rate, 
		      double max_depth_rate, 
		      double m_max_depth_rate_speed);

  void propagateSpeed(NodeRecord&, const ThrustMap&, double delta_time, 
		      double thrust, double rudder,
		      double max_accel, double max_decel);

  void propagateHeading(NodeRecord&, double delta_time, double rudder,
			double thrust, double turn_rate, 
			double rotate_speed);

  // Differential Thrust Modes
  void propagateSpeedDiffMode(NodeRecord&, const ThrustMap&, double delta_time, 
			      double thrust_left, double thrust_right,
			      double max_accel, double max_decel);
  
  void propagateHeadingDiffMode(NodeRecord&, double delta_time, double rudder,
				double thrust_left, double thrust_right, 
				double rotate_speed);

 protected:
  bool m_thrust_mode_reverse;
};

#endif




