/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / MIT Cambridge MA            */
/*    FILE: RefineryObAvoidV24.cpp                               */
/*    DATE: Oct 30th, 2023                                       */
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
#include "BuildUtils.h"
#include "AngleUtils.h"
#include "RefineryObAvoidV24.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

RefineryObAvoidV24::RefineryObAvoidV24(IvPDomain domain)
{
  // Initialize configuration variables
  m_verbose = false;
  m_initialized_domain = false;

  // Sanity check: course and speed must be in the given
  // domain. (index is -1 if not found in domain)
  m_crs_ix = domain.getIndex("course");
  m_spd_ix = domain.getIndex("speed");
  if(m_crs_ix == m_spd_ix)
    return;
  if(domain.getVarPoints(m_crs_ix) == 0)
    return;
  if(domain.getVarPoints(m_spd_ix) == 0)
    return;

  m_domain = domain;
  m_initialized_domain = true;
}

//-----------------------------------------------------------
// Procedure: setRefineRegions()

void RefineryObAvoidV24::setRefineRegions(ObShipModelV24 obship_model)
{
  if(!m_initialized_domain)
    return;

  m_obship_model = obship_model;

  setRefineRegionsAll();
}


//-----------------------------------------------------------
// Procedure: setRefineRegionsAll()

void RefineryObAvoidV24::setRefineRegionsAll()
{
  // Part 1: Set the original refine regions
  setRefineRegionsMajor();
  setRefineRegionsSideLock();

  // Part 2: Process the plateaus and basins and make sure that
  // none of them overlap.
  vector<IvPBox> boxes = m_plat_regions;
  for(unsigned int i=0; i<m_basin_regions.size(); i++)
    boxes.push_back(m_basin_regions[i]);
  boxes = makeRegionsApart(boxes);
  
  // Part 3: Now that we are sure we have non-overlapping regions,
  //         sort them again into basins and plateaus
  m_plat_regions.clear();
  m_basin_regions.clear();
  for(unsigned int i=0; i<boxes.size(); i++) {
    if(boxes[i].getPlat() < 0)
      m_basin_regions.push_back(boxes[i]);
    else
      m_plat_regions.push_back(boxes[i]);
  }
}

//-----------------------------------------------------------
// Procedure: setRefineRegionsMajor()
//      Note: The utility function buildBoxesSpdAll(), is a
//            BuildUtils utility that may split a piece that
//            wraps around through zero degrees, into two pcs.

void RefineryObAvoidV24::setRefineRegionsMajor()
{
  m_obship_model.updateBngExtremes();
  if(m_verbose) {
    cout << "RefineryObAvoidV24::setRefineRegionsMajor()" << endl; 
    cout << "RefineryObAvoidV24:" << endl;
    m_obship_model.printBnds();
  }
  
  double rim_hdg_min = m_obship_model.getRimBngMinToPoly();
  double rim_hdg_max = m_obship_model.getRimBngMaxToPoly();
  unsigned int rim_bng_unhit_cnt = m_obship_model.getRimBngUnhitCount();

  bool plateaus_exist = true;
  if((rim_hdg_min < 0) || (rim_hdg_max < 0))
    plateaus_exist = false;
  if(rim_bng_unhit_cnt == 0)
    plateaus_exist = false;

  if(plateaus_exist) {
    vector<IvPBox> plat_regions;
    plat_regions = buildBoxesSpdAll(m_domain, rim_hdg_min, rim_hdg_max);
    addPlatRegions(plat_regions, 4);
    if(m_verbose) {
      for(unsigned int i=0; i<plat_regions.size(); i++) {
	cout << "pr[" << i << "] (" << plat_regions.size() << "):" << endl;
	plat_regions[i].print();
      }
    }
  }
  
  double gut_hdg_min = m_obship_model.getGutBngMinToPoly();
  double gut_hdg_max = m_obship_model.getGutBngMaxToPoly();
  unsigned int gut_bng_hit_cnt = m_obship_model.getRimBngUnhitCount();

  bool basins_exist = true;
  if((gut_hdg_min < 0) || (gut_hdg_max < 0))
    basins_exist = false;
  if(gut_bng_hit_cnt == 0)
    basins_exist = false;
  
  if(basins_exist) {
    vector<IvPBox> basin_regions;
    basin_regions = buildBoxesSpdAll(m_domain, gut_hdg_min, gut_hdg_max);
    addBasinRegions(basin_regions);
    if(m_verbose) {
      for(unsigned int i=0; i<basin_regions.size(); i++) {
	cout << "br[" << i << "] (" << basin_regions.size() << "):" << endl;
	basin_regions[i].print();
      }
    }
  }
}

//-----------------------------------------------------------
// Procedure: setRefineRegionsSideLock()

void RefineryObAvoidV24::setRefineRegionsSideLock()
{
  if(m_side_lock == "")
    return;

  double osh = m_obship_model.getOSH();
  
  double hdg_min = angle360(osh + 165);
  double hdg_max = angle360(osh + 350);
  if(m_side_lock == "port") {
    hdg_min = angle360(osh + 10);
    hdg_max = angle360(osh + 195);
  }

  vector<IvPBox> basin_regions;
  basin_regions = buildBoxesSpdAll(m_domain, hdg_min, hdg_max);
  addBasinRegions(basin_regions, -3);
}

//----------------------------------------------------------------
// Procedure: getHdgSnappedLow()

double RefineryObAvoidV24::getHdgSnappedLow(double hdg) const
{
  // Sanity checks
  if((hdg < 0) || (hdg >= 360))
    hdg = angle360(hdg);

  unsigned int uint_val = m_domain.getDiscreteVal(m_crs_ix, hdg, 0);
  double new_hdg = m_domain.getVal(m_crs_ix, uint_val);

  return(new_hdg);
}

//----------------------------------------------------------------
// Procedure: getHdgSnappedHigh()

double RefineryObAvoidV24::getHdgSnappedHigh(double hdg) const
{
  // Sanity checks
  if((hdg < 0) || (hdg >= 360))
    hdg = angle360(hdg);

  // Edge case
  if(hdg > m_domain.getVarHigh(m_crs_ix))
    return(0);
  
  unsigned int uint_val = m_domain.getDiscreteVal(m_crs_ix, hdg, 1);
  double new_hdg = m_domain.getVal(m_crs_ix, uint_val);

  return(new_hdg);
}

//----------------------------------------------------------------
// Procedure: getHdgSnappedProx()

double RefineryObAvoidV24::getHdgSnappedProx(double hdg) const
{
  // Sanity check
  if((hdg < 0) || (hdg >= 360))
    hdg = angle360(hdg);

  // Edge case
  double domain_hdg_high = m_domain.getVarHigh(m_crs_ix);
  if(hdg > domain_hdg_high) {
    double domain_hdg_delta = m_domain.getVarDelta(m_crs_ix);
    if((hdg - domain_hdg_high) > (domain_hdg_delta/2))
      return(0);
    else
      return(domain_hdg_high);
  }
  
  unsigned int uint_val = m_domain.getDiscreteVal(m_crs_ix, hdg, 1);
  double new_hdg = m_domain.getVal(m_crs_ix, uint_val);

  return(new_hdg);
}

//----------------------------------------------------------------
// Procedure: getSpdSnappedLow()

double RefineryObAvoidV24::getSpdSnappedLow(double spd) const
{
  unsigned int uint_val = m_domain.getDiscreteVal(m_spd_ix, spd, 0);
  double new_spd = m_domain.getVal(m_spd_ix, uint_val);

  return(new_spd);
}

//----------------------------------------------------------------
// Procedure: getSpdSnappedHigh()

double RefineryObAvoidV24::getSpdSnappedHigh(double spd) const
{
  unsigned int uint_val = m_domain.getDiscreteVal(m_spd_ix, spd, 1);
  double new_spd = m_domain.getVal(m_spd_ix, uint_val);

  return(new_spd);
}

//----------------------------------------------------------------
// Procedure: getSpdSnappedProx()

double RefineryObAvoidV24::getSpdSnappedProx(double spd) const
{
  unsigned int uint_val = m_domain.getDiscreteVal(m_spd_ix, spd, 2);
  double new_spd = m_domain.getVal(m_spd_ix, uint_val);

  return(new_spd);
}

//----------------------------------------------------------------
// Procedure: addPlatRegion

void RefineryObAvoidV24::addPlatRegion(IvPBox region, double platval)
{
  region.setPlat(platval);
  m_plat_regions.push_back(region);
}

//----------------------------------------------------------------
// Procedure: addPlatRegions

void RefineryObAvoidV24::addPlatRegions(vector<IvPBox> regions,
				     double platval)
{
  for(unsigned int i=0; i<regions.size(); i++) {
    regions[i].setPlat(platval);
    m_plat_regions.push_back(regions[i]);
  }
}

//----------------------------------------------------------------
// Procedure: addBasinRegions

void RefineryObAvoidV24::addBasinRegions(vector<IvPBox> regions,
				      double platval)
{
  for(unsigned int i=0; i<regions.size(); i++) {
    regions[i].setPlat(platval);
    m_basin_regions.push_back(regions[i]);
  }
}
