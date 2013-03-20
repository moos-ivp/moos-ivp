/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PostingQueue.cpp                                     */
/*    DATE: Mar 15th 2013                                        */
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

#include "PostingQueue.h"

using namespace std;

//---------------------------------------------------------
// Procedure: push

void PostingQueue::push(VarDataPair pair)
{
  m_pqueue.push(pair);
}

//---------------------------------------------------------
// Procedure: pop

VarDataPair PostingQueue::pop()
{
  VarDataPair pair;
  if(m_pqueue.empty())
    return(pair);

  pair = m_pqueue.top();
  m_pqueue.pop();
  return(pair);
}
