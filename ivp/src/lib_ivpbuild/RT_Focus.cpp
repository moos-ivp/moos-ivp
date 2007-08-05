/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: RT_Focus.cpp                                         */
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

#include <assert.h>
#include "RT_Focus.h"
#include "BuildUtils.h"
#include "Regressor.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

RT_Focus::RT_Focus(Regressor *g_reg) 
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


PDMap* RT_Focus::create(PDMap *pdmap, 
			const IvPBox& region, 
			const IvPBox& unibox)
{
  if(!pdmap || !m_regressor)
    return(0);
  
  int degree = pdmap->getDegree();
  if(degree != m_regressor->getDegree())
    return(0);
  
  // First ensure that the dimensions all agree
  IvPDomain domain = pdmap->getDomain();
  int dim_pdmap = domain.size();
  int dim_region = region.getDim();
  int dim_unibox = unibox.getDim();

  if((dim_pdmap != dim_region) || (dim_region != dim_unibox))
    return(0);

  int i, j;
  int dim = dim_pdmap;

  // Then ensure the region extents are within the domain extents
  for(i=0; i<dim; i++) {
    if((region.pt(i,0) < 0) ||
       (region.pt(i,1) > domain.getVarPoints(i)) ||
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

  for(i=0; i<pdmap_size; i++) {
#if 1
    IvPBox *newbox = pdmap->getBox(i)->copy();
#endif
#if 0
    IvPBox *newbox = pdmap->bx(i);
    pdmap->bx(i) = 0;
#endif
    if(region.intersect(newbox))
      int_boxes.push_back(newbox);
    else
      non_boxes.push_back(newbox);
  }

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

  // Next create the set of uniform sized boxes in the voided region
  BoxSet* boxset = makeUniformDistro(region, unibox, degree);
  BoxSetNode *bsn = boxset->retBSN(FIRST);
  while(bsn) {
    new_boxes.push_back(bsn->getBox());
    bsn = bsn->getNext();
  }
  delete(boxset);


  int old_count = non_boxes.size();
  int new_count = new_boxes.size();

  int pcs = old_count + new_count;
  
  PDMap *new_pdmap = new PDMap(pcs, domain, degree);

  for(i=0; i<old_count; i++)
    new_pdmap->bx(i) = non_boxes[i];

  for(i=0; i<new_count; i++) {
    m_regressor->setWeight(new_boxes[i]);
    new_pdmap->bx(i+old_count) = new_boxes[i];
  }
  
  delete(pdmap);
  return(new_pdmap);
}












