/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PlatModelGenerator.h                                 */
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

#ifndef PLAT_MODEL_GENERATOR_HEADER
#define PLAT_MODEL_GENERATOR_HEADER

#include <string>
#include "PMGen.h"

class PlatModelGenerator 
{
 public:
  PlatModelGenerator();
  ~PlatModelGenerator() {};

public: // Setters
  PlatModel generate(double osx, double osy, double osh, double osv=0);
  bool setParams(std::string);
  void setCurrTime(double utc) {m_curr_utc=utc;}

  std::string getParamStr(std::string) const;
  double getParamDbl(std::string) const;

  bool stalePModel() {return(true);}

  double getTurnRate();
  
protected:
  void updateHdgHist(double osh);
  
private: // Config vars
  std::string m_alg;

  double m_hdg_hist_length;  // seconds
  
private: // State vars

  bool   m_stale_pmodel;
  
  PMGen *m_pmgen;

  std::list<double> m_recent_val_osh;
  std::list<double> m_recent_utc_osh;

  double m_curr_utc;
  
  unsigned int m_count;
};

#endif 
