/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PostingQueue.h                                       */
/*    DATE: March 23rd, 2013                                     */
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

#ifndef UHZ_CLASSIFY_PRIORITY_QUEUE
#define UHZ_CLASSIFY_PRIORITY_QUEUE

#include "ClassifyEntry.h"
#include <vector>
#include <queue>

class ClassifyQueue {
 public:
  ClassifyQueue();
  virtual ~ClassifyQueue() {}

  void push(ClassifyEntry entry);
  void pushTop(ClassifyEntry entry);
  
  ClassifyEntry pop();

  std::vector<ClassifyEntry> getCopyAllEntries() const;
  std::vector<std::string>   getStringDump() const;

  void clear();

  bool empty() const;
  unsigned int size() const;

 private:

  std::priority_queue<ClassifyEntry, std::vector<ClassifyEntry>, 
    std::greater<std::vector<ClassifyEntry>::value_type> > m_pqueue;

  ClassifyEntry m_active_entry;
  bool          m_no_active_entry;

  unsigned int  m_total_entries_ever;
};

#endif 







