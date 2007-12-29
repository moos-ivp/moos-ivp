/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Loiter.h                                         */
/*    DATE: July 26th 2005 In Elba w/ M.Grunde, P.Newman         */
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
 
#ifndef BHV_LOITER_HEADER
#define BHV_LOITER_HEADER

#include <string>
#include "IvPBehavior.h"
#include "XYPolygon.h"
#include "LoiterEngine.h"
#include "WaypointEngine.h"

class IvPDomain;
class BHV_Loiter : public IvPBehavior {
public:
  BHV_Loiter(IvPDomain);
  ~BHV_Loiter() {};
  
  bool         setParam(std::string, std::string);
  void         onIdleState();
  IvPFunction* onRunState();

protected:
  bool         updateInfoIn();
  void         updateCenter();
  IvPFunction* buildIPF(const std::string&);
  void         updateInfoOut();
  void         updateInfoOutNull();

private:  // Configuration Parameters
  bool         m_clockwise;
  double       m_desired_speed;
  double       m_acquire_dist;
  bool         m_center_activate;

private:  // State Structures
  LoiterEngine   m_loiter_engine;
  WaypointEngine m_waypoint_engine;
  
private:  // State Variables
  bool         m_acquire_mode;   // true:acquiring, false:loitering
  double       m_dist_to_poly;   // Curr ownship dist to poly (mtrs)
  double       m_ptx;            // Curr waypt   x/lon (mtrs)
  double       m_pty;            // Curr waypt   y/lat (mtrs)
  double       m_osx;            // Curr owhship x/lon (mtrs)
  double       m_osy;            // Curr ownship y/lat (mtrs)
  double       m_osh;            // Curr ownship Heading (degs)
  int          m_iterations;     // Number of calls to onRunState

  bool         m_poly_changed;
  bool         m_center_pending; 
  std::string  m_center_assign;  
};
#endif










