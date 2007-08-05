/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Waypoint.h                                       */
/*    DATE: Nov 2004                                             */
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
 
#ifndef BHV_WAYPOINT_HEADER
#define BHV_WAYPOINT_HEADER

#include <string>
#include "IvPBehavior.h"
#include "WaypointEngine.h"

class BHV_Waypoint : public IvPBehavior {
public:
  BHV_Waypoint(IvPDomain);
  ~BHV_Waypoint() {};
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);

protected:
  bool         updateInfoIn();
  bool         setNextWaypoint();
  IvPFunction* buildOF(std::string);
  void         updateInfoOut();

protected:
  WaypointEngine waypoint_engine;

  double    cruise_speed;
  double    lead_distance;

protected: // intermediate or object global variables.
  double    osSPD;
  double    osX;
  double    osY;
  double    ptX;
  double    ptY;
  double    iptX;
  double    iptY;
};
#endif









