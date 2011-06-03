/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_WaypointRateClosure.h                            */
/*    DATE: Sep 28th 2006                                        */
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
 
#ifndef AOF_WAYPOINT_RATE_CLOSURE_HEADER
#define AOF_WAYPOINT_RATE_CLOSURE_HEADER

#include "AOF.h"
#include "IvPDomain.h"

class AOF_WaypointRateClosure: public AOF {
 public:
  AOF_WaypointRateClosure(IvPDomain);
  ~AOF_WaypointRateClosure() {};

public: // virtuals defined
  double evalBox(const IvPBox*) const; 
  bool   setParam(const std::string&, double);
  bool   initialize();

protected:
  double os_y;   // Ownship Lat position at time Tm.
  double os_x;   // Ownship Lon position at time Tm.
  double pt_y;   
  double pt_x;  
  double desired_spd;
  double max_speed;

  bool   os_y_set;  
  bool   os_x_set;  
  bool   pt_y_set;   
  bool   pt_x_set;  
  bool   desired_spd_set;

  double angle_to_wpt;

  int    crs_ix;  // Index of "course" variable in IvPDomain
  int    spd_ix;  // Index of "speed" variable in IvPDomain
};

#endif








