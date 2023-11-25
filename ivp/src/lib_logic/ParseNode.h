/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ParseNode.h                                          */
/*    DATE: October 10th 2006                                    */
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

#ifndef PARSE_NODE_HEADER
#define PARSE_NODE_HEADER

#include <string>

class ParseNode {

public:
  ParseNode(const std::string& raw_string);
  virtual ~ParseNode();

  ParseNode* copy();

  std::string getRawCondition() const {return(m_raw_string);}

  std::vector<std::string> recursiveGetVarNames() const;

  void recursiveSetVarVal(std::string, std::string);
  void recursiveSetVarVal(std::string, double);
  void recursiveClearVarVal();
  
  bool recursiveEvaluate() const;
  void recursiveExpandMacro(std::string, std::string);

  bool recursiveSyntaxCheck(int=0);
  bool recursiveParse(bool allow_dblequals=true);

  void print(std::string s="") const;

protected:
  bool evaluate(const std::string& relation, 
		double left, double right) const;
  bool evaluate(const std::string& relation, 
		double left, std::string right) const;
  bool evaluate(const std::string& relation, 
		std::string left, double right) const;
  bool evaluate(const std::string& relation, 
		std::string left, std::string right) const;

  void expandMacro(const std::string& macro, std::string value);
  
protected:
  
  std::string    m_raw_string;
  std::string    m_relation;

  ParseNode*     m_left_node;
  ParseNode*     m_right_node;

  std::string    m_string_val;  
  double         m_double_val;  
  bool           m_string_set;  
  bool           m_double_set;  
};
#endif










