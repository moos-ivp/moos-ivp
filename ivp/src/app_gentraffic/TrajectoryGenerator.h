/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TrajectoryGenerator.h                                */
/*    DATE: July 16th, 2007                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef TRAJECTORY_GENERATOR_HEADER
#define TRAJECTORY_GENERATOR_HEADER

#include "TrajectorySet.h"


class TrajectoryGenerator
{
public:
  TrajectoryGenerator() {};
  virtual ~TrajectoryGenerator() {};
  
  void randomSeed();
  
  void generate(double center_x, double center_y, double radius,   
		double variance, double amount, double speed);

  TrajectorySet getTSet() {return(m_trajectory_set);};
  
protected:
  
  TrajectorySet m_trajectory_set;
  
};

#endif




