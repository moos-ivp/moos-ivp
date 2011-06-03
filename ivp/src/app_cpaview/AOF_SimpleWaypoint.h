/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_SimpleWaypoint.h                                 */
/*    DATE: Feb 22th 2009                                        */
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
 
#ifndef AOF_SIMPLE_WAYPOINT_HEADER
#define AOF_SIMPLE_WAYPOINT_HEADER

#include "AOF.h"
#include "IvPDomain.h"

class AOF_SimpleWaypoint: public AOF {
 public:
  AOF_SimpleWaypoint(IvPDomain);
  ~AOF_SimpleWaypoint() {};

public: // virtuals defined
  double evalPoint(const std::vector<double>&) const; 
  bool   setParam(const std::string&, double);
  bool   initialize();

protected:
  // Initialization parameters
  double m_osx;   // Ownship x position at time Tm.
  double m_osy;   // Ownship y position at time Tm.
  double m_ptx;   // x component of next the waypoint.
  double m_pty;   // y component of next the waypoint.
  double m_desired_spd;

 // Initialization parameter set flags
  bool   m_osx_set;  
  bool   m_osy_set;  
  bool   m_ptx_set;  
  bool   m_pty_set;   
  bool   m_desired_spd_set;

  // Cached values for more efficient evalBox calls
  double m_angle_to_wpt;
  double m_min_speed;
  double m_max_speed;
};

#endif


