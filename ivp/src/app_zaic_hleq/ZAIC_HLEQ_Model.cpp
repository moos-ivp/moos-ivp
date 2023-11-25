/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ZAIC_HLEQ_Model.cpp                                  */
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

#include <iostream>
#include "ZAIC_HLEQ_Model.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

ZAIC_HLEQ_Model::ZAIC_HLEQ_Model() : ZAIC_Model()
{
  m_curr_mode = 0;  // 0:medval 1:lowval 2:hghval 3:lowval_util 4:hghval_util
  m_zaic_leq  = 0;
  m_zaic_heq  = 0;
  
  setDomain(100);
}

//-------------------------------------------------------------
// Procedure: getIvPFunction()
//      Note: Virtual function overloaded

IvPFunction *ZAIC_HLEQ_Model::getIvPFunction()
{
  if(m_leq_mode)
    return(m_zaic_leq->extractOF());
  else
    return(m_zaic_heq->extractOF());
}

//-------------------------------------------------------------
// Procedure: setDomain

void ZAIC_HLEQ_Model::setDomain(unsigned int domain_pts)
{
  if(domain_pts < 101)
    domain_pts = 100;
  if(domain_pts > 1001)
    domain_pts = 1001;

  // Initialize the IvP Domain with the new number of points
  IvPDomain ivp_domain;
  ivp_domain.addDomain("x", 0, domain_pts-1, domain_pts);
  
  // Rebuild the zaic with default initial values
  double summit = (domain_pts / 4.0);
  double base   = (domain_pts / 10.0);
  double minutil = 0;
  double maxutil = 100;

  m_zaic_leq = new ZAIC_LEQ(ivp_domain, "x");  
  m_zaic_leq->setSummit(summit);
  m_zaic_leq->setBaseWidth(base);
  m_zaic_leq->setMinMaxUtil(minutil, maxutil);

  m_zaic_heq = new ZAIC_HEQ(ivp_domain, "x");  
  m_zaic_heq->setSummit(summit);
  m_zaic_heq->setBaseWidth(base);
  m_zaic_heq->setMinMaxUtil(minutil, maxutil);
}

//-------------------------------------------------------------
// Procedure: currMode

void ZAIC_HLEQ_Model::currMode(int new_mode)
{
  if((new_mode >= 0) && (new_mode <= 4))
    m_curr_mode = new_mode; 
}

//-------------------------------------------------------------
// Procedure: moveX

void ZAIC_HLEQ_Model::moveX(double delta)
{
  if(!m_zaic_leq || !m_zaic_heq)
    return;

  IvPDomain domain = m_zaic_leq->getIvPDomain();

  //double dom_low  = domain.getVarLow(0);
  //double dom_high = domain.getVarHigh(0);
  
  if(m_curr_mode==0) {   // Altering Summit
    double summit = m_zaic_leq->getParam("summit");
    summit += delta;
    m_zaic_leq->setSummit(summit);
    m_zaic_heq->setSummit(summit);
  }
  else if(m_curr_mode == 1) { // Altering Base Width
    double base_width = m_zaic_leq->getParam("basewidth");
    base_width += delta;
    if(base_width < 0)
      base_width = 0;
    m_zaic_leq->setBaseWidth(base_width);
    m_zaic_heq->setBaseWidth(base_width);
  }
  else if(m_curr_mode == 2) { // Altering Min Utility
    double minutil = m_zaic_leq->getParam("minutil");
    double maxutil = m_zaic_leq->getParam("maxutil");
    double new_min = minutil + delta;
    if(new_min < 0)
      new_min = 0;
    if(new_min > maxutil)
      new_min = maxutil;
    if(new_min < maxutil) {
      m_zaic_leq->setMinMaxUtil(new_min, maxutil);
      m_zaic_heq->setMinMaxUtil(new_min, maxutil);
    }
  }
  else if(m_curr_mode == 3) { // Altering Max Utility
    double minutil = m_zaic_leq->getParam("minutil");
    double maxutil = m_zaic_leq->getParam("maxutil");
    double new_max = maxutil + delta;
    if(new_max < 0)
      new_max = 0;
    if(new_max > 200)
      new_max = 200;
    if(new_max > minutil) {
      m_zaic_leq->setMinMaxUtil(minutil, new_max);
      m_zaic_heq->setMinMaxUtil(minutil, new_max);
    }
  }
  else if(m_curr_mode==4) {   // Altering Summit
    double summit_delta = m_zaic_leq->getParam("summit_delta");
    summit_delta += delta;
    m_zaic_leq->setSummitDelta(summit_delta);
    m_zaic_heq->setSummitDelta(summit_delta);
  }
  else
    cout << "Uh-Oh!  Mode problem" << endl;
}

//----------------------------------------------------------------
// Procedure: getSummit

double ZAIC_HLEQ_Model::getSummit()
{
  if(!m_zaic_leq || !m_zaic_heq)
    return(0);
  return(m_zaic_leq->getParam("summit"));
}

//----------------------------------------------------------------
// Procedure: getSummitDelta()

double ZAIC_HLEQ_Model::getSummitDelta()
{
  if(!m_zaic_leq || !m_zaic_heq)
    return(0);
  return(m_zaic_leq->getParam("summit_delta"));
}

//----------------------------------------------------------------
// Procedure: getBaseWidth()

double ZAIC_HLEQ_Model::getBaseWidth()
{
  if(!m_zaic_leq || !m_zaic_heq)
    return(0);
  return(m_zaic_leq->getParam("basewidth"));
}

//----------------------------------------------------------------
// Procedure: getMinUtil

double ZAIC_HLEQ_Model::getMinUtil()
{
  if(!m_zaic_leq || !m_zaic_heq)
    return(0);
  return(m_zaic_leq->getParam("minutil"));
}

//----------------------------------------------------------------
// Procedure: getMaxUtil

double ZAIC_HLEQ_Model::getMaxUtil()
{
  if(!m_zaic_leq || !m_zaic_heq)
    return(0);
  return(m_zaic_leq->getParam("maxutil"));
}





