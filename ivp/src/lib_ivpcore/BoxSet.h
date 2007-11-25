/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BoxSet.h                                             */
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
  int  getSize()     { return(m_size);  };
  int  size()        { return(m_size);  };

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










