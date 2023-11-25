/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LogicBuffer.H                                        */
/*    DATE: Sep 19th 2010 On the flight to IPAM                  */
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

#ifndef LOGIC_BUFFER_HEADER
#define LOGIC_BUFFER_HEADER

#include <string>
#include <vector>
#include <set>
#include "InfoBuffer.h"
#include "LogicCondition.h"

class LogicBuffer {
public:
  LogicBuffer() {m_info_buffer=0;}
  ~LogicBuffer() {};

public:
  bool addNewCondition(std::string);

  void setInfoBuffer(InfoBuffer*);
  
  void updateInfoBuffer(std::string var, std::string val);
  void setCurrTime(double v);
  
  void updateInfoBuffer(std::string var, double val);

  bool checkConditions(std::string required="all");

  unsigned int size() const {return(m_logic_conditions.size());}

  std::set<std::string> getAllVarsSet() const;
  std::vector<std::string> getAllVars() const;
  std::vector<std::string> getInfoBuffReport(bool allvars=false) const;

  std::string getNotableCondition() const {return(m_notable_condition);}

  double getCurrTime() const;

  std::vector<std::string> getSpec(std::string pad="") const;
  
protected:
  InfoBuffer *m_info_buffer;
  
  std::vector<LogicCondition> m_logic_conditions;

  // notable_condition is a failed condition if required=all
  // notable_condition is a passed condition if required=any
  std::string m_notable_condition;

  // cache of relevant vars, updated upon new conditions
  std::set<std::string> m_logic_vars;

};
#endif

