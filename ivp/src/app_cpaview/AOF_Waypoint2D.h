/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_Waypoint2D.h                                     */
/*    DATE: Jan 4th 2006                                         */
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
 
#ifndef AOF_WAYPOINT2D_HEADER
#define AOF_WAYPOINT2D_HEADER

#include "AOF.h"
#include "IvPDomain.h"
#include "AngleUtils.h"

class AOF_Waypoint2D: public AOF {
 public:
  AOF_Waypoint2D(IvPDomain);
  ~AOF_Waypoint2D() {};

public: // virtuals defined
  double evalBox(const IvPBox*) const;
  bool   setParam(const std::string&, double);
  bool   initialize() {
    current_angle = relAng(osLON, osLAT, ptLON, ptLAT);
    return(true);
  };

protected:
  double osLAT;   // Ownship Lat position at time Tm.
  double osLON;   // Ownship Lon position at time Tm.
  double ptLAT;   
  double ptLON;  
  double desired_speed;
  double current_angle;
};

#endif







