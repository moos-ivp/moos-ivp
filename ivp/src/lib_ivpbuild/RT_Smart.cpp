/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RT_Smart.cpp                                         */
/*    DATE: Jan 20th, 2006                                       */
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

#include <iostream>
#include "RT_Smart.h"
#include "BuildUtils.h"
#include "Regressor.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

RT_Smart::RT_Smart(Regressor *g_reg) 
{
  m_regressor = g_reg;
  m_verbose   = false;
}

//-------------------------------------------------------------
// Procedure: create
//   Purpose: Augment the given PDMap with an *additional* number
//            of pieces based on the given priority queue, which 
//            stores pieces prioritized based on the poorness of
//            regression fit during the phase when uniform pieces
//            were constructed.

PDMap* RT_Smart::create(PDMap *pdmap, PQueue& pqueue, 
			int amt, double thresh)
{
  if(m_verbose)
    cout << "================ RT_Directed ===============" << endl;

  if(!pdmap || pqueue.null() || (amt < 1))
    return(pdmap);

  if(m_verbose)
    cout << "Pieces begin: " << pdmap->size() << endl;
  
  pdmap->growBoxArray(amt);
  int dim = pdmap->getDim();

  double worst_err = pqueue.returnBestVal();
  int    worst_box = pqueue.removeBest();
  while((amt > 0) && (worst_box != -1) && (worst_err > thresh)) {

    IvPBox *cut_box = pdmap->bx(worst_box);

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

    IvPBox *new_box = cutBox(cut_box, sdim_ix);

    if(new_box) {
      double err1 = m_regressor->setWeight(cut_box, true);
      double err2 = m_regressor->setWeight(new_box, true);

#if 0 // mikerb aug16
      double maxval1 = cut_box->maxval();
      double minval1 = cut_box->minval();
      double delta1  = maxval1 - minval1;

      double maxval2 = new_box->maxval();
      double minval2 = new_box->minval();
      double delta2  = maxval2 - minval2;
      
#endif
      
      int newix = pdmap->size();
      pdmap->bx(newix) = new_box;
      pdmap->growBoxCount();
      
      // Now update the PQueue if appropriate
      if(!cut_box->isPtBox())
	pqueue.insert(worst_box, err1);
      if(!new_box->isPtBox())
	pqueue.insert(newix, err2);
      amt--;
    }
    worst_err = pqueue.returnBestVal();
    worst_box = pqueue.removeBest();
  }

  pdmap->updateGrid();

  if(m_verbose)
    cout << "Pieces end:   " << pdmap->size() << endl;

  return(pdmap);
}



















