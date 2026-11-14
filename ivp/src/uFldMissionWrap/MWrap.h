/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: MWrap.h                                         */
/*    DATE: November 12th, 2026                             */
/************************************************************/

#ifndef MWRAP_HEADER
#define MWRAP_HEADER

#include <string>

class MWrap 
{
 public:
  MWrap();
  ~MWrap() {};

  void setVName(std::string s) {m_vname=s;}
  void setPass(bool v)         {m_pass=v; m_pass_set=true;}
  void setScore(double v)      {m_score=v; m_score_set=true;}
  void setUTC(double v)        {m_utc=v;}
  
  std::string getVName() const {return(m_vname);}
  bool        getPass() const  {return(m_pass);}
  double      getScore() const {return(m_score);}
  double      getUTC() const   {return(m_utc);}
  
  std::string getSpec() const;
  bool valid() const;
  
 private: // State variables
  std::string m_vname;
  bool        m_pass;
  bool        m_pass_set;
  double      m_score;
  bool        m_score_set;
  double      m_utc;
};

MWrap stringToMWrap(std::string);

#endif 
