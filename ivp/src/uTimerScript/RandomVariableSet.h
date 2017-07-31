/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RandomVariableSet.h                                  */
/*    DATE: Dec 18th 2009                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef RANDOM_VARIABLE_SET_HEADER
#define RANDOM_VARIABLE_SET_HEADER

#include <string>
#include <vector>
#include "RandomVariable.h"
#include "RandVarUniform.h"
#include "RandVarGaussian.h"

class RandomVariableSet 
{
 public:
  RandomVariableSet() {}
  ~RandomVariableSet();

 public:
  std::string  addRandomVar(const std::string& spec);
  std::string  addRandomVarUniform(const std::string& spec);
  std::string  addRandomVarGaussian(const std::string& spec);
  void         reset(const std::string& key, double timestamp=0);

  unsigned int size() const  {return(m_rvar_vector.size());}
  std::string  getVarName(unsigned int index) const;
  std::string  getKeyName(unsigned int index) const;
  std::string  getType(unsigned int index) const;
  double       getValue(unsigned int index) const;
  double       getMinVal(unsigned int index) const;
  double       getMaxVal(unsigned int index) const;

  bool         contains(const std::string& varname) const;

  std::string  getStringValue(unsigned int index) const;
  std::string  getStringSummary(unsigned int index) const;
  std::string  getParams(unsigned int index) const;

  void         print() const;

 protected: // Configuration Parameters

  std::vector<RandomVariable*> m_rvar_vector;
};

#endif 








