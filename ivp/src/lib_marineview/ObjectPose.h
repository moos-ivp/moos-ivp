/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ObjectPose.h                                         */
/*    DATE: Nov 17th 2004                                        */
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

#ifndef OBJECT_POSE_HEADER
#define OBJECT_POSE_HEADER

class ObjectPose
{
 public:
  ObjectPose() 
    {m_valid=false; m_ll_set=false;};
  ObjectPose(double x, double y, double theta, 
	     double spd, double depth=0) {
    m_x=x; 
    m_y=y; 
    m_theta=theta; 
    m_speed=spd; 
    m_depth=depth; 
    if(m_depth >= 0)
      m_valid=true;
  };

  bool   isValid() const     {return(m_valid); };
  bool   isLatLonSet() const {return(m_ll_set);};
  double getX() const        {return(m_x);};
  double getY() const        {return(m_y);};
  double getTheta() const    {return(m_theta);};
  double getSpeed() const    {return(m_speed);};
  double getDepth() const    {return(m_depth);};
  double getLat() const      {return(m_lat);};
  double getLon() const      {return(m_lon);};

  void  setX(double val)     {m_x = val;};
  void  setY(double val)     {m_y = val;};
  void  setTheta(double val) {m_theta = val;};
  void  setSpeed(double val) {m_speed = val;};
  void  setDepth(double val) {m_depth = val;};

  void  setLatLon(double lat, double lon) {
    m_lat = lat;
    m_lon = lon;
    m_ll_set = true;
  };
  
 private:
  double m_x; 
  double m_y; 
  double m_theta;
  double m_speed;  
  double m_depth;
  bool   m_valid;
  bool   m_ll_set;
  double m_lat;
  double m_lon;
};

#endif








