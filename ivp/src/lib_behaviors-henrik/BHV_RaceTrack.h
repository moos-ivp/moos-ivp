/*****************************************************************/
/*    NAME: Henrik Schmidt                                       */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: BHV_RaceTrack.h                                   */
/*    DATE: Mar 3, 2007                                          */
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
 
#ifndef BHV_RACETRACK_HEADER
#define BHV_RACETRACK_HEADER

#include <string>
#include "IvPBehavior.h"

class IvPDomain;
class BHV_RaceTrack : public IvPBehavior {
public:
  BHV_RaceTrack(IvPDomain);
  ~BHV_RaceTrack() {};
  
  bool         setParam(std::string, std::string);
  IvPFunction* onRunState();
  void         onIdleState();

protected:
  void         updateStart();

private: // Behavior Parameters
  double track_length, approach_angle, dist_tolerance;
  double distance_traveled,heading,track_width,deploy_x,deploy_y;
  double head_x,head_y,new_heading,speed, heading_out, heading_in,head_angle;
  double bearing;
  double deploy_x_out,deploy_y_out;
  double end_x_out,end_y_out;
  double deploy_x_in,deploy_y_in;
  double end_x_in,end_y_in;
  int leg;

  bool m_clockwise;
  bool         m_deploy_pending; 
  std::string  m_deploy_location;  

private: // Behavior State Variables
  bool        first_iteration;
};
#endif
 
