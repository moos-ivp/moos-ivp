/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RandomVariable.h                                     */
/*    DATE: Dec 18th 2009                                        */
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



