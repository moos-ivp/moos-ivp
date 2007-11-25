/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ParseNode.h                                          */
/*    DATE: October 10th 2006                                    */
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

#ifndef PARSE_NODE_HEADER
#define PARSE_NODE_HEADER

#include <string>

class ParseNode {

public:
  ParseNode(std::string raw_string);
  virtual ~ParseNode();

  ParseNode* copy();

  std::string getRawCondition() {return(m_raw_string);};

  std::vector<std::string> recursiveGetVarNames();

  void recursiveSetVarVal(std::string, std::string);
  void recursiveSetVarVal(std::string, double);
  void recursiveClearVarVal();
  
  bool recursiveEvaluate();

  bool recursiveSyntaxCheck(int=0);
  void recursiveParse();

  void print(std::string s="");

protected:
  bool evaluate(const std::string& relation, 
		double left, double right);
  bool evaluate(const std::string& relation, 
		double left, std::string right);
  bool evaluate(const std::string& relation, 
		std::string left, double right);
  bool evaluate(const std::string& relation, 
		std::string left, std::string right);

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









