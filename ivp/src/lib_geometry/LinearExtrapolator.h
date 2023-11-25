/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LinearExtrapolator.h                                 */
/*    DATE: May 2nd, 2007                                        */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#ifndef LINEAR_EXTRAPOLATOR_HEADER
#define LINEAR_EXTRAPOLATOR_HEADER

#include <vector>
#include <string>

class LinearExtrapolator {
public:
  LinearExtrapolator();
  virtual ~LinearExtrapolator() {}

  void setDecay(double s, double e) {
    m_decay_start = s; 
    m_decay_end   = e;
  }

  void setPosition(double x, double y, double s, double h, double t) {
    m_xpos = x;
    m_ypos = y;
    m_spd  = s;
    m_hdg  = h;
    m_timestamp = t;
    m_position_set = true;
  }

  bool getPosition(double& xpos, double& ypos, double timestamp);

  bool isDecayMaxed() {return(m_decay_maxed);}

  std::string getFailureReason() const {return(m_failure_reason);}
  
 protected:
  double m_xpos;         // Position in meters
  double m_ypos;         // Position in meters
  double m_spd;          // Speed in meters per second
  double m_hdg;          // Heading in degress
  double m_decay_start;  // Time in seconds
  double m_decay_end;    // Time in seconds
  double m_timestamp;    // Time in seconds
  bool   m_position_set; // false until first setPosition

  bool   m_decay_maxed;

  std::string m_failure_reason;
  
};

#endif














