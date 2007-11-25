/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BoxSet.cpp                                           */
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

#include <iostream>
#include <assert.h>
#include "BoxSet.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BoxSet::BoxSet()
{
  m_head = 0; 
  m_tail = 0; 
  m_size = 0; 
}

//---------------------------------------------------------------
// Destructor

BoxSet::~BoxSet()
{
  BoxSetNode *thisBSN = m_head;
  while(thisBSN != 0) {
    BoxSetNode *next = thisBSN->m_next;
    delete(thisBSN);
    thisBSN = next;
  }
}

//---------------------------------------------------------------
// Procedure: makeEmpty
//   Purpose: o Remove and delete all BSNs in the set, but
//              do NOT delete any of the boxes.

void BoxSet::makeEmpty()
{
  BoxSetNode *aBSN = m_head;
  BoxSetNode *nextBSN = 0;
  while(aBSN != 0) {
    nextBSN = aBSN->m_next;
    delete(aBSN);
    aBSN = nextBSN;
  }      
  m_head = 0; 
  m_tail = 0; 
  m_size = 0; 
} 

//---------------------------------------------------------------
// Procedure: makeEmptyAndDeleteBoxes
//   Purpose: o Remove and delete all BSNs in the set, and 
//              DO delete all boxes as well.

void BoxSet::makeEmptyAndDeleteBoxes()
{
  BoxSetNode *aBSN = m_head;
  BoxSetNode *nextBSN = 0;
  while(aBSN != 0) {
    nextBSN = aBSN->m_next;
    delete(aBSN->getBox());
    delete(aBSN);
    aBSN = nextBSN;
  }
  m_head=0; 
  m_tail=0; 
  m_size=0; 
} 

//---------------------------------------------------------------
// Procedure: addBox
//   Purpose: o Create and add a BSN to the set.
//            o The new BSN contains the given box. 

void BoxSet::addBox(IvPBox* b, int end)
{
  BoxSetNode *bsn = new BoxSetNode(b);
  if(end==FIRST)
    addBSN(*bsn, FIRST);
  else
    addBSN(*bsn, LAST);
}

//---------------------------------------------------------------
// Procedure: addBSN
//   Purpose: o Add the given BSN to the set. 

void BoxSet::addBSN(BoxSetNode& node, int end)
{
  if(end==FIRST) {
    if(m_head != 0)
      m_head->m_prev = &node;
    node.m_prev = 0;
    node.m_next = m_head;
    m_head = &node;
    if(m_tail == 0)
      m_tail = &node;
  }
  else {
    if(m_tail != 0)
      m_tail->m_next = &node;
    node.m_prev = m_tail;
    node.m_next = 0;
    m_tail      = &node;
    if(m_head == 0)
      m_head = &node;
  }
  m_size++;
}

//---------------------------------------------------------------
// Procedure: retBSN
//   Purpose: o Return a pointer to the BSN at the chosen end.
//            o The contents of this BoxSet are NOT affected.

BoxSetNode *BoxSet::retBSN(int end)
{
  if(end == FIRST) 
    return(m_head);
  else
    return(m_tail);
}

//---------------------------------------------------------------
// Procedure: remBSN
//   Purpose: o Remove and Return a pointer to the BSN at the 
//              chosen end.
//            o The BSN is NOT deleted. Perhaps by the user.
//            o The Box in the BSN is NOT affected.
//            o Returns 0 if BoxSet is empty.

BoxSetNode *BoxSet::remBSN(int end)
{
  BoxSetNode *ret = 0;
  if(end==FIRST) {
    if(m_head == 0) 
      return(0);
    ret = m_head;
    m_head = m_head->m_next;
    if(m_head == 0)          // There was only one in the Set
      m_tail = 0;
    else
      m_head->m_prev = 0;
    ret->m_next = 0;
  }
  else {
    if(m_tail == 0)
      return(0);
    ret = m_tail;
    m_tail = m_tail->m_prev;
    if(m_tail == 0)         // There was only one in the Set
      m_head = 0;
    else
      m_tail->m_next = 0;
    ret->m_prev = 0;
  }
  m_size--;
  return(ret);
}

//---------------------------------------------------------------
// Procedure: remBSN
//   Purpose: o Remove the BSN from the set. 
//            o The BSN is NOT deleted. Perhaps by the user.
//            o The Box in the BSN is NOT affected.

void BoxSet::remBSN(BoxSetNode* bsn)
{
  if(bsn == m_head) {
    remBSN(FIRST);     // m_size decremented within
    return;
  }
  if(bsn == m_tail) {
    remBSN(LAST);      // m_size decremented within
    return;
  }
  // else
  bsn->m_prev->m_next = bsn->m_next;
  bsn->m_next->m_prev = bsn->m_prev;
  m_size--;
}

//---------------------------------------------------------------
// Procedure: merge
//   Purpose: o Attach the given BoxSet to the end of THIS BoxSet.
//            o The given BoxSet will be empty afterwards.

void BoxSet::merge(BoxSet& bs)
{
  m_size += bs.getSize();
  bs.m_size = 0;
  if(m_head == 0) {    // if THIS boxset is empty
    m_head = bs.m_head;
    m_tail = bs.m_tail;
    bs.m_tail = 0;
    bs.m_head = 0;
    return;
  }
  if(bs.m_head == 0)   // if given BS is empty
    return;

  m_tail->m_next = bs.m_head;   // otherwise....
  bs.m_head->m_prev = m_tail;
  m_tail    = bs.m_tail;
  bs.m_tail = 0;
  bs.m_head = 0;
}
    
//---------------------------------------------------------------
// Procedure: mergeCopy
//   Purpose: o Attach COPY of given BoxSet to end of THIS BoxSet.
//            o Differs from "merge" in that given BoxSet not changed.

void BoxSet::mergeCopy(BoxSet& bs)
{
  if(bs.getSize() == 0)
    return;

  BoxSetNode *newBSN;
  BoxSetNode *cBSN = bs.retBSN(FIRST);
  while(cBSN != 0) {
    m_size++;
    if(!m_head) {                  // case where THIS BoxSet is empty
      newBSN = new BoxSetNode(cBSN->getBox());
      m_head = newBSN;
      m_tail = newBSN;
    }
    else {
      newBSN = new BoxSetNode(cBSN->getBox());
      m_tail->m_next = newBSN;
      newBSN->m_prev = m_tail;
	m_tail = newBSN;
    }
    cBSN = cBSN->m_next;
  }
}   

//---------------------------------------------------------------
// Procedure: subtractFrom
//   Purpose: Subtract THIS BoxSet from the given box (b), and 
//            return the difference as a new BoxSet.

#if 0
BoxSet *BoxSet::subtractFrom(IvPBox *b) 
{ 
  BoxSet *intBS = new BoxSet;
  intBS->addBox(b->copy());
  BoxSetNode *tbsn = this->m_head;
  while(tbsn != 0) {
    IvPBox *tbox = tbsn->getBox();
    BoxSet *nextIntBS = new BoxSet;
    BoxSetNode *ibsn = intBS->retBSN();
    while(ibsn != 0) {
      IvPBox *ibox = ibsn->getBox();
      //BoxSet *resultBS = subtractBox(ibox, tbox);
      BoxSet *resultBS = 0;
      nextIntBS->merge(*resultBS);
      delete(resultBS);
      ibsn = ibsn->getNext();
    }
    delete(intBS);
    intBS = nextIntBS;
    tbsn = tbsn->getNext();
  }
  return(intBS);
}
#endif

//---------------------------------------------------------------
// Procedure: print
//   Purpose: 

void BoxSet::print() 
{ 
  BoxSetNode *current = m_head;
  if(current == 0)
    cout << "** empty list **" << endl;
 
  while(current != 0) {
    current->m_box->print();
    current = current->m_next;
  }
}

//---------------------------------------------------------------
// Procedure: removeDups
//   Purpose: Remove and delete all BoxSetNodes containing 
//            duplicate boxes. Will remove the second occurance
//            of any duplicate.

void BoxSet::removeDups()
{
  BoxSetNode *bsn = m_head;
  while(bsn!=0) {                    // Clear marks on 
    bsn->getBox()->mark() = false;      // all boxes so we
    bsn = bsn->getNext();               // can remove dups.
  }

  bsn = m_head;                           // Remove duplicates
  while(bsn!=0) {                       // by marking each
    BoxSetNode *nextbsn=bsn->getNext(); // on first encounter
    IvPBox *abox = bsn->getBox();       // and removing on
    if(abox->mark()) {                  // second encounter.
      this->remBSN(bsn);
      delete(bsn);
    }
    abox->mark() = true;
    bsn = nextbsn;  
  }
}










