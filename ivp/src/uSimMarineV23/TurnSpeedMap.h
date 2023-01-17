/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TurnSpeedMap.h                                       */
/*    DATE: Feb 4th, 2022                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
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





