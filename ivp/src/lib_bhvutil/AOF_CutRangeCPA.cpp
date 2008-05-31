/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_CutRangeCPA.cpp                                  */
/*    DATE: Nov 4th, 2006                                        */
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
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <math.h> 
#include "AOF_CutRangeCPA.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor
//      args: gcnlat  Given Contact Latitude Position
//      args: gcnlon  Given Contact Longitude Position
//      args: gcncrs  Given Contact Course
//      args: gcnspd  Given Contact Speed
//      args: goslat  Given Ownship Latitude Position
//      args: goslon  Given Ownship Latitude Position

AOF_CutRangeCPA::AOF_CutRangeCPA(IvPDomain gdomain)
  : AOF(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");

  m_osx_set = false;
  m_osy_set = false;
  m_cnx_set = false;
  m_cny_set = false;
  m_cnh_set = false;
  m_cnv_set = false;
  m_tol_set = false;

  m_cpa_engine = 0;
  m_patience   = 100;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_CutRangeCPA::setParam(const string& param, double param_val)
{
  if((param == "oslat") || (param == "m_osy")) {
    m_osy = param_val;
    m_osy_set = true;
    return(true);
  }
  else if((param == "oslon") || (param == "oslon")) {
    m_osx = param_val;
    m_osx_set = true;
    return(true);
  }
  else if((param == "cnlat") || (param == "cnlat")) {
    m_cny = param_val;
    m_cny_set = true;
    return(true);
  }
  else if((param == "cnlon") || (param == "cnlon")) {
    m_cnx = param_val;
    m_cnx_set = true;
    return(true);
  }
  else if((param == "cncrs") || (param == "cncrs")) {
    m_cnh = param_val;
    m_cnh_set = true;
    return(true);
  }
  else if((param == "cnspd") || (param == "cnspd")) {
    m_cnv = param_val;
    m_cnv_set = true;
    return(true);
  }
  else if(param == "tol") {
    m_tol = param_val;
    m_tol_set = true;
    return(true);
  }
  else if(param == "patience") {
    if((param_val < 0) || (param_val > 100))
      return(false);
    m_patience = param_val;
    return(true);
  }
  // To support backward compatibility - these terms allow, but ignored.
  else if(param == "min_util_cpa_dist")
    return(true);
  else if(param == "max_util_cpa_dist")
    return(true);
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_CutRangeCPA::initialize()
{
  if((m_crs_ix==-1)||(m_spd_ix==-1))
    return(false);
  
  if(!m_osx_set || !m_osy_set || !m_cnx_set || !m_tol_set)
    return(false);
  
  if(!m_cny_set || !m_cnh_set || !m_cnv_set)
    return(false);
  
  if(m_tol < 1)
    return(false);
  
  m_distance_os_cn = distPointToPoint(m_osx, m_osy, m_cnx, m_cny); 
  
  // This AOF should not be used if the distance between ownship and 
  // the point of interest is already zero. The user should have 
  // checked for this prior to creation and initialization.
  if(m_distance_os_cn <= 0)
    return(false);

  m_cpa_engine = new CPAEngine(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);

  double max_ownship_spd = m_domain.getVarHigh(m_spd_ix);

  m_cpa_engine->minMaxROC(max_ownship_spd, 360, m_min_roc, m_max_roc);
  
  m_range_roc = m_max_roc - m_min_roc;

  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Eval given <Course, Speed> tuple given by a 2D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_CutRangeCPA::evalBox(const IvPBox *b) const
{
  double eval_crs = 0;
  double eval_spd = 0;

  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);

  // Calculate the CPA distance and the RateOfClosure for a maneuver
  double roc;
  double eval_dist = m_cpa_engine->evalCPA(eval_crs, eval_spd, m_tol, &roc);
  double metric_eval = metric(eval_dist);

  // Calculate the normalized RateOfClosure based on the ROC and Range
  double nroc = 0;
  if(m_range_roc > 0)
    nroc = ((roc - m_min_roc) / (m_range_roc)) * 100.0;

  // Calculate a valuation based on the ROC and CPA
  double pct = m_patience / 100.0;
  double compromise = ((1.0-pct) * nroc) + (pct * metric_eval);

  return(compromise);
}

//----------------------------------------------------------------
// Procedure: metric
//     Notes: The given value is a CPA (Closest Point of Approach) 
//            value. By definition, for any candidate maneuver, 
//            this value must be between (a) the current distance 
//            between ownship and the contact and (b) zero. So the
//            utility is based simply on where the value lies in 
//            this range.

double AOF_CutRangeCPA::metric(double gval) const
{
  // This shouldn't happen, but we'll check anyway in case the number
  // is negative due to machine precision of zero.
  if(gval < 0)
    return(100);

  // This shouldn't happen, but we'll check anyway since a zero would
  // mean a NaN in the devision below.
  if(m_distance_os_cn <= 0)
    return(0);

  // This shouldn't happen, but we'll check anyway 
  if(gval > m_distance_os_cn)
    return(0);

  double pct = gval / m_distance_os_cn;
  return(100 - (pct * 100));
}









