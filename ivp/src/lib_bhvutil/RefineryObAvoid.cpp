/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RefineryObAvoid.cpp                                  */
/*    DATE: September 5th, 2019                                  */
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
#include "RefineryObAvoid.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

RefineryObAvoid::RefineryObAvoid(IvPDomain domain)
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

void RefineryObAvoid::setRefineRegions(ObShipModel obship_model)
{
  if(!m_initialized_domain)
    return;

  m_obship_model = obship_model;
  m_obship_model.print("RefineryObAvoid::setRefineRegions()");

  setRefineRegionsAll();
}


//-----------------------------------------------------------
// Procedure: setRefineRegionsAll()

void RefineryObAvoid::setRefineRegionsAll()
{
  // Part 1: Set the original refine regions
  setRefineRegionsMajor();
  setRefineRegionsMinor();

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

void RefineryObAvoid::setRefineRegionsMajor()
{
  if(m_verbose)
    cout << "RefineryObAvoid::setRefineRegionsMajor()" << endl; 

  // Part 1: Make the major plateau region(s). This is the piece
  // that essentially points away from the obstacle. But the
  // obstacle_buff is used which is original obstacle expanded
  // expanded by the distance of max_util_cpa/
  double bmin = m_obship_model.getCPABngMaxToPoly();
  double bmax = m_obship_model.getCPABngMinToPoly();
  bmin = getHdgSnappedHigh(bmin);
  bmax = getHdgSnappedLow(bmax);
  if(m_verbose) {
    cout << "Major Plateau:" << endl; 
    cout << "bmin:" << bmin << endl; 
    cout << "bmax:" << bmax << endl; 
  }

  vector<IvPBox> plat_regions;
  plat_regions = buildBoxesSpdAll(m_domain, bmin, bmax);
  addPlatRegions(plat_regions, 4);

  // Part 2: Make the major basin. This is the piece that depends
  // on which side the vehicle is passing the object. It will
  // disallow turns that turn back toward passing the object on
  // the other side. It will allow turns back in this direction
  // only up to the bearing between ownship and the center point
  // of the object.
  string passing_side = m_obship_model.getPassingSide();
  if(passing_side == "port") {
    bmin = m_obship_model.getThetaB();
    bmax = m_obship_model.getObcentBng();
    bmin = getHdgSnappedLow(bmin);
    bmax = getHdgSnappedHigh(bmax);
  }
  else if(passing_side == "star") {
    bmin = m_obship_model.getObcentBng();
    bmax = m_obship_model.getThetaB();
    bmin = getHdgSnappedHigh(bmin);
    bmax = getHdgSnappedLow(bmax);
  }
  // If we are not passing toward either side, then we skip the
  // creation of this basin piece.
  else 
    return;
  
  if(m_verbose) {
    cout << "Major Basin:" << endl; 
    cout << "Passing Side:" << passing_side << endl; 
    cout << "bmin:" << bmin << endl; 
    cout << "bmax:" << bmax << endl; 
  }

  vector<IvPBox> basin_regions;
  basin_regions = buildBoxesSpdAll(m_domain, bmin, bmax);
  if(m_verbose)
    cout << "Done RefineryObAvoid::setRefineRegionsMajor()" << endl; 

  addBasinRegions(basin_regions, -5);
}

//-----------------------------------------------------------
// Procedure: setRefineRegionsMinor()
//      Note: The utility function buildBoxesSpdAll(), is a
//            BuildUtils utility that may split a piece that
//            wraps around through zero degrees, into two pcs.

void RefineryObAvoid::setRefineRegionsMinor()
{
  if(m_verbose)
    cout << "RefineryObAvoid::setRefineRegionsMinor()" << endl; 

  // ================================================================
  // Part 1: Make the minor plateau region(s). This is the piece that
  // essentially uses the range to the buffer obstacle and determines
  // what is the minimum speed needed to get to the buffer obstacle.
  // It then creates a plateau in all heading values for speeds
  // between zero and this minimum speed.
  // ================================================================
  XYPolygon obstacle_buff_max = m_obship_model.getObstacleBuffMax();
  double osx = m_obship_model.getOSX();
  double osy = m_obship_model.getOSY();
  double ttc = m_obship_model.getAllowableTTC();
  if(ttc <= 0)
    return;
  
  double bdist  = obstacle_buff_max.dist_to_point(osx, osy);
  double minspd = bdist / ttc;
  
  double smin = m_domain.getVarLow(m_spd_ix);
  double smax = getSpdSnappedLow(minspd);

  IvPBox plat_region;
  plat_region = buildBoxHdgAll(m_domain, smin, smax);
  addPlatRegion(plat_region, 2);
  
  // ================================================================
  // Part 2: Make the minor basin region(s). This is the piece that
  // results in a collision with the core obstacle for a certain range
  // of speeds (from some speed to the max speed), and a certain range
  // of headings (from bng_min_to_poly to bng_max_to_poly).

  // Note: we want to be conservative in choosing the heading range.
  // Maneuvers not covered by this basin will still be evaluated by
  // the behavior, so there is no harm on erring on the side of too
  // small of a region. Same with the speed range.
  XYPolygon obstacle = m_obship_model.getObstacle();
  double bng_min_to_poly = m_obship_model.getBngMinToPoly();
  double bng_max_to_poly = m_obship_model.getBngMaxToPoly();
  
  double hmin = getHdgSnappedProx(bng_min_to_poly);
  double hmax = getHdgSnappedProx(bng_max_to_poly);
  
  double dist1 = m_obship_model.getBngMinDistToPoly();
  double dist2 = m_obship_model.getBngMaxDistToPoly();
  double range = dist1;
  if(dist2 > dist1)
    range = dist2;
  smin = range / ttc;
  smin = getSpdSnappedHigh(smin);
  smax = m_domain.getVarHigh(m_spd_ix);

  vector<IvPBox> basin_regions = buildBoxesHdgSpd(m_domain, hmin, hmax,
						  smin, smax);
  addBasinRegions(basin_regions, -4);
}

//----------------------------------------------------------------
// Procedure: getHdgSnappedLow()

double RefineryObAvoid::getHdgSnappedLow(double hdg) const
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

double RefineryObAvoid::getHdgSnappedHigh(double hdg) const
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

double RefineryObAvoid::getHdgSnappedProx(double hdg) const
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

double RefineryObAvoid::getSpdSnappedLow(double spd) const
{
  unsigned int uint_val = m_domain.getDiscreteVal(m_spd_ix, spd, 0);
  double new_spd = m_domain.getVal(m_spd_ix, uint_val);

  return(new_spd);
}

//----------------------------------------------------------------
// Procedure: getSpdSnappedHigh()

double RefineryObAvoid::getSpdSnappedHigh(double spd) const
{
  unsigned int uint_val = m_domain.getDiscreteVal(m_spd_ix, spd, 1);
  double new_spd = m_domain.getVal(m_spd_ix, uint_val);

  return(new_spd);
}

//----------------------------------------------------------------
// Procedure: getSpdSnappedProx()

double RefineryObAvoid::getSpdSnappedProx(double spd) const
{
  unsigned int uint_val = m_domain.getDiscreteVal(m_spd_ix, spd, 2);
  double new_spd = m_domain.getVal(m_spd_ix, uint_val);

  return(new_spd);
}

//----------------------------------------------------------------
// Procedure: addPlatRegion

void RefineryObAvoid::addPlatRegion(IvPBox region, double platval)
{
  region.setPlat(platval);
  m_plat_regions.push_back(region);
}

//----------------------------------------------------------------
// Procedure: addPlatRegions

void RefineryObAvoid::addPlatRegions(vector<IvPBox> regions,
				     double platval)
{
  for(unsigned int i=0; i<regions.size(); i++) {
    regions[i].setPlat(platval);
    m_plat_regions.push_back(regions[i]);
  }
}

//----------------------------------------------------------------
// Procedure: addBasinRegions

void RefineryObAvoid::addBasinRegions(vector<IvPBox> regions,
				      double platval)
{
  for(unsigned int i=0; i<regions.size(); i++) {
    regions[i].setPlat(platval);
    m_basin_regions.push_back(regions[i]);
  }
}

