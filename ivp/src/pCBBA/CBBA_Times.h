/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CBBA_Times.h                                         */
/*    DATE: Aug 17th, 2011 (Waterloo)                            */
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

#ifndef CBBA_TIMES_HEADER
#define CBBA_TIMES_HEADER

#include <list>

class CBBA_Times
{
 public:
  CBBA_Times(unsigned int max_size=100);
  virtual ~CBBA_Times() {};

  void setMaxTasks(unsigned int v) {m_max_size=v;};

  bool addTask(double, bool ok_dups=false);
  bool removeTask(double);

  void print() const;

  void clear() {m_tasks.clear();};

 protected: 

  std::list<unsigned int> m_tasks; 

  unsigned int m_max_size;

};

#endif 

