/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PQueue.h                                             */
/*    DATE: December 8th, 2000                                   */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef PQUEUE_HEADER
#define PQUEUE_HEADER

#include <vector>

class PQueue { 
public:
  PQueue(int levels=0, bool max=true);  
  ~PQueue() {}

  void    insert(int key, double keyval);

  int     getLevels()    {return(m_levels);}
  bool    isSortByMax()  {return(m_sort_by_max);}
  bool    null()         {return(m_levels == 0);}

  int     removeBest();
  double  returnBestVal();

  // size() returns number of elements in the priority queue, not
  // the size of the priority queue data structure (m_size).
  int     size()             {return(m_end_ix+1);}

public: // Debugging
  void    print();
  void    printLiteral();

protected:
  int  left(int ix)          {return((2*ix)+1);}
  int  right(int ix)         {return((2*ix)+2);}
  int  parent(int ix)        {return((ix-1)/2);}
  bool heapify(int ix);

protected:
  std::vector<int>    m_key;
  std::vector<double> m_keyval;

  int      m_levels;
  int      m_end_ix;       // index of last active element
  bool     m_sort_by_max;  // true if max val is top priority
  int      m_size;         // size of the array
  int      m_num_leaves;   // number of leaves in full tree;
  int      m_num_inodes;   // number non-leaves in full tree;
};
#endif



















