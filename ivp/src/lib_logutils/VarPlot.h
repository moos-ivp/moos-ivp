/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: VarPlot.h                                            */
/*    DATE: Oct 10th 2011                                        */
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

#ifndef VAR_PLOT_HEADER
#define VAR_PLOT_HEADER

#include <string>
#include <vector>
#include <list>
#include "VarPlotEntry.h"

class VarPlot {
 public:
  VarPlot();
  ~VarPlot() {}
  
 public: // Settting
  void   setVName(std::string s)     {m_vname=s;}
  void   setVarName(std::string s)   {m_varname=s;}
  void   setSource(std::string s )   {m_srcname=s; m_source.clear();}
  void   setBaseUTC(double v)        {m_base_utc=v;}
  bool   setValue(double gtime, std::string gvalue, std::string src="");

 public: // Modification
  void   applySkew(double skew);

 public: // Getting
  double       getTimeByIndex(unsigned int index) const;
  std::string  getVarName() const  {return(m_varname);}
  bool         containsTime(double) const;
  void         print() const;
  unsigned int getMaxLenSource() const {return(m_max_len_source);}
  unsigned int getMaxLenSrcAux() const {return(m_max_len_srcaux);}
  unsigned int size() const            {return(m_entry.size());}

 public: // Getting Entries and Sources
  std::string  getEntryByIndex(unsigned int index) const;
  std::string  getSourceByIndex(unsigned int index) const;
  double       getTStampByIndex(unsigned int index) const;

  std::string  getEntryByTime(double gtime) const;
  std::string  getSourceByTime(double gtime) const;
  double       getTStampByTime(double gtime) const;

  std::vector<std::string> getEntriesUpToTime(double gtime) const;
  std::vector<std::string> getSourcesUpToTime(double gtime) const;
  std::vector<double>      getTStampsUpToTime(double gtime) const;

  std::vector<std::string> getEntriesPastTime(double gtime) const;
  std::vector<std::string> getSourcesPastTime(double gtime) const;
  std::vector<double>      getTStampsPastTime(double gtime) const;

  std::list<VarPlotEntry> getVarPlotEntriesUpToTime(double gtime) const;
  std::list<VarPlotEntry> getVarPlotEntriesPastTime(double gtime) const;

protected:
  std::string m_vname;
  std::string m_varname;
  std::string m_srcname; // Non-empty only if src same for all entries 

  std::vector<double>      m_time;
  std::vector<std::string> m_source;
  std::vector<std::string> m_srcaux;
  std::vector<std::string> m_entry;

  double m_base_utc;

  unsigned int m_max_len_source;
  unsigned int m_max_len_srcaux;
};
#endif 









