/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RandPairPoly.h                                       */
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

#ifndef RAND_PAIR_POLY_HEADER
#define RAND_PAIR_POLY_HEADER

#include <string>
#include "RandomPair.h"
#include "XYPolygon.h"

class RandPairPoly : public RandomPair
{
 public:
  RandPairPoly();
  virtual ~RandPairPoly() {}

 public: // Overloaded virtual functions
  //bool   setParam(std::string, double) {return(true);}
  bool   setParam(std::string, std::string);
  void   reset();
  
  std::string getStringSummary() const;

 private:

  XYPolygon m_poly;
};

#endif 





