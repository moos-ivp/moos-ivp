/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VState.h                                             */
/*    DATE: Oct 25th 2004                                        */
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

#ifndef VSTATE_HEADER
#define VSTATE_HEADER

#include <string>

// A self explanatory class encapsulating the vehicle's state. 
// This is essentially a data package that keeps things neat.
class VState
{
public:
  VState();
  ~VState() {};

 public: // Setters
  void   setName(std::string str)           {m_name=str;};
  void   setX(double v, double t=0)         {m_x=v; m_x_ts=t;};
  void   setY(double v, double t=0)         {m_y=v; m_y_ts=t;};
  void   setLat(double v, double t=0)       {m_lat=v; m_lat_ts=t;};
  void   setLon(double v, double t=0)       {m_lon=v; m_lon_ts=t;};
  void   setHeading(double v, double t=0)   {m_hdg=v; m_hdg_ts=t;};
  void   setSpeed(double v, double t=0)     {m_spd=v; m_spd_ts=t;};
  void   setDepth(double v, double t=0)     {m_dep=v; m_dep_ts=t;};
  void   setHeadingOG(double v, double t=0) {m_hog=v; m_hog_ts=t;};
  void   setSpeedOG(double v, double t=0)   {m_sog=v; m_sog_ts=t;};
  void   setYaw(double v, double t=0)       {m_yaw=v; m_yaw_ts=t;};

  void   setTimeUTC(double v)               {m_time_utc=v;};

 public: // Getters
  std::string getName() const     {return(m_name);};
  double   getX() const           {return(m_x);};
  double   getY() const           {return(m_y);};
  double   getLat() const         {return(m_lat);};
  double   getLon() const         {return(m_lon);};
  double   getHeading() const     {return(m_hdg);};
  double   getSpeed() const       {return(m_spd);};
  double   getDepth() const       {return(m_dep);};
  double   getHeadingOG() const   {return(m_hog);};
  double   getSpeedOG() const     {return(m_sog);};
  double   getTimeUTC() const     {return(m_time_utc);};
  double   getYaw() const         {return(m_yaw);};

  bool     isSet(std::string) const;
  double   timeSet(std::string) const;
  double   lastUpdate() const;

 private:
  std::string m_name;
  double m_x;                 // X position
  double m_y;                 // Y position
  double m_lat;               // Latitude position
  double m_lon;               // Longitude position
  double m_hdg;               // Heading position (radians)
  double m_spd;               // Speed
  double m_dep;               // Depth
  double m_hog;               // Heading over ground (degrees)
  double m_sog;               // Speed over ground
  double m_yaw;               // Vehicle yaw 

  double m_time_utc;          // Time UTC
  
  double m_x_ts;
  double m_y_ts;
  double m_lat_ts;
  double m_lon_ts;
  double m_hdg_ts;
  double m_spd_ts;
  double m_dep_ts;
  double m_hog_ts;
  double m_sog_ts;
  double m_yaw_ts;
};
#endif 

