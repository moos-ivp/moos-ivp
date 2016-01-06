/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT CSAIL, Dept of Mechanical Engineering       */
/*    FILE: CPAEvent.h                                      */
/*    DATE: Dec 21st 2015                                   */
/************************************************************/

#ifndef CPA_EVENT_HEADER
#define CPA_EVENT_HEADER

#include <string>

class CPAEvent
{
 public:
  CPAEvent()
    {m_cpa=0; m_x=0; m_y=0;}
  CPAEvent(std::string v1, std::string v2, double cpa)
    {m_vname1=v1; m_vname2=v2; m_cpa=cpa; m_x=0; m_y=0;}
  ~CPAEvent() {};

  void setVName1(std::string s) {m_vname1=s;}
  void setVName2(std::string s) {m_vname2=s;}
  void setCPA(double v)         {m_cpa=v;}
  void setX(double v)           {m_x=v;}
  void setY(double v)           {m_y=v;}

  std::string getVName1() const {return(m_vname1);}
  std::string getVName2() const {return(m_vname2);}
  double      getCPA() const    {return(m_cpa);}
  double      getX() const      {return(m_x);}
  double      getY() const      {return(m_y);}
  
 protected: 
  std::string  m_vname1;
  std::string  m_vname2;
  double       m_cpa;
  double       m_x;
  double       m_y;
};

#endif 
