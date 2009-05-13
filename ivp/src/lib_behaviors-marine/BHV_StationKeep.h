/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_StationKeep.h                                    */
/*    DATE: Aug 25 2006                                          */
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
 
#ifndef BHV_STATION_KEEP_HEADER
#define BHV_STATION_KEEP_HEADER

#include <string>
#include <vector>
#include <list>
#include "IvPBehavior.h"
#include "XYSegList.h"

class BHV_StationKeep : public IvPBehavior {
public:
  BHV_StationKeep(IvPDomain);
  ~BHV_StationKeep() {};
  
  bool         setParam(std::string, std::string);
  void         onIdleState();
  IvPFunction* onRunState();

protected:
  bool         updateInfoIn();  
  bool         updateCenter();
  void         postStationMessage(bool);
  void         updateHibernationState();
  bool         historyShowsProgressStart();
  bool         historyShowsProgressEnd();

private:  // Configuration Parameters
  double      m_station_x;
  double      m_station_y;
  bool        m_station_set;

  double      m_outer_radius;
  double      m_inner_radius;
  bool        m_center_activate;

  // The outer speed is the speed at the outer radius. The 
  // desired_speed will decrease linearly until the inner
  // ring, and will be zero inside the inner ring.
  double      m_outer_speed;
  
  // The extra_speed is the desired speed should the
  // vehicle drift outside the outer radius.
  double      m_extra_speed;

  // A UUV station-keeping may need to hibernate until it drifts
  // beyond a radius. The hibernation allows it to float to zero
  // depth. It may need to dive to depth when it is not hibernating.
  double      m_hibernation_radius;
  std::string m_pskmode_variable;

private:  // State Variables
  bool         m_center_pending;
  std::string  m_center_assign;
  double       m_osx;
  double       m_osy;
  double       m_currtime;
  double       m_dist_to_station;
  double       m_dist_to_station_prev;
  std::string  m_hibernation_state;
  std::string  m_transit_state;

  std::list<double> m_distance_history;    // distance
  std::list<double> m_distance_thistory;   // time recorded
};

#endif










