/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_HDG.cpp                                         */
/*    DATE: May 11th 2016                                        */
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
#include <cmath>
#include "ZAIC_HDG.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

ZAIC_HDG::ZAIC_HDG(IvPDomain domain, string varname) 
{
  m_summit  = 0;
  m_hdelta  = 0;
  m_ldelta  = 0;
  m_hdelta_util = 50;
  m_ldelta_util = 50;

  m_lminutil = 0;
  m_hminutil = 0;
  m_maxutil  = 100;

  m_domain_high  = 0;
  m_domain_low   = 0;
  m_domain_pts   = 0;
  m_domain_delta = 0;

  m_ivp_domain = subDomain(domain, varname);
  if(m_ivp_domain.size() == 0)
    return;

  unsigned int domain_ix = m_ivp_domain.getIndex(varname);

  m_domain_high  = m_ivp_domain.getVarHigh(domain_ix);
  m_domain_low   = m_ivp_domain.getVarLow(domain_ix);
  m_domain_pts   = m_ivp_domain.getVarPoints(domain_ix);
  m_domain_delta = m_ivp_domain.getVarDelta(domain_ix);

  //=================================================================
  // Sanity Check - this ZAIC utility is meant to work on a domain
  // whose range is [0, 360), and has at least 4 domain points. If it
  // doesn't meet all these criteria, create a sensible alternative.
  //=================================================================
  if((m_domain_low < 0) || (m_domain_high >= 360) || (m_domain_pts < 4)) {
    IvPDomain domain;
    domain.addDomain(varname, 0, 359, 360);
    m_ivp_domain = domain;
    m_domain_high  = m_ivp_domain.getVarHigh(0);
    m_domain_low   = m_ivp_domain.getVarLow(0);
    m_domain_pts   = m_ivp_domain.getVarPoints(0);
    m_domain_delta = m_ivp_domain.getVarDelta(0);
  }
  
  vector<double> ptvals(m_domain_pts, 0);
  m_ptvals = ptvals;  
}

//-------------------------------------------------------------
// Procedure: setParams

bool ZAIC_HDG::setParams(double summit, double ldelta, double hdelta,
			 double ldelta_util, double hdelta_util,  
			 double lminutil, double hminutil, double maxutil)
{
  if(m_ivp_domain.size() == 0)
    return(false);

  bool ok = setSummit(summit);
  ok = ok && setLowDelta(ldelta);
  ok = ok && setHighDelta(hdelta);
  ok = ok && setLowDeltaUtil(ldelta_util);
  ok = ok && setHighDeltaUtil(hdelta_util);
  ok = ok && setMinMaxUtil(lminutil, hminutil, maxutil);

  return(ok);
}

//-------------------------------------------------------------
// Procedure: setSummit

bool ZAIC_HDG::setSummit(double val)
{
  if((m_ivp_domain.size() == 0) ||(val < m_domain_low) || (val > m_domain_high))
    return(false);
  
  m_summit = val;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setLowDelta

bool ZAIC_HDG::setLowDelta(double val)
{
  if(m_ivp_domain.size() == 0)
    return(false);

  if(val < 0) 
    val = 0;
  else if(val > 180) 
    val = 180;

  m_ldelta = val;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setHighDelta

bool ZAIC_HDG::setHighDelta(double val)
{
  if(m_ivp_domain.size() == 0)
    return(false);

  if(val < 0)
    val = 0;
  else if(val > 180) 
    val = 180;

  m_hdelta = val;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setLowDeltaUtil

bool ZAIC_HDG::setLowDeltaUtil(double val)
{
  if(m_ivp_domain.size() == 0)
    return(false);

  if(val < m_lminutil)
    val = m_lminutil;
  else if(val > m_maxutil)
    val = m_maxutil;

  m_ldelta_util = val;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setHighDeltaUtil

bool ZAIC_HDG::setHighDeltaUtil(double val)
{
  if(m_ivp_domain.size() == 0)
    return(false);

  if(val < m_hminutil)
    val = m_hminutil;
  else if(val > m_maxutil)
    val = m_maxutil;

  m_hdelta_util = val;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setMinMaxUtil

bool ZAIC_HDG::setMinMaxUtil(double lminval, double hminval, double maxval)
{
  // Part 1: Sanity checks resulting in return of false
  if(m_ivp_domain.size() == 0)
    return(false);
  if((lminval > maxval) || (hminval > maxval))
    return(false);
  if((lminval == maxval) && (hminval == maxval))
    return(false);


  // Part 2: Sanity checks we can deal with and fix
  if(lminval < 0)
    lminval = 0;
  if(hminval < 0)
    hminval = 0;

  
  // Part 3: Ok now make the assignments
  m_lminutil = lminval;
  m_hminutil = hminval;
  m_maxutil  = maxval;

  // Part 4: Enforce that the delta utils are (a) less than the
  // maxutil, and each greater than the minutils on either side.
  if(m_ldelta_util < m_lminutil)
    m_ldelta_util = m_lminutil;
  if(m_ldelta_util > m_maxutil)
    m_ldelta_util = m_maxutil;
  
  if(m_hdelta_util < m_hminutil)
    m_hdelta_util = m_hminutil;
  if(m_hdelta_util > m_maxutil)
    m_hdelta_util = m_maxutil;
  
  return(true);
}



//-------------------------------------------------------------
// Procedure: getParam

double ZAIC_HDG::getParam(string param)
{
  if(param == "summit")
    return(m_summit);
  else if(param == "lowdelta")
    return(m_ldelta);
  else if(param == "highdelta")
    return(m_hdelta);
  else if(param == "lowdeltautil")
    return(m_ldelta_util);
  else if(param == "highdeltautil")
    return(m_hdelta_util);

  else if(param == "minutil")
    return(m_lminutil);
  else if(param == "lminutil")
    return(m_lminutil);
  else if(param == "hminutil")
    return(m_hminutil);

  else if(param == "maxutil")
    return(m_maxutil);
  else
    return(0);
}

//-------------------------------------------------------------
// Procedure: extractOF
//   Purpose: Build and return for the caller an IvP objective
//            function built from the pdmap. Once this is done
//            the caller "owns" the PDMap. The reason for this is
//            that the pdmap is assumed to be too expensive to 
//            replicate for certain applications.

IvPFunction *ZAIC_HDG::extractOF()
{
  if(m_ivp_domain.size() == 0)
    return(0);

  for(unsigned int i=0; i<m_domain_pts; i++)
    m_ptvals[i] = evalPoint(i);

  //cout << "summit: " << m_summit << endl;
  //cout << "ldelta: " << m_ldelta << endl;
  //cout << "hdelta: " << m_hdelta << endl;
  //cout << "ldutil: " << m_ldelta_util << endl;
  //cout << "hdutil: " << m_hdelta_util << endl;
  
  PDMap *pdmap = setPDMap();
  if(!pdmap)
    return(0);

  pdmap->updateGrid();
  IvPFunction *ipf = new IvPFunction(pdmap);

  return(ipf);
}



//-------------------------------------------------------------
// Procedure: evalPoint(int)

double ZAIC_HDG::evalPoint(unsigned int ix)
{
  if(ix >= m_ptvals.size())
    return(0);

  double ixval = (((double)(ix)) * m_domain_delta) + m_domain_low;

  // Sanity check
  if((ixval < 0) || (ixval >= 360))
    return(0);
  
  double return_val = 0;
  //==================================================
  // Part 1: Determine if eval from left or right
  //==================================================
  bool eval_to_right = true;
  if(m_summit < 180) {
    if(ixval < m_summit)
      eval_to_right = false;
    else if((m_summit + (360 - ixval)) < 180)
      eval_to_right = false;
  }
  else {
    if((m_summit > ixval) && ((m_summit - ixval) < 180))
      eval_to_right = false;
  }

  //==================================================
  // Part 2: Evaluate given the point is on the right
  //         (essentially a starboard turn)
  //==================================================
  if(eval_to_right) {
    double dist_from_med = (ixval - m_summit);
    if(dist_from_med < 0)
      dist_from_med += 360;

    if(dist_from_med == m_hdelta)
      return_val = m_hdelta_util;
    else if(dist_from_med < m_hdelta) {
      // Div by zero really should never happen but just to be
      // safe, handle it here.
      if(m_hdelta == 0)
	return(0);
      double slope = (m_maxutil - m_hdelta_util) / m_hdelta;
      double rise  = slope * dist_from_med;
      return_val = m_maxutil - rise;
    }
    else {
      double brk_pt = m_summit + m_hdelta;
      if(brk_pt >= 360)
	brk_pt -= 360;
      double dist_from_brk = (ixval - brk_pt);
      if(dist_from_brk < 0)
	dist_from_brk += 360;
      double slope = (m_hdelta_util - m_hminutil) / (180 - m_hdelta);
      double rise  = slope * dist_from_brk;
      return_val = m_hdelta_util - rise;
    }
  }
  
  //==================================================
  // Part 3: Evaluate given the point is on the left
  //         (essentially a port turn)
  //==================================================
  if(!eval_to_right) {
    double dist_from_med = (m_summit - ixval);
    if(dist_from_med < 0)
      dist_from_med += 360;

    if(dist_from_med == m_ldelta)
      return_val = m_ldelta_util;
    else if(dist_from_med < m_ldelta) {
      // Div by zero really should never happen but just to be
      // safe, handle it here.
      if(m_ldelta == 0)
	return(0);
      double slope = (m_maxutil - m_ldelta_util) / m_ldelta;
      double rise  = slope * dist_from_med;
      return_val = m_maxutil - rise;
    }
    else {
      double brk_pt = m_summit - m_ldelta;
      if(brk_pt < 0)
	brk_pt += 360;
      double dist_from_brk = (brk_pt - ixval);
      if(dist_from_brk < 0)
	dist_from_brk += 360;
      if(dist_from_brk >= 360)
	dist_from_brk -= 360;
      double run = (180 - m_ldelta);
      // Div by zero really should never happen but just to be
      // safe, handle it here.
      if(run == 0)
	return(0);
      double slope = (m_ldelta_util - m_lminutil) / run;
      double rise  = slope * dist_from_brk;
      return_val = m_ldelta_util - rise;
    }
  }

  //cout << "ixval:"   << ixval;
  //cout << "  right:" << eval_to_right;
  //cout << "  util:"  << return_val << endl;

  return(return_val);				       
				       
}

//-------------------------------------------------------------
// Procedure: setPDMap()

PDMap *ZAIC_HDG::setPDMap(double tolerance)
{
  int    first_pt  = 0;
  double first_val = m_ptvals[0];

  bool   trend = false; // No trend to start
  double s_m = 0;
  double s_b = 0;

  vector<IvPBox*> pieces;
  for(unsigned int i=1; i<m_domain_pts; i++) {

    if(!trend) {
      trend  = true;
      s_m = (m_ptvals[i] - first_val) / (i - first_pt);
      s_b = (m_ptvals[i] - (s_m * i));
    }

    // Project from the running linear line what the value 
    // should be at this point [i]. Then compare to the actual
    // point and see if it lies within the tolerance.
    double ext_val = (s_m * i) + s_b;
    bool lbreak = false;
    if((fabs(ext_val - m_ptvals[i])) > tolerance)
      lbreak = true;

    // In addition to applying the tolerance criteria, also
    // declare a break if there is a change in sign of slope
    // to nail exactly a peak in the function.
    double loc_m = (m_ptvals[i] - m_ptvals[i-1]) / (i - (i-1));
    if((loc_m < 0) && (s_m > 0))
      lbreak = true;
    if((loc_m > 0) && (s_m < 0))
      lbreak = true;
    
    bool last_point = (i == m_domain_pts-1);
    
    if(last_point) {
      IvPBox *piece = new IvPBox(1,1);
      if(lbreak) {
	piece->setPTS(0, i, i);
	piece->wt(0) = 0;
	piece->wt(1) = m_ptvals[i];
	pieces.push_back(piece);
      }
      else {
	piece->setPTS(0, first_pt, i);
	double rise   = m_ptvals[i] - first_val;
	double run    = i - first_pt;
	double slope  = rise / run;
	double intcpt = first_val - (slope * first_pt);
	piece->wt(0) = slope;
	piece->wt(1) = intcpt;
	pieces.push_back(piece);
      }
    }
	
    if(lbreak) {
      IvPBox *piece = new IvPBox(1,1);
      piece->setPTS(0, first_pt, i-1);
      double rise   = m_ptvals[i-1] - first_val;
      double run    = (i-1) - first_pt;
      double slope  = rise / run;
      double intcpt = first_val - (slope * first_pt);
      piece->wt(0) = slope;
      piece->wt(1) = intcpt;
      pieces.push_back(piece);
      trend = false;
      first_pt  = i;
      first_val = m_ptvals[i]; 
    }
    
  }
  
  unsigned int piece_count = pieces.size();

  PDMap *pdmap = new PDMap(piece_count, m_ivp_domain, 1);

  for(unsigned int i=0; i<piece_count; i++)
    pdmap->bx(i) = pieces[i];

  return(pdmap);
}




