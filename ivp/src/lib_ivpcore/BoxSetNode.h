/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BoxSetNode.h                                         */
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
 
#ifndef BOXSETNODE_HEADER
#define BOXSETNODE_HEADER

#include "IvPBox.h"

class IvPBox;
class BoxSetNode {
friend class BoxSet;
public:
  BoxSetNode()            {m_prev=0; m_next=0; m_box=0;}
  BoxSetNode(IvPBox *b)   {m_prev=0; m_next=0; m_box=b;}
  ~BoxSetNode() {}

  BoxSetNode *getNext()   {return(m_next);}
  BoxSetNode *getPrev()   {return(m_prev);}
  IvPBox     *getBox()    {return(m_box);}

private:
  BoxSetNode  *m_prev;
  BoxSetNode  *m_next;
  IvPBox      *m_box;
};


#endif






















































