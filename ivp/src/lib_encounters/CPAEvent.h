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
  CPAEvent();
  CPAEvent(std::string v1, std::string v2, double cpa=0);
  CPAEvent(std::string spec);
  ~CPAEvent() {};

  void setVName1(std::string s) {m_vname1=s;}
  void setVName2(std::string s) {m_vname2=s;}
  void setCPA(double v)         {m_cpa=v;}
  void setEFF(double v)         {m_eff=v;}
  void setX(double v)           {m_x=v;}
  void setY(double v)           {m_y=v;}
  void setID(int id)            {m_id=id;}

  std::string getVName1() const {return(m_vname1);}
  std::string getVName2() const {return(m_vname2);}
  double      getCPA() const    {return(m_cpa);}
  double      getEFF() const    {return(m_eff);}
  double      getX() const      {return(m_x);}
  double      getY() const      {return(m_y);}
  int         getID() const     {return(m_id);}
  
  std::string getSpec() const;
  
 protected: 
  std::string  m_vname1;  // The two vehicles involved
  std::string  m_vname2; 
  double       m_cpa;     // The core info: cpa distance
  double       m_eff;     // Efficiency perhaps filled later
  double       m_x;       // (x,y) midpt at time of cpa
  double       m_y;
  int          m_id;
};

#endif 
