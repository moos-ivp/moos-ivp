/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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

  IvPDomain domain = m_regressor->getAOF()->getDomain();

  IvPBox universe = domainToBox(domain);

  int degree = m_regressor->getDegree();

  BoxSet *boxset = makeUniformDistro(universe, unifbox, degree);

  handleOverlappingPlateaus();
  
  boxset = subtractPlateaus(boxset);

  int vsize = boxset->size();
  if(vsize == 0)
    return(0);

  int plateau_pcs = (int)(m_plateaus.size());
  
  PDMap *pdmap = new PDMap(vsize + plateau_pcs, domain, degree);
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
// Procedure: handleOverlappingPlateaus()

void RT_UniformX::handleOverlappingPlateaus()
{
  bool changed = true;
  if(m_plateaus.size() < 2)
    return;

  vector<IvPBox> plats;
  plats.push_back(m_plateaus[0]);
  
  list<IvPBox> oplats;
  for(unsigned int i=1; i<m_plateaus.size(); i++)
    oplats.push_back(m_plateaus[i]);

  
  bool restart = false;
  while(!restart && (oplats.size() != 0)) {

    IvPBox oplat = oplats.front();
    oplats.pop_front();

    
    bool restart = false;
    for(unsigned int i=0; (i<plats.size() && !restart); i++) {
      if(plats[i].intersect(&oplat)) { 
	changed = true;
	restart = true;
	BoxSet *bs = subtractBox(oplat, plats[i]);
	while(bs->size() > 0) {
	  restart = true;
	  BoxSetNode *bsn = bs->remBSN();
	  if(bsn) {
	    IvPBox new_plat = *(bsn->getBox());
	    oplats.push_back(new_plat);
	    delete(bsn->getBox());
	    delete(bsn);
	  }
	}
	delete(bs);
      }
    }
    if(!restart)
      plats.push_back(oplat);
  }

  if(changed && m_verbose) {
    cout << "CHANGED!!!!!!!!!!" << endl;
    cout << "Old Plateaus:" << endl;
    for(unsigned int i=0; i<m_plateaus.size(); i++)
      m_plateaus[i].print();
    cout << "New Plateaus:" << endl;
    for(unsigned int i=0; i<plats.size(); i++)
      plats[i].print();
  }
  
  m_plateaus = plats;
}	      
