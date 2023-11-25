/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TurnSpeedMap.h                                       */
/*    DATE: Feb 4th, 2022                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef TURN_SPEED_MAP
#define TURN_SPEED_MAP

class TurnSpeedMap
{
public:
  TurnSpeedMap();
  ~TurnSpeedMap() {}
  
 public:
  bool setFullSpeed(double);
  bool setNullSpeed(double);
  bool setFullRate(double);
  bool setNullRate(double);

  double getFullSpeed() const {return(m_full_speed);}
  double getNullSpeed() const {return(m_null_speed);}
  double getFullRate() const  {return(m_full_rate);}
  double getNullRate() const  {return(m_null_rate);}
  
  double getTurnRate(double speed);

 protected:
  double m_full_speed;
  double m_null_speed;
  double m_full_rate;
  double m_null_rate;
};

#endif






