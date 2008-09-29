/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_ConstantDepth.cpp                                */
/*    DATE: Jul 3rd 2005                                         */
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
#include <iostream>
#include <math.h> 
#include <stdlib.h>
#include "BHV_ConstantDepth.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "ZAIC_PEAK.h"
#include "LogicUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_ConstantDepth::BHV_ConstantDepth(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "bhv_constantdepth");

  m_domain = subDomain(m_domain, "depth");

  m_desired_depth = 0;
  m_peakwidth     = 0;
  m_basewidth     = 2;

  // The default duration at the IvPBehavior level is "-1", which
  // indicates no duration applied to the behavior by default. By
  // setting to zero here, we force the user to provide a duration
  // value otherwise it will timeout immediately.
  m_duration      = 0;
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_ConstantDepth::setParam(string param, string val) 
{
  if(isConditionalParamString(val))
    return(handleConditionalParam(param, val));

  if(IvPBehavior::setParam(param, val))
    return(true);

  if(param == "depth") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_desired_depth = dval;
    return(true);
  }

  if(param == "peakwidth") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_peakwidth = dval;
    return(true);
  }
  
  if(param == "basewidth") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    m_basewidth = dval;
    return(true);
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: handleConditionalParams

bool BHV_ConstantDepth::handleConditionalParam(string param, string val)
{
#if 0
  ConditionalParam condi_param;
  condi_param.setFromString(param, val);

  if(condi_param.ok()) {
    cout << "param: " << condi_param.getParam() << endl;
    cout << "val:   " << condi_param.getParamVal() << endl;
    cout << "condi: " << endl;

    LogicCondition condi = condi_param.getCondition();
    condi.print();
  }

  if(!condi_param.ok())
    return(false);

  string param_val = condi_param.getParamVal();
  param_lock = true;
  bool ok = setParam(param, param_val);
  param_lock = false;

  if(!ok)
    return(false);

  condi_params.push_back(condi_param);
#endif
  return(true);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_ConstantDepth::onRunState() 
{
  if(!m_domain.hasDomain("depth")) {
    postEMessage("No 'depth' variable in the helm domain");
    return(0);
  }

  ZAIC_PEAK zaic(m_domain, "depth");
  zaic.setSummit(m_desired_depth);
  zaic.setBaseWidth(m_basewidth);
  zaic.setPeakWidth(m_peakwidth);

  IvPFunction *ipf = zaic.extractOF();
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}








