/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI - MIT Dept. Mechanical Engineering */
/*    FILE: BHV_ConstantDepth.cpp                                */
/*    DATE: Jul 3rd 2005                                         */
/*                                                               */
/* IvPHelm - Interval Programming Autonomous Control Helm        */
/*                                                               */
/* A set of libraries and applications for a behavior-based      */
/* autonomous control architecture with IvP action selection.    */
/*                                                               */
/* Patent Pending under U.S. Patent Application                  */
/* Ser. Nos. 10/631,527 and 10/911,765.                          */
/*                                                               */
/* Permission to use, copy, modify, and distribute this          */
/* software is CURRENTLY NOT PERMITTED, except by the            */
/* author(s).  We intend to make this software available         */
/* under a BSD style license by the end of 2005, at which        */
/* time permission to copy, modify and distribute will be        */
/* granted for non-commercial purposes.                          */
/*                                                               */
/* IN NO EVENT SHALL THE U.S.GOVERNMENT BE LIABLE TO ANY PARTY   */
/* FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL   */
/* DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF    */
/* THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE U.S.         */
/* GOVERNMENT HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH        */
/* DAMAGE.                                                       */
/*                                                               */
/* THE U.S. GOVERNMENT SPECIFICALLY DISCLAIMS ANY WARRANTIES,    */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF      */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE     */
/* U.S. GOVERNMENT HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE,    */
/* SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.             */
/*****************************************************************/
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h> 
#include <stdlib.h>
#include "BHV_Adaptive_YoYo.h"
#include "ZAIC_PEAK.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Adaptive_YoYo::BHV_Adaptive_YoYo(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_Adaptive_YoYo");
  this->setParam("unifbox", "depth=2");
  this->setParam("gridbox", "depth=5");

  m_domain = subDomain(m_domain, "depth");

  desired_depth = 0;
  addInfoVars("YoYo_depth");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Adaptive_YoYo::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if(param == "width") 
    {
      width = (int) atof(val.c_str());
    }

  if(param == "peak_width") 
    {
      pwidth = (int) atof(val.c_str());
    }

  return(false);
}

//-----------------------------------------------------------
// Procedure: produceOF
//
//          ^     desired-depth     
//      100-|           .
//          |          / \
//          |         /   \ 
//          |        /     \
// Utility  |       /       \
//          |      /         \
//          |     /           \
//          |    /             \
//          |___/_______________\__________________
//                    Depth

IvPFunction *BHV_Adaptive_YoYo::onRunState() 
{
  
  bool ok; 
  double desired_depth = getBufferDoubleVal("YoYo_depth",  ok);

  if(!ok) {
    postEMessage("error,BHV_Adaptive_YoYo: No yoyo depth info.");
    return(false);
  }

  ZAIC_PEAK depth_zaic(m_domain,"depth");
  depth_zaic.setSummit(desired_depth);
  depth_zaic.setValueWrap(true);
  depth_zaic.setPeakWidth(pwidth);
  depth_zaic.setBaseWidth(width);
  depth_zaic.setMinMaxUtil(0,100);

  IvPFunction *of = depth_zaic.extractOF();

  of->setPWT(m_priority_wt);

  return(of);
}


