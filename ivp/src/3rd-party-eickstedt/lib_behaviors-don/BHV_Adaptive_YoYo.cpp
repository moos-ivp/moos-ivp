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
#include "BHV_Adaptive_YoYo.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Adaptive_YoYo::BHV_Adaptive_YoYo(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_Adaptive_YoYo");
  this->setParam("unifbox", "depth=5");
  this->setParam("gridbox", "depth=10");

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


  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState
//
//          ^     desired-depth     
//      100-|           .
//          |          / \
//          | piece-1 /   \ piece-2
//          |        /     \
// Utility  |       /       \
//          |      /         \
//          |     /           \
//          |    /             \
//          |___/_______________\__________________
//                    Depth

IvPFunction *BHV_Adaptive_YoYo::onRunState() 
{
  double peak_utility = 100;

  int    depthIndex  = m_domain.getIndex("depth");

  if(depthIndex == -1) {
    postEMessage("No 'depth' variable in the helm domain");
    return(0);
  }

  bool ok; 
  double desired_depth = getBufferDoubleVal("YoYo_depth",  ok);

  if(!ok) {
    postEMessage("error,BHV_Adaptive_YoYo: No yoyo depth info.");
    return(false);
  }


  double depthBase   = m_domain.getVarLow(depthIndex);
  double depthDelta  = m_domain.getVarDelta(depthIndex);
  int    depthPoints = m_domain.getVarPoints(depthIndex);
  
  double double_index = (desired_depth - depthBase) / depthDelta;
  int    domain_index = (int)(floor(double_index + 0.5));
  double slope = peak_utility / domain_index;

  IvPBox *piece0 = new IvPBox(1,1);
  IvPBox *piece1 = 0;

  piece0->setPTS(0, 0, domain_index);
  piece0->wt(0) = slope;
  piece0->wt(1) = 0.0;

  if(domain_index < depthPoints) {
    piece1 = new IvPBox(1,1);
    piece1->setPTS(0, domain_index+1, depthPoints);
    piece1->wt(0) = -slope;
    piece1->wt(1) = 2.0 * peak_utility;
  }

  PDMap *pdmap;
  if(piece1) {
    pdmap = new PDMap(2, m_domain, 1);
    pdmap->bx(1) = piece1;
  }
  else
    pdmap = new PDMap(1, m_domain, 1);
  pdmap->bx(0) = piece0;


  pdmap->updateGrid();

  IvPFunction *of = new IvPFunction(pdmap);
  of->setPWT(m_priority_wt);

  return(of);
}


