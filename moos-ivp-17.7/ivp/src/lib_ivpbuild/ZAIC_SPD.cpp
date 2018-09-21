/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_SPD.cpp                                         */
/*    DATE: June 1st 2015                                        */
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
#include "ZAIC_SPD.h"
#include "BuildUtils.h"
#include "PDMapBuilder.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

ZAIC_SPD::ZAIC_SPD(IvPDomain g_domain, const string& g_varname) 
{
  m_medspd       = 0;
  m_lowspd       = 0;
  m_hghspd       = 0;
  m_lowspd_util  = 0;
  m_hghspd_util  = 0;

  m_lminutil     = 0;
  m_hminutil     = 0;
  m_maxutil      = 100;
  
  m_ivp_domain = subDomain(g_domain, g_varname);

  m_domain_high = 0;
  m_domain_low  = 0;

  if(m_ivp_domain.size() == 1) {
    m_domain_high  = m_ivp_domain.getVarHigh(0);
    m_domain_low   = m_ivp_domain.getVarLow(0);
  }

}

//-------------------------------------------------------------
// Procedure: setParams
//    Return: true if all ok

bool ZAIC_SPD::setParams(double medspd, double lowspd, double hghspd, 
			 double lowspd_util, double hghspd_util,
			 double lminutil, double hminutil, double maxutil)
{
  bool ok = true;
  ok = ok && setMedSpeed(medspd);
  ok = ok && setLowSpeed(lowspd);
  ok = ok && setHghSpeed(hghspd);
  ok = ok && setLowSpeedUtil(lowspd_util);
  ok = ok && setHghSpeedUtil(hghspd_util);
  ok = ok && setMinMaxUtil(lminutil, hminutil, maxutil);

  return(ok);
}


//-------------------------------------------------------------
// Procedure: setMedSpeed

bool ZAIC_SPD::setMedSpeed(double val)
{
  // Part 1: Sanity check resulting in return of false
  if(m_ivp_domain.size() == 0)
    return(false);

  // Part 2: Sanity check we can deal with and fix
  if(val > m_domain_high)
    val = m_domain_high;
  if(val < m_domain_low)
    val = m_domain_low;

  // Part 3: Ok now make the assignment
  m_medspd = val;

  // Part 4: Enforce that lowspd <= medspd <= hghspd
  if(m_lowspd > m_medspd)
    m_lowspd = m_medspd;
  if(m_hghspd < m_medspd)
    m_hghspd = m_medspd;
  
  return(true);
}


//------------------------------------------------
// Procedure: setLowSped

bool ZAIC_SPD::setLowSpeed(double val)
{
  // Part 1: Sanity check resulting in return of false
  if(m_ivp_domain.size() == 0)
    return(false);

  // Part 2: Sanity check we can deal with and fix
  if(val > m_domain_high)
    val = m_domain_high;
  if(val < m_domain_low)
    val = m_domain_low;

  // Part 3: Ok now make the assignment
  m_lowspd = val;

  // Part 4: Enforce that lowspd <= medspd <= hghspd
  if(m_medspd < m_lowspd)
    m_medspd = m_lowspd;
  if(m_hghspd < m_lowspd)
    m_hghspd = m_lowspd;
  
  return(true);
}

//------------------------------------------------
// Procedure: setHghSpeed

bool ZAIC_SPD::setHghSpeed(double val)
{
  // Part 1: Sanity check resulting in return of false
  if(m_ivp_domain.size() == 0)
    return(false);
  
  // Part 2: Sanity check we can deal with and fix
  if(val > m_domain_high)
    val = m_domain_high;
  if(val < m_domain_low)
    val = m_domain_low;

  // Part 3: Ok now make the assignment
  m_hghspd = val;

  // Part 4: Enforce that lowspd <= medspd <= hghspd
  if(m_medspd > m_hghspd)
    m_medspd = m_hghspd;
  if(m_lowspd > m_hghspd)
    m_lowspd = m_hghspd;
  
  return(true);
}

//------------------------------------------------
// Procedure: setLowSpeedUtil

bool ZAIC_SPD::setLowSpeedUtil(double val)
{
  // Part 1: Sanity check resulting in return of false
  if(m_ivp_domain.size() == 0)
    return(false);

  // Part 2: Sanity check we can deal with and fix
  if(val < 0)
    val = 0;
  if(val > m_maxutil)
    val = m_maxutil;
  
  // Part 3: Ok now make the assignment
  m_lowspd_util = val;
  
  // Part 4: Enforce that lminutil <= lowspd_util
  if(m_lminutil > m_lowspd_util)
    m_lminutil = m_lowspd_util;

  return(true);
}

//------------------------------------------------
// Procedure: setHghSpeedUtil

bool ZAIC_SPD::setHghSpeedUtil(double val)
{
  // Part 1: Sanity check resulting in return of false
  if(m_ivp_domain.size() == 0)
    return(false);

  // Part 2: Sanity check we can deal with and fix
  if(val < 0)
    val = 0;
  if(val > m_maxutil)
    val = m_maxutil;

  // Part 3: Ok now make the assignment
  m_hghspd_util = val;

  // Part 4: Enforce that hminutil <= hghspd_util
  if(m_hminutil > m_hghspd_util)
    m_hminutil = m_hghspd_util;

  return(true);
}

//-------------------------------------------------------------
// Procedure: setMinMaxUtil

bool ZAIC_SPD::setMinMaxUtil(double lminval, double hminval, double maxval)
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

  // Part 4: Enforce that low/hghspd utils are (a) less than the
  // maxutil, and each greater than the minutils on either side.
  if(m_lowspd_util < m_lminutil)
    m_lowspd_util = m_lminutil;
  if(m_lowspd_util > m_maxutil)
    m_lowspd_util = m_maxutil;
  
  if(m_hghspd_util < m_hminutil)
    m_hghspd_util = m_hminutil;
  if(m_hghspd_util > m_maxutil)
    m_hghspd_util = m_maxutil;
  
  return(true);
}

//-------------------------------------------------------------
// Procedure: getParam

double ZAIC_SPD::getParam(string param)
{
  if(param == "medspd")
    return(m_medspd);
  else if(param == "lowspd")
    return(m_lowspd);
  else if(param == "hghspd")
    return(m_hghspd);
  else if(param == "lowspd_util")
    return(m_lowspd_util);
  else if(param == "hghspd_util")
    return(m_hghspd_util);
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

IvPFunction *ZAIC_SPD::extractOF()
{
  if(m_ivp_domain.size() == 0)
    return(0);
  
  setPointLocations();

  PDMap *pdmap = setPDMap();
  if(!pdmap)
    return(0);

  pdmap->updateGrid();
  IvPFunction *ipf = new IvPFunction(pdmap);

  return(ipf);
}


//-------------------------------------------------------------
// Procedure: setPointLocations() 
//                                                             
//     |         -------------o                                 |
//     |   o----/               \                               |
//     |  /                       \                             |
//     | /                          \                           |
//     | /                           o                          |
//     | /                                  \                   | 
//     |/                                                \      |
//     o-----------------------------o--------------------------o
//     0   1                   2     3                           
//                                                             

void ZAIC_SPD::setPointLocations()
{
  m_dom.clear();
  m_rng.clear();

  if(m_ivp_domain.size() != 1)
    return;
  
  cout << "========================================*" << endl;
  cout << "medspd: " << m_medspd << endl;
  cout << "lowspd: " << m_lowspd << endl;
  cout << "hghspd: " << m_hghspd << endl;
  cout << "lowspd_util: " << m_lowspd_util << endl;
  cout << "hghspd_util: " << m_hghspd_util << endl;

  int    domain_pts  = m_ivp_domain.getVarPoints(0);
  double delta       = m_ivp_domain.getVarDelta(0);

  // Most of the time the domain_low value for speed is zero. In case
  // it is not, we handle it here:
  double lowspd_diff = m_lowspd - m_domain_low;
  double medspd_diff = m_medspd - m_domain_low;
  double hghspd_diff = m_hghspd - m_domain_low;

  int ipt_low   = 0;
  int ipt_one   = (int)((lowspd_diff/delta)+0.5);
  int ipt_two   = (int)((medspd_diff/delta)+0.5);
  int ipt_three = (int)((hghspd_diff/delta)+0.5);
  int ipt_high  = domain_pts - 1;

  // Ensure that both middle points are within bounds and that
  // point one is never greater than point two
  if(ipt_one < 0)    ipt_one   = 0;
  if(ipt_two < 0)    ipt_two   = 0;
  if(ipt_three < 0)  ipt_three = 0;

  if(ipt_one > ipt_high)    ipt_one   = ipt_high;
  if(ipt_two > ipt_high)    ipt_two   = ipt_high;
  if(ipt_three > ipt_high)  ipt_three = ipt_high;

  if(ipt_one   > ipt_two)   ipt_one   = ipt_two;
  if(ipt_three < ipt_two)   ipt_three = ipt_two;

  if(ipt_low != ipt_two) {
    m_dom.push_back(ipt_low);
    m_rng.push_back(m_lminutil);
  }
  
  if((ipt_one != ipt_low) && (ipt_one != ipt_two)){
    m_dom.push_back(ipt_one);
    m_rng.push_back(m_lowspd_util);
  }

  m_dom.push_back(ipt_two);
  m_rng.push_back(m_maxutil);

  if(ipt_three != ipt_two) {
    m_dom.push_back(ipt_three);
    m_rng.push_back(m_hghspd_util);
  }

  if(ipt_high != ipt_three) {
    m_dom.push_back(ipt_high);
    m_rng.push_back(m_hminutil);
  }

  for(unsigned int i=0; i<m_dom.size(); i++) {
    cout << "[" << i << "]: dom:" << m_dom[i] << ", val:" << m_rng[i] << endl;
  }
}
     

//-------------------------------------------------------------
// Procedure: setPDMap()

PDMap *ZAIC_SPD::setPDMap()
{
  PDMapBuilder builder;
  builder.setIvPDomain(m_ivp_domain);
  builder.setDomainVals(m_dom);
  builder.setRangeVals(m_rng);
  
  PDMap *pdmap = builder.getPDMap();
  return(pdmap);
}





