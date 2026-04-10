/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PMGen.h                                              */
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

#ifndef PMGEN_HEADER
#define PMGEN_HEADER

#include <string>
#include <list>
#include "PlatModel.h"

class PMGen
{
 public:
  PMGen() {};
  virtual ~PMGen() {};

public: // Setters
  virtual PlatModel generate(double osx, double osy,
			     double osh, double osv) = 0;

  virtual bool setParam(std::string, std::string) {return(true);}

  virtual std::string getAlg() const {return("");}
  virtual std::string getParamStr(std::string) const {return("");}
  virtual double getParamDbl(std::string) const {return(0);}  

  void setHdgHist(std::list<double> hdgs, std::list<double> utcs);

  double getHdgAvg(double time_window);
  
protected:
  std::string m_algorithm;

  std::list<double> m_recent_val_osh;
  std::list<double> m_recent_utc_osh;
};

#endif 
