/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ModeEntry.h                                          */
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

#ifndef MODE_ENTRY_HEADER
#define MODE_ENTRY_HEADER

#include <vector>
#include <string>
#include "LogicCondition.h"

class ModeEntry {
public:
  ModeEntry() {};
  ~ModeEntry() {};

  bool setHead(std::string mode_var, std::string mode_val);
  bool addCondition(std::string);
  bool setElseValue(std::string);

  std::vector<std::string> getConditionVars();

  std::string getModeVarName()      {return(m_mode_var);};
  std::string getModeVarValue()     {return(m_mode_val);};
  std::string getModeVarElseValue() {return(m_mode_val_else);};
  std::string getModePrefix()       {return(m_mode_prefix);};
  std::string getModeParent();

  void print();
  void clear();

  // IO for Conditions - Related to conditions evaluations.
  void clearConditionVarVals();
  void setVarVal(const std::string&, const std::string&);
  void setVarVal(const std::string&, double);

  bool evalConditions();
  bool evalModeVarConditions();

protected:
  std::string  m_mode_var;
  std::string  m_mode_val;
  std::string  m_mode_val_else;
  std::string  m_mode_prefix;
  
  std::vector<LogicCondition> m_logic_conditions;
  std::vector<bool>           m_modevar_conditions;
};
#endif

