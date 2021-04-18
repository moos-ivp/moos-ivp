/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RT_Directed.cpp                                      */
/*    DATE: Mar 20th, 2007                                       */
/*    NOTE: "RT_" stands for "Reflector Tool"                    */
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

#include "RT_Directed.h"
#include "BuildUtils.h"

#include <iostream>

using namespace std;

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
			   const IvPBox& unibox)
{
  if(m_verbose)
    cout << "================ RT_Directed ===============" << endl;
  if(!pdmap)
    return(0);
  
  int degree = pdmap->getDegree();
  
  // First ensure that the dimensions all agree
  IvPDomain domain = pdmap->getDomain();
  int dim_pdmap  = domain.size();
  int dim_region = region.getDim();
  int dim_unibox = unibox.getDim();

  if((dim_pdmap != dim_region) || (dim_region != dim_unibox))
    return(0);

  int dim = dim_pdmap;

  // Then ensure the region extents are within the domain extents
  for(int i=0; i<dim; i++) {
    if((region.pt(i,0) < 0) ||
       (region.pt(i,1) > (int)(domain.getVarPoints(i))) ||
       (region.pt(i,0) > region.pt(i,1)))
      return(0);
  }
  
  // Then ensure the unibox extents are within the region extents
  for(int i=0; i<dim; i++) {
    if((unibox.pt(i,1) - unibox.pt(i,0)) > (region.pt(i,1) - region.pt(i,0)))
      return(0);
  }
 
  int pdmap_size = pdmap->size();

  if(m_verbose) {
    cout << "Region:" << endl;
    region.print();
  }
  
  
  // Next create a copy of all the boxes in the PDMap sorting them 
  // based on whether they intersect the given region.
  vector<IvPBox*> non_boxes;
  vector<IvPBox*> int_boxes;
  for(int i=0; i<pdmap_size; i++) {
    IvPBox *newbox = pdmap->bx(i);
    pdmap->bx(i) = 0;
    if(region.intersect(newbox))
      int_boxes.push_back(newbox);
    else
      non_boxes.push_back(newbox);
  }
  if(m_verbose) {
    cout << "Non-Boxes: " << non_boxes.size() << endl;
    cout << "Int-Boxes: " << int_boxes.size() << endl;
  }
  
  // Next subtract the region box from all boxes that intersect it.  
  vector<IvPBox*> new_boxes;

  for(unsigned int i=0; i<int_boxes.size(); i++) {
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

  if(m_verbose)
    cout << "New-Boxes: " << new_boxes.size() << endl;
  
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

  for(int i=0; i<old_count; i++)
    new_pdmap->bx(i) = non_boxes[i];
  for(int i=0; i<new_count; i++)
    new_pdmap->bx(i+old_count) = new_boxes[i];
  
  delete(pdmap);
  return(new_pdmap);
}






