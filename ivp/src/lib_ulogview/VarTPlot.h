/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VarTPlot.h                                           */
/*    DATE: Oct1011                                              */
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

#ifndef VAR_TPLOT_HEADER
#define VAR_TPLOT_HEADER

#include <string>
#include <vector>
#include <ALogEntry.h>

class VarTPlot {
 public:
  VarTPlot();
  ~VarTPlot() {};
  
 public: // Settting
  void   setVehiName(std::string s)  {m_vehiname = s;};
  void   setVarName(std::string s)   {m_varname  = s;};
  void   setBaseUTC(double v)        {m_base_utc=v;};
  bool   addEntry(const ALogEntry&);

 public: // Getting

  double       getTimeByIndex(unsigned int index) const;
  double       getTimeUTCByIndex(unsigned int index) const;
  unsigned int getIndexByTime(double time) const;
  unsigned int getIndexByTimeUTC(double time) const;
  
  ALogEntry    getEntryByIndex(unsigned int index) const;
  ALogEntry    getEntryByTime(double gtime) const;
  ALogEntry    getEntryByTimeUTC(double gtime) const;

  std::string  getVarName() const  {return(m_varname);};
  double       getBaseUTC() const  {return(m_base_utc);};
  std::string  getVehiName() const {return(m_vehiname);};
  unsigned int size() const        {return(m_entry.size());};

  bool         containsTime(double) const;
  bool         containsTimeUTC(double) const;

  void         print() const;

protected:
  // Vehicle name (usually same as the MOOS community name)
  std::string m_vehiname;    
  // The MOOS variable name
  std::string m_varname;  

  std::vector<ALogEntry> m_entry;

  double m_base_utc;
};
#endif 




