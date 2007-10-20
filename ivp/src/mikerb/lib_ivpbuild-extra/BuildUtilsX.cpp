/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BuildUtilsX.cpp                                      */
/*    DATE: May 15th, 2003                                       */
/*    DATE: July 5th 2007 (Separated from BuildUtils)            */
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

#include <iostream>
#include <assert.h>
#include <math.h>
#include "BuildUtils.h"
#include "BuildUtilsX.h"
#include "MBUtils.h"
#include "BoxSet.h"

using namespace std;

//------------------------------------------------------------------
// Procedure: cluster
//   Purpose: Take each box in boxes[] with the given value and 
//            "cluster" them into (likely) larger boxes to reduce
//            the overall size of boxes[] to improve efficiency
//            WITHOUT a loss of precision.

void cluster(PDMap *pdmap, double val1, double val2, double newval, 
	     const IvPBox& unifbox)
{
  int saveCount = 0;
  int boxCount  = pdmap->size();

  for(int i=0; i<boxCount; i++)           // First pass just to count
    if((pdmap->bx(i)->maxVal() < val1) || // number of boxes to save.
       (pdmap->bx(i)->maxVal() > val2)) {
      pdmap->bx(saveCount) = pdmap->bx(i);
      saveCount++;
    }
  pdmap->growBoxCount(saveCount-boxCount);
  pdmap->updateGrid();

  fillVoid(pdmap, newval, unifbox);    
  pdmap->updateGrid();
}

//------------------------------------------------------------------
// Procedure: fillVoid
//      Note: Assumes the given pdmap


void fillVoid(PDMap *pdmap, double voidval, const IvPBox& unifBox)
{
  int  i, degree = pdmap->getDegree();
  IvPDomain domain = pdmap->getDomain();
  IvPGrid*   grid  = pdmap->getGrid();


  // Create a "void" pdmap containing the pieces to fill into the void
  IvPBox universe = domainToBox(domain);

  BoxSet* boxset = makeUniformDistro(universe, unifBox, degree);
  int     usize = boxset->size();
  
  PDMap *vpdmap = new PDMap(usize, domain, degree);
  BoxSetNode* bsn = boxset->retBSN(FIRST);
  int index = 0;
  while(bsn) {
    pdmap->bx(index) = bsn->getBox();
    index++;
    bsn = bsn->getNext();
  }
  delete(boxset);

  BoxSet voidboxes;
  int bc = vpdmap->size();
  for(i=0; i<bc; i++) {
    vpdmap->bx(i)->setWT(voidval);
    //voidboxes.addBox(vpdmap->bx(i)->copy());
    IvPBox *newbox = new IvPBox(*(vpdmap->bx(i)));
    voidboxes.addBox(newbox);
  }
  delete(vpdmap);

  BoxSet newboxes;
  bool done = false;
  while(!done) {
    // Pop a voidbox - it will either be broken down with the
    // new boxes added back to the voidboxes, or added to the 
    // newboxes set if it doesn't intersect with any box in
    // the original pdmap.
    bsn = voidboxes.remBSN(FIRST);
    IvPBox *vbox = bsn->getBox();
    delete(bsn);
    
    // Get the set of boxes from original pdmap that intersect
    // with the voidbox.
    BoxSet *iboxes = grid->getBS(vbox);
    if(iboxes->getSize()==0)
      newboxes.addBox(vbox);
    else {
      IvPBox *ibox = iboxes->retBSN()->getBox();

      BoxSet *boxset = subtractBox(*vbox, *ibox);
      BoxSetNode *bsn = boxset->retBSN(FIRST);
      while(bsn) {
	voidboxes.addBox(bsn->getBox(), LAST);
	bsn = bsn->getNext();
      }
      delete(boxset);
    }

    if(voidboxes.getSize() == 0)
      done = true;
  }

  // Now we have all the new boxes that fill the void. Just 
  // need to grow the pdmap and add them to the pdmap.
  pdmap->growBoxArray(newboxes.getSize());
  bsn = newboxes.retBSN();
  int boxCount = pdmap->size();
  while(bsn) {
    pdmap->bx(boxCount) = bsn->getBox();
    boxCount++;
    bsn = bsn->getNext();
  }
  pdmap->growBoxCount(newboxes.getSize());
}
