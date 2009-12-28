/************************************************************/
/*    NAME: Michael Benjamin, H.Schmidt, J.Leonard          */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: RandomVariable.h                                */
/*    DATE: Dec 18th 2009                                   */
/************************************************************/

#ifndef RANDOM_VARIABLE_SET_HEADER
#define RANDOM_VARIABLE_SET_HEADER

#include <string>
#include <vector>
#include "RandomVariable.h"

class RandomVariableSet 
{
 public:
  RandomVariableSet() {};
  virtual ~RandomVariableSet() {};

 public:
  unsigned int size() {return(m_rvar_vector.size());};
  bool         contains(const std::string& varname);
  void         addRandomVar(const RandomVariable&);
  std::string  addRandomVar(const std::string& spec);
  void         reset(const std::string& key, double timestamp=0);

  std::string  getVarName(unsigned int index);
  std::string  getStringValue(unsigned int index);
  std::string  getStringSummary(unsigned int index);
  double       getValue(unsigned int index);

 protected: // Configuration Parameters
  std::vector<RandomVariable> m_rvar_vector;
};

#endif 


