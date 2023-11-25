/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RT_Uniform.cpp                                       */
/*    DATE: Aug 26th 2005 (from OFR_AOF written long ago)        */
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

#include "RT_Uniform.h"
#include "BuildUtils.h"
#include "Regressor.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: create
//   Purpose: Make a uniform IvP function based on the given box.
//            Each uniform piece will have the same size as the
//            given box.

PDMap* RT_Uniform::create(const IvPBox* unifbox, const IvPBox* gelbox)
{
  if(!unifbox)
    return(0);

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
    index++;
    bsn = bsn->getNext();
  }
  delete(boxset);

  bool gridset = false;
  if(gelbox)
    gridset = pdmap->setGelBox(*gelbox);
  
  if(!gridset)
    pdmap->setGelBox(*unifbox);
  

  // Do later, after directed refinement done?
  // pdmap->updateGrid(1,1);
  
  return(pdmap);
}

