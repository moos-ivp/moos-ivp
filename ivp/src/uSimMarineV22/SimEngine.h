/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: SimEngine.h                                          */
/*    DATE: Mar 8th, 2005 just another day at CSAIL              */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef SIM_ENGINE_HEADER
#define SIM_ENGINE_HEADER

#include "NodeRecord.h"
#include "ThrustMap.h"
#include "TurnSpeedMap.h"

class SimEngine
{
public:
  SimEngine();
  ~SimEngine() {}

public:
  void setThrustModeReverse(bool v)        {m_thrust_mode_reverse=v;}
  void setTurnSpeedMap(TurnSpeedMap tsmap) {m_turn_speed_map=tsmap;}

  void setVerbose(bool v=true) {m_verbose=v;}
  void setTurnSpdLoss(double);
  
public:
  void propagate(NodeRecord&, double delta_time, double prior_heading,
		 double prior_speed, double drift_x, double drift_y);

  void propagateDepth(NodeRecord&, double delta_time, 
		      double elevator_angle, double buoyancy_rate, 
		      double max_depth_rate, 
		      double m_max_depth_rate_speed);

  void propagateSpeed(NodeRecord&, const ThrustMap&, double delta_time, 
		      double thrust, double rudder,
		      double max_accel, double max_decel,
		      double max_sail_spd=-1);

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

  double m_turn_spd_loss;
  
  TurnSpeedMap m_turn_speed_map;

  bool m_verbose;
};

#endif

