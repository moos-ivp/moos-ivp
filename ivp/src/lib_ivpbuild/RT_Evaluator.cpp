/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RT_Evaluator.cpp                                     */
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

#include <iostream>
#include "RT_Evaluator.h"
#include "BuildUtils.h"
#include "Regressor.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

RT_Evaluator::RT_Evaluator(Regressor *regressor) 
{
  m_regressor = regressor;
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


void RT_Evaluator::evaluate(PDMap *pdmap, PQueue& pqueue)
{
  // Sanity Checks
  if(!pdmap || !m_regressor)
    return;
  if(pdmap->getDomain().size() != m_regressor->getAOF()->getDim())
    return;

  // If PQueue is null, just set piece weights
  if(pqueue.null()) {
    for(int i=0; i<pdmap->size(); i++) 
      m_regressor->setWeight(pdmap->bx(i), false);
  }
  // If PQueue is not null, set weights, calc delta, add to PQueue
  else {
    for(int i=0; i<pdmap->size(); i++) {
      double delta = m_regressor->setWeight(pdmap->bx(i), true);
      pqueue.insert(i, delta);
    }
  }
}



