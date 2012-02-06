/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Path.h                                               */
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

#ifndef CBBA_PATH_HEADER
#define CBBA_PATH_HEADER

#include <list>
#include <map>

class Path
{
 public:
  Path(unsigned int ms=100);
  virtual ~Path() {};

  void setMaxTasks(unsigned int v) {m_max_size=v;};

  bool addTask(unsigned int, double time=0);
  bool removeTask(unsigned int);

  void print() const;

  void clear() {m_tasks.clear();};

 protected: 

  bool m_dups_ok;

  std::list<unsigned int> m_tasks; 

  std::map<unsigned int, double> m_times;

  unsigned int m_max_size;

};

#endif 

