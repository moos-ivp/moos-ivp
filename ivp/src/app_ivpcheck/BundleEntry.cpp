/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BundleEntry.cpp                                      */
/*    DATE: Sep 27th 2006                                        */
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
#include <cstdlib>
#include "BundleEntry.h"
#include "MBUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "FunctionEncoder.h"
#include "FColorMap.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

BundleEntry::BundleEntry(const string& ipf_str)
{
  m_ipf_str  = ipf_str;
  m_pieces   = 0;
  m_priority = 0;
}

//-------------------------------------------------------------
// Procedure: getIvPFunction()

IvPFunction* BundleEntry::getIvPFunction()
{
  IvPFunction *ipf = StringToIvPFunction(m_ipf_str);

  if(ipf) {
    m_pieces = ipf->getPDMap()->size();
    m_priority = ipf->getPWT();
  }

  return(ipf);
}



//-------------------------------------------------------------
// Procedure: getQuadSet()
//      Note: The quadset member variable will remain unpopulated
//            until the first time it is queried for.

QuadSet BundleEntry::getQuadSet(IvPDomain ivp_domain)
{
  QuadSet empty_qset;

  IvPFunction *new_ipf = StringToIvPFunction(m_ipf_str);
  if(!new_ipf)
    return(empty_qset);

  PDMap *pdmap = new_ipf->getPDMap();
  if(!pdmap)
    return(empty_qset);

  m_pieces   = pdmap->size();
  m_priority = new_ipf->getPWT();

  IvPDomain domain = pdmap->getDomain();
  
  // Case where new_ipf defined only over COURSE
  if((domain.hasDomain("course")) && (!domain.hasDomain("speed"))) {
    IvPDomain spd_domain;
    spd_domain.addDomain("speed", 0, 5, 21);
    ZAIC_PEAK spd_zaic(spd_domain, "speed");
    spd_zaic.setSummit(2.5);
    spd_zaic.setPeakWidth(20);
    IvPFunction *spd_of = spd_zaic.extractOF();
    OF_Coupler coupler;
    new_ipf = coupler.couple(new_ipf, spd_of);
  }
  // Case where new_ipf defined only over SPEED
  if((!domain.hasDomain("course")) && (domain.hasDomain("speed"))) {
    IvPDomain crs_domain;
    crs_domain.addDomain("course", 0, 359, 360);
    ZAIC_PEAK crs_zaic(crs_domain, "course");
    crs_zaic.setSummit(180);
    crs_zaic.setPeakWidth(360);
    IvPFunction *crs_of = crs_zaic.extractOF();
    OF_Coupler coupler;
    new_ipf = coupler.couple(new_ipf, crs_of);
  }

  if(!new_ipf)
    return(empty_qset);

  // Make no assumption about the pwt of the IvPFunction produced by
  // the Coupler. Assign the pwt to be the original IvPFunction weight.
  new_ipf->setPWT(m_priority);

  QuadSet quadset;
  quadset.applyIPF(new_ipf);

  delete(new_ipf);
  return(quadset);
}


