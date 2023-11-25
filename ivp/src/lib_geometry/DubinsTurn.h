/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: DubinsTurn.h                                         */
/*    DATE: Nov 11th 2018                                        */
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
 
#ifndef DUBINS_TURN_HEADER
#define DUBINS_TURN_HEADER

#include <vector>

class DubinsTurn {
public:
  DubinsTurn(double osx=0, double osy=0, double osh=0, double rad=0);
  ~DubinsTurn() {};

  void setTurn(double new_desired_hdg);

  double getArcCX() const {return(m_ax);}
  double getArcCY() const {return(m_ay);}
  double getArcRad() const {return(m_rad);}
  double getArcLangle() const {return(m_langle);}
  double getArcRangle() const {return(m_rangle);}

  double getOSRayX() const {return(m_rx);}
  double getOSRayY() const {return(m_ry);}

  double getArcLen() const {return(m_arc_length);}
  double getTurnHdg() const {return(m_new_hdg);}
  
  bool   starTurn() const {return(m_turn_type == 1);}
  bool   portTurn() const {return(m_turn_type == -1);}
  bool   noTurn()   const {return(m_turn_type == 0);}
  
public:
  
private:
  // configuration variables
  double m_osx;
  double m_osy;
  double m_osh;
  double m_rad;

  // Arg passed to determine the variables
  double m_new_hdg;

  // Variables dynamically determined
  double m_ax;
  double m_ay;
  double m_langle;
  double m_rangle;

  double m_rx;
  double m_ry;

  double m_arc_length;

  int    m_turn_type;
};

#endif

