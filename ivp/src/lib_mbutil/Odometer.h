/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Odometer.h                                           */
/*    DATE: Jan 31st 2020                                        */
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
 
#ifndef ODOMETER_HEADER
#define ODOMETER_HEADER

class Odometer {
 public:
  Odometer();
  ~Odometer() {}

  void   setXY(double, double);
  void   setX(double);
  void   setY(double);

  void   reset(double utc=0);
  void   resetExtent();
  
  void   pause()   {m_paused = true;}
  void   unpause() {m_paused = false;}
  
  void   updateTime(double);
  void   updateDistance();
  void   updateDistance(double x, double y);

  bool   isPaused() const     {return(m_paused);}
  double getTotalDist() const {return(m_total_distance);}
  double getMaxExtent() const {return(m_max_extent);}

  double getTotalElapsed(double utc=-1);
  
 private: // State variables

  double m_curr_x;
  double m_curr_y;
  double m_prev_x;
  double m_prev_y;
  double m_orig_x;
  double m_orig_y;

  double m_curr_utc;
  double m_start_utc;
  
  bool   m_nav_x_received;
  bool   m_nav_y_received;

  double m_total_distance;

  // max_extent is the farthest dist from orig_x,y
  double m_max_extent;

  bool   m_paused;
};

#endif

