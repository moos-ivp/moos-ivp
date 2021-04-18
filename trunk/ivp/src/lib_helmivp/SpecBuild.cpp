/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SpecBuild.h                                          */
/*    DATE: Feb 11, 2010                                         */
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
#include "SpecBuild.h"

using namespace std;

//--------------------------------------------------------------
// Procedure: setBehaviorKind()

void SpecBuild::setBehaviorKind(string bhv_kind, unsigned int lnum)
{
  m_bhv_kind = bhv_kind;
  m_bhv_kind_lnum = lnum;
}

//--------------------------------------------------------------
// Procedure: addBadConfig()

void SpecBuild::addBadConfig(string line, unsigned int lnum) 
{
  m_bad_config_lines.push_back(line);
  m_bad_config_lnums.push_back(lnum);
}

//--------------------------------------------------------------
// Procedure: deleteBehavior()

void SpecBuild::deleteBehavior() 
{
  if(m_behavior) 
    delete(m_behavior);
  m_behavior = 0;
}

//--------------------------------------------------------------
// Procedure: getBehaviorName()

string SpecBuild::getBehaviorName() 
{
  if(m_behavior)
    return(m_behavior->getDescriptor());
  return("");
}

//--------------------------------------------------------------
// Procedure: getBadConfigLine()

string SpecBuild::getBadConfigLine(unsigned int ix) 
{
  if(ix < m_bad_config_lines.size())
    return(m_bad_config_lines[ix]);
  return("");
}
  
//--------------------------------------------------------------
// Procedure: getBadConfigLineNum()

unsigned int  SpecBuild::getBadConfigLineNum(unsigned int ix) 
{
  if(ix < m_bad_config_lnums.size())
    return(m_bad_config_lnums[ix]);
  return(0);
}

//--------------------------------------------------------------
// Procedure: getHelmStartMessages()

vector<VarDataPair> SpecBuild::getHelmStartMessages()
{
  vector<VarDataPair> messages;

  if(m_behavior) {
    messages = m_behavior->getMessages();
    m_behavior->clearMessages();
  }

  return(messages);
}

//--------------------------------------------------------------
// Procedure: print()

void SpecBuild::print()
{
  cout << "------------------------------------" << endl;
  cout << "SpecBuild Report on BHV_Kind: " << m_bhv_kind << endl;
  cout << "  valid: " << valid() << endl;
  cout << "  bhv_kind_result: " << m_bhv_kind_result << endl;
  cout << "  # of bad config lines: " << m_bad_config_lines.size();
  unsigned int i, isize = m_bad_config_lines.size(); 
  for(i=0; i<isize; i++) {
    cout << "[" << i << "]: Line #" << m_bad_config_lnums[i] << endl;
    cout << "[" << i << "]:" << m_bad_config_lines[i] << endl;
  }
}










