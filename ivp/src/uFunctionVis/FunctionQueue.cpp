/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: FunctionQueue.cpp                                   */
/*    DATE: Aug 18th 2006                                       */
/****************************************************************/

#include <iostream>
#include "FunctionQueue.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

FunctionQueue::FunctionQueue(unsigned int g_queue_limit)
{
  if(g_queue_limit >= 0)
    m_queue_limit = g_queue_limit;
  else
    m_queue_limit = 0;
    
  m_latest_iteration   = 0;
  m_latest_collective  = 0;

  m_low_adjust         = 0;
  m_high_adjust        = 0;
}

//-------------------------------------------------------------
// Procedure: addFunction(string, string, string)
//      Note: returns the descriptor of the new QueueEntry.

string FunctionQueue::addFunction(string ivpfunction)
{
  QueueEntry qentry(ivpfunction);
  m_entries.push_back(qentry);

  unsigned int q_iteration = qentry.getIteration();
  if(q_iteration > m_latest_iteration)
    m_latest_iteration = q_iteration;

  if(m_entries.size() > m_queue_limit)
    m_entries.pop_front();

  return(qentry.getDescriptor());
}

//-------------------------------------------------------------
// Procedure: addHelmIter

void FunctionQueue::addHelmIter(int g_iter_index, int g_of_count)
{
  m_iter_index.push_back(g_iter_index);
  m_of_count.push_back(g_of_count);

  if(m_iter_index.size() > m_queue_limit) {
    m_iter_index.pop_front();
    m_of_count.pop_front();
  }
}

//-------------------------------------------------------------
// Procedure: getQuadSet()

QuadSet FunctionQueue::getQuadSet(string descriptor)
{
  list<QueueEntry>::reverse_iterator p;
  for(p = m_entries.rbegin(); p != m_entries.rend(); p++) {
    QueueEntry &entry = *p;
    if(entry.getDescriptor() == descriptor) {
      m_pieces = entry.size();
      return(entry.getQuadSet(m_colormap_type, m_low_adjust, 
			      m_high_adjust));
    }
  }

  QuadSet empty_quadset;
  return(empty_quadset);
}


//-------------------------------------------------------------
// Procedure: getCollectiveQuadSet()

QuadSet FunctionQueue::getCollectiveQuadSet()
{
  // If the current iteration hasn't changed since the last time
  // a collective quadset was computed, just return the last one.
  if(m_latest_collective == m_latest_iteration)
    return(m_collective_quadset);

  m_collective_quadset = QuadSet();
  m_latest_collective = m_latest_iteration;

  // Get all the QueueEntries of the previous iteration since 
  // presumably they have all arrived in the queue if the current
  // iteration is one greater.

  // int iter_matches = 0;

  list<QueueEntry>::reverse_iterator p;
  for(p = m_entries.rbegin(); p != m_entries.rend(); p++) {
    QueueEntry &entry = *p;
    if((entry.getIteration()+1) == m_latest_iteration) {
      QuadSet quad_set = entry.getQuadSet();
      m_collective_quadset.addQuadSet(quad_set);
    }
  }

  FColorMap cmap;
  cmap.setType(m_colormap_type);
  m_collective_quadset.applyColorMap(cmap);
  m_collective_quadset.normalize(0, 200);

  return(m_collective_quadset);
}


