/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RefineryCPA.cpp                                      */
/*    DATE: November 14th, 2017                                  */
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
#include <sstream>
#include <cmath>
#include "AngleUtils.h"
#include "BuildUtils.h"
#include "RefineryCPA.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

RefineryCPA::RefineryCPA()
{
  // Initialize state variables
  m_initialized = false;
  m_crs_ix      = 0;
  m_spd_ix      = 0;
  m_crs_pts     = 0;
  m_spd_pts     = 0;

  m_contact_range = 0;
  m_range_gamma   = 0;
  m_range_epsilon = 0;
  m_os_cn_abs_bng = 0;
  m_theta_delta   = 0;

  m_os_port_of_cn = false;
  m_os_star_of_cn = false;

  m_cpa_queries = 0;
  
  // Initialize configuration variables
  m_osx = 0;
  m_osy = 0;

  m_cnx = 0;
  m_cnx = 0;
  m_cnh = 0;
  m_cnv = 0;

  m_ostol = 0;

  m_cpa_engine = 0;

  m_verbose = false;

  m_quit1 = false;
  m_quit2 = false;
  m_quit3 = false;
}

//-----------------------------------------------------------
// Procedure: init()

bool RefineryCPA::init(double osx, double osy, double cnx, double cny,
		       double cnh, double cnv, double ostol,
		       double min_ucd, double max_ucd, IvPDomain domain,
		       CPAEngine* cpa_engine)
{
  if(m_verbose) {
    cout << "RefineryCPA: Initializing: " << endl;
    cout << "  min_util_cpa_dist: " << min_ucd << endl;
    cout << "  max_util_cpa_dist: " << max_ucd << endl;
  }
  
  // Sanity checks
  if((min_ucd < 0) || (max_ucd < 0) || (ostol < 0))
    return(false);
  if(min_ucd > max_ucd)
    return(false);
  if(!cpa_engine)
    return(false);

  int crs_ix = domain.getIndex("course");
  int spd_ix = domain.getIndex("speed");

  // More Sanity checks (index is -1 if not found in domain)
  if(crs_ix == spd_ix)
    return(false);
  if((crs_ix < 0) || (spd_ix < 0))
    return(false);

  unsigned int crs_pts = domain.getVarPoints(crs_ix);
  unsigned int spd_pts = domain.getVarPoints(spd_ix);

  if((crs_pts == 0) || (spd_pts == 0))
    return(false);
  
  // Ok, all that can be checked for has been checked...
  m_initialized = true;

  m_osx = osx;
  m_osy = osy;
  m_cnx = cnx;
  m_cny = cny;
  m_cnh = cnh;
  m_cnv = cnv;
  
  m_ostol = ostol;

  m_min_util_cpa_dist = min_ucd;
  m_max_util_cpa_dist = max_ucd;

  m_cpa_engine = cpa_engine;

  m_domain = domain;

  m_crs_ix  = (unsigned int)(crs_ix);
  m_spd_ix  = (unsigned int)(spd_ix);
  m_crs_pts = crs_pts;
  m_spd_pts = spd_pts;

  m_contact_range = cpa_engine->getRange();
  m_range_gamma   = cpa_engine->getRangeGamma();
  m_range_epsilon = cpa_engine->getRangeEpsilon();
  m_os_port_of_cn = cpa_engine->portOfContact();
  m_os_star_of_cn = cpa_engine->starboardOfContact();
  m_os_cn_abs_bng = cpa_engine->ownshipContactAbsBearing();
  
  // If max_util_cpa_dist is greater than current range to contact then
  // reduce to equal current range to contact. This is the right thing
  // to do as it will encourage range opening maneuvers, but also because
  // the asin(double) function below will return NAN if given an argument
  // greater than 1.
  if(m_max_util_cpa_dist > m_contact_range)
    m_max_util_cpa_dist = m_contact_range;
  
  double theta_delta_rads = 0;
  if((m_max_util_cpa_dist > 0) && (m_contact_range > 0))
    theta_delta_rads = asin(m_max_util_cpa_dist / m_contact_range);
  m_theta_delta = radToDegrees(theta_delta_rads);

  return(true);
}



//-----------------------------------------------------------
// Procedure: getRefineRegions()

vector<IvPBox> RefineryCPA::getRefineRegions()
{
  if(m_verbose) {
    cout << "RefineryCPA: getRefineRegions: " << endl;
    cout << "  Q1:" << boolToString(m_quit1);
    cout << "  Q2:" << boolToString(m_quit2);
    cout << "  Q3:" << boolToString(m_quit3) << endl;
  }
  
  vector<IvPBox> null_regions;
  if(!m_initialized || !m_cpa_engine)
    return(null_regions);

  bool os_fore_of_cn = m_cpa_engine->foreOfContact();

  if(os_fore_of_cn)
    return(getRefineRegionsFore());

  return(getRefineRegionsAft());
}


//-----------------------------------------------------------
// Procedure: getRefineRegionsAft()

vector<IvPBox> RefineryCPA::getRefineRegionsAft()
{
  if(m_verbose)
    cout << "====Start====== RefineryCPA::getRefineRegions AFT" << endl; 

  double os_to_cn_bng = relAng(m_osx, m_osy, m_cnx, m_cny);
  double cn_to_os_bng = angle360(os_to_cn_bng + 180);
  
  m_logic_case = "aft";
  vector<IvPBox> null_regions;
  vector<IvPBox> regions;
  double max_spd = m_domain.getVarHigh(m_spd_ix);
  
  //===========================================================
  // Part A: Build a single piece, in all directions, limited by
  //         the current contact range rate from os position  (A1)
  //===========================================================
  // If ownship is aft of the contact, this speed should be negative
  double cn_spd_in_os_pos = m_cpa_engine->getCNSpeedInOSPos();
  if(cn_spd_in_os_pos > 0)
    return(null_regions);

  double range_to_close = m_contact_range - m_max_util_cpa_dist;
  if(m_verbose) {
    cout << "  RefCPA: Range_to_close: " << range_to_close << endl;
    cout << "  RefCPA: contact_range: " << m_contact_range << endl;
    cout << "  RefCPA: max_util_cpa_dist: " << m_max_util_cpa_dist << endl;
    cout << "  RefCPA: m_ostol: " << m_ostol << endl;
  }
  
  double inner_max_spd = -cn_spd_in_os_pos;
  if(m_verbose)
    cout << "  RefCPA: Inner_max_spd(A): " << inner_max_spd << endl;

  double xtra_spd = 0;
  if((range_to_close > 0) && (m_ostol > 0))
    xtra_spd = range_to_close / m_ostol;
  inner_max_spd += xtra_spd;
  if(m_verbose)
    cout << "RefCPA: Inner_max_spd(B): " << inner_max_spd << endl;

  inner_max_spd = m_domain.getNextLowerVal(m_spd_ix, inner_max_spd, 0);
  if(m_verbose)
    cout << "RefCPA: Inner_max_spd(C): " << inner_max_spd << endl;
  if(inner_max_spd > 0) {
    IvPBox hdg_all_region = buildBoxHdgAll(m_domain, 0, inner_max_spd);
    regions.push_back(hdg_all_region);
    if(m_verbose) {
      cout << "BldBoxHdgAll: inner_max_spd: " << inner_max_spd << endl;
      cout << "region: " << endl;
      hdg_all_region.print();
      cout << "RefCPA: Domain: ";
      m_domain.print();
    }
  }
  
  
  //===========================================================
  // Part B: Build the piece(s) to either side of the contact
  //===========================================================
  double pass_hdg = m_cnh;
  double cross_hdg = 0;
  if(m_os_star_of_cn) {
    cross_hdg = angle360(m_os_cn_abs_bng - m_theta_delta);
    cross_hdg = m_domain.getNextLowerVal(m_crs_ix, cross_hdg, 1, true);
  }
  else {
    cross_hdg = angle360(m_os_cn_abs_bng + m_theta_delta);
    cross_hdg = m_domain.getNextHigherVal(m_crs_ix, cross_hdg, 0, true);
  }

  if(m_verbose) {
    cout << "  cross_hdg (pre): " << cross_hdg << endl;
    cout << "  pass_hdg (pre):  " << pass_hdg << endl;
    cout << "  theta_delta:  " << m_theta_delta << endl;
    cout << "  m_range_gamma:  " << m_range_gamma << endl;
  }
  
  if(m_range_gamma < m_max_util_cpa_dist) {    
    if(m_os_star_of_cn) {
      pass_hdg = angle360(m_os_cn_abs_bng + m_theta_delta);
      if(m_verbose) cout << "pass_hdg (pre_star):  " << pass_hdg << endl;
      pass_hdg = getMinLastSatHdgAtSpd(max_spd, pass_hdg);
      if(m_verbose) cout << "pass_hdg (post_star):  " << pass_hdg << endl;
    }
    else {
      pass_hdg = angle360(m_os_cn_abs_bng - m_theta_delta);
      if(m_verbose) cout << "pass_hdg (pre_port):  " << pass_hdg << endl;
      pass_hdg = getMaxLastSatHdgAtSpd(max_spd, pass_hdg);
      if(m_verbose) cout << "pass_hdg (post_port):  " << pass_hdg << endl;
    }
  }


  if(m_os_star_of_cn)
    cross_hdg = getMaxLastSatHdgAtSpd(max_spd, cross_hdg);
  else
    cross_hdg = getMinLastSatHdgAtSpd(max_spd, cross_hdg);

  if(m_verbose) {
    cout << "  cross_hdg (post): " << cross_hdg << endl;
    cout << "  pass_hdg (post):  " << pass_hdg << endl;
    cout << "  m_os_cn_abs_bng:  " << m_os_cn_abs_bng << endl;
  }

  // We can be certain that the min/max range is a reflex angle (> 180)
  //double min_hdg = angleMinReflex(pass_hdg, cross_hdg);
  //double max_hdg = angleMaxReflex(pass_hdg, cross_hdg);

  double min_hdg_raw = pass_hdg;
  double max_hdg_raw = cross_hdg;

#if 0
  if(m_os_port_of_cn) {
    min_hdg_raw = cross_hdg;
    max_hdg_raw = pass_hdg;
  }
#endif
   
  //double min_hdg = angleMinReflex(min_hdg_raw, max_hdg_raw);
  //double max_hdg = angleMaxReflex(min_hdg_raw, max_hdg_raw);
  
  double min_hdg = angleMinContains(min_hdg_raw, max_hdg_raw, cn_to_os_bng);
  double max_hdg = angleMaxContains(min_hdg_raw, max_hdg_raw, cn_to_os_bng);
  

  double inner_max_spd_next = m_domain.getNextHigherVal(m_spd_ix, inner_max_spd, 1, 0);

  if(m_verbose) {
    cout << "min_hdg: " << min_hdg << endl;
    cout << "max_hdg: " << max_hdg << endl;
    cout << "inner_max_spd_next:" << inner_max_spd_next << endl;
    cout << "max_spd:" << max_spd << endl;
  }
  
  //vector<IvPBox> boxes = buildBoxesSpdAll(m_domain, min_hdg, max_hdg);
  vector<IvPBox> boxes = buildBoxesHdgSpd(m_domain, min_hdg, max_hdg,
					  inner_max_spd_next, max_spd);
  for(unsigned int i=0; i<boxes.size(); i++)
    regions.push_back(boxes[i]);

  if(m_verbose)
    cout << "====END============ RefineryCPA::getRefineRegions AFT" << endl; 
  return(regions);
}

//-----------------------------------------------------------
// Procedure: getRefineRegionsFore()

vector<IvPBox> RefineryCPA::getRefineRegionsFore()
{
  if(m_range_gamma < m_max_util_cpa_dist) {
    m_logic_case = "fore_in_rgam";
    return(getRefineRegionsForeInRGam());
  }
  else {
    m_logic_case = "fore_out_rgam";
    return(getRefineRegionsForeOutRGam());
  }
}

//-----------------------------------------------------------
// Procedure: getRefineRegionsForeOutRGam()
//     Note:  Case were gamma range is greater than max_util_cpa_dist.
//            That is, if ownship were to not move, speed 0, contact
//            would not come within max_util_cpa_dist. Ownship not
//            in velocity cone,

vector<IvPBox> RefineryCPA::getRefineRegionsForeOutRGam()
{
  vector<IvPBox> null_regions;
  vector<IvPBox> regions;
  double max_spd = m_domain.getVarHigh(m_spd_ix);

  if(m_verbose) {
    cout << "***********************************************************" << endl;
    cout << "In getRefineRegionsForeOutRGam()                           " << endl;
    cout << "***********************************************************" << endl;
  }
  
  //===========================================================
  // Part A: Build a piece or pair of pieces wrapping the cone
  //         out to top speed
  //===========================================================
  
  //===========================================================
  // Part A: o Build an initial total-speed heading interval. An 
  //           interval of headings where all spds for each hdg
  //           has a cpa(hdg,spd) value >= max_util_cpa_dist.
  //         o The cross_hdg will be tight, but the pass_hdg
  //           will remain an non-tight bound for now until the
  //           the max_innner_spd is determined in Part B.
  //===========================================================
  
  double pass_hdg  = m_cnh;
  double cross_hdg = 0;

  double min_hdg = 0;
  double max_hdg = 0;
  
  if(m_os_star_of_cn) {
    cross_hdg = angle360(m_os_cn_abs_bng - m_theta_delta);
    if(m_verbose) {
      cout << "cross_hdg (pre): " << cross_hdg << endl;
      cout << "pre:getMaxLastSatHdgAtSpd(): " << endl;
      cout << "  --} max_spd: " << max_spd << endl; 
      cout << "  **--} cross_hdg(1): " << cross_hdg << endl; 
    }
    cross_hdg = m_domain.getSnappedVal(m_crs_ix, cross_hdg, 0);
    cross_hdg = getMaxLastSatHdgAtSpd(max_spd, cross_hdg);
    if(m_verbose) {
      cout << "cross_hdg (post): " << cross_hdg << endl;
      cout << "  **--} cross_hdg(2): " << cross_hdg << endl;  
      cout << "  **--} cross_hdg(2): " << cross_hdg << endl; 
    }
    min_hdg = pass_hdg;
    max_hdg = cross_hdg;    
  }
  else {
    cross_hdg = angle360(m_os_cn_abs_bng + m_theta_delta);
    if(m_verbose) {
      cout << "cross_hdg (pre): " << cross_hdg << endl;
      cout << "pre:getMinLastSatHdgAtSpd(): " << endl;
      cout << "  --} max_spd: " << max_spd << endl; 
      cout << "  --} cross_hdg: " << cross_hdg << endl; 
    }
    
    cross_hdg = getHdgSnappedHgh(cross_hdg);
    //if(cross_hdg > m_domain.getVarHigh(m_crs_ix))
    //  cross_hdg = 0;
    //else
    //  cross_hdg = m_domain.getSnappedVal(m_crs_ix, cross_hdg, 1);
    if(m_verbose) cout << "cross_hdg (post): " << cross_hdg << endl;
    cross_hdg = getMinLastSatHdgAtSpd(max_spd, cross_hdg);
    if(m_verbose) cout << "  **--} cross_hdg(2): " << cross_hdg << endl; 
    min_hdg = cross_hdg;
    max_hdg = pass_hdg;
  }

  if(m_verbose) {
    cout << "......theta_delta:" << m_theta_delta << endl;
    cout << "......pass_hdg:"   << pass_hdg << endl;
    cout << "......cross_hdg:" << cross_hdg << endl;
  }
    
  // Special EDGE case - if cross_hdg is -2 this means a full 360 
  // sweep was performed and no cpa(hdg,spd) values were found 
  // below max_util_cpa_dist. One big piece!
  if(cross_hdg == -2) {
    IvPBox all_region = buildBoxHdgAll(m_domain, 0, max_spd);
    regions.push_back(all_region);
    return(regions);
  }
  
  //===========================================================
  // Part B: Build a single piece, in all directions, limited by
  //         the current contact range rate from os position
  //===========================================================

  //double min_hdgx = angleMinReflex(pass_hdg, cross_hdg);
  //double max_hdgx = angleMaxReflex(pass_hdg, cross_hdg);

  if(m_verbose) {
    cout << "Pre max_inner_spd calc: " << endl;
    cout << "  min_hdg: " << min_hdg << endl;
    cout << "  max_hdg: " << max_hdg << endl;
  }
  
  double max_inner_spd = 0;
  if(m_os_star_of_cn)
    max_inner_spd = getMinSpdClockwise(max_hdg, min_hdg, max_spd);
  else 
    max_inner_spd = getMinSpdCounterClock(max_hdg, min_hdg, max_spd);

  if(m_verbose)
    cout << "max_inner_spd: " << max_inner_spd << endl;
  
  if(max_inner_spd > 0) {
    IvPBox hdg_all_region = buildBoxHdgAll(m_domain, 0, max_inner_spd);
    regions.push_back(hdg_all_region);
  }

  //if(m_quit1)
  //  return(regions);  // mikerb

  //===========================================================
  // Part C: Tighten pass_hdg now that max_inner_spd is known.
  //         Build the box or pair of boxes.
  //===========================================================

  if(m_verbose) cout << "pass_hdg (pre): " << pass_hdg << endl;
  if(m_os_star_of_cn) {
    if(m_verbose) {
      cout << "Entering getPassHdgCounterClock()" << endl;
      cout << "pass_hdg:" << pass_hdg << endl;
      cout << "max_spd:" << max_spd << endl;
      cout << "max_inner_spd:" << max_inner_spd << endl;
    }
    pass_hdg = getPassHdgCounterClock(pass_hdg, max_spd, max_inner_spd);
    min_hdg = pass_hdg;
    max_hdg = cross_hdg;    
  }
  else {
    if(m_verbose) {
      cout << "Entering getPassHdgClockwise()" << endl;
      cout << "pass_hdg:" << pass_hdg << endl;
      cout << "max_spd:" << max_spd << endl;
      cout << "max_inner_spd:" << max_inner_spd << endl;
    }
    pass_hdg = getPassHdgClockwise(pass_hdg, max_spd, max_inner_spd);
    if(m_verbose) cout << " ** NEW pass_hdg:" << pass_hdg << endl;
    min_hdg = cross_hdg;
    max_hdg = pass_hdg;
  }

  if(m_verbose) {
    cout << "pass_hdg (post): " << pass_hdg << endl;
    cout << "==>min_hdg:" << min_hdg << endl;
    cout << "==>max_hdg:" << max_hdg << endl;
  }
  
  vector<IvPBox> boxes = buildBoxesSpdAll(m_domain, min_hdg, max_hdg);
  for(unsigned int i=0; i<boxes.size(); i++) {
    regions.push_back(boxes[i]);

    if(m_verbose) {
      cout << "FFFF_REG:" << endl;
      boxes[i].print();
    }
  }
    
  if(m_quit1)
    return(regions);  // mikerb

  //===========================================================
  // Part D: Capture pieces of the shadow zone (beyond the cone)
  //===========================================================

  bool do_shadows = true;
  double cnv_up = m_domain.getSnappedVal(m_spd_ix, m_cnv, 1);
  if(cnv_up >= max_spd)
    do_shadows = false;
  
  
  double mid_spd1 = ((max_spd - cnv_up) * 2/3) + cnv_up;
  double mid_spd2 = ((max_spd - cnv_up) * 1/3) + cnv_up;
  mid_spd1 = m_domain.getSnappedVal(m_spd_ix, mid_spd1, 1);
  mid_spd2 = m_domain.getSnappedVal(m_spd_ix, mid_spd2, 1);
  double mid_spdx = m_domain.getNextLowerVal(m_spd_ix, mid_spd1, 2);

  if(mid_spd1 > max_spd)
    mid_spd1 = max_spd;
  if(mid_spd2 < cnv_up)
    mid_spd2 = cnv_up;
  if(mid_spd1 <= mid_spd2)
    do_shadows = false;
  
  if(do_shadows) {
    if(m_os_star_of_cn) {
      double mid_hdg1 = getMinLastSatHdgAtSpd(mid_spd1, min_hdg);
      if(m_verbose) {
	cout << "min_hdg:" << min_hdg << endl;
	cout << "mid_spd1:" << mid_spd1 << endl;
	cout << "mid_spdx:" << mid_spdx << endl;
	cout << "mid_spd2:" << mid_spd2 << endl;
	cout << "mid_hdg1:" << mid_hdg1 << endl;
      }
      vector<IvPBox> boxes1 = buildBoxesHdgSpd(m_domain, mid_hdg1, min_hdg,
					       mid_spd1, max_spd);
      for(unsigned int i=0; i<boxes1.size(); i++)
	regions.push_back(boxes1[i]);
      
      double mid_hdg2 = getMinLastSatHdgAtSpd(mid_spd2, min_hdg);
      if(m_verbose) cout << "mid_hdg2:" << mid_hdg2 << endl;
      vector<IvPBox> boxes2 = buildBoxesHdgSpd(m_domain, mid_hdg2, min_hdg,
					       mid_spd2, mid_spdx);
      for(unsigned int i=0; i<boxes2.size(); i++)
	regions.push_back(boxes2[i]);
    }
    else {
      double mid_hdg1 = getMaxLastSatHdgAtSpd(mid_spd1, max_hdg);
      vector<IvPBox> boxes1 = buildBoxesHdgSpd(m_domain, max_hdg, mid_hdg1,
					       mid_spd1, max_spd);
      for(unsigned int i=0; i<boxes1.size(); i++)
	regions.push_back(boxes1[i]);
      
      double mid_hdg2 = getMaxLastSatHdgAtSpd(mid_spd2, max_hdg);
      vector<IvPBox> boxes2 = buildBoxesHdgSpd(m_domain, max_hdg, mid_hdg2,
					       mid_spd2, mid_spdx);
      for(unsigned int i=0; i<boxes2.size(); i++)
	regions.push_back(boxes2[i]);
    }
  }

  //===========================================================
  // Part E: Capture a piece of the fore shadow (front of the cone)
  //===========================================================

  bool do_fore = true;

  double fore_spdx = 0;
  if(max_inner_spd > 0)
    fore_spdx = m_domain.getNextHigherVal(m_spd_ix, max_inner_spd, 2);
    
  double fore_spd1 = ((cnv_up - max_inner_spd) * 1/3) + max_inner_spd;
  fore_spd1 = m_domain.getSnappedVal(m_spd_ix, fore_spd1, 1);
  if(fore_spd1 >= max_spd)
    fore_spd1 = max_spd;

  if(fore_spd1 <= fore_spdx)
    do_fore = false;

  if(m_verbose) {
    cout << "do_fore:" << do_fore << endl;
    cout << "fore_spdx: " << fore_spdx << endl;
    cout << "fore_spd1: " << fore_spd1 << endl;
  }
  
  if(do_fore) {
    if(m_os_star_of_cn) {
      max_hdg = m_domain.getSnappedVal(m_crs_ix, max_hdg, 0);
      double mid_hdg1 = getMaxLastSatHdgAtSpd(fore_spd1, max_hdg);
      if(m_verbose) cout << "star:mid_hdg1: " << mid_hdg1 << endl;
      mid_hdg1 = m_domain.getSnappedVal(m_crs_ix, mid_hdg1, 0);
      if(m_verbose) {
	cout << "star:mid_hdg1: " << mid_hdg1 << endl;
	cout << "star:max_hdg: " << max_hdg << endl;
      }
      vector<IvPBox> boxes = buildBoxesHdgSpd(m_domain, max_hdg, mid_hdg1,
					      fore_spdx, fore_spd1);
      for(unsigned int i=0; i<boxes.size(); i++) {
	if(m_verbose)
	  boxes[i].print();
	regions.push_back(boxes[i]);
      }
    }
    else {
      min_hdg = getHdgSnappedHgh(min_hdg);
      //if(min_hdg > m_domain.getVarHigh(m_crs_ix))
      //min_hdg = 0;
      //else
      //	min_hdg = m_domain.getSnappedVal(m_crs_ix, min_hdg, 1);
      double mid_hdg1 = getMinLastSatHdgAtSpd(fore_spd1, min_hdg);
      if(m_verbose) cout << "port:mid_hdg1: " << mid_hdg1 << endl;
      mid_hdg1 = m_domain.getSnappedVal(m_crs_ix, mid_hdg1, 1);
      if(m_verbose) cout << "port:mid_hdg1: " << mid_hdg1 << endl;
      vector<IvPBox> boxes = buildBoxesHdgSpd(m_domain, mid_hdg1, min_hdg,
					      fore_spdx, fore_spd1);
      for(unsigned int i=0; i<boxes.size(); i++)
	regions.push_back(boxes[i]);
    }
  }

  if(m_verbose) {
    cout << "***********************************************************" << endl;
    cout << "***********************************************************" << endl;
    cout << "****************    E      N     D          ***************" << endl;
    cout << "***********************************************************" << endl;
    cout << "***********************************************************" << endl;
  }
  
  return(regions);
}


//-----------------------------------------------------------
// Procedure: getRefineRegionsForeInRGam()
//      Note: Ownship is IN the velocity cone. That is, if ownship were
//            to not move, speed 0, contact would come within
//            max_util_cpa_dist.

vector<IvPBox> RefineryCPA::getRefineRegionsForeInRGam()
{
  vector<IvPBox> null_regions;
  vector<IvPBox> regions;
  
  if(m_verbose) {
    cout << "***********************************************************" << endl;
    cout << "In getRefineRegionsForeInRGam()                            " << endl;
    cout << "***********************************************************" << endl;
  }
  
  // =======================================================
  // Part A: Calculate terms/info general to all steps
  // =======================================================
  double port_hdg = angle360(m_cnh - 90);
  double star_hdg = angle360(m_cnh + 90);
  double cn_flee_hdg = angle360(m_cnh - 180);
  double cn_flee_hdg_step_down = getHdgSnappedLow(cn_flee_hdg);
  double cn_flee_hdg_step_up = getHdgSnappedHgh(cn_flee_hdg);
  
  // =======================================================
  // Part B: Determine the initial wedge or wedge-pair
  // =======================================================
  double port_speed = getMaxSpdAtHdg(port_hdg);
  double star_speed = getMaxSpdAtHdg(star_hdg);
  //double flee_speed = getFleeSpeed();
  double flee_speed = m_cnv;
  flee_speed = getSpdSnappedHgh(flee_speed);
  
  // Determine which of the three axis-speeds is max.
  double inner_max_spd = flee_speed;
  if(port_speed > inner_max_spd)
    inner_max_spd = port_speed;
  if(star_speed > inner_max_spd)
    inner_max_spd = star_speed;
  
#if 1
  double sit_cpa = evalCPA(0,0,m_ostol);
  if(m_verbose) cout << "SIT cpa: " << sit_cpa << endl;
  if(sit_cpa >= m_max_util_cpa_dist) {
    if(m_ostol != 0) { 
      inner_max_spd = (m_contact_range / m_ostol) - m_cnv; 
      if(m_verbose) {
	cout << "m_cnv: " << m_cnv << endl;
	cout << "m_contact_range: " << m_contact_range << endl;
	cout << "m_ostol: " << m_ostol << endl;
	cout << "(m_contact_range / m_ostol): " << m_contact_range / m_ostol << endl;
      }
    }
    inner_max_spd = getSpdSnappedHgh(inner_max_spd);
  }
#endif
  
  
  if(m_verbose) cout << "inner_max_spd(1):" << inner_max_spd << endl;
  double port_bnd_hdg = getMinFirstSatHdgAtSpd(inner_max_spd, cn_flee_hdg_step_up);
  
  if(m_verbose) cout << "cn_flee_hdg_step_down:" << cn_flee_hdg_step_down << endl;
  double star_bnd_hdg = getMaxFirstSatHdgAtSpd(inner_max_spd, cn_flee_hdg_step_down);
  
  inner_max_spd = m_domain.getNextHigherVal(m_spd_ix, inner_max_spd, 0);
  
  
  double min_spd = inner_max_spd;
  double max_spd = m_domain.getVarHigh(m_spd_ix);
  
  if(m_verbose) { 
    cout << "---" << endl;
    cout << "inner_max_spd(2):" << inner_max_spd << endl;
    cout << "cn_flee_hdg: " << cn_flee_hdg << endl;
    cout << "port_hdg: " << port_hdg << endl;
    cout << "star_hdg: " << star_hdg << endl;
    cout << "port_spd: " << port_speed << endl;
    cout << "star_spd: " << star_speed << endl;
    cout << "flee_spd: " << flee_speed << endl;
    cout << "port_bnd_hdg: " << port_bnd_hdg << endl;
    cout << "star_bnd_hdg: " << star_bnd_hdg << endl;
    cout << "min_spd: " << min_spd << endl;
    cout << "max_spd: " << max_spd << endl;
    cout << "---" << endl;
  }
  
  if((port_bnd_hdg >= 0) && (star_bnd_hdg >= 0)) {
    vector<IvPBox> boxes; 
    boxes = buildBoxesHdgSpd(m_domain, port_bnd_hdg, star_bnd_hdg,
			     min_spd, max_spd);
    for(unsigned int i=0; i<boxes.size(); i++)
      regions.push_back(boxes[i]);
  }
  
  if(m_verbose) {
    cout << "port speed:" << port_speed << endl;
    cout << "star speed:" << star_speed << endl;
    cout << "flee speed:" << flee_speed << endl;
  }
  
  if(m_quit2)
    return(regions);  // mikerb
  
  
  // =======================================================
  // Part C: Refine inner multi-piece areas not in initial piece
  // =======================================================
  double inner_max_spd_m1 = m_domain.getNextLowerVal(m_spd_ix,
						     inner_max_spd, 1);

  if(m_verbose) 
    cout << "inner_max_spd_m1:" << inner_max_spd_m1 << endl;
  
  if(port_speed > 0) {
    double port_inner_spd = ((flee_speed - port_speed)/4) + port_speed;
    port_inner_spd = getSpdSnappedHgh(port_inner_spd);
    if(m_verbose) {
      cout << "port_inner_spd:" << port_inner_spd << endl;
      cout << "cn_flee_hdg:" << cn_flee_hdg << endl;
      cout << "cn_hdg:" << m_cnh << endl;
    }
    
#if 0
    double port_min_hdg = getMinFirstSatHdgAtSpd(port_inner_spd, cn_flee_hdg_step_up);
    double port_max_hdg = getMaxFirstSatHdgAtSpd(port_inner_spd, m_cnh);
    if(m_verbose) {
      cout << "port_min_hdg:" << port_min_hdg << endl;
      cout << "port_max_hdg:" << port_max_hdg << endl;
    }
#endif

#if 1
    double port_min_hdg = getMinFirstSatHdgAtSpd(port_inner_spd, cn_flee_hdg_step_up);
    double port_max_hdg = getMaxLastSatHdgAtSpd(port_inner_spd, port_min_hdg);
    if(m_verbose) {
      cout << "port_min_hdg:" << port_min_hdg << endl;
      cout << "port_max_hdg:" << port_max_hdg << endl;
    }
#endif
    
    if(port_inner_spd < inner_max_spd_m1) {
      vector<IvPBox> boxes; 
      boxes = buildBoxesHdgSpd(m_domain, port_min_hdg, port_max_hdg,
			       port_inner_spd, inner_max_spd_m1);
      if(m_verbose)
	cout << "port_speed_boxes: " << endl;
      for(unsigned int i=0; i<boxes.size(); i++) {
	regions.push_back(boxes[i]);
	if(m_verbose)
	  boxes[i].print();
      }
    }
  }
  
  if(star_speed > 0) {
    double star_inner_spd = ((flee_speed - star_speed)/4) + star_speed;
    star_inner_spd = getSpdSnappedHgh(star_inner_spd);
    if(m_verbose)
      cout << "star_inner_spd:" << star_inner_spd << endl;


#if 0 
    double star_min_hdg = getMinFirstSatHdgAtSpd(star_inner_spd, m_cnh);
    double star_max_hdg = getMaxFirstSatHdgAtSpd(star_inner_spd, cn_flee_hdg_step_down);
    if(m_verbose) {
      cout << "star_min_hdg:" << star_min_hdg << endl;
      cout << "star_max_hdg:" << star_max_hdg << endl;
    }
#endif

#if 1
    double star_max_hdg = getMaxFirstSatHdgAtSpd(star_inner_spd, cn_flee_hdg_step_down);
    double star_min_hdg = getMinLastSatHdgAtSpd(star_inner_spd, star_max_hdg);
    if(m_verbose) {
      cout << "+star_min_hdg:" << star_min_hdg << endl;
      cout << "+star_max_hdg:" << star_max_hdg << endl;
    }
#endif
    
    if(star_inner_spd < inner_max_spd_m1) {
      vector<IvPBox> boxes; 
      boxes = buildBoxesHdgSpd(m_domain, star_min_hdg, star_max_hdg,
			       star_inner_spd, inner_max_spd_m1);
      if(m_verbose)
	cout << "star_speed_boxes:" << endl;
      for(unsigned int i=0; i<boxes.size(); i++) {
	regions.push_back(boxes[i]);
	if(m_verbose)
	  boxes[i].print();
      }
    }
  }

  if(1 || m_quit3)
    return(regions); // mikerb

  // =======================================================
  // Part D: Refine outer multi-piece areas not in initial piece
  // =======================================================

  if(port_speed != -1) {
    double port_inner_spd = inner_max_spd;
    port_inner_spd += (max_spd - inner_max_spd) / 2;

    double port_min_hdg = getMinFirstSatHdgAtSpd(port_inner_spd, cn_flee_hdg);
    
    vector<IvPBox> boxes; 
    boxes = buildBoxesHdgSpd(m_domain, port_min_hdg, port_bnd_hdg,
			     port_inner_spd, max_spd);
    for(unsigned int i=0; i<boxes.size(); i++)
      regions.push_back(boxes[i]);
  }

  if(star_speed != -1) {
    double star_inner_spd = inner_max_spd;
    star_inner_spd += (max_spd - inner_max_spd) / 2;
    
    double star_max_hdg = getMaxFirstSatHdgAtSpd(star_inner_spd, cn_flee_hdg);
    
    vector<IvPBox> boxes; 
    boxes = buildBoxesHdgSpd(m_domain, star_bnd_hdg, star_max_hdg,
			     star_inner_spd, max_spd);
    for(unsigned int i=0; i<boxes.size(); i++)
      regions.push_back(boxes[i]);
  }
  
  return(regions);
}


//-----------------------------------------------------------
// Procedure: getFleeSpeed()

double RefineryCPA::getFleeSpeed() const
{
  double flee_speed = m_cnv;
  double maxu_sq = m_max_util_cpa_dist * m_max_util_cpa_dist;
  double rgam_sq = m_range_gamma * m_range_gamma;
  double dist_cn_to_close = m_range_epsilon  - sqrt(maxu_sq - rgam_sq);
  if(dist_cn_to_close > 0) {
    double spd_cn_to_close = dist_cn_to_close / m_ostol;
    flee_speed = m_cnv - spd_cn_to_close;
  }

  return(flee_speed);
}




//----------------------------------------------------------------
// Procedure: getMinSpdAtHdg()
//   Purpose: For a given heading, find LOWEST speed value such that
//            cpa(hdg,spd) meets or exceeds m_max_util_cpa_dist.
//      Note: Start by checking the lowest speed value and then
//            repeatedly trying higher values until the first speed 
//            is found that is >= m_max_util_cpa_dist
//            (Search radially outward starting at the center)
//   Assumes: CPA values go low to high as speed goes from low to high
//   Returns: A number in the range [0,vmax], unless no speed values
//            meet or exceed the m_max_util_cpa_util threshold.
//            In this case a value of -1 is returned.

double RefineryCPA::getMinSpdAtHdg(double hdg) 
{
  unsigned int total_spds = m_domain.getVarPoints(m_spd_ix);

  if(m_verbose) {
    cout << "Total Speeds: " << total_spds << endl;
    cout << "CPA Dist: " << m_max_util_cpa_dist << endl;
    cout << "OSTOL: " << m_ostol << endl;
    cout << "HDG: " << hdg << endl;
  }
    
  double rval = -1;
  for(unsigned int i=0; i<total_spds; i++) {
    double spd = m_domain.getVal(m_spd_ix, i);
    double cpa = evalCPA(hdg, spd, m_ostol);
    if(m_verbose) cout << "speed=" << spd << ", cpa=" << cpa << endl;
    if(cpa >= m_max_util_cpa_dist)
      return(rval);
  }
  return(rval);
}

//----------------------------------------------------------------
// Procedure: getMaxSpdAtHdg()
//   Purpose: For a given heading, find Highest speed value such that
//            cpa(hdg,spd) meets or exceeds m_max_util_cpa_dist.
//      Note: Start by checking the highest speed value and then
//            repeatedly trying lower values until the first speed 
//            is found that is >= m_max_util_cpa_dist
//            (Search radially inward starting at the perimeter)
//   Assumes: CPA values go low to high as speed goes from low to high
//   Returns: A number in the range [0,vmax], unless no speed values
//            meet or exceed the m_max_util_cpa_util threshold.
//            In this case a value of -1 is returned.

double RefineryCPA::getMaxSpdAtHdg(double hdg) 
{
  unsigned int total_spds = m_domain.getVarPoints(m_spd_ix);

  if(m_verbose) {
    cout << "Total Speeds: " << total_spds << endl;
    cout << "CPA Dist: " << m_max_util_cpa_dist << endl;
    cout << "OSTOL: " << m_ostol << endl;
    cout << "HDG: " << hdg << endl;
  }
    
  double rval = -1;
  for(unsigned int i=0; i<total_spds; i++) {
    unsigned int index = (total_spds-1) - i;
    double spd = m_domain.getVal(m_spd_ix, index);
    double cpa = evalCPA(hdg, spd, m_ostol);
    if(m_verbose) {
      cout << "speed=" << spd << ", hdg=" << hdg << ",ostol=" << m_ostol;
      cout << ", cpa=" << cpa << endl;
    }
    if(cpa >= m_max_util_cpa_dist)
      rval = spd;
    else
      return(rval);
  }
  return(rval);
}

//----------------------------------------------------------------
// Procedure: getMinFirstSatHdgAtSpd()
//   Purpose: For a given speed and initial heading, find LOWEST
//            heading value such that cpa(hdg,spd) exceeds
//            m_max_util_cpa_dist.
//      Note: Start by checking the initial heading value and then
//            repeatedly trying higher values until success, returning
//            the first heading that succeeds
//            (Search clockwise)
//   Returns: A number in the range [0,359], unless no heading values
//            satisfies the m_max_util_cpa_util threshold. In this case
//            a value of -1 is returned.

double RefineryCPA::getMinFirstSatHdgAtSpd(double spd, double hmin) 
{
  if(m_verbose)
    cout << "In getMinFirstSatHdgAtSpd(): spd,hdt: " << spd << "," << hmin << endl;
  // Sanity checks
  if(!m_initialized)
    return(-1);
  if(spd <= 0)
    return(-1);
  if(hmin < m_domain.getVarLow(m_crs_ix))
    return(-1);
  if(hmin > m_domain.getVarHigh(m_crs_ix))
     return(-1);

  unsigned int total_crs_pts = m_domain.getVarPoints(m_crs_ix);

  unsigned int first_ix = m_domain.getDiscreteVal(m_crs_ix, hmin, 1);

  for(unsigned int i=0; i<total_crs_pts; i++) {
    unsigned int index = first_ix + i;
    if(index >= total_crs_pts)
      index -= total_crs_pts;
    double hdg = m_domain.getVal(m_crs_ix, index);
    double cpa = evalCPA(hdg, spd, m_ostol);

    if(m_verbose)
      cout << "hdg:" << hdg << ", cpa:" << cpa << endl;

    //if(m_verbose) {
    //  cout << "getMinFirstSatHdgAtSpd: " << endl;
    //  cout << "spd=" << spd << ", hdg=" << hdg << ", cpa=" << cpa << endl;
    // }
    if(cpa >= m_max_util_cpa_dist)
      return(hdg);
  }
  return(-1);
}
  
//----------------------------------------------------------------
// Procedure: getMaxFirstSatHdgAtSpd()
//   Purpose: For a given speed and initial heading, find HIGHEST
//            heading value such that cpa(hdg,spd) exceeds
//            m_max_util_cpa_dist.
//      Note: Start by checking the initial heading value and then
//            repeatedly trying lower values until success, returning
//            the first heading that succeeds
//            (Search counter-clockwise)
//   Returns: A number in the range [0,359], unless no heading values
//            satisfies the m_max_util_cpa_util threshold. In this case
//            a value of -1 is returned.

double RefineryCPA::getMaxFirstSatHdgAtSpd(double spd, double hmax) 
{
  if(m_verbose)
    cout << "In getMaxFirstSatHdgAtSpd(): spd,hdg" << spd << "," << hmax << endl;
  // Sanity checks
  if(!m_initialized)
    return(-1);
  if(spd <= 0)
    return(-1);
  if(hmax < m_domain.getVarLow(m_crs_ix))
    return(-1);
  if(hmax > m_domain.getVarHigh(m_crs_ix))
     return(-1);

  unsigned int total_crs_pts = m_domain.getVarPoints(m_crs_ix);

  unsigned int first_ix = m_domain.getDiscreteVal(m_crs_ix, hmax, 2);

  for(unsigned int i=0; i<total_crs_pts; i++) {
    unsigned int index = first_ix + (total_crs_pts - i);
    if(index >= total_crs_pts)
      index -= total_crs_pts;
    double hdg = m_domain.getVal(m_crs_ix, index);
    double cpa = evalCPA(hdg, spd, m_ostol);

    if(m_verbose)
      cout << "hdg:" << hdg << ", cpa:" << cpa << endl;
    
    //if(m_verbose) {
    //  cout << "getMaxFirstSatHdgAtSpd: " << endl;
    //  cout << "spd=" << spd << ", hdg=" << hdg << ", cpa=" << cpa << endl;
    // }
    if(cpa >= m_max_util_cpa_dist) {
      if(m_verbose)
	cout << "getMaxFirstSatHdgAtSpd:" << hdg << endl;
      return(hdg);
    }
  }
  return(-1);
}
  
//----------------------------------------------------------------
// Procedure: getMinLastSatHdgAtSpd()
//   Purpose: For a given speed and initial heading, find LOWEST/Last
//            heading value such that cpa(hdg,spd) exceeds
//            m_max_util_cpa_dist.
//      Note: Start by checking the initial heading value and then
//            repeatedly trying lower values until failure, returning
//            the last heading just prior to failure.
//            (Search counter-clockwise)
//   Returns: A number in the range [0,359], unless no heading values
//            fail the m_max_util_cpa_util threshold. In this case
//            a value of -2 is returned.
//   Returns: If first hdg value fails, return this hdg value immediately.

double RefineryCPA::getMinLastSatHdgAtSpd(double spd, double hmax) 
{
  // Sanity checks
  if(!m_initialized)
    return(-1);
  if(spd <= 0)
    return(-1);
  if(hmax < m_domain.getVarLow(m_crs_ix))
    return(-1); 

  
  // if((hmax > 359) && (hmax < 360))
  //  hmax = 0;

  if(hmax > m_domain.getVarHigh(m_crs_ix))
     return(-1);

  unsigned int total_crs_pts = m_domain.getVarPoints(m_crs_ix);

  unsigned int first_ix = m_domain.getDiscreteVal(m_crs_ix, hmax, 2);

  double prev_hdg = hmax;
  for(unsigned int i=0; i<total_crs_pts; i++) {
    unsigned int index = first_ix + (total_crs_pts - i);
    if(index >= total_crs_pts)
      index -= total_crs_pts;
    double hdg = m_domain.getVal(m_crs_ix, index);
    double cpa = evalCPA(hdg, spd, m_ostol);
    //if(m_verbose) {
    //  cout << "getMinLastSatHdgAtSpd: " << endl;
    //  cout << "spd=" << spd << ", hdg=" << hdg << ", cpa=" << cpa << endl;
    // }
    if(cpa < m_max_util_cpa_dist)
      return(prev_hdg);
    prev_hdg = hdg;
  }

  return(-2);
}
  
//----------------------------------------------------------------
// Procedure: getMaxLastSatHdgAtSpd()
//   Purpose: For a given speed and initial heading, find HIGHEST/Last
//            heading value such that cpa(hdg,spd) exceeds
//            m_max_util_cpa_dist.
//      Note: Start by checking the initial heading value and then
//            repeatedly trying higher values until failure, returning
//            the last heading just prior to failure.
//            (Search clockwise)
//   Returns: A number in the range [0,359], unless no heading values
//            fail the m_max_util_cpa_util threshold. In this case
//            a value of -2 is returned.
//   Returns: If first hdg value fails, return this hdg value immediately.

double RefineryCPA::getMaxLastSatHdgAtSpd(double spd, double hmin)
{
  // Sanity checks
  if(!m_initialized)
    return(-1);
  if(spd <= 0)
    return(-1);
  if(hmin < m_domain.getVarLow(m_crs_ix))
    return(-1);

  if((hmin > 359) && (hmin < 360))
    hmin = 359;
  
  if(hmin > m_domain.getVarHigh(m_crs_ix))
     return(-1);

  unsigned int total_crs_pts = m_domain.getVarPoints(m_crs_ix);

  unsigned int first_ix = m_domain.getDiscreteVal(m_crs_ix, hmin, 2);

  double prev_hdg = hmin;
  for(unsigned int i=0; i<total_crs_pts; i++) {
    unsigned int index = first_ix + i;
    if(index >= total_crs_pts)
      index -= total_crs_pts;
    double hdg = m_domain.getVal(m_crs_ix, index);
    double cpa = evalCPA(hdg, spd, m_ostol);
    //if(m_verbose) {
    //  cout << "getMaxLastSatHdgAtSpd: " << endl;
    //  cout << "spd=" << spd << ", hdg=" << hdg << ", cpa=" << cpa << endl;
    // }
    if(cpa < m_max_util_cpa_dist)
      return(prev_hdg);
    prev_hdg = hdg;
  }

  return(-2);
}
  

//----------------------------------------------------------------
// Procedure: getMinSpdClockwise
//   Purpose: For a given speed and range of headings, find the largest
//            speed for which all headings in the range result in 
//            cpa(hdg,spd) values that >= m_max_util_cpa_dist.
//      Note: Start by checking the initial heading value and then
//            sweeping clockwise, repeatedly trying higher headings
//            until the final heading is reached. Each time a heading
//            value fails, retry with one speed notch lower.
//   Returns: A number in the range [0,max_speed].

double RefineryCPA::getMinSpdClockwise(double hmin, double hmax,
				       double start_spd)
{
  // Sanity checks
  if(!m_initialized)
    return(-1);
  if(!validHdg(hmin) || !validHdg(hmax) || !validSpd(start_spd))
    return(-1);
  
  unsigned int total_crs_pts = m_domain.getVarPoints(m_crs_ix);
  unsigned int range_crs_pts = getHdgDiscPts(hmin, hmax, 1);

  unsigned int first_ix = m_domain.getDiscreteVal(m_crs_ix, hmin, 2);

  double curr_spd = start_spd;

  for(unsigned int i=0; i<range_crs_pts; i++) {
    unsigned int index = first_ix + i;
    if(index >= total_crs_pts)
      index -= total_crs_pts;
    double hdg = m_domain.getVal(m_crs_ix, index);
    double cpa = evalCPA(hdg, curr_spd, m_ostol);
    if(m_verbose) cout << "getMinSpdClockwise: spd=" << curr_spd << ", hdg="
	 << hdg << ", cpa=" << cpa << endl;
    if(cpa < m_max_util_cpa_dist) {
      bool done = false;
      while(!done) {
	//if(m_verbose) cout << "  curr_spd: " << curr_spd << endl;
	curr_spd = m_domain.getNextLowerVal(m_spd_ix, curr_spd, 0);
	//if(m_verbose) cout << "  new_curr_spd: " << curr_spd << endl;
	if(curr_spd == 0)
	  return(0);
	cpa = evalCPA(hdg, curr_spd, m_ostol);
	if(cpa >= m_max_util_cpa_dist) 
	  done = true;
      }
    }
  }
  return(curr_spd);
}
  

//----------------------------------------------------------------
// Procedure: getMinSpdCounterClock
//   Purpose: For a given speed and range of headings, find the largest
//            speed for which all headings in the range result in 
//            cpa(hdg,spd) values >= m_max_util_cpa_dist.
//      Note: Start by checking the initial heading value and then
//            sweeping clockwise, repeatedly trying higher headings
//            until the final heading is reached. Each time a heading
//            value fails, retry with one speed notch lower.
//   Returns: A number in the range [0,max_speed].

double RefineryCPA::getMinSpdCounterClock(double hmin, double hmax,
					  double start_spd)
{
  // Sanity checks
  if(!m_initialized)
    return(-1);
  if(!validHdg(hmin) || !validHdg(hmax) || !validSpd(start_spd))
    return(-1);
  
  unsigned int total_crs_pts = m_domain.getVarPoints(m_crs_ix);
  unsigned int range_crs_pts = getHdgDiscPts(hmin, hmax, 1);

  unsigned int first_ix = m_domain.getDiscreteVal(m_crs_ix, hmax, 2);

  double curr_spd = start_spd;

  for(unsigned int i=0; i<range_crs_pts; i++) {
    unsigned int index = first_ix + (total_crs_pts - i);
    if(index >= total_crs_pts)
      index -= total_crs_pts;
    double hdg = m_domain.getVal(m_crs_ix, index);
    double cpa = evalCPA(hdg, curr_spd, m_ostol);
    //if(m_verbose)
    //  cout << "getMinSpdCounterClock: spd=" << curr_spd << ", hdg="
    //	   << hdg << ", cpa=" << cpa << endl;
    if(cpa < m_max_util_cpa_dist) {
      bool done = false;
      while(!done) {
	//if(m_verbose) cout << "  curr_spd: " << curr_spd << endl;
	curr_spd = m_domain.getNextLowerVal(m_spd_ix, curr_spd, 0);
	//if(m_verbose) cout << "  new_curr_spd: " << curr_spd << endl;
	if(curr_spd == 0)
	  return(0);
	cpa = evalCPA(hdg, curr_spd, m_ostol);
	if(cpa >= m_max_util_cpa_dist) 
	  done = true;
      }
    }
  }
  return(curr_spd);
}
  
//----------------------------------------------------------------
// Procedure: getPassHdgClockwise

double RefineryCPA::getPassHdgClockwise(double hdg, double start_spd,
					double end_spd)
{
  // Sanity checks
  if(!m_initialized)
    return(-1);
  if(!validHdg(hdg) || !validSpd(start_spd))
    return(-1);
  
  // Phase 1: Initial sweep clockwise until a (hdg,spd) pair has a
  // cpa value that is less than m_max_util_cpa_dist.

  double hdgx = getMaxLastSatHdgAtSpd(start_spd, hdg);
  // Sanity check: If sweep fails, just reutrn initial hdg
  if(hdgx < 0)
    return(hdg);

  // Phase 2: Secondary sweep counter-clockwise until a hdg is found
  // that has a cpa(hdg,spd) >= m_max_util_cpa_dist for all speeds
  
  unsigned int total_crs_pts = m_domain.getVarPoints(m_crs_ix);
  unsigned int first_ix = m_domain.getDiscreteVal(m_crs_ix, hdgx, 2);
  double curr_spd = start_spd;

  if(m_verbose)
    cout << "getPassHdgClockwise: max_util_cpa=" << m_max_util_cpa_dist << endl;
  for(unsigned int i=0; i<total_crs_pts; i++) {
    unsigned int index = first_ix + (total_crs_pts - i);
    if(index >= total_crs_pts)
      index -= total_crs_pts;

    double hdg = m_domain.getVal(m_crs_ix, index);
    double cpa = evalCPA(hdg, curr_spd, m_ostol);
    //cout << "getPassHdgClockwise: spd=" << curr_spd << ", hdg="
    // << hdg << ", cpa=" << cpa << endl;

    // if initial cpa succeeds, try lower speeds. Perhaps down to 0.
    if(cpa >= m_max_util_cpa_dist) {
      bool done = false;
      double prev_curr_spd = curr_spd;
      while(!done) {
	//if(m_verbose) cout << "  curr_spd: " << curr_spd << endl;
	curr_spd = m_domain.getNextLowerVal(m_spd_ix, curr_spd, 0);
	//if(m_verbose) cout << "  new_curr_spd: " << curr_spd << endl;
	if((curr_spd == 0) || (curr_spd <= end_spd))
	  return(hdg);
	double try_cpa = evalCPA(hdg, curr_spd, m_ostol);
	if(try_cpa < m_max_util_cpa_dist) {
	  done = true;
	  curr_spd = prev_curr_spd;
	}
	else
	  prev_curr_spd = curr_spd;
      }
    }

    // if initial cpa fails, try higher speeds. Perhaps up to start_spd.
    else {
      bool done = false;
      while(!done) {
	double new_curr_spd = m_domain.getNextHigherVal(m_spd_ix, curr_spd, 0);
	if(new_curr_spd == curr_spd)
	  done = true;
	else {
	  curr_spd = new_curr_spd;
	  double try_cpa = evalCPA(hdg, curr_spd, m_ostol);
	  if(try_cpa >= m_max_util_cpa_dist) {
	    done = true;
	  }
	}
      }
    }
    
  }
  // If the Phase 2 sweep made its way all the way through the for-loop,
  // all 360 degrees, something went wrong and we default to return the
  // initial hdg.
  return(hdg);
}
  
//----------------------------------------------------------------
// Procedure: getFleeSpdClockwise
#if 0
double RefineryCPA::getFleeSpdClockwise(double hdg, double start_spd,
					double end_spd)
{
  // Sanity checks
  if(!m_initialized)
    return(-1);
  if(!validHdg(hdg) || !validSpd(start_spd))
    return(-1);
  
  unsigned int total_crs_pts = m_domain.getVarPoints(m_crs_ix);
  unsigned int first_ix = m_domain.getDiscreteVal(m_crs_ix, hdgx, 0);
  double curr_spd = start_spd;

  for(unsigned int i=0; i<total_crs_pts; i++) {
    unsigned int index = first_ix + i;
    if(index >= total_crs_pts)
      index -= total_crs_pts;

    double hdg = m_domain.getVal(m_crs_ix, index);
    double cpa = evalCPA(hdg, curr_spd, m_ostol);
    if(m_verbose)
      cout << "getFleeSpdClockwise: spd=" << curr_spd << ", hdg="
	   << hdg << ", cpa=" << cpa << endl;

    // if initial cpa 
    if(cpa < m_max_util_cpa_dist) {
      bool done = false;
      double prev_curr_spd = curr_spd;
      while(!done) {
	curr_spd = m_domain.getNextHigherVal(m_spd_ix, curr_spd, 0);
	if(curr_spd >= end_spd)
	  return(curr_spd);
	double try_cpa = evalCPA(hdg, curr_spd, m_ostol);
	if(try_cpa >= m_max_util_cpa_dist) {
	  done = true;
	  curr_spd = prev_curr_spd;
	}
	else
	  prev_curr_spd = curr_spd;
      }
    }

    // if initial cpa fails, try higher speeds. Perhaps up to start_spd.
    else {
      bool done = false;
      while(!done) {
	double new_curr_spd = m_domain.getNextHigherVal(m_spd_ix, curr_spd, 0);
	if(new_curr_spd == curr_spd)
	  done = true;
	else {
	  curr_spd = new_curr_spd;
	  double try_cpa = evalCPA(hdg, curr_spd, m_ostol);
	  if(try_cpa >= m_max_util_cpa_dist) {
	    done = true;
	  }
	}
      }
    }
    
  }
  // If the Phase 2 sweep made its way all the way through the for-loop,
  // all 360 degrees, something went wrong and we default to return the
  // initial hdg.
  return(hdg);
}
#endif
  
//----------------------------------------------------------------
// Procedure: getPassHdgCounterClock

double RefineryCPA::getPassHdgCounterClock(double hdg, double start_spd,
					   double end_spd)
{
  // Sanity checks
  if(!m_initialized)
    return(-1);
  if(!validHdg(hdg) || !validSpd(start_spd))
    return(-1);
  
  // Phase 1: Initial sweep counter-clockwise until a (hdg,spd)
  // pair has a cpa value that is less than m_max_util_cpa_dist.

  double hdgx = getMinLastSatHdgAtSpd(start_spd, hdg);
  // Sanity check: If sweep fails, just reutrn initial hdg
  if(hdgx < 0)
    return(hdg);

  // Phase 2: Secondary sweep clockwise until a hdg is found
  // that has a cpa(hdg,spd) >= m_max_util_cpa_dist for all speeds
  
  unsigned int total_crs_pts = m_domain.getVarPoints(m_crs_ix);
  unsigned int first_ix = m_domain.getDiscreteVal(m_crs_ix, hdgx, 2);
  double curr_spd = start_spd;

  if(m_verbose)
    cout << "getPassHdgCounterClock: max_util_cpa=" << m_max_util_cpa_dist << endl;
  for(unsigned int i=0; i<total_crs_pts; i++) {
    unsigned int index = first_ix + i;
    if(index >= total_crs_pts)
      index -= total_crs_pts;

    double hdg = m_domain.getVal(m_crs_ix, index);
    double cpa = evalCPA(hdg, curr_spd, m_ostol);
    if(m_verbose)
      cout << "getPassHdgCounterClock: spd=" << curr_spd << ", hdg="
	   << hdg << ", cpa=" << cpa << endl;
    
    // if initial cpa succeeds, try lower speeds. Perhaps down to 0.
    if(cpa >= m_max_util_cpa_dist) {
      bool done = false;
      double prev_curr_spd = curr_spd;
      while(!done) {
	//cout << "  curr_spd: " << curr_spd << endl;
	curr_spd = m_domain.getNextLowerVal(m_spd_ix, curr_spd, 0);
	//cout << "  new_curr_spd: " << curr_spd << endl;
	if((curr_spd == 0) || (curr_spd <= end_spd))
	  return(hdg);
	double try_cpa = evalCPA(hdg, curr_spd, m_ostol);
	if(try_cpa < m_max_util_cpa_dist) { 
	  done = true;
	  curr_spd = prev_curr_spd;
	}
	else
	  prev_curr_spd = curr_spd;
      }
    }

    // if initial cpa fails, try higher speeds. Perhaps up to start_spd.
    else {
      bool done = false;
      while(!done) {
	double new_curr_spd = m_domain.getNextHigherVal(m_spd_ix, curr_spd, 0);
	if(new_curr_spd == curr_spd)
	  done = true;
	else {
	  curr_spd = new_curr_spd;
	  double try_cpa = evalCPA(hdg, curr_spd, m_ostol);
	  if(try_cpa >= m_max_util_cpa_dist) {
	    done = true;
	  }
	}
      }
    }

  }
  // If the Phase 2 sweep made its way all the way through the for-loop,
  // all 360 degrees, something went wrong and we default to return the
  // initial hdg.
  return(hdg);
}
  

//----------------------------------------------------------------
// Procedure: getHdgSnappedHgh()

double RefineryCPA::getHdgSnappedHgh(double hdg) const
{
  if(hdg > m_domain.getVarHigh(m_crs_ix))
    return(0);
  
  unsigned int uint_val = m_domain.getDiscreteVal(m_crs_ix, hdg, 1);
  double new_hdg = m_domain.getVal(m_crs_ix, uint_val);

  return(new_hdg);
}

//----------------------------------------------------------------
// Procedure: getHdgSnappedLow()

double RefineryCPA::getHdgSnappedLow(double hdg) const
{
  unsigned int uint_val = m_domain.getDiscreteVal(m_crs_ix, hdg, 0);
  double new_hdg = m_domain.getVal(m_crs_ix, uint_val);

  return(new_hdg);
}

//----------------------------------------------------------------
// Procedure: getSpdSnappedHgh()

double RefineryCPA::getSpdSnappedHgh(double spd) const
{
  unsigned int uint_val = m_domain.getDiscreteVal(m_spd_ix, spd, 1);
  double new_spd = m_domain.getVal(m_spd_ix, uint_val);

  return(new_spd);
}

//----------------------------------------------------------------
// Procedure: getSpdSnappedLow()

double RefineryCPA::getSpdSnappedLow(double spd) const
{
  unsigned int uint_val = m_domain.getDiscreteVal(m_spd_ix, spd, 0);
  double new_spd = m_domain.getVal(m_spd_ix, uint_val);

  return(new_spd);
}

//----------------------------------------------------------------
// Procedure: validHdg()

bool RefineryCPA::validHdg(double hval) const
{
  if(hval < m_domain.getVarLow(m_crs_ix))
    return(false);
  if(hval > m_domain.getVarHigh(m_crs_ix))
    return(false);
  return(true);
}

//----------------------------------------------------------------
// Procedure: validSpd()

bool RefineryCPA::validSpd(double sval) const
{
  if(sval < m_domain.getVarLow(m_spd_ix))
    return(false);
  if(sval > m_domain.getVarHigh(m_spd_ix))
    return(false);
  return(true);
}

//----------------------------------------------------------------
// Procedure: getHdgDiscPts()
//
//      Note: snap = 0: values are truncated down
//            snap = 1: values are ceiling'ed up
//            snap = 2: values are to the nearest discrete point  

unsigned int RefineryCPA::getHdgDiscPts(double hmin, double hmax,
					int snap) const
{
  double delta = m_domain.getVarDelta(m_crs_ix);
  if(delta <= 0)
    return(0);
  
  double range = 0;
  if(hmin <= hmax)  // No wrap
    range = hmax - hmin;
  else              // Wrap
    range = (360 - hmin) + hmax;

  double dcount = 1 + (range / delta);

  unsigned int count = 0;
  if(snap == 0) 
    count = (unsigned int)(dcount);
  else if(snap == 1) {
    count = (unsigned int)(dcount);
    if((double)(count) != dcount)
      count++;
  }
  else if(snap == 2) 
    count = (unsigned int)(dcount + 0.5);

  return(count);  
}

