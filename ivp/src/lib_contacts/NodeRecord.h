/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: NodeRecord.h                                         */
/*    DATE: Feb 27th 2010                                        */
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

#ifndef NODE_RECORD_HEADER
#define NODE_RECORD_HEADER

#include <string>
#include <map>

class NodeRecord
{
 public:
  NodeRecord(std::string vname="", std::string vtype="");
  ~NodeRecord() {}

  void setX(double v)         {m_x=v;   m_x_set=true;}
  void setY(double v)         {m_y=v;   m_y_set=true;}
  void setLat(double v)       {m_lat=v; m_lat_set=true;}
  void setLon(double v)       {m_lon=v; m_lon_set=true;}
  void setSpeed(double v)     {m_speed=v;      m_speed_set=true;}
  void setSpeedOG(double v)   {m_speed_og=v;   m_speed_og_set=true;}
  void setHeading(double v)   {m_heading=v;    m_heading_set=true;}
  void setHeadingOG(double v) {m_heading_og=v; m_heading_og_set=true;}
  void setYaw(double v)       {m_yaw=v;        m_yaw_set=true;}
  void setPitch(double v)     {m_pitch=v;      m_pitch_set=true;}
  void setDepth(double v)     {m_depth=v;      m_depth_set=true;}
  void setLength(double v)    {m_length=v;     m_length_set=true;}
  void setTimeStamp(double v) {m_timestamp=v;  m_timestamp_set=true;}
  void setTransparency(double v) {m_transparency=v; m_transparency_set=true;}
  void setAltitude(double v)  {m_altitude=v;   m_altitude_set=true;}

  void setTrajectory(std::string s) {m_trajectory=s; m_trajectory_set=true;}
  
  void setThrustModeReverse(bool v) {m_thrust_mode_reverse=v;}

  void setName(std::string s)    {m_name=s;}
  void setGroup(std::string s)   {m_group=s;}
  void setType(std::string s)    {m_type=s;}
  void setColor(std::string s)   {m_color=s;}
  void setMode(std::string s)    {m_mode=s;}
  void setModeAux(std::string s) {m_mode_aux=s;}
  void setAllStop(std::string s) {m_allstop=s;}
  void setLoadWarning(std::string s) {m_load_warning=s;}

  void setProperty(std::string, std::string);

  void setIndex(int i)         {m_index=i;}

  int    getIndex() const      {return(m_index);}
  double getX() const          {return(m_x);}
  double getY() const          {return(m_y);}
  double getLat() const        {return(m_lat);}
  double getLon() const        {return(m_lon);}
  double getSpeed() const      {return(m_speed);}
  double getSpeedOG() const    {return(m_speed_og);}
  double getHeading() const    {return(m_heading);}
  double getHeadingOG() const  {return(m_heading_og);}
  double getYaw() const        {return(m_yaw);}
  double getPitch() const      {return(m_pitch);}
  double getDepth() const      {return(m_depth);}
  double getAltitude() const   {return(m_altitude);}
  double getLength() const     {return(m_length);}
  double getTimeStamp() const  {return(m_timestamp);}
  double getTransparency() const {return(m_transparency);}
  std::string getTrajectory() const {return(m_trajectory);}

  bool   isSetX() const        {return(m_x_set);}
  bool   isSetY() const        {return(m_y_set);}
  bool   isSetLatitude() const {return(m_lat_set);}
  bool   isSetLongitude() const {return(m_lon_set);}
  bool   isSetSpeed() const     {return(m_speed_set);}
  bool   isSetSpeedOG() const   {return(m_speed_og_set);}
  bool   isSetHeading() const   {return(m_heading_set);}
  bool   isSetHeadingOG() const {return(m_heading_og_set);}
  bool   isSetYaw() const       {return(m_yaw_set);}
  bool   isSetPitch() const     {return(m_pitch_set);}
  bool   isSetDepth() const     {return(m_depth_set);}
  bool   isSetAltitude() const  {return(m_altitude_set);}
  bool   isSetLength() const    {return(m_length_set);}
  bool   isSetTimeStamp() const {return(m_timestamp_set);}
  bool   isSetTransparency() const {return(m_transparency_set);}
  bool   isSetTrajectory() const {return(m_trajectory_set);}

  bool   getThrustModeReverse() const {return(m_thrust_mode_reverse);}
  double getElapsedTime(double) const;
  bool   hasProperty(std::string) const;
  bool   valid() const;
  bool   valid(std::string check) const;
  bool   valid(std::string check, std::string& why) const;
  std::string getProperty(std::string) const;

  std::string getName(std::string s="") const;
  std::string getGroup(std::string s="") const;
  std::string getType(std::string s="") const;
  std::string getColor(std::string s="") const;
  std::string getMode(std::string s="") const;
  std::string getModeAux(std::string s="") const;
  std::string getAllStop(std::string s="") const;
  std::string getLoadWarning(std::string s="") const;

  std::string getSpec(bool terse=false) const;

  std::string getStringValue(std::string) const;

 protected: 
  double m_x;
  double m_y;
  double m_speed;
  double m_speed_og;    // Speed over ground
  double m_heading;
  double m_heading_og;  // Heading over ground
  double m_yaw;
  double m_pitch;
  double m_depth;
  double m_altitude;
  double m_lat;
  double m_lon;
  double m_length;
  double m_timestamp;
  double m_transparency;
  std::string  m_trajectory;
  std::string  m_name;
  std::string  m_group;
  std::string  m_type;
  std::string  m_color;
  std::string  m_mode;
  std::string  m_mode_aux;
  std::string  m_allstop;
  std::string  m_load_warning;
  
  bool m_x_set;
  bool m_y_set;
  bool m_lat_set;
  bool m_lon_set;
  bool m_speed_set;
  bool m_speed_og_set;
  bool m_heading_set;
  bool m_heading_og_set;
  bool m_yaw_set;
  bool m_pitch_set;
  bool m_depth_set;
  bool m_altitude_set;
  bool m_length_set;
  bool m_timestamp_set;
  bool m_transparency_set;
  bool m_thrust_mode_reverse;
  bool m_trajectory_set;

  int  m_index;

  // General provisions for using this record to store additional
  // information about this contact
  std::map<std::string, std::string> m_properties;
};

#endif 

