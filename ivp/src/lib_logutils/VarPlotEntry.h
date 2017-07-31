/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VarPlotEntry.h                                       */
/*    DATE: Mar 3rd 2015                                         */
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

#ifndef VAR_PLOT_ENTRY_HEADER
#define VAR_PLOT_ENTRY_HEADER

#include <string>

class VarPlotEntry
{
 public:
  VarPlotEntry(double tstamp=0)  {m_tstamp=tstamp;}
  virtual ~VarPlotEntry() {}

  // Setters
  void setVarName(std::string s)   {m_varname=s;}
  void setVarVal(std::string s)    {m_varval=s;}
  void setVarSrcAux(std::string s) {m_varsrcaux=s;}
  void setVarSource(std::string v) {m_varsrc=v;}
  void setVName(std::string v)     {m_vname=v;}
  
  // Getters
  double      getTStamp() const    {return(m_tstamp);}
  std::string getVarName() const   {return(m_varname);}
  std::string getVarVal() const    {return(m_varval);}
  std::string getVarSrcAux() const {return(m_varsrcaux);}
  std::string getSource() const    {return(m_varsrc);}
  std::string getVName() const     {return(m_vname);}
  std::string getFormatted() const {return(m_formatted);}

  // Formatting
  void format(int, int, int, int, int, bool, bool, bool, bool);
  void clearFormat() {m_formatted="";}

private:
  double      m_tstamp;

  std::string m_varname;
  std::string m_varsrc;
  std::string m_varval;
  std::string m_varsrcaux;
  std::string m_vname;
  std::string m_formatted;
};

#endif




