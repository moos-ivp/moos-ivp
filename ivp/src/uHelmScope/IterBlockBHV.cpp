/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IterBlockBHV.cpp                                     */
/*    DATE: Apr 12th 2008                                        */
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

#include "IterBlockBHV.h"

using namespace std;

//------------------------------------------------------------
// Procedure: addErrorMsg()
// Procedure: addWarningMsg()
// Procedure: addPostingMsg()
// Procedure: addInfoMsg()

void IterBlockBHV::addErrorMsg(string msg)
{
  m_error_msgs.push_back(msg);
}
    
void IterBlockBHV::addWarningMsg(string msg)
{
  m_warning_msgs.push_back(msg);
}
    
void IterBlockBHV::addPostingMsg(string msg)
{
  m_posting_msgs.push_back(msg);
}
    
void IterBlockBHV::addInfoMsg(string msg)
{
  m_info_msgs.push_back(msg);
}
    


