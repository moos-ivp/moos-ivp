/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_SPD_Model.cpp                                   */
/*    DATE: May 9th, 2016                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

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
// Procedure: setDomain

void ZAIC_SPD_Model::setDomain(unsigned int domain_pts)
{
  if(domain_pts < 101)
    domain_pts = 100;
  if(domain_pts > 1001)
    domain_pts = 1001;

  // Initialize the IvP Domain with the new number of points
  IvPDomain ivp_domain;
  ivp_domain.addDomain("x", 0, domain_pts-1, domain_pts);
  
  // Rebuild the zaic with default initial values
  double medspd = 0.50 * domain_pts;
  double lowspd = 0.15 * domain_pts;
  double hghspd = 0.75 * domain_pts;
  double lowspd_util = 80; 
  double hghspd_util = 10; 
  
  ZAIC_SPD *new_zaic = new ZAIC_SPD(ivp_domain, "x");
  bool ok = new_zaic->setParams(medspd, lowspd, hghspd, 
				lowspd_util, hghspd_util);

  m_zaic_spd = new_zaic;
}

//-------------------------------------------------------------
// Procedure: currMode

void ZAIC_SPD_Model::currMode(int new_mode)
{
  if((new_mode >= 0) && (new_mode <= 4))
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
