/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ParseNode.cpp                                        */
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

#include <iostream>
#include <vector>
#include <stdlib.h>
#include "ParseNode.h"
#include "LogicUtils.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

ParseNode::ParseNode(string raw_string)
{
  m_raw_string = raw_string;

  m_left_node  = 0;
  m_right_node = 0;

  m_double_val = 0;
  m_double_set = false;
  m_string_set = false;
}

//----------------------------------------------------------------
// Destructor

ParseNode::~ParseNode()
{
  if(m_left_node)
    delete(m_left_node);
  if(m_right_node)
    delete(m_right_node);
}

//----------------------------------------------------------------
// Procedure: copy()

ParseNode* ParseNode::copy()
{
  ParseNode *new_node = new ParseNode(m_raw_string);
  
  new_node->m_relation   = m_relation;
  new_node->m_string_val = m_string_val;
  new_node->m_double_val = m_double_val;
  new_node->m_string_set = m_string_set;
  new_node->m_double_set = m_double_set;
  
  if(m_left_node)
    new_node->m_left_node = m_left_node->copy();

  if(m_right_node)
    new_node->m_right_node = m_right_node->copy();

  return(new_node);
}


//----------------------------------------------------------------
// Procedure: recursiveGetVarNames()

vector<string> ParseNode::recursiveGetVarNames()
{
  vector<string> rvector;

  if(m_left_node) {
    vector<string> svector = m_left_node->recursiveGetVarNames();
    rvector = mergeVectors(rvector, svector);
  }

  if(m_right_node) {
    vector<string> svector = m_right_node->recursiveGetVarNames();
    rvector = mergeVectors(rvector, svector);
  }

  if(m_relation == "variable") 
    rvector.push_back(m_raw_string);

  rvector = removeDuplicates(rvector);
  return(rvector);
}

//----------------------------------------------------------------
// Procedure: recursiveSetVarVal()

void ParseNode::recursiveSetVarVal(string varname, string varval)
{
  if(m_relation == "variable") {
    if(m_raw_string == varname) {
      // ok to overwrite a previous string-value, but cannot
      // overwrite if previously set with a double value.
      if(m_double_set != true) {
	m_string_val = varval;
	m_string_set = true;
      }
    }
    return;
  }

  if(m_left_node)
    m_left_node->recursiveSetVarVal(varname, varval);

  if(m_right_node)
    m_right_node->recursiveSetVarVal(varname, varval);
}

//----------------------------------------------------------------
// Procedure: recursiveSetVarVal()
//      Note: Its ok to overwrite a previously set double-value, but
//            if this node has been previously set with a string val,
//            overwriting is NOT allowed.

void ParseNode::recursiveSetVarVal(string varname, double varval)
{
  if(m_relation == "variable") {
    if(m_raw_string == varname) {
      // ok to overwrite a previous double-value, but cannot
      // overwrite if previously set with a string value.
      if(m_string_set != true) {
	m_double_val = varval;
	m_double_set = true;
      }
    }
    return;
  }

  if(m_left_node)
    m_left_node->recursiveSetVarVal(varname, varval);

  if(m_right_node)
    m_right_node->recursiveSetVarVal(varname, varval);
}

//----------------------------------------------------------------
// Procedure: recursiveClearVarVal()

void ParseNode::recursiveClearVarVal()
{
  if(m_relation == "variable") {
    m_string_val = "";
    m_double_val = 0;
    m_string_set = false;
    m_double_set = false;
    return;
  }
  
  if(m_left_node)
    m_left_node->recursiveClearVarVal();

  if(m_right_node)
    m_right_node->recursiveClearVarVal();
}

//----------------------------------------------------------------
// Procedure: recursiveEvaluate()

bool ParseNode::recursiveEvaluate()
{
  if(m_relation == "not") {
    if(!m_left_node)
      return(false);
    return(!m_left_node->recursiveEvaluate());
  }

  // Recursion should never progress down to a leaf node.
  // So null sub-nodes are indication of fatal error.
  if(!m_left_node || !m_right_node)
    return(false);
  
  if(m_relation == "or") {
    bool result_left  = m_left_node->recursiveEvaluate();
    bool result_right = m_right_node->recursiveEvaluate();
    return(result_left || result_right);
  }
  
  if(m_relation == "and") {
    bool result_left  = m_left_node->recursiveEvaluate();
    bool result_right = m_right_node->recursiveEvaluate();
    return(result_left && result_right);
  }

  // Handle RIGHT Side
  string type_left    = "unset";
  string string_left  = "";
  double double_left  = 0;
  if(m_left_node->m_string_set) {
    type_left = "string";
    string_left = m_left_node->m_string_val;
  }
  else if(m_left_node->m_double_set) {
    type_left = "double";
    double_left = m_left_node->m_double_val;
  }


  // Handle RIGHT Side
  string type_right   = "unset";
  string string_right = "";
  double double_right = 0;;
  if(m_right_node->m_relation == "string") {
    type_right = "string";
    string_right = m_right_node->m_raw_string;
  }
  else if(m_right_node->m_relation == "double") {
    type_right = "double";
    double_right = atof(m_right_node->m_raw_string.c_str());
  }
  else if(m_right_node->m_relation == "variable") {
    if(m_right_node->m_string_set) {
      type_right = "string";
      string_right = m_right_node->m_string_val;
    }
    else if(m_right_node->m_double_set) {
      type_right = "double";
      double_right = m_right_node->m_double_val;
    }
  }

  if((type_left == "string") && (type_right == "string"))
    return(evaluate(m_relation, string_left, string_right));
  if((type_left == "string") && (type_right == "double"))
    return(evaluate(m_relation, string_left, double_right));
  if((type_left == "double") && (type_right == "string"))
    return(evaluate(m_relation, double_left, string_right));
  if((type_left == "double") && (type_right == "double"))
    return(evaluate(m_relation, double_left, double_right));

  return(false);
}


//----------------------------------------------------------------
// Procedure: recursiveSyntaxCheck(int)
//      Note: side==0  - root node
//            side==1  - left side
//            side==2  - right side

bool ParseNode::recursiveSyntaxCheck(int side)
{
  if((m_left_node==0) && (m_right_node==0)) {
    m_relation = "leaf";
    if(side==0)
      return(false);

    else if(side==1) {
      bool ok = isValidVariable(m_raw_string);
      if(ok)
	m_relation = "variable";
      return(ok);
    }

    else if(side==2) {
      if(isValidRightVariable(m_raw_string)) {
	m_relation = "variable";
	m_raw_string = stripBlankEnds(m_raw_string);
	int len = m_raw_string.length();
	m_raw_string.replace(len-1, 1, "");
	m_raw_string.replace(0, 2, "");
	return(true);
      }
      else if(isNumber(m_raw_string)) {
	m_relation = "double";
	return(true);
      }
      else if(isValidLiteral(m_raw_string)) {
	m_relation = "string";
	return(true);
      }
      else
	return(false);
    }
    else
      return(false);
  }
  
  // Case where there is ONE subnode
  else if((m_left_node!=0) && (m_right_node==0)) {
    if(m_relation != "not")
      return(false);
    bool ok = m_left_node->recursiveSyntaxCheck(0);
    return(ok);
  }

  // Case where there are TWO subnodes
  else if((m_left_node!=0) && (m_right_node!=0)) {
    bool ok1;
    if((m_relation=="or") || (m_relation=="and"))
      ok1 = m_left_node->recursiveSyntaxCheck(0);
    else
      ok1 = m_left_node->recursiveSyntaxCheck(1);
    
    bool ok2;
    if((m_relation=="or") || (m_relation=="and"))
      ok2 = m_right_node->recursiveSyntaxCheck(0);
    else
      ok2 = m_right_node->recursiveSyntaxCheck(2);
    
    return(ok1 & ok2);
  }
  
  else
    return(false);
}


//----------------------------------------------------------------
// Procedure: recursiveParse()

void ParseNode::recursiveParse()
{
  vector<string> svector = parseRelation(m_raw_string);
  int vsize = svector.size();

  if((vsize != 2) && (vsize != 3))
    return;

  if(vsize == 2) {
    m_relation  = "not";
    m_left_node = new ParseNode(svector[1]);
    m_left_node->recursiveParse();
  }

  if(vsize == 3) {
    m_relation   = svector[0];
    // Enforce that left/right args to and/or must be in parens
    if((m_relation=="and") || (m_relation=="or")) {
      if(!globalParens(svector[1]) && !globalNotParens(svector[1]))
	return;
      if(!globalParens(svector[2]) && !globalNotParens(svector[2]))
	return;
    }

    m_left_node  = new ParseNode(svector[1]);
    m_right_node = new ParseNode(svector[2]);
    m_left_node->recursiveParse();
    m_right_node->recursiveParse();
  }
}


//----------------------------------------------------------------
// Procedure: evaluate

bool ParseNode::evaluate(const string& relation, 
			 string left, string right)
{
  if(isQuoted(left))  left  = stripQuotes(left);
  if(isQuoted(right)) right = stripQuotes(right);
  
  if(relation == "=")  
    return(left == right);
  else if(relation == "!=") 
    return(left != right);
  else if(relation == "<")  
    return(left <  right);
  else if(relation == "<=") 
    return(left <= right);
  else if(relation == ">")  
    return(left >  right);
  else if(relation == ">=") 
    return(left >= right);
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: evaluate

bool ParseNode::evaluate(const string& relation, 
			 double left, double right)
{
  if(relation == "=")  
    return(left == right);
  else if(relation == "!=") 
    return(left != right);
  else if(relation == "<")  
    return(left <  right);
  else if(relation == "<=") 
    return(left <= right);
  else if(relation == ">")  
    return(left >  right);
  else if(relation == ">=") 
    return(left >= right);
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: evaluate

bool ParseNode::evaluate(const string& relation, 
			 string left, double right)
{
  if(isQuoted(left))  
    left = stripQuotes(left);

  if(!isNumber(left))
    return(false);
  
  return(evaluate(relation, atof(left.c_str()), right));
}


//----------------------------------------------------------------
// Procedure: evaluate

bool ParseNode::evaluate(const string& relation, 
			 double left, string right)
{
  if(isQuoted(right))  
    right = stripQuotes(right);

  if(!isNumber(right))
    return(false);

  return(evaluate(relation, left, atof(right.c_str())));
}


//----------------------------------------------------------------
// Procedure: print()

void ParseNode::print(string indent_str)
{
  indent_str += "  ";


  cout << indent_str << "ParseNode: [" + m_relation << "]: ";

  if(m_relation == "variable") {
    if(m_string_set)
      cout << "[" << m_string_val << "] ";
    if(m_double_set)
      cout << "[" << m_double_val << "] ";
  }

  if(!m_left_node && !m_right_node) {
    cout << "[" << m_raw_string << "] " << endl;
    return;
  }

  cout << endl;

  if(m_left_node) {
    cout << indent_str << "LeftNode: " << endl;
    m_left_node->print(indent_str);
  }
  else
    cout << indent_str << "Left Node is NULL!!!" << endl;
  
  if(m_right_node) {
    cout << indent_str << "RightNode: " << endl;
    m_right_node->print(indent_str);
  }
  else
    cout << indent_str << "Right Node: NULL" << endl;
}



