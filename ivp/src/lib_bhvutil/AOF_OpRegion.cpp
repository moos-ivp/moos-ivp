/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng, MIT Cambridge MA             */
/*    FILE: AOF_OpRegion                                         */
/*    DATE: Aug 10th, 2026                                       */
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

#include <cmath>
#include <string>
#include "AOF_OpRegion.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------
// Constructor()

AOF_OpRegion::AOF_OpRegion(IvPDomain gdomain) :
  AOF(gdomain)
{
  // Init config vars
  m_min_util_eta = 5;
  m_max_util_eta = 25;
  m_min_util_cpa = 10;
  m_max_util_cpa = 25;
  m_cpa_window   = 45; 

  m_eta_factored = true;
  m_cpa_factored = true;
  
  // Init state vars
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");

  m_min_util_eta_is_set = false;
  m_max_util_eta_is_set = false;
  m_min_util_cpa_is_set = false;
  m_max_util_cpa_is_set = false;
  m_cpa_window_is_set = false;
}

//----------------------------------------------------------------
// Procedure: setParam()
//      Note: osx, osy, osh, osv embedded in the plat model

bool AOF_OpRegion::setParam(const string& param, double val)
{
  if((param == "min_util_eta") && (val > 0)) {
    m_min_util_eta = val;
    m_min_util_eta_is_set = true;
  }
  else if((param == "max_util_eta") && (val > 0)) {
    m_max_util_eta = val;
    m_max_util_eta_is_set = true;
  }
  else if((param == "min_util_cpa") && (val > 0)) {
    m_min_util_cpa = val;
    m_min_util_cpa_is_set = true;
  }
  else if((param == "max_util_cpa") && (val > 0)) {
    m_max_util_cpa = val;
    m_max_util_cpa_is_set = true;
  }
  else if((param == "cpa_window") && (val > 0)) {
    m_cpa_window = val;
    m_cpa_window_is_set = true;
  }
  else
    return(false);

  return(true);
}

//----------------------------------------------------------------
// Procedure: setParam()

bool AOF_OpRegion::setParam(const string& param, const string& value)
{
  if(param == "eta_factored")
    return(setBooleanOnString(m_eta_factored, value));
  else if(param == "cpa_factored")
    return(setBooleanOnString(m_cpa_factored, value));

  return(false);
}

//----------------------------------------------------------------
// Procedure: initialize()

bool AOF_OpRegion::initialize()
{
  cout << "AOF_OpRegion eta_factored: " << boolToString(m_eta_factored) << endl;
  
  // Part 1: Sanity Checks
  if(m_crs_ix == -1) 
    return(postMsgAOF("crs_ix is not set"));
  if(m_spd_ix == -1) 
    return(postMsgAOF("spd_ix is not set"));

  if(!m_min_util_eta_is_set)
    return(postMsgAOF("min_util_eta is not set"));
  if(!m_max_util_eta_is_set)
    return(postMsgAOF("max_util_eta not set"));
  
  if(m_min_util_eta > m_max_util_eta)
    return(postMsgAOF("min_util_eta is greater than max_util_eta"));

  // osx, osy, osh, osv embedded in the plat model
  if(!m_plat_model.valid()) 
    return(postMsgAOF("Invalid PlatModel"));

  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox()

double AOF_OpRegion::evalBox(const IvPBox *b) const
{
  //===============================================================
  // Part 1: Declare utility range and get the eval crs and speed
  //===============================================================
  double eval_crs = 0;
  double eval_spd = 0;
  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);

  // Edge case
  if(eval_spd <= 0)
    return(100);

  // Part 1: Calculate the two forms of util if they are being used
  double util_eta = 0;
  if(m_eta_factored)
    util_eta = evalUtilETA(eval_crs, eval_spd);

  double util_cpa = 0;
  if(m_cpa_factored) {
    util_cpa = evalUtilCPA(eval_crs, eval_spd);
    if(eval_spd > 3) {
      cout << "eval_crs=" << doubleToStringX(eval_crs,1);
      cout << ", eval_spd=" << doubleToString(eval_spd,1);
      cout << ", util_cpa=" << doubleToString(util_cpa) << endl;
    }
  }
      

  // Part 2: Combine the two forms of util if applicable
  double final_util = 0;

  if(m_eta_factored)
    final_util = util_eta;

  if(m_cpa_factored) {
    if(!m_eta_factored)
      final_util = util_cpa;
    else { 
      if(util_cpa < util_eta) {
	final_util = util_cpa;
      }
    }
  }

  if(eval_spd > 3)
    cout << "final_util=" << doubleToString(final_util,2) << endl;
  
  return(final_util);
}

//----------------------------------------------------------------
// Procedure: evalUtilETA()

double AOF_OpRegion::evalUtilETA(double eval_crs,
				 double eval_spd) const
{
  double min_util = getKnownMin();
  double max_util = getKnownMax();
  double rng_util = max_util - min_util;
  // Sanity check
  if(rng_util <= 0)
    return(0);

  XYSeglr seglr = m_plat_model.getTurnSeglr(eval_crs);
  double  dist_to_exit = m_gpoly.distSeglrToExitGP(seglr);
  double  eta = dist_to_exit / eval_spd;

  //cout << "e crs: " << doubleToString(eval_crs) << endl;
  //cout << "e seglr: " << seglr.get_spec() << endl; 
  //cout << "e eta: " << doubleToString(eta) << endl;

  
  if(eta < m_min_util_eta)
    return(min_util);
  else if(eta >= m_max_util_eta)
    return(max_util);

  double range = m_max_util_eta - m_min_util_eta;
  if(range <= 0)
    return(0);

  double part = eta - m_min_util_eta;
  double pct  = part / range;
  double rval = pct * rng_util;
  
  return(rval);
}

//----------------------------------------------------------------
// Procedure: evalUtilCPA()

double AOF_OpRegion::evalUtilCPA(double eval_crs,
				 double eval_spd) const
{
  double min_util = getKnownMin();
  double max_util = getKnownMax();
  double rng_util = max_util - min_util;
  // Sanity check
  if(rng_util <= 0)
    return(0);
  

  XYSeglr seglr = m_plat_model.getTurnSeglr(eval_crs);

  double cpa_eval_dist = eval_spd * m_cpa_window;

  bool verbose = false;
  if(eval_spd > 3) {
    cout << endl;
    verbose = true;
  }
  double cpa = m_gpoly.cpaSeglrToGP(seglr, cpa_eval_dist, verbose);

  if(eval_spd > 3) {
    cout << "crs: " << doubleToStringX(eval_crs,1);
    cout << ", spd: " << doubleToString(eval_spd,1) << endl;
    cout << "seglr: " << seglr.get_spec() << endl; 
    cout << "dist: " << cpa_eval_dist << endl; 
    cout << "cpa: " << doubleToString(cpa) << endl;
    cout << "min_util_cpa=" << doubleToStringX(m_min_util_cpa,1);
    cout << ", max_util_cpa=" << doubleToStringX(m_max_util_cpa,1);
  }

  if(cpa < m_min_util_cpa)
    return(min_util);
  else if(cpa >= m_max_util_cpa)
    return(max_util);

  double range = m_max_util_cpa - m_min_util_cpa;
  if(range <= 0)
    return(0);

  
  double part = cpa - m_min_util_cpa;
  double pct  = part / range;
  double rval = pct * rng_util;

  if(eval_spd > 3) {
    cout << "range=" << doubleToString(range);
    cout << ",rng_util=" << doubleToString(rng_util);
    cout << ",part=" << doubleToString(part,2);
    cout << ",pct=" << doubleToString(pct,2);
    cout << ",rval=" << doubleToString(pct,2) << endl;
  }
  return(rval);
}
