/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TrajectorySet.cpp                                    */
/*    DATE: July 16th, 2007                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include <iostream>
#include "TrajectorySet.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

void TrajectorySet::addTrajectory(double sx, double sy, double sv,
				  double sh, double dx, double dy)
{
  start_x.push_back(sx);
  start_y.push_back(sy);
  start_spd.push_back(sv);
  start_hdg.push_back(sh);
  dest_x.push_back(dx);
  dest_y.push_back(dy);
}

