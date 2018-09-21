/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CommandItem.cpp                                      */
/*    DATE: July 2nd, 2016                                       */
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
#include "CommandItem.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

CommandItem::CommandItem() 
{
}

//----------------------------------------------------------------
// Procedure: setCmdPostStr()

bool CommandItem::setCmdPostStr(string var, string sval)
{
  if(strContainsWhite(var))
    return(false);

  m_moosvar = var;
  m_valstr  = sval;
  m_valdbl  = 0;
  m_valtype = "string";

  return(true);
}

//----------------------------------------------------------------
// Procedure: setCmdPostDbl()

bool CommandItem::setCmdPostDbl(string var, double dval)
{
  if(strContainsWhite(var))
    return(false);

  m_moosvar = var;
  m_valstr  = "";
  m_valdbl  = dval;
  m_valtype = "double";

  return(true);
}


//----------------------------------------------------------------
// Procedure: addCmdPostReceiver()

bool CommandItem::addCmdPostReceiver(string receiver)
{
  if(strContainsWhite(receiver))
    return(false);

  m_receivers.push_back(receiver);
  return(true);
}

//----------------------------------------------------------------
// Procedure: limitedVNames()
//   purpose: Prune the current vector of receiver names keeping only
//            those in the given set, and "all" and "each".

void CommandItem::limitedVNames(set<string> vnames)
{
  vector<string> subset_receivers;
  
  for(unsigned int i=0; i<m_receivers.size(); i++) {
    if(vnames.count(m_receivers[i]) ||
       (m_receivers[i] == "all") ||
       (m_receivers[i] == "each")) {
      subset_receivers.push_back(m_receivers[i]);
    }
  }
  
  m_receivers = subset_receivers;
}

//----------------------------------------------------------------
// Procedure: getCmdReceiver()

string CommandItem::getCmdReceiver(unsigned int ix) const
{
  if(ix >= m_receivers.size())
    return("");
  
  return(m_receivers[ix]);
}

//----------------------------------------------------------------
// Procedure: getAllReceivers()

vector<string> CommandItem::getAllReceivers() const
{
  return(m_receivers);
}

//----------------------------------------------------------------
// Procedure: hasReceiver()

bool CommandItem::hasReceiver(string vname) const
{
  for(unsigned int i=0; i<m_receivers.size(); i++) {
    if(m_receivers[i] == vname)
      return(true);
  }

  return(false);
}

//----------------------------------------------------------------
// Procedure: print()

void CommandItem::print() const
{
  cout << "[" << m_label << "]" << endl; 

  cout << "  " << m_moosvar << " = ";
  if(m_valtype == "string")
    cout << m_valstr << "  (type=string)" << endl;
  else if(m_valtype == "double")
    cout << m_valdbl << "  (type=double)" << endl;
  else
    cout << "undefined_type" << endl;
  
  for(unsigned int i=0; i<m_receivers.size(); i++) 
    cout << "  To: " << m_receivers[i] << endl;
}





