/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ModeEntry.h                                          */
/*    DATE: Dec 26th, 2008                                       */
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

#ifndef MODE_ENTRY_HEADER
#define MODE_ENTRY_HEADER

#include <vector>
#include <string>
#include "LogicCondition.h"

class ModeEntry {
public:
  ModeEntry() {}
  ~ModeEntry() {}

  bool setHead(std::string mode_var, std::string mode_val);
  bool addCondition(std::string);
  bool setElseValue(std::string);

  std::vector<std::string> getConditionVars();

  std::string getModeVarName()      {return(m_mode_var);}
  std::string getModeVarValue()     {return(m_mode_val);}
  std::string getModeVarElseValue() {return(m_mode_val_else);}
  std::string getModePrefix()       {return(m_mode_prefix);}
  std::string getModeParent();

  void print();
  void clear();

  // IO for Conditions - Related to conditions evaluations.
  void clearConditionVarVals();
  void setVarVal(const std::string&, const std::string&);
  void setVarVal(const std::string&, double);

  bool evalConditions();
  bool evalModeVarConditions();

  // Added by tes to support graphviz
  std::vector<LogicCondition> getNonModeLogicConditions() const
    {
      std::vector<LogicCondition> non_mode_logic_cond;
      for(int i = 0, n = m_logic_conditions.size(); i < n; ++i)
	{
          if(!m_modevar_conditions.at(i))
	    non_mode_logic_cond.push_back(m_logic_conditions[i]);
	}
      return non_mode_logic_cond;
    }

protected:
  std::string  m_mode_var;
  std::string  m_mode_val;
  std::string  m_mode_val_else;
  std::string  m_mode_prefix;
  
  std::vector<LogicCondition> m_logic_conditions;
  std::vector<bool>           m_modevar_conditions;
};

#endif











