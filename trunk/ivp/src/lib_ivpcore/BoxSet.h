/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BoxSet.h                                             */
/*    DATE: Too long ago to remember (1996-1999)                 */
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

// Test change

#ifndef BOXSET_HEADER
#define BOXSET_HEADER

#include "BoxSetNode.h"

#define FIRST 0
#define LAST  1

class BoxSet {
public:
  BoxSet();
  ~BoxSet();

  void makeEmpty();
  void makeEmptyAndDeleteBoxes();
  int  getSize()     { return(m_size); }
  int  size()        { return(m_size); }

  void        addBox(IvPBox*, int end=FIRST);
  void        addBSN(BoxSetNode&, int end=FIRST);
  BoxSetNode *retBSN(int end=FIRST);
  BoxSetNode *remBSN(int end=FIRST);
  void        remBSN(BoxSetNode*);

  void  merge(BoxSet&);
  void  mergeCopy(BoxSet&);
  void  print();
  void  removeDups();

private:
  BoxSetNode *m_head;
  BoxSetNode *m_tail;
  int         m_size;
};
#endif



















