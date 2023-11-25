/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BoxSet.cpp                                           */
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

#include <iostream>
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




















