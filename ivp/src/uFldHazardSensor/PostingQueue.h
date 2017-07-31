/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PostingQueue.h                                       */
/*    DATE: March 15th, 2013                                     */
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

#ifndef POSTING_PRIORITY_QUEUE
#define POSTING_PRIORITY_QUEUE

#include "VarDataPair.h"
#include <vector>
#include <queue>

class PostingQueue {
 public:
  PostingQueue();
  virtual ~PostingQueue() {}

  void push(VarDataPair pair);
  
  VarDataPair pop();

  bool empty() const {return(m_pqueue.empty());}
  bool size()  const {return(m_pqueue.size());}

  std::priority_queue<VarDataPair, std::vector<VarDataPair>, std::less<std::vector<VarDataPair>::value_type> > m_pqueue;

  unsigned int m_total_entries_ever;

};

#endif 







