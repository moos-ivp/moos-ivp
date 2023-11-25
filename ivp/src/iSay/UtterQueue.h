/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: UtterQueue.h                                         */
/*    DATE: May 28th, 2013                                       */
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

#ifndef UTTER_PRIORITY_QUEUE
#define UTTER_PRIORITY_QUEUE

#include "Utterance.h"
#include <vector>
#include <queue>

class UtterQueue {
 public:
  UtterQueue();
  virtual ~UtterQueue() {}

  void push(Utterance utter);
  void pushTop(Utterance utter);

  Utterance pop();

  void clear();
  bool empty() const {return(m_pqueue.empty());}

  unsigned int size()  const {return(m_pqueue.size());}
  
  std::vector<Utterance> getCopyAllEntries() const;

 private:
  std::priority_queue<Utterance, std::vector<Utterance>, 
    std::greater<std::vector<Utterance>::value_type> > m_pqueue;

  unsigned int m_total_entries_ever;
};

#endif 






