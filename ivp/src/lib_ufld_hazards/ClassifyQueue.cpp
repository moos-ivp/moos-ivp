/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ClassifyQueue.cpp                                    */
/*    DATE: Mar 23rd 2013                                        */
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
#include "ClassifyQueue.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

ClassifyQueue::ClassifyQueue()
{
  m_total_entries_ever = 0;

  // Indicates whether there is an "active" entry.
  m_no_active_entry = true;
}

//---------------------------------------------------------
// Procedure: push

void ClassifyQueue::push(ClassifyEntry new_entry)
{
  // Part 1: Adjust priority to implement FIFO for entries of equal priority
  double order_factor = (double)(m_total_entries_ever) / MAX_ENTRIES_EVER;
  double new_priority = new_entry.getPriority() - order_factor;

  new_entry.setPriority(new_priority);

  // Part 2: Push the entry either on the queue or to active entry
  if(m_no_active_entry) {
    m_active_entry = new_entry;
    m_no_active_entry = false;
  }
  else
    m_pqueue.push(new_entry);

  // Part 3: Make note of the total number of entries ever
  m_total_entries_ever++;
}

//---------------------------------------------------------
// Procedure: pushTop

void ClassifyQueue::pushTop(ClassifyEntry new_entry)
{
  if(m_no_active_entry)
    push(new_entry);
  else {
    // If "de-activating" an entry, push it back onto the priority queue
    // without altering its priority.
    m_pqueue.push(m_active_entry);
    m_active_entry = new_entry;
  }
}

//---------------------------------------------------------
// Procedure: pop

ClassifyEntry ClassifyQueue::pop()
{
  ClassifyEntry entry;
  if(m_no_active_entry)
    return(entry);

  entry = m_active_entry;
  // If there's a non-empty queue, move the top entry to active
  if(m_pqueue.size() > 0) {
    m_active_entry = m_pqueue.top();
    m_pqueue.pop();
  }
  else 
    m_no_active_entry = true;   

  return(entry);
}


//---------------------------------------------------------
// Procedure: getCopyAllEntries()

vector<ClassifyEntry> ClassifyQueue::getCopyAllEntries() const
{
  vector<ClassifyEntry> rvector;
  if(m_no_active_entry)
    return(rvector);
  
  rvector.push_back(m_active_entry);
  if(m_pqueue.size() == 0)
    return(rvector);

  priority_queue<ClassifyEntry, vector<ClassifyEntry>, 
		 greater<std::vector<ClassifyEntry>::value_type> > copy_queue;
  
  copy_queue = m_pqueue;
  
  while(copy_queue.size() > 0) {
    ClassifyEntry entry = copy_queue.top();
    rvector.push_back(entry);
    copy_queue.pop();
  }
  
  return(rvector);
}

//---------------------------------------------------------
// Procedure: getStringDump()

vector<string> ClassifyQueue::getStringDump() const
{
  vector<string> rvector;

  vector<ClassifyEntry> ce_vector = getCopyAllEntries();
  unsigned int i, vsize = ce_vector.size();
  for(i=0; i<vsize; i++) {
    XYHazard hazard = ce_vector[i].getHazard();
    string sentry = hazard.getLabel() + ":";
    sentry += doubleToStringX(ce_vector[i].getPriority(), 9);
    rvector.push_back(sentry);
  }
  
  return(rvector);
}

//---------------------------------------------------------
// Procedure: clear
//      Note: The STL priority queue apparently does not have a native
//            clear or erase function.

void ClassifyQueue::clear()
{
  priority_queue<ClassifyEntry, vector<ClassifyEntry>, greater<vector<ClassifyEntry>::value_type> > new_pqueue;

  m_pqueue = new_pqueue;
  m_no_active_entry = true;
}


//---------------------------------------------------------
// Procedure: empty

bool ClassifyQueue::empty() const
{
  return(m_pqueue.empty() && m_no_active_entry);
}


//---------------------------------------------------------
// Procedure: size

unsigned int ClassifyQueue::size() const
{
  if(m_no_active_entry)
    return(0);
  
  return(m_pqueue.size() + 1);
}









