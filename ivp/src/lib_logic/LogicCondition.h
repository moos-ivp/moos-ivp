/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LogicCondition.h                                     */
/*    DATE: October 2006                                         */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#ifndef LOGIC_CONDITION_HEADER
#define LOGIC_CONDITION_HEADER

#include <string>
#include <vector>
#include "ParseNode.h"

class LogicCondition {
public:
  LogicCondition();
  
  LogicCondition(const LogicCondition&);

  ~LogicCondition();
  
  const LogicCondition &operator=(const LogicCondition&);

  bool setCondition(const std::string&);
  void setAllowDoubleEquals(bool v) {m_allow_dblequals=v;}

  void expandMacro(std::string macro, std::string val);
  
  std::string getRawCondition() const {
    if(m_node) 
      return(m_node->getRawCondition());
    else
      return("");
  }
  
  std::vector<std::string> getVarNames() const;
  
  void clearVarVals()
    {if(m_node) m_node->recursiveClearVarVal();}
  
  void setVarVal(std::string var, std::string val)
    {if(m_node) m_node->recursiveSetVarVal(var,val);}
  
  void setVarVal(std::string var, double val) 
    {if(m_node) m_node->recursiveSetVarVal(var,val);}

  bool eval() const;
  
  void print() const
    {if(m_node) m_node->print();}

protected:
  ParseNode *m_node;

  bool  m_allow_dblequals;
};

#endif




















