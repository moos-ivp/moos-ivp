/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CommandFolio.cpp                                     */
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
#include "CommandFolio.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

CommandFolio::CommandFolio() 
{
  m_limited_vnames = false;
}

//----------------------------------------------------------------
// Procedure: addCmdItem()

bool CommandFolio::addCmdItem(CommandItem item)
{
  if(item.totalReceivers() == 0)
    return(false);

  m_cmd_items.push_back(item);

  return(true);
}

//----------------------------------------------------------------
// Procedure: limitedVNames()
//   purpose: For each CommandItem, prune the current vector of
//            receiver names keeping only those in the given set,
//            and "all" and "each".

void CommandFolio::limitedVNames(set<string> vnames)
{
  if(vnames.size() == 0)
    return;

  for(unsigned int i=0; i<m_cmd_items.size(); i++) 
    m_cmd_items[i].limitedVNames(vnames);

  m_limited_vnames = true;
  m_set_limited_vnames = vnames;
}



//----------------------------------------------------------------
// Procedure: getCmdItem

CommandItem CommandFolio::getCmdItem(unsigned int ix) const
{
  CommandItem empty_item;
  if(ix >= m_cmd_items.size())
    return(empty_item);

  return(m_cmd_items[ix]);
}

//----------------------------------------------------------------
// Procedure: getAllCmdItems()

vector<CommandItem> CommandFolio::getAllCmdItems() const
{
  return(m_cmd_items);
}

//----------------------------------------------------------------
// Procedure: getCmdColor()

string CommandFolio::getCmdColor(string label) const
{
  string bcolor;
  for(unsigned int i=0; i<m_cmd_items.size(); i++) {
    string icolor = m_cmd_items[i].getCmdColor();
    if((m_cmd_items[i].getCmdLabel() == label) && (icolor != ""))
      bcolor = m_cmd_items[i].getCmdColor();
  }
  return(bcolor);
}

//----------------------------------------------------------------
// Procedure: getAllReceivers()

set<string> CommandFolio::getAllReceivers() const
{
  set<string> return_set;

  for(unsigned int i=0; i<m_cmd_items.size(); i++) {
    vector<string> receivers = m_cmd_items[i].getAllReceivers(); 
    for(unsigned int j=0; j<receivers.size(); j++)
      return_set.insert(receivers[j]);
  }

  return(return_set);
}

//----------------------------------------------------------------
// Procedure: getAllLabels()

set<string> CommandFolio::getAllLabels(string vname) const
{
  set<string> return_set;
  
  for(unsigned int i=0; i<m_cmd_items.size(); i++) {
    if(m_cmd_items[i].hasReceiver(vname)) {
      string label = m_cmd_items[i].getCmdLabel(); 
      return_set.insert(label);
    }
  }

  return(return_set);
}

//----------------------------------------------------------------
// Procedure: print()

void CommandFolio::print() const
{
  cout << "=====================================================" << endl;
  cout << "CommandFolio: " << m_cmd_items.size() << " items" << endl;
  for(unsigned int i=0; i<m_cmd_items.size(); i++)
    m_cmd_items[i].print();
}





