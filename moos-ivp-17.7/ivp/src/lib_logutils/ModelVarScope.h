/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ModelVarScope.h                                      */
/*    DATE: Feb 28th 2015                                        */
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

#ifndef VAR_SCOPE_MODEL_HEADER
#define VAR_SCOPE_MODEL_HEADER

#include <vector>
#include <string>
#include "VarPlot.h"
#include "ALogDataBroker.h"
#include "VarPlotEntry.h"

class ModelVarScope
{
 public:
  ModelVarScope();
  virtual ~ModelVarScope() {}

  // Setters
  void   setDataBroker(ALogDataBroker, unsigned int mix);
  void   setTime(double tstamp);
  void   setVarPlot(unsigned int mix);
  void   addVarPlot(unsigned int mix);
  void   delVarPlot(unsigned int mix);

  void   setShowVName(bool v)   {m_show_vname = v;}
  void   setShowVarName(bool v) {m_show_varname = v;}
  void   setShowSource(bool v)  {m_show_source = v;}
  void   setShowSrcAux(bool v)  {m_show_srcaux = v;}

  void   reformat();

  // Getters
  double        getCurrTime() const {return(m_curr_time);}
  unsigned int  getVPlotSize(std::string) const;
  
  std::vector<std::string>  getPastEntries() const;
  std::vector<std::string>  getSoonEntries() const;
 
 protected:
  std::vector<std::string>  getEntries(const std::list<VarPlotEntry>&) const;
  std::list<VarPlotEntry>   mergePlots(std::list<VarPlotEntry>, 
				       std::list<VarPlotEntry>) const;

  void   reformat(std::list<VarPlotEntry>&);

private:
  double         m_curr_time;
  ALogDataBroker m_dbroker;

  // Vehicle name stays constant once it is set initially
  std::string m_vname; 

  std::list<VarPlotEntry> m_prev_entries;
  std::list<VarPlotEntry> m_soon_entries;

  bool m_show_vname;
  bool m_show_varname;
  bool m_show_source;
  bool m_show_srcaux;

  unsigned int m_max_len_varname;
  unsigned int m_max_len_vname;
  unsigned int m_max_len_source;
  unsigned int m_max_len_srcaux;
  
};

#endif




