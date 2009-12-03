/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BoxSetNode.h                                         */
/*    DATE: Too long ago to remember (1996-1999)                 */
/*                                                               */
/* (IvP) The interval programming model is a mathematical        */
/* programming model for representing and solving multi-         */
/* objective optimization problems.                              */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/
 
#ifndef BOXSETNODE_HEADER
#define BOXSETNODE_HEADER

#include "IvPBox.h"

class IvPBox;
class BoxSetNode {
friend class BoxSet;
public:
  BoxSetNode()            {m_prev=0; m_next=0; m_box=0;};
  BoxSetNode(IvPBox *b)   {m_prev=0; m_next=0; m_box=b;};
  ~BoxSetNode() {};

  BoxSetNode *getNext()   {return(m_next);};
  BoxSetNode *getPrev()   {return(m_prev);};
  IvPBox     *getBox()    {return(m_box);};

private:
  BoxSetNode  *m_prev;
  BoxSetNode  *m_next;
  IvPBox      *m_box;
};


#endif













































