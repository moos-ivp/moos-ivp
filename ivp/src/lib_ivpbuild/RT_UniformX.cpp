/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RT_UniformX.cpp                                      */
/*    DATE: Nov 29th 2017 (from RT_Uniform))                     */
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

#include "RT_UniformX.h"
#include "BuildUtils.h"
#include "Regressor.h"
#include <iostream>

using namespace std;

//-------------------------------------------------------------
// Procedure: create
//   Purpose: Make a uniform IvP function based on the given box.
//            Each uniform piece will have the same size as the
//            given box.

PDMap* RT_UniformX::create(const IvPBox& unifbox, const IvPBox& gelbox)
{
  if(unifbox.getDim() == 0)
    return(0);

  IvPDomain domain   = m_regressor->getAOF()->getDomain();
  IvPBox    universe = domainToBox(domain);
  int       degree   = m_regressor->getDegree();

  BoxSet *boxset = makeUniformDistro(universe, unifbox, degree);

  handleOverlappingPlatBasins();
  
  boxset = subtractPlateaus(boxset);
  boxset = subtractBasins(boxset);

  int remaining_pcs = boxset->size();
  int plateau_pcs   = (int)(m_plateaus.size());
  int basin_pcs     = (int)(m_basins.size());

  int total_pdmap_pcs = remaining_pcs + plateau_pcs + basin_pcs;
  if(total_pdmap_pcs <= 0)
    return(0);

  PDMap *pdmap = new PDMap(total_pdmap_pcs, domain, degree);
  BoxSetNode *bsn = boxset->retBSN(FIRST);
  int index = 0;
  while(bsn) {
    pdmap->bx(index) = bsn->getBox();
    index++;
    bsn = bsn->getNext();
  }
  delete(boxset);

  for(unsigned int i=0; i<m_plateaus.size(); i++) {
    pdmap->bx(index) = m_plateaus[i].copy();
    index++;
  }
  for(unsigned int i=0; i<m_basins.size(); i++) {
    pdmap->bx(index) = m_basins[i].copy();
    index++;
  }

  bool gridset = false;
  if(gelbox.getDim() != 0)
    gridset = pdmap->setGelBox(gelbox);
  
  if(!gridset)
    pdmap->setGelBox(unifbox);
  

  // Do later, after directed refinement done?
  // pdmap->updateGrid(1,1);
  
  //cout << "   pdmap size: " << pdmap->size() << endl;
  return(pdmap);
}


//------------------------------------------------------------------
// Procedure: handleOverlappingPlatBasins()
//   Purpose: Process the plateaus and basins and make sure that
//            none of them overlap.

void RT_UniformX::handleOverlappingPlatBasins()
{
  if((m_plateaus.size() + m_basins.size()) < 2)
    return;

  // Part 1: First combine the plateaus and basins into one vector
  // of boxes. They need to be together since they will all be
  // ordered based on the magnitude of the "plat value". Higher
  // magnitude plat values will overrule lower ones. 
  vector<IvPBox> boxes = m_plateaus;
  for(unsigned int i=0; i<m_basins.size(); i++)
    boxes.push_back(m_basins[i]);

  // Part 2: Do the separating if needed
  boxes = makeRegionsApart(boxes);
  
  // Part 3: Now that we are sure we have non-overlapping regions,
  // sort them again into basins and plateaus
  m_plateaus.clear();
  m_basins.clear();
  for(unsigned int i=0; i<boxes.size(); i++) {
    if(boxes[i].getPlat() < 0)
      m_basins.push_back(boxes[i]);
    else
      m_plateaus.push_back(boxes[i]);
  }
}	      

//------------------------------------------------------------------
// Procedure: subtractPlateaus()

BoxSet* RT_UniformX::subtractPlateaus(BoxSet *bset)
{
  BoxSet *retbset = bset;
  for(unsigned int i=0; i<m_plateaus.size(); i++)
    retbset = subtractPlateau(retbset, m_plateaus[i]);
  
  return(retbset);
}

//------------------------------------------------------------------
// Procedure: subtractPlateau()

BoxSet* RT_UniformX::subtractPlateau(BoxSet* bset, const IvPBox& plat)
{
  BoxSet rembs;  // The remainders
  
  BoxSetNode *bsn = bset->retBSN();
  while(bsn) {
    BoxSetNode *nextbsn = bsn->getNext();
    IvPBox* box = bsn->getBox();
    
    if(!plat.intersect(box)) {
      bsn = nextbsn;
      continue;
    }
    
    if(!containedWithinBox(*box, plat)) {
      BoxSet *bs = subtractBox(*box, plat);
      rembs.merge(*bs);
      delete(bs);
    }

    bset->remBSN(bsn);
    delete(bsn->getBox());
    delete(bsn);
    bsn = nextbsn;
  }

  
  bset->merge(rembs);
  return(bset);
}


//------------------------------------------------------------------
// Procedure: subtractBasins()

BoxSet* RT_UniformX::subtractBasins(BoxSet *bset)
{
  BoxSet *retbset = bset;
  for(unsigned int i=0; i<m_basins.size(); i++)
    retbset = subtractBasin(retbset, m_basins[i]);
  
  return(retbset);
}

//------------------------------------------------------------------
// Procedure: subtractBasin()

BoxSet* RT_UniformX::subtractBasin(BoxSet* bset, const IvPBox& basin)
{
  BoxSet rembs;  // The remainders
  
  BoxSetNode *bsn = bset->retBSN();
  while(bsn) {
    BoxSetNode *nextbsn = bsn->getNext();
    IvPBox* box = bsn->getBox();
    
    if(!basin.intersect(box)) {
      bsn = nextbsn;
      continue;
    }
    
    if(!containedWithinBox(*box, basin)) {
      BoxSet *bs = subtractBox(*box, basin);
      rembs.merge(*bs);
      delete(bs);
    }

    bset->remBSN(bsn);
    delete(bsn->getBox());
    delete(bsn);
    bsn = nextbsn;
  }

  
  bset->merge(rembs);
  return(bset);
}



