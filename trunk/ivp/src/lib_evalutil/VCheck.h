/****************************************************************/
/*   NAME: Mike Benjamin                                        */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: VCheck.h                                             */
/*   DATE: Sep 30th, 2020                                       */
/****************************************************************/

#ifndef V_CHECK_HEADER
#define V_CHECK_HEADER

#include <string> 

class VCheck 
{
 public:
  VCheck();
  ~VCheck() {};

  // comment
  bool setVarCheck(std::string);
  
  void setVarName(std::string s)      {m_varname=s;}
  void setValString(std::string s)    {m_val_str=s; m_val_set=true;}
  void setValDouble(double v)         {m_val_dbl=v; m_val_set=true;}
  void setTimeStamp(double v)         {m_tstamp=v; m_tstamp_set=true;}
  void setMaxTimeDelta(double v)      {m_max_tdelta=v; m_delta_set=true;}
  void setMaxValDelta(double v)       {m_max_vdelta=v; m_delta_set=true;}
  void setEvaluated(bool v=true)      {m_evaluated=v;}
  void setSatisfied(bool v)           {m_satisfied=v;}

  void setActualDelta(double v)       {m_actual_delta=v;}
  void setActualTime(double v)        {m_actual_time=v;}
  void setActualValDbl(double v)      {m_actual_val_dbl=v;}
  void setActualValStr(std::string s) {m_actual_val_str=s;}
  
  std::string getVarName() const      {return(m_varname);}
  std::string getValString() const    {return(m_val_str);}
  double      getValDouble() const    {return(m_val_dbl);}
  double      getTimeStamp() const    {return(m_tstamp);}
  double      getMaxTimeDelta() const {return(m_max_tdelta);}
  double      getMaxValDelta() const  {return(m_max_vdelta);}
  bool        isEvaluated() const     {return(m_evaluated);}
  bool        isSatisfied() const     {return(m_satisfied);}
  double      getActualDelta() const  {return(m_actual_delta);}
  double      getActualTime() const   {return(m_actual_time);}
  double      getActualValDbl() const {return(m_actual_val_dbl);}
  std::string getActualValStr() const {return(m_actual_val_str);}

  void print() const;
  
 private: // Configuration variables
  std::string m_varname;
  std::string m_val_str;
  double      m_val_dbl;
  double      m_tstamp;
  double      m_max_tdelta;
  double      m_max_vdelta;

  bool   m_val_set;
  bool   m_tstamp_set;
  bool   m_delta_set;

 private: // state
  bool   m_evaluated;
  bool   m_satisfied;

  double m_actual_delta;
  double m_actual_time;
  double m_actual_val_dbl;
  std::string m_actual_val_str;
};

// Overload the < operator 
bool operator< (const VCheck& vcheck1, const VCheck& vcheck2);
bool operator> (const VCheck& vcheck1, const VCheck& vcheck2);

#endif 
