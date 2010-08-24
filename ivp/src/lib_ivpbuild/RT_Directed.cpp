/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: RT_Directed.cpp                                      */
/*    DATE: Mar 20th, 2007                                       */
/*    NOTE: "RT_" stands for "Reflector Tool"                    */
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

#include "RT_Directed.h"
#include "BuildUtils.h"
#include "Regressor.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

RT_Directed::RT_Directed(Regressor *g_reg) 
{
  m_regressor = g_reg;
}

//-------------------------------------------------------------
// Procedure: create
//   Purpose: Refine the given PDMap to be uniform in the region
//            provided. The uniformity in this region is given by
//            the size of the unibox.
//      Note: The given PDMap will be destroyed in this process 
//            and a new PDMap created and returned. Some of the
//            boxes from the old PDMap will be stolen and used in
//            the new PDmap. 


PDMap* RT_Directed::create(PDMap *pdmap, const IvPBox& region, 
			   const IvPBox& unibox, PQueue& pqueue)
{
  if(!pdmap || !m_regressor)
    return(0);
  
  int degree = pdmap->getDegree();
  if(degree != m_regressor->getDegree())
    return(0);
  
  // First ensure that the dimensions all agree
  IvPDomain domain = pdmap->getDomain();
  int dim_pdmap  = domain.size();
  int dim_region = region.getDim();
  int dim_unibox = unibox.getDim();

  if((dim_pdmap != dim_region) || (dim_region != dim_unibox))
    return(0);

  int i;
  int dim = dim_pdmap;

  // Then ensure the region extents are within the domain extents
  for(i=0; i<dim; i++) {
    if((region.pt(i,0) < 0) ||
       (region.pt(i,1) > (int)(domain.getVarPoints(i))) ||
       (region.pt(i,0) > region.pt(i,1)))
      return(0);
  }
  
  // Then ensure the unibox extents are within the region extents
  for(i=0; i<dim; i++) {
    if((unibox.pt(i,1) - unibox.pt(i,0)) > (region.pt(i,1) - region.pt(i,0)))
      return(0);
  }
 
  int pdmap_size = pdmap->size();
  
  // Next create a copy of all the boxes in the PDMap sorting them 
  // based on whether they intersect the given region.
  vector<IvPBox*> non_boxes;
  vector<IvPBox*> int_boxes;
  vector<IvPBox*> new_boxes;

  // The idx_map is an mapping from the index in the given PDMap to
  // the index in the soon-to-be-created PDMap, only for those pieces 
  // that do not intersect the region box. This mapping will be used
  // to preserve the PQueue information. It assumes that these N boxes
  // that do not intersect the region box will be at indices [0]-[n-1]
  // in the new PDMap.
  vector<int> idx_map;
  idx_map.resize(pdmap_size);

  for(i=0; i<pdmap_size; i++) {

    // Older version did this - unnecessary work
    // IvPBox *newbox = pdmap->getBox(i)->copy();
    
    IvPBox *newbox = pdmap->bx(i);
    pdmap->bx(i) = 0;
    if(region.intersect(newbox)) {
      int_boxes.push_back(newbox);
      idx_map[i] = -1;
    }
    else {
      non_boxes.push_back(newbox);
      idx_map[i] = non_boxes.size()-1;
    }
  }

  //---------------------------------
  if(!pqueue.null())
    updatePQueue(pqueue, idx_map);
  //---------------------------------
  
  // Next subtract the region box from all boxes that intersect it.  
  int vsize = int_boxes.size();
  for(i=0; i<vsize; i++) {
    IvPBox *ibox = int_boxes[i];
    BoxSet *boxset = subtractBox(*ibox, region);
    delete(ibox);
    BoxSetNode *bsn = boxset->retBSN(FIRST);
    while(bsn) {
      new_boxes.push_back(bsn->getBox());
      bsn = bsn->getNext();
    }
    delete(boxset);
  }

#if 1
  // Next create the set of uniform sized boxes in the voided region
  BoxSet* boxset = makeUniformDistro(region, unibox, degree);
  BoxSetNode *bsn = boxset->retBSN(FIRST);
  while(bsn) {
    new_boxes.push_back(bsn->getBox());
    bsn = bsn->getNext();
  }
  delete(boxset);
#endif

  int old_count = non_boxes.size();
  int new_count = new_boxes.size();


  int pcs = old_count + new_count;
  
  PDMap *new_pdmap = new PDMap(pcs, domain, degree);

  for(i=0; i<old_count; i++)
    new_pdmap->bx(i) = non_boxes[i];

  for(i=0; i<new_count; i++) {
    if(!pqueue.null()) {
      double delta = m_regressor->setWeight(new_boxes[i], true);
      pqueue.insert(i+old_count, delta);
    }
    else
      m_regressor->setWeight(new_boxes[i], false);
    
    new_pdmap->bx(i+old_count) = new_boxes[i];
  }
  
  delete(pdmap);
  return(new_pdmap);
}

//-------------------------------------------------------------
// Procedure: updatePQueue
//   Purpose: Modify the Priority Queue to contain only boxes that
//            do not intersect the given region. 
// Algorithm: Create a new priority queue to replace existing one.
//            Pop each element of the existing queue, which is an 
//             index in the given PDMap. Get the IvPBox related to
//             that index, see if it intersects the region. If not,
//             add the queue key and keyval to the new PQueue. 
//            When done, simply delete the old queue, and replace 
//             with the new queue.


void RT_Directed::updatePQueue(PQueue& pqueue, const vector<int>& idx_map)
{
  // If there is no priority queue being used - just return.
  if(pqueue.null())
    return;

  // Get the important constructor info of the existing queue
  int  levels   = pqueue.getLevels();
  bool sort_max = pqueue.isSortByMax();
  
  // Make an empty queue of the same size as the existing one
  // For filling in with the boxes not intesecting the region box.
  PQueue new_pqueue(levels, sort_max);
  
  int  msize = idx_map.size();  

  // Get the top key and keyval of the current queue. Always have
  // to get the keyval first because by popping the top key, access
  // to the top keyval is lost.
  double q_keyval = pqueue.returnBestVal();
  int    q_key    = pqueue.removeBest();
  
  // When the queue is eventually empty, it will return -1
  while(q_key != -1) {
    if((q_key > 0) && (q_key < msize)) {
      int new_key = idx_map[q_key];
      if(new_key != -1)
	new_pqueue.insert(new_key, q_keyval);
    }
    q_keyval = pqueue.returnBestVal();
    q_key    = pqueue.removeBest();   
  }
  
  pqueue = new_pqueue;
}
  
