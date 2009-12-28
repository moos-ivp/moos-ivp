/************************************************************/
/*    NAME: Michael Benjamin, H.Schmidt, J.Leonard          */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: EnumVariable.h                                  */
/*    DATE: Dec 26th 2009                                   */
/************************************************************/

#ifndef RANDOM_VARIABLE_HEADER
#define RANDOM_VARIABLE_HEADER

#include <vector>
#include <string>

class EnumVariable 
{
 public:
  EnumVariable();
  virtual ~EnumVariable() {};

 public:
  void        setVarName(std::string str)   {m_varname=str;}; 
  void        setKeyName(std::string str)   {m_keyname=str;};
  void        addElement(std::string, double weight=1);
  std::string reset(std::string key, double timestamp=-1);
  double      getAge(double timestamp);

  std::string getVarName() const {return(m_varname);};
  std::string getKeyName() const {return(m_keyname);};
  std::string getValue() const;

  std::string getStringSummary();

 protected: // Configuration Parameters
  std::string m_varname;
  std::string m_keyname;
  std::vector<std::string> m_elements;
  std::vector<double>      m_weights;

 protected: // State Variables
  double         m_timestamp;
  unsigned int   m_index;
  double         m_total_weight;
};

#endif 
