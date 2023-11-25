/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RandVarUniform.h                                     */
/*    DATE: May 20th 2012                                        */
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

#ifndef RAND_VAR_UNIFORM_HEADER
#define RAND_VAR_UNIFORM_HEADER

#include <string>
#include "RandomVariable.h"

class RandVarUniform : public RandomVariable
{
 public:
  RandVarUniform();
  virtual ~RandVarUniform() {}

 public: // Overloaded virtual functions
  bool   setParam(std::string, double);
  void   reset();
  
  std::string getStringSummary() const;
};

#endif 










