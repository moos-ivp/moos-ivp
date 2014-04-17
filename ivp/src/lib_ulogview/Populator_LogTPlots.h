/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_LogTPlots.h                                */
/*    DATE: Jun0505 (Sun in Kenmorre), Sep2811                   */
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

#ifndef POPULATOR_LOGTPLOTS_HEADER
#define POPULATOR_LOGTPLOTS_HEADER

#include <string>
#include <map>
#include <vector>
#include "LogTPlot.h"
#include "ALogEntry.h"

class Populator_LogTPlots 
{
public:
  Populator_LogTPlots() {m_logstart_utc=0;};
  ~Populator_LogTPlots() {};

  bool     populateFromEntries(const std::vector<ALogEntry>&);

  LogTPlot getLogPlot(unsigned int index) const;
  LogTPlot getLogPlot(const std::string& varname) const;
  int      size() const  {return(m_logplots.size());};

  void        setLogStartUTC(double v)       {m_logstart_utc=v;};
  void        setVName(const std::string& s) {m_vname=s;};
  std::string getVName() const               {return(m_vname);};

protected:
  std::string               m_vname;
  std::vector<LogTPlot>     m_logplots;
  double                    m_logstart_utc;

  // Mapping from logplot variable to index in m_loplots vector
  std::map<std::string, unsigned int> m_logplot_var_map;
};
#endif 





