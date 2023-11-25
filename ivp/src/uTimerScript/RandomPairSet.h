/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RandomPairSet.h                                      */
/*    DATE: Nov 22nd 2016                                        */
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

#ifndef RANDOM_PAIR_SET_HEADER
#define RANDOM_PAIR_SET_HEADER

#include <string>
#include <vector>
#include "RandomPair.h"
#include "RandPairPoly.h"

class RandomPairSet 
{
 public:
  RandomPairSet() {}
  ~RandomPairSet();

 public:
  std::string  addRandomPair(std::string spec);
  std::string  addRandomPairPoly(std::string spec);
  void         reset(std::string key, double timestamp=0);

  unsigned int size() const  {return(m_rpair_vector.size());}

  std::string  getVarName1(unsigned int index) const;
  std::string  getVarName2(unsigned int index) const;
  std::string  getKeyName(unsigned int index) const;
  std::string  getType(unsigned int index) const;
  double       getValue1(unsigned int index) const;
  double       getValue2(unsigned int index) const;

  bool         contains(std::string varname) const;

  std::string  getStringValue1(unsigned int index) const;
  std::string  getStringValue2(unsigned int index) const;
  std::string  getStringSummary(unsigned int index) const;
  std::string  getParams(unsigned int index) const;

  void         print() const;

 protected: // Configuration Parameters

  std::vector<RandomPair*> m_rpair_vector;
};

#endif 









