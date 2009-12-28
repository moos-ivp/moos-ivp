/************************************************************/
/*    NAME: Michael Benjamin, H.Schmidt, J.Leonard          */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: RandomVariable.h                                */
/*    DATE: Dec 18th 2009                                   */
/************************************************************/

#ifndef RANDOM_VARIABLE_HEADER
#define RANDOM_VARIABLE_HEADER

#include <string>

class RandomVariable 
{
 public:
  RandomVariable();
  virtual ~RandomVariable() {};

 public:
  void   setVarName(std::string str)   {m_varname=str;}; 
  void   setKeyName(std::string str)   {m_keyname=str;};
  bool   setRange(double, double);
  double reset(double timestamp=-1);
  double getAge(double timestamp);

  std::string getVarName() const {return(m_varname);};
  std::string getKeyName() const {return(m_keyname);};
  std::string getStringValue() const;
  double  getValue() const {return(m_value);};

  std::string getStringSummary();

 protected: // Configuration Parameters
  std::string m_varname;
  std::string m_keyname;
  double      m_min_val;
  double      m_max_val;

 protected: // State Variables
  double      m_value;
  double      m_timestamp;
};

#endif 
