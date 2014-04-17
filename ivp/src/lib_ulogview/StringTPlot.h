/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: StrintTPlot.h                                        */
/*    DATE: Oct0711                                              */
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

#ifndef STRING_TPLOT_HEADER
#define STRING_TPLOT_HEADER

#include <string>
#include <vector>
#include "TimePlot.h"

class StringTPlot : public TimePlot {
 public:
  StringTPlot();
  ~StringTPlot() {};
  
 public: // Settting
  void   setVarName(std::string s)   {m_varname = s;};
  bool   setValue(double gtime, 
		  const std::string& value,
		  const std::string& source="", 
		  const std::string& srcaux="");

 public: // Getting

  std::string getValueByIndex(unsigned int index) const;
  std::string getSourceByIndex(unsigned int index) const;
  std::string getSrcAuxByIndex(unsigned int index) const;

  std::string getValueByTime(double gtime) const;
  std::string getValueByTimeUTC(double gtime) const;

  std::string getVarName() const  {return(m_varname);};

  void   print() const;

  
protected:
  std::string m_varname;  // MOOS variable name

  std::vector<std::string> m_value;
  std::vector<std::string> m_source;
  std::vector<std::string> m_srcaux;

  unsigned int m_max_strlen;
};
#endif 













