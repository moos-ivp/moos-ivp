/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UtterQueue.cpp                                       */
/*    DATE: May 28th 2013                                        */
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
#include "UtterQueue.h"

using namespace std;

//---------------------------------------------------------
// Constructor

UtterQueue::UtterQueue()
{
  m_total_entries_ever = 0;
}

//---------------------------------------------------------
// Procedure: push

void UtterQueue::push(Utterance new_utter)
{
  double order_factor = (double)(m_total_entries_ever) / MAX_ENTRIES_EVER;
  double new_priority = new_utter.getPriority() - order_factor;

  new_utter.setPriority(new_priority);

  m_pqueue.push(new_utter);

  m_total_entries_ever++;
}

//---------------------------------------------------------
// Procedure: pushTop

void UtterQueue::pushTop(Utterance new_utter)
{
  if(m_pqueue.size() == 0) {
    push(new_utter);
    return;
  }

  Utterance top_utter = m_pqueue.top();
  double    top_prior = top_utter.getPriority();
  
  new_utter.setPriority(top_prior+1);
  push(new_utter);
}

//---------------------------------------------------------
// Procedure: pop

Utterance UtterQueue::pop()
{
  Utterance utter;
  if(m_pqueue.empty())
    return(utter);

  utter = m_pqueue.top();
  m_pqueue.pop();
  return(utter);
}


//---------------------------------------------------------
// Procedure: clear
//      Note: The STL priority queue apparently does not have a native
//            clear or erase function. May 2013.

void UtterQueue::clear()
{
  std::priority_queue<Utterance, std::vector<Utterance>, std::greater<std::vector<Utterance>::value_type> > new_pqueue;
  
  m_pqueue = new_pqueue;
}


//---------------------------------------------------------
// Procedure: getCopyAllEntries()

vector<Utterance> UtterQueue::getCopyAllEntries() const
{
  vector<Utterance> rvector;

  priority_queue<Utterance, vector<Utterance>, 
		 greater<std::vector<Utterance>::value_type> > copy_queue;
  
  copy_queue = m_pqueue;
  
  while(copy_queue.size() > 0) {
    Utterance utter = copy_queue.top();
    rvector.push_back(utter);
    copy_queue.pop();
  }
  
  return(rvector);
}






