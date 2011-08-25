/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ModeEntry.cpp                                        */
/*    DATE: Dec 26th, 2008                                       */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
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


