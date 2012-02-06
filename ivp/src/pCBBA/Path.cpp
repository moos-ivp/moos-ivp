/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Path.cpp                                             */
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

#include <iostream>
#include "Path.h"

using namespace std;

//------------------------------------------------------------
// Constructor

Path::Path(unsigned int max_size)
{
  m_max_size = max_size;
  m_dups_ok  = false;
}

//------------------------------------------------------------
// Procedure: addTask

bool Path::addTask(unsigned int index, double task_time)
{
  if(m_tasks.size() >= m_max_size)
    return(false);

  if(!m_dups_ok) {
    list<unsigned int>::iterator p;
    for(p=m_tasks.begin(); p!=m_tasks.end(); p++) {
      if(index == *p)
	return(false);
    }
  }

  m_tasks.push_back(index);
  if(task_time > 0)
    m_times[index] = task_time;
  return(true);
}

//------------------------------------------------------------
// Procedure: removeTask

bool Path::removeTask(unsigned int index)
{
  bool found = false;

  list<unsigned int>::iterator p;
  for(p=m_tasks.begin(); p!=m_tasks.end();) {
    if(*p == index) {
      p = m_tasks.erase(p);
      found = true;
    }
    else
      ++p;
  }

  return(found);
}

//------------------------------------------------------------
// Procedure: print()

void Path::print() const
{
  cout << "PATH: Total Number of Tasks:" << m_tasks.size() << endl;

  unsigned int i=0;
  list<unsigned int>::const_iterator p;
  for(p=m_tasks.begin(); p!=m_tasks.end(); p++) {
    cout << "[" << i << "]: " << *p << endl;
    i++;
  }
}
