/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmPlot.h                                           */
/*    DATE: Jul1509, Sep2811                                     */
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

#ifndef HELM_TPLOTX_HEADER
#define HELM_TPLOTX_HEADER

#include <string>
#include <vector>
#include "HelmReport.h"

class HelmPlot 
{
public:
  HelmPlot();
  ~HelmPlot() {}

  void   setVehiLength(double len)  {m_vehi_length = len;}
  void   setVehiType(std::string);
  
  bool   addEntry(double time, std::string helm_summary);

  double getTimeByIndex(unsigned int index) const;
  double getTimeByIterAdd(double curr_time, 
			  unsigned int iter_offset) const;
  double getTimeByIterSub(double curr_time, 
			  unsigned int iter_offset) const;

  unsigned int getIterByIndex(unsigned int) const;
  unsigned int getIterByTime(double) const;
  
  std::string  getValueByIndex(std::string query, unsigned int index) const;
  std::string  getValueByTime(std::string query, double gtime) const;
  
  std::string  getVName()       const {return(m_vname);}
  std::string  getVType()       const {return(m_vehi_type);}
  double       getVLength()     const {return(m_vehi_length);}
  unsigned int size()           const {return(m_time.size());}
  void         print()          const;

  bool         containsTime(double) const;

  double getCurrTime() {return(0);}

  unsigned int getIndexByIter(unsigned int) const {return(0);}

protected:
  std::string m_vname; 
  std::string m_vehi_type;
  double      m_vehi_length;
  
  // Parallel indices each index once per IVPHELM_SUMMARY
  std::vector<double>     m_time;
  std::vector<HelmReport> m_helm_reports;
};
#endif 









