/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Waypoint.h                                       */
/*    DATE: July 11 2006                                         */
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
 
#ifndef BHV_ZIGZAG_HEADER
#define BHV_ZIGZAG_HEADER

#include <string>
#include <vector>
#include "IvPBehavior.h"
#include "XYSegList.h"

class BHV_ZigZag : public IvPBehavior {
public:
  BHV_ZigZag(IvPDomain);
  ~BHV_ZigZag() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

protected:
  bool setNextWaypoint();

protected:
  int       current_waypt;
  int       repeat;
  double    arrival_radius;
  double    cruise_speed;
  double    current_cpa;
  double    nm_radius;
  double    lead_distance;
  XYSegList seglist;
  bool      two_dim;

protected: // intermediate or object global variables.

  double    ray_heading;
  double    zig_period;
  double    zig_amplitude;

  double    osSPD;
  double    osX;
  double    osY;
  double    ptX;
  double    ptY;
  double    ptSPD;
  double    iptX;
  double    iptY;
};
#endif








