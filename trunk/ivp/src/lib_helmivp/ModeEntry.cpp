/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ModeEntry.cpp                                        */
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

#include <iostream>
#include "ModeEntry.h"
#include "MBUtils.h"

using namespace std;

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
    if(m_modevar_conditions[i]) {
      result = result && m_logic_conditions[i].eval();
    }
  }
  return(result);
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
  logic_condition.setAllowDoubleEquals(false);
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

//------------------------------------------------------------------
// Procedure: getModeParent

string ModeEntry::getModeParent()
{
  vector<string> svector = parseString(m_mode_prefix, ':');
  unsigned int vsize = svector.size();
  if(vsize > 0) 
    return(svector[vsize-1]);
  else
    return("");
}

//------------------------------------------------------------------
// Procedure: print()

void ModeEntry::print()
{
  cout << "++Mode Var:    " << m_mode_var       << endl;
  cout << "  Mode Val:    " << m_mode_val       << endl;
  cout << "  Mode Else:   " << m_mode_val_else  << endl;
  cout << "  Mode Prefix: " << m_mode_prefix    << endl;
  cout << "  MVConds:    (" << m_modevar_conditions.size() <<")"<< endl;
  cout << "  Conditions: (" << m_logic_conditions.size() << ")"<<endl;
  unsigned int i, vsize = m_logic_conditions.size();
  for(i=0; i<vsize; i++) {
    string raw = m_logic_conditions[i].getRawCondition();
    cout << "    " << raw;
    if(m_modevar_conditions[i])
      cout << "(modevar)" << endl;
    else
      cout << "(not-modevar)" << endl;
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
  m_modevar_conditions.clear();
}

//------------------------------------------------------------------
// Procedure: clearConditionVarVals

void ModeEntry::clearConditionVarVals()
{
  unsigned int i, vsize = m_logic_conditions.size();
  for(i=0; i<vsize; i++)
    m_logic_conditions[i].clearVarVals();
}











