/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LogicCondition.cpp                                   */
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

#include <iostream>
#include "LogicCondition.h"

using namespace std;


//------------------------------------------------------ 
// Procedure: Constructor

LogicCondition::LogicCondition()
{
  m_node = 0;
}

//------------------------------------------------------ 
// Procedure: Destructor

LogicCondition::~LogicCondition()
{
  if(m_node)
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

vector<string> LogicCondition::getVarNames()
{
  vector<string> rvector;
  
  if(m_node)
    rvector = m_node->recursiveGetVarNames();
  
  return(rvector);
}


//----------------------------------------------------------------
// Procedure: setCondition()

bool LogicCondition::setCondition(string str)
{
  if(m_node)
    delete(m_node);

  m_node = new ParseNode(str);
  m_node->recursiveParse();
  
  bool ok = m_node->recursiveSyntaxCheck();

  if(!ok)
    delete(m_node);

  return(ok);
}


//----------------------------------------------------------------
// Procedure: eval

bool LogicCondition::eval()
{
  if(m_node)
    return(m_node->recursiveEvaluate());
  else
    return(false);
}
	   

	   


