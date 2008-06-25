/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: RT_AutoPeak.cpp                                      */
/*    DATE: Jun 21st, 2008                                       */
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

#include "RT_AutoPeak.h"
#include "BuildUtils.h"
#include "Regressor.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

RT_AutoPeak::RT_AutoPeak(Regressor *g_reg) 
{
  m_regressor = g_reg;
}

//-------------------------------------------------------------
// Procedure: create
//   Purpose: Augment the given PDMap with an *additional* number
//            of pieces based on the given priority queue, which 
//            stores pieces prioritized based on the poorness of
//            regression fit during the phase when uniform pieces
//            were constructed.

PDMap* RT_AutoPeak::create(PDMap *pdmap)
{
  if(!pdmap)
    return(0);

  int dim   = pdmap->getDim();
  int psize = pdmap->size();

  PQueue pqueue(8,true);

  for(int i=0; i<psize; i++) {
    double pmax = pdmap->getBox(i)->maxVal();
    pqueue.insert(i, pmax);
  }

  vector<IvPBox*> newboxes;

  bool done = false;
  while(!done) {
    int max_index = pqueue.removeBest();
    IvPBox *cut_box;
    if(max_index < psize) 
      cut_box = pdmap->bx(max_index);
    else
      cut_box = newboxes[max_index-psize];
    
    if(cut_box->isPtBox())
      done = true;
    else {
      
      // Find the longest dimension the cut_box to split on
      int sdim_ix = 0;
      int sdim_sz = (cut_box->pt(0,1) - cut_box->pt(0,0)) + 1;
      for(int d=1; d<dim; d++) {
	int sz = (cut_box->pt(d,1) - cut_box->pt(d,0)) + 1;
	if(sz > sdim_sz) {
	  sdim_sz = sz;
	  sdim_ix = d;
	}
      }
      
      // Now cut the box along the longest dimension
      // 
      // If the piece interior function is linear (degree==1), then
      // be smart about the split. Likely making the split near the 
      // high end will result in a quicker convergence to isolating
      // the maximum point in the function. The "quarterBox" function 
      // will split a box with roughly a 2:1 ratio rather then the 
      // cutBox which splits with roughly a 1;1 ratio. The decision
      // on which way to skew the "quartering" is based on the slope
      // of the coefficient for the chosen dimension. Slopes less than
      // one are going "down", slopes greater than one are going "up".
      IvPBox *new_box = 0;
      if(m_regressor->getDegree()==1) {
	bool split_high = true;
	if(cut_box->wt(sdim_ix) < 1)
	  split_high = false;
	new_box = quarterBox(cut_box, sdim_ix, split_high);
      }
      else
	new_box = cutBox(cut_box, sdim_ix);

      // If no errors, set the new weights, add back to the pqueue
      if(new_box) {
	m_regressor->setWeight(cut_box, false);
	m_regressor->setWeight(new_box, false);
	

	// Now update the PQueue if appropriate
	pqueue.insert(max_index, cut_box->maxVal());
	pqueue.insert(psize+newboxes.size(), new_box->maxVal());
	newboxes.push_back(new_box);
      }
    }
  }

  int amt = newboxes.size();
  pdmap->growBoxArray(amt);
  for(int k=0; k<amt; k++) {
    pdmap->bx(psize+k) = newboxes[k];
    pdmap->growBoxCount();
  }
  
  pdmap->updateGrid();
  return(pdmap);
}
