/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_HDG_Model.cpp                                   */
/*    DATE: May 10th, 2016                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include "ZAIC_HDG_Model.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

ZAIC_HDG_Model::ZAIC_HDG_Model() : ZAIC_Model()
{
  m_curr_mode = 0;  // 0:medval 1:lowval 2:hghval 3:lowval_utile 4:hghval_util
  m_zaic_hdg  = 0;
  
  setDomain(100);
}

//-------------------------------------------------------------
// Procedure: getIvPFunction()
//      Note: Virtual function overloaded

IvPFunction *ZAIC_HDG_Model::getIvPFunction()
{
  return(m_zaic_hdg->extractOF());
}

//-------------------------------------------------------------
// Procedure: setDomain

void ZAIC_HDG_Model::setDomain(unsigned int domain_pts)
{
  if(domain_pts < 101)
    domain_pts = 100;
  if(domain_pts > 1001)
    domain_pts = 1001;

  // Initialize the IvP Domain with the new number of points
  IvPDomain ivp_domain;
  ivp_domain.addDomain("x", 0, domain_pts-1, domain_pts);
  
  // Rebuild the zaic with default initial values
  double summit = 180;
  double ldelta = 10;
  double hdelta = 40;
  double ldelta_util = 50;
  double hdelta_util = 75;
  double lmin_util = 0; 
  double hmin_util = 35; 
  double max_util = 100; 
  
  ZAIC_HDG *new_zaic = new ZAIC_HDG(ivp_domain, "x");
  bool ok = new_zaic->setParams(summit, ldelta, hdelta, 
				ldelta_util, hdelta_util, 
				lmin_util, hmin_util, max_util);

  if(ok)
    m_zaic_hdg = new_zaic;
}

//-------------------------------------------------------------
// Procedure: currMode

void ZAIC_HDG_Model::currMode(int new_mode)
{
  if((new_mode >= 0) && (new_mode <= 6))
    m_curr_mode = new_mode; 
}

//-------------------------------------------------------------
// Procedure: moveX

void ZAIC_HDG_Model::moveX(double delta)
{
  if(!m_zaic_hdg)
    return;

  //double ldutil   = m_zaic_hdg->getParam("lowdelta");
  //double hdutil   = m_zaic_hdg->getParam("highdelta");
  
  if(m_curr_mode==0) {   // Altering summit
    double summit   = m_zaic_hdg->getParam("summit");
    summit += delta;
    if(summit < 360)
      summit += 360;
    if(summit >= 360)
      summit -= 360;
    m_zaic_hdg->setSummit(summit);
  }
  else if(m_curr_mode == 1) { // Altering LowDelta
    double ldelta   = m_zaic_hdg->getParam("lowdelta");
    ldelta += delta;
    ldelta = vclip(ldelta, 0, 359);
    m_zaic_hdg->setLowDelta(ldelta);
  }
  else if(m_curr_mode == 2) { // Altering HighDelta
  double hdelta   = m_zaic_hdg->getParam("highdelta");
    hdelta += delta;
    hdelta = vclip(hdelta, 0, 359);
    m_zaic_hdg->setHighDelta(hdelta);
  }
  else if(m_curr_mode == 3) { // Altering LowDeltaUtil
    double ldeltautil = m_zaic_hdg->getParam("lowdeltautil");
    ldeltautil += delta;
    ldeltautil = vclip(ldeltautil, 0, 100);
    m_zaic_hdg->setLowDeltaUtil(ldeltautil);
  }
  else if(m_curr_mode == 4) { // Altering HighDeltaUtil
    double hdeltautil = m_zaic_hdg->getParam("highdeltautil");
    hdeltautil += delta;
    hdeltautil = vclip(hdeltautil, 0, 100); 
    m_zaic_hdg->setHighDeltaUtil(hdeltautil);
  }
  else if(m_curr_mode == 5) { // Altering LowMinUtil
    double lminutil = m_zaic_hdg->getParam("lminutil");
    double hminutil = m_zaic_hdg->getParam("hminutil");
    double maxutil  = m_zaic_hdg->getParam("maxutil");
    lminutil += delta;
    lminutil = vclip(lminutil, 0, 100);
    m_zaic_hdg->setMinMaxUtil(lminutil, hminutil, maxutil);
  }
  else if(m_curr_mode == 6) { // Altering HighMinUtil
    double lminutil = m_zaic_hdg->getParam("lminutil");
    double hminutil = m_zaic_hdg->getParam("hminutil");
    double maxutil  = m_zaic_hdg->getParam("maxutil");
    hminutil += delta;
    hminutil = vclip(hminutil, 0, 100);
    m_zaic_hdg->setMinMaxUtil(lminutil, hminutil, maxutil);
  }
  else
    cout << "Uh-Oh!  Mode problem" << endl;
}

//----------------------------------------------------------------
// Procedure: getSummit()

double ZAIC_HDG_Model::getSummit()
{
  if(!m_zaic_hdg)
    return(0);
  return(m_zaic_hdg->getParam("summit"));
}

//----------------------------------------------------------------
// Procedure: getLowDelta()

double ZAIC_HDG_Model::getLowDelta()
{
  if(!m_zaic_hdg)
    return(0);
  return(m_zaic_hdg->getParam("lowdelta"));
}

//----------------------------------------------------------------
// Procedure: getHighDelta()

double ZAIC_HDG_Model::getHighDelta()
{
  if(!m_zaic_hdg)
    return(0);
  return(m_zaic_hdg->getParam("highdelta"));
}

//----------------------------------------------------------------
// Procedure: getLowDeltaUtil()

double ZAIC_HDG_Model::getLowDeltaUtil()
{
  if(!m_zaic_hdg)
    return(0);
  return(m_zaic_hdg->getParam("lowdeltautil"));
}

//----------------------------------------------------------------
// Procedure: getHighDeltaUtil()

double ZAIC_HDG_Model::getHighDeltaUtil()
{
  if(!m_zaic_hdg)
    return(0);
  return(m_zaic_hdg->getParam("highdeltautil"));
}

//----------------------------------------------------------------
// Procedure: getLowMinUtil()

double ZAIC_HDG_Model::getLowMinUtil()
{
  if(!m_zaic_hdg)
    return(0);
  return(m_zaic_hdg->getParam("lminutil"));
}

//----------------------------------------------------------------
// Procedure: getHighMinUtil()

double ZAIC_HDG_Model::getHighMinUtil()
{
  if(!m_zaic_hdg)
    return(0);
  return(m_zaic_hdg->getParam("hminutil"));
}

//----------------------------------------------------------------
// Procedure: getMaxUtil()

double ZAIC_HDG_Model::getMaxUtil()
{
  if(!m_zaic_hdg)
    return(0);
  return(m_zaic_hdg->getParam("maxutil"));
}








