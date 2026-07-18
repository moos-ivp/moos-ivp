/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PMGen_Holonomic.h                                    */
/*    DATE: Oct 24th, 2023                                       */
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

#ifndef PMGEN_HOLONOMIC_HEADER
#define PMGEN_HOLONOMIC_HEADER

#include <string>
#include "PMGen.h"

class PMGen_Holonomic : public PMGen
{
 public:
  PMGen_Holonomic() {};
  ~PMGen_Holonomic() {};

public: // Setters
  PlatModel generate(double osx, double osy,
		     double osh, double osv);

  bool setParam(std::string, std::string) {return(true);}
  bool setParam(std::string, double) {return(true);}

  std::string getAlg() const {return("holo");}
};

#endif 
