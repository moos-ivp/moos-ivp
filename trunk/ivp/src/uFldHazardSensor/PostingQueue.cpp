/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PostingQueue.cpp                                     */
/*    DATE: Mar 15th 2013                                        */
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

#define MAX_ENTRIES_EVER 1000000
#include "PostingQueue.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PostingQueue::PostingQueue()
{
  m_total_entries_ever = 0;
}

//---------------------------------------------------------
// Procedure: push

void PostingQueue::push(VarDataPair pair)
{
  double order_factor = (double)(m_total_entries_ever) / MAX_ENTRIES_EVER;

  double new_priority = pair.get_priority() - order_factor;
  pair.set_priority(new_priority);

  m_pqueue.push(pair);

  m_total_entries_ever++;
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







