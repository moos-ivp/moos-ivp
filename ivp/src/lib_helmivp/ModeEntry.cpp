/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ModeEntry.cpp                                        */
/*    DATE: Dec 26th, 2008                                       */
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
#include "ModeEntry.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------------
// Procedure: print()

void ModeEntry::print()
{
  cout << "++Mode Var:    " << m_mode_var       << endl;
  cout << "  Mode Val:    " << m_mode_val       << endl;
  cout << "  Mode Else:   " << m_mode_val_else  << endl;
  cout << "  Mode Prefix: " << m_mode_prefix    << endl;
  cout << "  Conditions:  " << endl;
  unsigned int i, vsize = m_logic_conditions.size();
  for(i=0; i<vsize; i++) {
    string raw = m_logic_conditions[i].getRawCondition();
    cout << "    " << raw << endl;
  }
}

//------------------------------------------------------------------
// Procedure: clear()

void ModeEntry::clear()
{
  m_mode_var       = "";
  m_mode_val       = "";
  m_mode_val_else  = "";
  m_mode_prefix    = "";

  m_logic_conditions.clear();
}

//------------------------------------------------------------------
// Procedure: clearConditionVarVals

void ModeEntry::clearConditionVarVals()
{
  unsigned int i, vsize = m_logic_conditions.size();
  for(i=0; i<vsize; i++)
    m_logic_conditions[i].clearVarVals();
}

//------------------------------------------------------------------
// Procedure: setVarVal(string,string)
//      Note: Conditions have variables whose values determine whether
//            the condition will evaluate to true/false. This function
//            takes a variable value pair, and for each condition will
//            SET the variable to that value. If a given condition has
//            no component related to the particular variable, or if the
//            variable's value type is a mis-match, the operation is 
//            simply ignored for that condition.

void ModeEntry::setVarVal(const string& var, const string& val)
{
  unsigned int i, vsize = m_logic_conditions.size();
  for(i=0; i<vsize; i++)
    m_logic_conditions[i].setVarVal(var, val);
}

//------------------------------------------------------------------
// Procedure: setVarVal(string,double)
//      Note: Conditions have variables whose values determine whether
//            the condition will evaluate to true/false. This function
//            takes a variable value pair, and for each condition will
//            SET the variable to that value. If a given condition has
//            no component related to the particular variable, or if the
//            variable's value type is a mis-match, the operation is 
//            simply ignored for that condition.

void ModeEntry::setVarVal(const string& var, double val)
{
  unsigned int i, vsize = m_logic_conditions.size();
  for(i=0; i<vsize; i++)
    m_logic_conditions[i].setVarVal(var, val);
}

//------------------------------------------------------------------
// Procedure: evalConditions
//      Note: This function will evaluate each of the logic conditions
//            and return true only if EACH of them evaluates to true. 
//            Presumably a series of setVarVal calls were previously 
//            made such that all the variables in the conditions 
//            reflect current values.

bool ModeEntry::evalConditions()
{
  bool result = true;
  unsigned int i, vsize = m_logic_conditions.size();
  for(i=0; i<vsize; i++)
    result = result && m_logic_conditions[i].eval();

  return(result);
}

//------------------------------------------------------------------
// Procedure: evalModeVarConditions

bool ModeEntry::evalModeVarConditions()
{
  bool result = true;
  unsigned int i, vsize = m_logic_conditions.size();
  for(i=0; i<vsize; i++) {
    if(m_modevar_conditions[i])
      result = result && m_logic_conditions[i].eval();
  }

  return(result);
}


//------------------------------------------------------------------
// Procedure: setEntry
// Example:   MODE = ACTIVE {DEPLOY==TRUE} INACTIVE

bool ModeEntry::setEntry(string str)
{
  string mode_var  = stripBlankEnds(biteString(str, '='));
  string remainder = stripBlankEnds(str);

  if((mode_var == "") || (remainder == ""))
    return(false);

  // Ensure that there is only one open brace and one close brace
  // And that the open brace is to the left of the close brace
  int leftcnt  = 0;
  int leftix   = -1;
  int rightcnt = 0;
  int rightix  = -1;
  int len = remainder.length();
  for(int i=0; i<len; i++) {
    if(remainder[i] == '{') {
      leftcnt++;
      leftix = i;
    }
    else if(remainder[i] == '}') {
      rightcnt++;
      rightix = i;
    }
  }
  if((leftcnt != 1) || (rightcnt != 1))
    return(false);
  if(leftix >= rightix)
    return(false);

  string mode_val = stripBlankEnds(biteString(remainder, '{'));
  remainder = stripBlankEnds(remainder);

  string condition = stripBlankEnds(biteString(remainder, '}'));
  string else_val  = stripBlankEnds(remainder);

  return(setEntry(mode_var, mode_val, condition, else_val));  
}


//------------------------------------------------------------------
// Procedure: setEntry

bool ModeEntry::setEntry(string mode_var,  string mode_val, 
			 string condition, string else_val)
{
  if(strContains(mode_var, ' ') || strContains(mode_var, '\t'))
    return(false);

  // Make sure the logic condtions string can be used to create a
  // syntactically correct LogicCondition object.
  // The separator "@#!#@" was added presumably by the caller most
  // likely representing a line-break in the input file.
  vector<LogicCondition> lvector;
  vector<string> svector = parseString(condition, "@#!#@");
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    LogicCondition logic_condition;
    bool ok_condition = logic_condition.setCondition(svector[i]);
    if(!ok_condition)
      return(false);
    else
      lvector.push_back(logic_condition);
  }

  // Check for a prefix condition - a condition that the mode variable
  // being set is currently equal to some value.
  // Example "MODE = ACTIVE"
  vsize = lvector.size();
  for(i=0; i<vsize; i++) {
    LogicCondition logic_condition = lvector[i];
    string raw   = logic_condition.getRawCondition();
    string left  = stripBlankEnds(biteString(raw, '='));
    string right = stripBlankEnds(raw); 
    if((left == mode_var) && (raw != "") && (m_mode_prefix == ""))
      m_mode_prefix = right;
  }

  m_logic_conditions = lvector;
  m_mode_var         = mode_var;
  m_mode_val         = mode_val;
  m_mode_val_else    = else_val;

  return(true);
}

//------------------------------------------------------------------
// Procedure: setHead

bool ModeEntry::setHead(string mode_var,  string mode_val)
{
  if(strContains(mode_var, ' ') || strContains(mode_var, '\t'))
    return(false);

  m_mode_var   = mode_var;
  m_mode_val   = mode_val;
  
  return(true);
}

//------------------------------------------------------------------
// Procedure: addCondition

bool ModeEntry::addCondition(string condition)
{
  condition = stripBlankEnds(condition);

  // Make sure the logic condtions string can be used to create a
  // syntactically correct LogicCondition object.
  LogicCondition logic_condition;
  bool ok_condition = logic_condition.setCondition(condition);
  if(!ok_condition)
    return(false);
  else {
    m_logic_conditions.push_back(logic_condition);
    m_modevar_conditions.push_back(false);
  }

  // Check for a prefix condition - a condition that the mode variable
  // being set is currently equal to some value.
  // Example "MODE = ACTIVE"
  unsigned int i, vsize = m_logic_conditions.size();
  for(i=0; i<vsize; i++) {
    LogicCondition logic_condition = m_logic_conditions[i];
    string raw   = logic_condition.getRawCondition();
    string left  = stripBlankEnds(biteString(raw, '='));
    string right = stripBlankEnds(raw); 
    if((left == m_mode_var) && (right != "") && (m_mode_prefix == "")) {
      m_mode_prefix = right;
      m_modevar_conditions[i] = true;
    }
  }

  return(true);
}


//------------------------------------------------------------------
// Procedure: setElseValue

bool ModeEntry::setElseValue(string else_val)
{
  vector<string> svector = parseString(else_val, ' ');
  if(svector.size() != 1)
    return(false);

  m_mode_val_else    = else_val;

  return(true);
}

//------------------------------------------------------------------
// Procedure: getConditionVars

vector<string> ModeEntry::getConditionVars()
{
  vector<string> return_vector;
  unsigned int i, vsize = m_logic_conditions.size();
  for(i=0; i<vsize; i++) {
    vector<string> svector = m_logic_conditions[i].getVarNames();
    return_vector = mergeVectors(return_vector, svector);
  }
  
  return_vector = removeDuplicates(return_vector);
  return(return_vector);
}
