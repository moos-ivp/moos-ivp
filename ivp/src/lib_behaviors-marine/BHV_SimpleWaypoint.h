/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_SimpleWaypoint.h                                 */
/*    DATE: July 1st 2007  (For purposes of simp illustration)   */
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
 
#ifndef BHV_SIMPLE_WAYPOINT_HEADER
#define BHV_SIMPLE_WAYPOINT_HEADER

#include <string>
#include "IvPBehavior.h"

class BHV_SimpleWaypoint : public IvPBehavior {
public:
  BHV_SimpleWaypoint(IvPDomain);
  ~BHV_SimpleWaypoint() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

protected: // Configuration Parameters
  double       m_arrival_radius;
  double       m_desired_speed;
  double       m_ptx;
  double       m_pty;
  std::string  m_ipf_type;

protected: // State Variables
  double    m_osx;
  double    m_osy;
};
#endif









