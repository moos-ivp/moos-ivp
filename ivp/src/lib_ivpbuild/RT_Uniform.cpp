/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: RT_Uniform.cpp                                       */
/*    DATE:                                                      */
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

#include <iostream>
#include "RT_Uniform.h"
#include "BuildUtils.h"
#include "Regressor.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

RT_Uniform::RT_Uniform(Regressor *g_reg) 
{
  m_regressor = g_reg;
}

//-------------------------------------------------------------
// Procedure: create
//   Purpose: Make a uniform IvP function based on the given box.
//            Each uniform piece will have the same size as the
//            given box.

PDMap* RT_Uniform::create(const IvPBox* unifbox, const IvPBox* gelbox, 
			  PQueue& pqueue)
{
  if(!unifbox)
    return(0);

  bool use_pqueue = true;
  if(pqueue.null())
    use_pqueue = false;
  
  // If a using a Priority Queue, we'll use the return values in 
  // the setWeight() calls. Return value is the noted error at 
  // sampled points w.r.t. the interior function chosen. We store 
  // box-index,error pair in the priority queue, which can be used
  // by other ReflectorTools for refining boxes with high error.

  IvPDomain domain = m_regressor->getAOF()->getDomain();

  IvPBox universe = domainToBox(domain);

  int degree = m_regressor->getDegree();
  
  BoxSet *boxset = makeUniformDistro(universe, *unifbox, degree);
  int vsize = boxset->size();
  if(vsize == 0)
    return(0);
  
  PDMap *pdmap = new PDMap(vsize, domain, degree);
  BoxSetNode *bsn = boxset->retBSN(FIRST);
  int index = 0;
  while(bsn) {
    pdmap->bx(index) = bsn->getBox();
    pdmap->bx(index)->ofindex() = index;
    index++;
    bsn = bsn->getNext();
  }
  delete(boxset);

  if(gelbox)
    pdmap->setGelBox(*gelbox);
  else
    pdmap->setGelBox(*unifbox);
  
  int unifCount = pdmap->size();
  for(int i=0; i<unifCount; i++) {
    if(use_pqueue) {
      double delta = m_regressor->setWeight(pdmap->bx(i), true);
      pqueue.insert(i, delta);
    }
    else
      m_regressor->setWeight(pdmap->bx(i), false);
  }

  pdmap->updateGrid(1,1);
  
  return(pdmap);
}










