/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_SPD_Model.cpp                                   */
/*    DATE: May 9th, 2016                                        */
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

#include <cstdlib>
#include <iostream>
#include "ZAIC_SPD_Model.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

ZAIC_SPD_Model::ZAIC_SPD_Model() : ZAIC_Model()
{
  m_curr_mode = 0;  // 0:medval 1:lowval 2:hghval 3:lowval_utile 4:hghval_util
  m_zaic_spd  = 0;
  
  setDomain(100);
}

//-------------------------------------------------------------
// Procedure: getIvPFunction()
//      Note: Virtual function overloaded

IvPFunction *ZAIC_SPD_Model::getIvPFunction()
{
  return(m_zaic_spd->extractOF());
}

//-------------------------------------------------------------
// Procedure: setParam()
//      Note: Virtual function overloaded

bool ZAIC_SPD_Model::setParam(string param, string value)
{
  cout << "KK setParam() param: " << param << ", value: " << value << endl;
  
  if(!m_zaic_spd)
    return(false);

  if(!isNumber(value))
    return(false);

  if(param == "medspd")
    m_zaic_spd->setMedSpeed(atof(value.c_str()));
  else if(param == "lowspd")
    m_zaic_spd->setLowSpeed(atof(value.c_str()));
  else if(param == "hghspd")
    m_zaic_spd->setHghSpeed(atof(value.c_str()));
  else if(param == "lowspd_util")
    m_zaic_spd->setLowSpeedUtil(atof(value.c_str()));
  else if(param == "hghspd_util")
    m_zaic_spd->setHghSpeedUtil(atof(value.c_str()));
  else if(param == "domain")
    setDomain(atoi(value.c_str()));
  else
    return(false);
  
  return(true);
}

//-------------------------------------------------------------
// Procedure: setDomain

void ZAIC_SPD_Model::setDomain(unsigned int domain_pts)
{
  if(domain_pts < 101)
    domain_pts = 100;
  if(domain_pts > 1001)
    domain_pts = 1001;

  cout << "setDomain: " << domain_pts << endl;
  
  // Initialize the IvP Domain with the new number of points
  IvPDomain ivp_domain;
  ivp_domain.addDomain("x", 0, domain_pts-1, domain_pts);
  
  // Rebuild the zaic with default initial values
  double medspd = 0.50 * domain_pts;
  double lowspd = 0.15 * domain_pts;
  double hghspd = 0.75 * domain_pts;
  double lowspd_util = 80; 
  double hghspd_util = 10; 

  // If a the zaic existed previously, keeping using thoss values
  if(m_zaic_spd) {
    medspd = getMedVal();
    lowspd = getLowVal();
    hghspd = getHghVal();
    lowspd_util = getLowValUtil();
    hghspd_util = getHghValUtil();
  }
  
  ZAIC_SPD *new_zaic = new ZAIC_SPD(ivp_domain, "x");
  bool ok = new_zaic->setParams(medspd, lowspd, hghspd, 
				lowspd_util, hghspd_util);

  if(ok)
    m_zaic_spd = new_zaic;
}

//-------------------------------------------------------------
// Procedure: currMode

void ZAIC_SPD_Model::currMode(int new_mode)
{
  if((new_mode >= 0) && (new_mode <= 6))
    m_curr_mode = new_mode; 
}

//-------------------------------------------------------------
// Procedure: moveX

void ZAIC_SPD_Model::moveX(double delta)
{
  if(!m_zaic_spd)
    return;
  IvPDomain domain = m_zaic_spd->getIvPDomain();

  double dom_low  = domain.getVarLow(0);
  double dom_high = domain.getVarHigh(0);
  double medval   = m_zaic_spd->getParam("medspd");
  double lowval   = m_zaic_spd->getParam("lowspd");
  double hghval   = m_zaic_spd->getParam("hghspd");
  
  if(m_curr_mode==0) {   // Altering MedVal
    medval += delta;
    medval = vclip(medval, dom_low, dom_high);
    m_zaic_spd->setMedSpeed(medval);
    if(medval < lowval) {
      lowval = medval;
      m_zaic_spd->setLowSpeed(lowval);
    }
    if(medval > hghval) {
      hghval = medval;
      m_zaic_spd->setHghSpeed(hghval);
    }
  }
  else if(m_curr_mode == 1) { // Altering LowVal
    lowval += delta;
    lowval = vclip(lowval, dom_low, medval);
    m_zaic_spd->setLowSpeed(lowval);
  }
  else if(m_curr_mode == 2) { // Altering HghVal
    hghval += delta;
    hghval = vclip(hghval, medval, dom_high);
    m_zaic_spd->setHghSpeed(hghval);
  }
  else if(m_curr_mode == 3) { // Altering LovValUtil
    double lowval_util = m_zaic_spd->getParam("lowspd_util");
    lowval_util += delta;
    lowval_util = vclip(lowval_util, 0, 100);
    m_zaic_spd->setLowSpeedUtil(lowval_util);
  }
  else if(m_curr_mode == 4) { // Altering HghValUtil
    double hghval_util = m_zaic_spd->getParam("hghspd_util");
    hghval_util += delta;
    hghval_util = vclip(hghval_util, 0, 100);
    m_zaic_spd->setHghSpeedUtil(hghval_util);
  }
  else if(m_curr_mode == 5) { // Altering LowMinUtil
    double lminutil = m_zaic_spd->getParam("lminutil");
    double hminutil = m_zaic_spd->getParam("hminutil");
    double maxutil  = m_zaic_spd->getParam("maxutil");
    lminutil += delta;
    lminutil = vclip(lminutil, 0, 100);
    m_zaic_spd->setMinMaxUtil(lminutil, hminutil, maxutil);
  }
  else if(m_curr_mode == 6) { // Altering HighMinUtil
    double lminutil = m_zaic_spd->getParam("lminutil");
    double hminutil = m_zaic_spd->getParam("hminutil");
    double maxutil  = m_zaic_spd->getParam("maxutil");
    hminutil += delta;
    hminutil = vclip(hminutil, 0, 100);
    m_zaic_spd->setMinMaxUtil(lminutil, hminutil, maxutil);
  }
  else
    cout << "Uh-Oh!  Mode problem" << endl;
}

//----------------------------------------------------------------
// Procedure: getMedVal()

double ZAIC_SPD_Model::getMedVal()
{
  if(!m_zaic_spd)
    return(0);   
  return(m_zaic_spd->getParam("medspd"));
}
 
//----------------------------------------------------------------
// Procedure: getLowVal()
 
double ZAIC_SPD_Model::getLowVal()
{
  if(!m_zaic_spd)
    return(0);
  return(m_zaic_spd->getParam("lowspd"));
}

//----------------------------------------------------------------
// Procedure: getHghVal()

double ZAIC_SPD_Model::getHghVal()
{
  if(!m_zaic_spd)
    return(0);
  return(m_zaic_spd->getParam("hghspd"));
}

//----------------------------------------------------------------
// Procedure: getLowValUtil()

double ZAIC_SPD_Model::getLowValUtil()
{
  if(!m_zaic_spd)
    return(0);
  return(m_zaic_spd->getParam("lowspd_util"));
}

//----------------------------------------------------------------
// Procedure: getHghValUtil()

double ZAIC_SPD_Model::getHghValUtil()
{
  if(!m_zaic_spd)
    return(0);
  return(m_zaic_spd->getParam("hghspd_util"));
}

//----------------------------------------------------------------
// Procedure: getLowMinUtil()

double ZAIC_SPD_Model::getLowMinUtil()
{
  if(!m_zaic_spd)
    return(0);
  return(m_zaic_spd->getParam("lminutil"));
}

//----------------------------------------------------------------
// Procedure: getHighMinUtil()

double ZAIC_SPD_Model::getHighMinUtil()
{
  if(!m_zaic_spd)
    return(0);
  return(m_zaic_spd->getParam("hminutil"));
}

//----------------------------------------------------------------
// Procedure: getMaxUtil()

double ZAIC_SPD_Model::getMaxUtil()
{
  if(!m_zaic_spd)
    return(0);
  return(m_zaic_spd->getParam("maxutil"));
}




