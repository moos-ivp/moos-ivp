/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogicCondition.cpp                                   */
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

#include <iostream>
#include "LogicCondition.h"

using namespace std;


//------------------------------------------------------ 
// Procedure: Constructor

LogicCondition::LogicCondition()
{
  m_node = 0;
  m_allow_dblequals = true;
}

//------------------------------------------------------ 
// Procedure: Destructor

LogicCondition::~LogicCondition()
{
  if(m_node) 
    delete(m_node);
  m_node = 0;
}

//------------------------------------------------------ 
// Procedure: Constructor

LogicCondition::LogicCondition(const LogicCondition &b)
{
  if(b.m_node)
    m_node = b.m_node->copy();
  else
    m_node = 0;
  m_allow_dblequals = true;
}

//----------------------------------------------------------------
// Procedure: operator=

const LogicCondition &LogicCondition::operator=(const LogicCondition &right)
{
  if(right.m_node)
    m_node = right.m_node->copy();
  else 
    m_node = 0;

  return(*this);
}

//----------------------------------------------------------------
// Procedure: getVarNames()

vector<string> LogicCondition::getVarNames() const
{
  vector<string> rvector;
  
  if(m_node)
    rvector = m_node->recursiveGetVarNames();
  
  return(rvector);
}


//----------------------------------------------------------------
// Procedure: setCondition()

bool LogicCondition::setCondition(const string& str)
{
  if(m_node) {
    delete(m_node);
    m_node = 0;
  }

  m_node = new ParseNode(str);

  bool ok_parse = m_node->recursiveParse(m_allow_dblequals);
  if(!ok_parse) {
    delete(m_node);
    m_node = 0;
    cout << "Bad Condition: " << str << endl;
    return(false);
  }

  bool ok_syntax = m_node->recursiveSyntaxCheck();
  if(!ok_syntax) {
    delete(m_node);
    m_node = 0;
    cout << "Bad Condition Syntax: " << str << endl;
    return(false);
  }

  return(true);
}


//----------------------------------------------------------------
// Procedure: eval

bool LogicCondition::eval() const
{
  if(m_node)
    return(m_node->recursiveEvaluate());
  else
    return(false);
}
	   

	   











