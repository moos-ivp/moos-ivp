/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PMGen_Dubins.h                                       */
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

#ifndef PMGEN_DUBINS_HEADER
#define PMGEN_DUBINS_HEADER

#include <string>
#include <vector>
#include "XYSeglr.h"
#include "PMGen.h"

class PMGen_Dubins : public PMGen
{
 public:
  PMGen_Dubins(double radius=30, double degs=10);
  ~PMGen_Dubins() {};

public: // Setters
  PlatModel generate(double osx, double osy,
		     double osh, double osv);

  bool setParam(std::string, std::string);
  bool setParam(std::string, double);

  std::string getParamStr(std::string) const {return("");}
  double getParamDbl(std::string) const;

  bool valid() const;

  std::string getAlg() const {return("dubins");}

protected:
  bool init(double osx, double osy, double osh);
  
protected:
  bool   m_valid;
  
  double m_radius;
  double m_spoke_degs;

  // Vector ix corresponds to one unit m_spoke_degs
  std::vector<double>  m_star_spoke_vx;
  std::vector<double>  m_star_spoke_vy;
  std::vector<double>  m_port_spoke_vx;
  std::vector<double>  m_port_spoke_vy;
  std::vector<XYSeglr> m_star_seglrs;
  std::vector<XYSeglr> m_port_seglrs;
};

#endif 
