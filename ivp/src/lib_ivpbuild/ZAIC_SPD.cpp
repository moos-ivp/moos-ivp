/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include "ZAIC_SPD.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "PDMapBuilder.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

ZAIC_SPD::ZAIC_SPD(IvPDomain domain, string varname) 
{
  if((domain.size() == 1) && (varname == ""))
    m_ivp_domain = domain;
  else if(domain.hasDomain(varname))
    m_ivp_domain = subDomain(domain, varname);

  m_domain_high = 0;
  m_domain_low  = 0;

  if(m_ivp_domain.size() == 1) {
    m_domain_high  = m_ivp_domain.getVarHigh(0);
    m_domain_low   = m_ivp_domain.getVarLow(0);
  }

  m_med_spd      = 0;
  m_min_spd_util = 0;
  m_max_spd_util = 0;
  m_max_util     = 100;

  // By default the low/hgh spd components are not used. They are
  // snapped to the min/max spd components/
  m_low_spd      = m_domain_low;
  m_hgh_spd      = m_domain_high;
  m_low_spd_util = 0;
  m_hgh_spd_util = 0;
}

//-------------------------------------------------------------
// Procedure: setParams()
//    Return: true if all ok

bool ZAIC_SPD::setParams(double med_spd, double low_spd, double hgh_spd, 
			 double low_spd_util, double hgh_spd_util,
			 double min_spd_util, double max_spd_util, double max_util)
{
  bool ok = true;
  ok = ok && setMedSpeed(med_spd);
  ok = ok && setLowSpeed(low_spd);
  ok = ok && setHghSpeed(hgh_spd);
  ok = ok && setLowSpeedUtil(low_spd_util);
  ok = ok && setHghSpeedUtil(hgh_spd_util);
  ok = ok && setMaxUtil(max_util);
  ok = ok && setMinSpdUtil(min_spd_util);
  ok = ok && setMaxSpdUtil(max_spd_util);

  ok = ok && adjustParams();
  
  return(ok);
}


//-------------------------------------------------------------
// Procedure: setMedSpeed()

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
  m_med_spd = val;

  return(true);
}


//------------------------------------------------
// Procedure: setLowSpeed()

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
  m_low_spd = val;

  return(true);
}


//------------------------------------------------
// Procedure: setHghSpeed()

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
  m_hgh_spd = val;
  
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
  if(val > m_max_util)
    val = m_max_util;
  
  // Part 3: Ok now make the assignment
  m_low_spd_util = val;
  
  return(true);
}

//------------------------------------------------
// Procedure: setHghSpeedUtil()

bool ZAIC_SPD::setHghSpeedUtil(double val)
{
  // Part 1: Sanity check resulting in return of false
  if(m_ivp_domain.size() == 0)
    return(false);

  // Part 2: Sanity check we can deal with and fix
  if(val < 0)
    val = 0;
  if(val > m_max_util)
    val = m_max_util;

  // Part 3: Ok now make the assignment
  m_hgh_spd_util = val;

  return(true);
}

//-------------------------------------------------------------
// Procedure: setMinSpdUtil()

bool ZAIC_SPD::setMinSpdUtil(double min_spd_util)
{
  if(m_min_spd_util < 0)
    m_min_spd_util = 0;
  m_min_spd_util = min_spd_util;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setMaxSpdUtil()

bool ZAIC_SPD::setMaxSpdUtil(double max_spd_util)
{
  if(m_max_spd_util < 0)
    m_max_spd_util = 0;
  
  m_max_spd_util = max_spd_util;

  return(true);
}

//-------------------------------------------------------------
// Procedure: setMaxUtil()

bool ZAIC_SPD::setMaxUtil(double max_util)
{
  if(m_max_util < 0)
    m_max_util = 0;
  m_max_util = max_util;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setMinMaxUtil()

bool ZAIC_SPD::setMinMaxUtil(double min_spd_util, double max_spd_util, double max_util)
{
  bool ok = true;

  ok = ok && setMaxUtil(max_util);
  ok = ok && setMinSpdUtil(min_spd_util);
  ok = ok && setMaxSpdUtil(max_spd_util);

  return(ok);
}

//-------------------------------------------------------------
// Procedure: disableLowSpeed()

void ZAIC_SPD::disableLowSpeed()
{
  m_low_spd = m_domain_low;
  m_low_spd_util = m_min_spd_util;
}

//-------------------------------------------------------------
// Procedure: disableHighSpeed()

void ZAIC_SPD::disableHighSpeed()
{
  m_hgh_spd = m_domain_high;
  m_hgh_spd_util = m_max_spd_util;
}

//-------------------------------------------------------------
// Procedure: adjustParams()

bool ZAIC_SPD::adjustParams()
{
  bool adjustment_made = false;
  // Ensure med_spd is somewhere in the domain range
  if(m_med_spd < m_domain_low) {
    m_med_spd = m_domain_low;
    adjustment_made = true;
  }
  if(m_med_spd > m_domain_high) {
    m_med_spd = m_domain_high;
    adjustment_made = true;
  }
  // Ensure low_spd is between domain_low and med_spd
  if(m_low_spd < m_domain_low) {
    m_low_spd = m_domain_low;
    adjustment_made = true;
  }
  if(m_low_spd > m_med_spd) {
    m_low_spd = m_med_spd;
    adjustment_made = true;
  }
  // Ensure high_spd is between med_spd and domain_high
  if(m_hgh_spd > m_domain_high) {
    m_hgh_spd = m_domain_high;
    adjustment_made = true;
  }
  if(m_hgh_spd < m_med_spd) {
    m_hgh_spd = m_med_spd;
    adjustment_made = true;
  }

  // Ensure utils are sane
  if(m_min_spd_util > m_max_util) {
    m_min_spd_util = m_max_util;
    adjustment_made = true;
  }
  if(m_low_spd_util > m_max_util) {
    m_low_spd_util = m_max_util;
    adjustment_made = true;
  }
  if(m_hgh_spd_util > m_max_util) {
    m_hgh_spd_util = m_max_util;
    adjustment_made = true;
  }
  if(m_max_spd_util > m_max_util) {
    m_max_spd_util = m_max_util;
    adjustment_made = true;
  }
  
  // If low_spd is same as domain_low, low_spd_util set to min_spd_util
  if(m_low_spd == m_domain_low) {
    m_low_spd_util = m_min_spd_util;
    adjustment_made = true;
  }
  // If hgh_spd is same as domain_high, hgh_spd_util set to max_spd_util
  if(m_hgh_spd == m_domain_high) {
    m_hgh_spd_util = m_max_spd_util;
    adjustment_made = true;
  }

  return(adjustment_made);
}


//-------------------------------------------------------------
// Procedure: getParam()

double ZAIC_SPD::getParam(string param)
{
  if((param == "medspd") || (param == "med_spd"))
    return(m_med_spd);
  else if((param == "lowspd") || (param == "low_spd"))
    return(m_low_spd);
  else if((param == "hghspd") || (param == "hgh_spd"))
    return(m_hgh_spd);
  else if((param == "lowspd_util") || (param == "low_spd_util"))
    return(m_low_spd_util);
  else if((param == "hghspd_util") || (param == "hgh_spd_util"))
    return(m_hgh_spd_util);
  else if((param == "lminutil") || (param == "min_spd_util"))
    return(m_min_spd_util);
  else if((param == "hminutil") || (param == "max_spd_util"))
    return(m_max_spd_util);
  else if((param == "maxutil") || (param == "max_util"))
    return(m_max_util);
  else
    return(0);
}


//-------------------------------------------------------------
// Procedure: extractOF()
//   Purpose: Build and return for the caller an IvP objective
//            function built from the pdmap. Once this is done
//            the caller "owns" the PDMap. The reason for this is
//            that the pdmap is assumed to be too expensive to 
//            replicate for certain applications.

IvPFunction *ZAIC_SPD::extractOF()
{
  if(m_ivp_domain.size() == 0)
    return(0);

  adjustParams();
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

  int    domain_pts  = m_ivp_domain.getVarPoints(0);
  double delta       = m_ivp_domain.getVarDelta(0);

  // Most of the time the domain_low value for speed is zero. In case
  // it is not, we handle it here:
  double lowspd_diff = m_low_spd - m_domain_low;
  double medspd_diff = m_med_spd - m_domain_low;
  double hghspd_diff = m_hgh_spd - m_domain_low;

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
    m_rng.push_back(m_min_spd_util);
  }
  
  if((ipt_one != ipt_low) && (ipt_one != ipt_two)){
    m_dom.push_back(ipt_one);
    m_rng.push_back(m_low_spd_util);
  }

  m_dom.push_back(ipt_two);
  m_rng.push_back(m_max_util);

  if(ipt_three != ipt_two) {
    m_dom.push_back(ipt_three);
    m_rng.push_back(m_hgh_spd_util);
  }

  if(ipt_high != ipt_three) {
    m_dom.push_back(ipt_high);
    m_rng.push_back(m_max_spd_util);
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


//-------------------------------------------------------------
// Procedure: getSummary()

vector<string> ZAIC_SPD::getSummary() const
{
  vector<string> svector;
  svector.push_back(domainToString(m_ivp_domain));

  svector.push_back("min_spd: " + doubleToStringX(m_domain_low));
  svector.push_back("low_spd: " + doubleToStringX(m_low_spd));
  svector.push_back("med_spd: " + doubleToStringX(m_med_spd));
  svector.push_back("hgh_spd: " + doubleToStringX(m_hgh_spd));
  svector.push_back("max_spd: " + doubleToStringX(m_domain_high));
  svector.push_back("min_spd_util: " + doubleToStringX(m_min_spd_util));
  svector.push_back("low_spd_util: " + doubleToStringX(m_low_spd_util));
  svector.push_back("med_spd_util: " + doubleToStringX(m_max_util));
  svector.push_back("hgh_spd_util: " + doubleToStringX(m_hgh_spd_util));
  svector.push_back("max_spd_util: " + doubleToStringX(m_max_spd_util));

  return(svector);
}

