/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TrajectorySet.h                                      */
/*    DATE: July 16th, 2007                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef TRAJECTORY_SET_HEADER
#define TRAJECTORY_SET_HEADER

#include <vector>

class TrajectorySet
{
public:
  TrajectorySet() {};
  virtual ~TrajectorySet() {};
  
  int    size() {return(start_x.size());};

  void   addTrajectory(double sx, double sy, double sv, double sh, 
		       double dx, double dy);
  double getStartX(int);
  double getStartY(int);
  double getStartSPD(int);
  double getStartHDG(int);
  double getDestX(int);
  double getDestY(int);

protected:
  std::vector<double>  start_x;
  std::vector<double>  start_y;
  std::vector<double>  start_spd;
  std::vector<double>  start_hdg;
  std::vector<double>  dest_x;
  std::vector<double>  dest_y;
};

#endif




