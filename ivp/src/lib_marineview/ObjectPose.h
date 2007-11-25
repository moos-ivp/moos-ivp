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
    {m_valid=false;};
  ObjectPose(float x, float y, float theta, 
	     float spd, float depth=0) {
    m_x=x; 
    m_y=y; 
    m_theta=theta; 
    m_speed=spd; 
    m_depth=depth; 
    if(m_depth >= 0)
      m_valid=true;
  };

  bool  isValid()           {return(m_valid); };
  float getX()              {return(m_x);};
  float getY()              {return(m_y);};
  float getTheta()          {return(m_theta);};
  float getSpeed()          {return(m_speed);};
  float getDepth()          {return(m_depth);};

  void  setX(float val)     {m_x = val;};
  void  setY(float val)     {m_y = val;};
  void  setTheta(float val) {m_theta = val;};
  void  setSpeed(float val) {m_speed = val;};
  void  setDepth(float val) {m_depth = val;};
  void  setInvalid()        {m_valid = false;};
  void  setValid()          {m_valid = true;};

 private:
  float m_x; 
  float m_y; 
  float m_theta;
  float m_speed;  
  float m_depth;
  bool  m_valid;
  
};

#endif








