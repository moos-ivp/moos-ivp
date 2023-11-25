/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ZAIC_PEAK_Model.cpp                                  */
/*    DATE: May 16th, 2016                                       */
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
#include "ZAIC_PEAK_Model.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

ZAIC_PEAK_Model::ZAIC_PEAK_Model() : ZAIC_Model()
{
  // Modes: 0:medval 1:lowval 2:hghval 3:lowval_util 4:hghval_util
  m_curr_mode = 0;  
  m_draw_mode = 1;

  m_zaic_peak1 = 0;
  m_zaic_peak2 = 0;

  m_value_wrap = true;

  setDomain(100);
}

//-------------------------------------------------------------
// Procedure: getIvPFunction()
//      Note: Virtual function overloaded

IvPFunction *ZAIC_PEAK_Model::getIvPFunction()
{
  if(m_draw_mode == 1)
    return(m_zaic_peak1->extractOF());
  if(m_draw_mode == 2)
    return(m_zaic_peak2->extractOF());

  IvPDomain domain = m_zaic_peak1->getIvPDomain();
  string varname = domain.getVarName(0);

  ZAIC_PEAK joined_zaic(domain, varname);
  joined_zaic.setSummit(m_zaic_peak1->getParam("summit"));
  joined_zaic.setSummitDelta(m_zaic_peak1->getParam("summitdelta"));
  joined_zaic.setBaseWidth(m_zaic_peak1->getParam("basewidth"));
  joined_zaic.setPeakWidth(m_zaic_peak1->getParam("peakwidth"));
  joined_zaic.addComponent();
  joined_zaic.setSummit(m_zaic_peak2->getParam("summit"), 1);
  joined_zaic.setSummitDelta(m_zaic_peak2->getParam("summitdelta"), 1);
  joined_zaic.setBaseWidth(m_zaic_peak2->getParam("basewidth"), 1);
  joined_zaic.setPeakWidth(m_zaic_peak2->getParam("peakwidth"), 1);
  joined_zaic.setValueWrap(m_value_wrap);
    
  if(m_draw_mode == 3)
    return(joined_zaic.extractOF(false));

  return(joined_zaic.extractOF(true));
}

//-------------------------------------------------------------
// Procedure: setDomain

void ZAIC_PEAK_Model::setDomain(unsigned int domain_pts)
{
  if(domain_pts < 101)
    domain_pts = 100;
  if(domain_pts > 1001)
    domain_pts = 1001;

  // Initialize the IvP Domain with the new number of points
  IvPDomain ivp_domain;
  ivp_domain.addDomain("x", 0, domain_pts-1, domain_pts);
  
  // Rebuild the zaics with default initial values
  double summit1 = (double)(domain_pts) * 0.25;
  double pwidth1 = 10;
  double bwidth1 = 25;
  double sdelta1 = 80;

  double summit2 = (double)(domain_pts) * 0.65;
  double pwidth2 = 5;
  double bwidth2 = 30;
  double sdelta2 = 45;

  double min_util = 0; 
  double max_util = 100; 
  
  ZAIC_PEAK *new_zaic1 = new ZAIC_PEAK(ivp_domain, "x");
  bool ok1 = new_zaic1->setParams(summit1, pwidth1, bwidth1,  
				  sdelta1, min_util, max_util);
  if(ok1)
    m_zaic_peak1 = new_zaic1;

  ZAIC_PEAK *new_zaic2 = new ZAIC_PEAK(ivp_domain, "x");
  bool ok2 = new_zaic2->setParams(summit2, pwidth2, bwidth2,  
				  sdelta2, min_util, max_util);
  
  if(ok2)
    m_zaic_peak2 = new_zaic2;

  m_zaic_peak1->setValueWrap(m_value_wrap);
  m_zaic_peak2->setValueWrap(m_value_wrap);
}

//-------------------------------------------------------------
// Procedure: currMode()
//     Modes: 0 Alter Summit on ZAIC 1
//            1 Alter PWidth on ZAIC 1
//            2 Alter BWidth on ZAIC 1
//            3 Alter SDelta on ZAIC 1
//     Modes: 4 Alter Summit on ZAIC 2
//            5 Alter PWidth on ZAIC 2
//            6 Alter BWidth on ZAIC 2
//            7 Alter SDelta on ZAIC 2

void ZAIC_PEAK_Model::currMode(int new_mode)
{
  if((new_mode >= 0) && (new_mode <= 9))
    m_curr_mode = new_mode; 
}

//-------------------------------------------------------------
// Procedure: drawMode()
//     Modes: 0 Draw only ZAIC 1
//            1 Draw only ZAIC 1
//            2 Draw ZAIC 1 and ZAIC 2
//            3 Draw Max from both ZAICS
//            4 Draw Sum from both ZAICS
//            5 Toggle between modes

void ZAIC_PEAK_Model::drawMode(int new_mode)
{
  if((new_mode < 0) || (new_mode > 4))
    return;

  if(new_mode == 0) {
    m_draw_mode++;
    if(m_draw_mode > 4)
      m_draw_mode = 1;
  }
  else
    m_draw_mode = new_mode;
}

//-------------------------------------------------------------
// Procedure: setValueWrap()

void ZAIC_PEAK_Model::setValueWrap(bool val)
{
  m_value_wrap = val;
  m_zaic_peak1->setValueWrap(m_value_wrap);
  m_zaic_peak2->setValueWrap(m_value_wrap);
}

//-------------------------------------------------------------
// Procedure: moveX()

void ZAIC_PEAK_Model::moveX(double delta)
{
  if(!m_zaic_peak1 || !m_zaic_peak2)
    return;

  double highval = m_zaic_peak1->getIvPDomain().getVarPoints(0);
  
  //===========================================================
  // Handle for PEAK 1
  //===========================================================
  if(m_curr_mode==0) {   // Altering summit
    double summit = m_zaic_peak1->getParam("summit");
    summit += delta;
    if(summit < 0)
      summit = 0;
    if(summit >= highval)
      summit = highval;
    m_zaic_peak1->setSummit(summit);
  }
  else if(m_curr_mode == 1) { // Altering PeakWidth
    double pwidth = delta + m_zaic_peak1->getParam("peakwidth");
    if(pwidth < 0)
      pwidth = 0;
    m_zaic_peak1->setPeakWidth(pwidth);
  }
  else if(m_curr_mode == 2) { // Altering BaseWidth
    double bwidth = delta + m_zaic_peak1->getParam("basewidth");
    if(bwidth < 0) 
      bwidth = 0;
    m_zaic_peak1->setBaseWidth(bwidth);
  }
  else if(m_curr_mode == 3) { // Altering SummitDelta
    double minutil = m_zaic_peak1->getParam("minutil");
    double maxutil = m_zaic_peak1->getParam("maxutil");
    double sdelta = delta + m_zaic_peak1->getParam("summitdelta");
    double range = maxutil - minutil;
    sdelta = vclip(sdelta, 0, range);
    m_zaic_peak1->setSummitDelta(sdelta);
  }

  //===========================================================
  // Handle for PEAK 2
  //===========================================================
  else if(m_curr_mode==4) {   // Altering summit
    double summit = m_zaic_peak2->getParam("summit");
    summit += delta;
    if(summit < 0)
      summit = 0;
    if(summit >= highval)
      summit = highval;
    m_zaic_peak2->setSummit(summit);
  }
  else if(m_curr_mode == 5) { // Altering PeakWidth
    double pwidth = delta + m_zaic_peak2->getParam("peakwidth");
    if(pwidth < 0)
      pwidth = 0;
    m_zaic_peak2->setPeakWidth(pwidth);
  }
  else if(m_curr_mode == 6) { // Altering BaseWidth
    double bwidth = delta + m_zaic_peak2->getParam("basewidth");
    if(bwidth < 0)
      bwidth = 0;
    m_zaic_peak2->setBaseWidth(bwidth);
  }
  else if(m_curr_mode == 7) { // Altering SummitDelta
    double minutil = m_zaic_peak2->getParam("minutil");
    double maxutil = m_zaic_peak2->getParam("maxutil");
    double sdelta = delta + m_zaic_peak2->getParam("summitdelta");
    double range = maxutil - minutil;
    sdelta = vclip(sdelta, 0, range);
    m_zaic_peak2->setSummitDelta(sdelta);
  }

  //===========================================================
  // Handle for PEAK 2
  //===========================================================
  else if(m_curr_mode==8) {   // Altering Max Util
    double minutil = m_zaic_peak1->getParam("minutil");
    double maxutil = delta + m_zaic_peak1->getParam("maxutil");
    if(maxutil < 1)
      maxutil = 1;
    if(maxutil <= minutil)
      maxutil = minutil+1;
    if(maxutil > 100)
      maxutil = 100;
    m_zaic_peak1->setMinMaxUtil(minutil, maxutil);
    m_zaic_peak2->setMinMaxUtil(minutil, maxutil);
  }

  else if(m_curr_mode==9) {   // Altering Min Util
    double minutil = delta + m_zaic_peak1->getParam("minutil");
    double maxutil = m_zaic_peak1->getParam("maxutil");
    if(minutil > 99)
      minutil = 99;
    if(minutil >= maxutil)
      minutil = maxutil-1;
    if(minutil < 0)
      minutil = 0;
    m_zaic_peak1->setMinMaxUtil(minutil, maxutil);
    m_zaic_peak2->setMinMaxUtil(minutil, maxutil);
  }

  else
    cout << "Uh-Oh!  Mode problem" << endl;
}

//----------------------------------------------------------------
// Procedure: getSummit()

double ZAIC_PEAK_Model::getSummit1()
{
  if(!m_zaic_peak1)
    return(0);
  return(m_zaic_peak1->getParam("summit"));
}

double ZAIC_PEAK_Model::getSummit2()
{
  if(!m_zaic_peak2)
    return(0);
  return(m_zaic_peak2->getParam("summit"));
}

//----------------------------------------------------------------
// Procedure: getPeakWidth()

double ZAIC_PEAK_Model::getPeakWidth1()
{
  if(!m_zaic_peak1)
    return(0);
  return(m_zaic_peak1->getParam("peakwidth"));
}

double ZAIC_PEAK_Model::getPeakWidth2()
{
  if(!m_zaic_peak2)
    return(0);
  return(m_zaic_peak2->getParam("peakwidth"));
}

//----------------------------------------------------------------
// Procedure: getBaseWidth()

double ZAIC_PEAK_Model::getBaseWidth1()
{
  if(!m_zaic_peak1)
    return(0);
  return(m_zaic_peak1->getParam("basewidth"));
}

double ZAIC_PEAK_Model::getBaseWidth2()
{
  if(!m_zaic_peak2)
    return(0);
  return(m_zaic_peak2->getParam("basewidth"));
}

//----------------------------------------------------------------
// Procedure: getSummitDelta()

double ZAIC_PEAK_Model::getSummitDelta1()
{
  if(!m_zaic_peak1)
    return(0);
  return(m_zaic_peak1->getParam("summitdelta"));
}

double ZAIC_PEAK_Model::getSummitDelta2()
{
  if(!m_zaic_peak2)
    return(0);
  return(m_zaic_peak2->getParam("summitdelta"));
}

//----------------------------------------------------------------
// Procedure: getMinUtil()

double ZAIC_PEAK_Model::getMinUtil()
{
  if(!m_zaic_peak1)
    return(0);
  return(m_zaic_peak1->getParam("minutil"));
}

//----------------------------------------------------------------
// Procedure: getMaxUtil()

double ZAIC_PEAK_Model::getMaxUtil()
{
  if(!m_zaic_peak1)
    return(0);
  return(m_zaic_peak1->getParam("maxutil"));
}





