/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PQueue.cpp                                           */
/*    DATE: December 8th, 2000                                   */
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
#include <stdlib.h>
#include "PQueue.h"

#define  MAX_PQUEUE_LEVELS 10

using namespace std;

//--------------------------------------------------------------
// Constructor:
//       Notes: Size of the heap is always a power of two (minus
//              one) ensuring that a full heap corresponds to a
//              full binary tree. This is done because when we
//              insert into a full heap, we will randomly replace
//              one of the leaf children. By using a full tree,
//              we always know immediately the number of leaves
//              and their indexes. A nice property of heaps is
//              the leaves are always in the second half of the
//              array, making it easy to pick a random leaf index.

PQueue::PQueue(int levels, bool g_sortbymax)
{
  m_levels = levels;

  if(m_levels < 0)
    m_levels = 0;
  else if(levels > MAX_PQUEUE_LEVELS) 
    m_levels = MAX_PQUEUE_LEVELS;
  
  m_size = 1;

  for(int j=0; j<(levels-1); j++)
    m_size *= 2;

  m_num_leaves = (m_size+1)/2;
  m_num_inodes = m_num_leaves - 1;

  m_sort_by_max = g_sortbymax;
  m_end_ix      = 0;
  
  m_key.resize(m_size);
  m_keyval.resize(m_size);

  for(int i=0; i<m_size; i++) {
    m_key[i]    = -1;
    m_keyval[i] = 0.0;
  }
}

//--------------------------------------------------------------
// Procedure: insert
//   Purpose: Insert new item into heap, and update corresponding
//            ratings[] information.
//      Note: See Cormen, Leiserson, and Rivest page 150. Mostly
//            take from this algorithm. Only differences are that
//            we have a separate (parallel) bvals array, and that
//            our array indexes run 0..(n-1) vs 1...n as in CLR.
//      Note: Removal time is O(lg n).

void PQueue::insert(int new_key, double new_keyval)
{
  if(m_levels <= 0)
    return;

  int new_ix;

  // Untested as of 7/4/07
  if(!m_sort_by_max)
    new_keyval *= -1;

  if(m_end_ix == 0) {
    m_key[0]    = new_key;
    m_keyval[0] = new_keyval;
    m_end_ix++;
  }

  // Currently Heap is full. So pick a random leaf to insert a 
  // new element. But if new_keyval is worse, then just return.
  if(m_end_ix == (m_size - 1)) {
    new_ix  = (rand() % m_num_leaves);
    new_ix += m_num_inodes;
    if(new_keyval < m_keyval[new_ix])
      return;
  }

  // Currently Heap has room. Stick new element at end before
  // sending up the tree (during the heapify call later).
  if(m_end_ix < (m_size - 1)) {
    m_end_ix++;
    new_ix = m_end_ix;
  }

  // If we are inserting into a full heap, then we know that some
  // box has to be removed and returned to the caller. Above, we 
  // determined that the given box, b, has a higher priority value 
  // than the one at the chosen leaf node, whose index is new_ix. 
  // So we know right now that boxes[new_ix] will be returned. If 
  // the heap was NOT full then boxes[new_ix] would be NULL, and
  // thats the return value we want.

  int     ix     = new_ix;
  int     parIX  = parent(ix);
  int     parBox = m_key[parIX];
  double  parVal = m_keyval[parIX];
  while((ix>0) && (parVal < new_keyval)) {
    m_key[ix]    = parBox;
    m_keyval[ix] = parVal;

    ix      = parent(ix);
    parIX   = parent(ix);
    parBox  = m_key[parIX];
    parVal  = m_keyval[parIX];
  }

  m_key[ix]    = new_key;
  m_keyval[ix] = new_keyval;
}

//--------------------------------------------------------------
// Procedure: removeBest
//   Purpose: Return and remove the highest priority element in the
//            queue. Heapify is then run on the root.
//      Note: Removal time is O(lg n).

int PQueue::removeBest()
{
  if((m_levels <= 0) || (m_end_ix < 0))
    return(-1);   // Heap underflow
  
  int retix = m_key[0];
  m_key[0] = m_key[m_end_ix];
  m_keyval[0] = m_keyval[m_end_ix];
  m_key[m_end_ix] = -1;
  m_keyval[m_end_ix] = 0;
  m_end_ix = m_end_ix - 1;
  heapify(0);
  return(retix);
}

//--------------------------------------------------------------
// Procedure: returnBestVal
//   Purpose: Return the keyval of the key at the top of the heap.
//            If m_sort_by_max is false, then the keyvals had their
//            sign reversed upon insertion, so we need to reverse
//            the sign again here.

double PQueue::returnBestVal()
{
  if((m_levels <= 0) || (m_end_ix < 0))
    return(0);   // Heap underflow
  
  if(m_sort_by_max)
    return(m_keyval[0]);
  else
    return(-1 * m_keyval[0]);
}


//--------------------------------------------------------------
// Procedure: heapify
//   Purpose: Restore the heap property for the subtree rooted
//            at the given index.
//      Note: See Cormen, Leiserson, and Rivest page 143. Mostly
//            take from this algorithm. Only differences are that
//            we have a separate (parallel) bvals array, and that
//            our array indexes run 0..(n-1) vs 1...n as in CLR.
//   Returns: true  if everything went properly.
//            false if problem (index out of range).

bool PQueue::heapify(int gIX)
{
  if(gIX > m_end_ix) 
    return(false);

  int largestIX  = gIX;
  int leftIX     = left(gIX);
  int rightIX    = right(gIX);
  int heapsize   = m_end_ix;

  double valLarge = m_keyval[gIX];

  if(leftIX <= heapsize) {
    double valLeft  = m_keyval[leftIX];
    if(valLeft > valLarge)
      largestIX = leftIX;
  }

  valLarge = m_keyval[largestIX];

  if(rightIX <= heapsize) {
    double valRight = m_keyval[rightIX];
    if(valRight > valLarge)
      largestIX = rightIX;
  }

  if(largestIX == gIX) 
    return(true);

  int    btemp     = m_key[gIX];
  double ftemp     = m_keyval[gIX];
  m_key[gIX]       = m_key[largestIX];
  m_keyval[gIX]       = m_keyval[largestIX];
  m_key[largestIX] = btemp;
  m_keyval[largestIX] = ftemp;
  heapify(largestIX);
  return(true);
}



//--------------------------------------------------------------
// Procedure: print

void PQueue::print()
{
  if((m_levels <= 0) || (m_end_ix < 0)) {
    cout << "Empty/Null Priority Queue." << endl;
    return;
  }
  while(m_key[0] != -1) {
    cout << "[" << m_key[0] << "] ";
    cout << "[" << m_keyval[0] << "] " << endl;
    removeBest();
  }
}

//--------------------------------------------------------------
// Procedure: printLiteral

void PQueue::printLiteral()
{
  if((m_levels <= 0) || (m_end_ix < 0)) {
    cout << "Empty/Null Priority Queue." << endl;
    return;
  }
  for(int i=0; i<=m_end_ix; i++) {
    cout << "[" << m_key[i] << "] ";
    cout << "[" << m_keyval[i] << "] " << endl;
  }
}









