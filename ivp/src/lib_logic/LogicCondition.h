/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LogicCondition.h                                     */
/*    DATE: October 2006                                         */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
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

  bool setCondition(std::string);

  std::string getRawCondition() {
    if(m_node) 
      return(m_node->getRawCondition());
    else
      return("");
  };
  
  std::vector<std::string> getVarNames();
  
  void clearVarVals()
    {if(m_node) m_node->recursiveClearVarVal();};
  
  void setVarVal(std::string var, std::string val)
    {if(m_node) m_node->recursiveSetVarVal(var,val);};
  
  void setVarVal(std::string var, double val) 
    {if(m_node) m_node->recursiveSetVarVal(var,val);};

  bool eval();
  
  void print()
    {if(m_node) m_node->print();};

protected:
  ParseNode *m_node;

};

#endif










