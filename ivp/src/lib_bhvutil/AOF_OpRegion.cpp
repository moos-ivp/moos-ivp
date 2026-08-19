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

  // Init state vars
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");

  m_min_util_eta_is_set = false;
  m_max_util_eta_is_set = false;
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
  else
    return(false);

  return(true);
}

//----------------------------------------------------------------
// Procedure: setParam()

bool AOF_OpRegion::setParam(const string& param, const string& value)
{
  return(false);
}

//----------------------------------------------------------------
// Procedure: initialize()

bool AOF_OpRegion::initialize()
{
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
// Procedure: evalBox

double AOF_OpRegion::evalBox(const IvPBox *b) const
{
#if 1
  //===============================================================
  // Part 1: Declare utility range and get the eval crs and speed
  //===============================================================

  //double min_util = getKnownMin();
  //double max_util = getKnownMax();
  double eval_crs = 0;
  double eval_spd = 0;
  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);

  // Edge case
  if(eval_spd <= 0)
    return(100);

  XYSeglr seglr = m_plat_model.getTurnSeglr(eval_crs);

  double dist_to_exit = m_gpoly.distSeglrToExitGP(seglr);

  //cout << "gpoly:" << m_gpoly.get_spec() << endl;
  //cout << "seglr:" << seglr.get_spec() << endl;  
  //cout << doubleToString(eval_crs,1) << ",";
  //cout << doubleToString(eval_spd,1) << ": dist:";
  //cout << doubleToString(dist_to_exit) << endl;
  
  double eta = dist_to_exit / eval_spd;

  if(eta < m_min_util_eta)
    return(0);
  else if(eta >= m_min_util_eta)
    return(100);

  double range = m_max_util_eta - m_min_util_eta;
  if(range <= 0)
    return(0);

  double part = eta - m_min_util_eta;
  double pct  = part / range;
  double rval = pct * 100;
  
  return(rval);
#endif
}
