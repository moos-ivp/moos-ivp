/*****************************************************************/
/*    NAME: Don Eickstedt                                        */
/*    ORGN: NAVSEA Newport RI                                    */
/*    FILE: BHV_YoYo.cpp                                         */
/*    DATE: 18 Dec, 2006                                         */
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

#include <iostream>
#include <math.h> 
#include "BHV_YoYo.h"
#include "BuildUtils.h"

using namespace std;

/******************************************************************************
 *                                                                            *
 *  Method: BHV_YoYo(IvPDomain gdomain) : IvPBehavior(gdomain)                *
 *                                                                            *
 *  Description: Constructor for the BHV_YoYo behavior class.                 *
 *                                                                            * 
 ******************************************************************************/

BHV_YoYo::BHV_YoYo(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_Adaptive_YoYo");
  this->setParam("unifbox", "depth=5");
  this->setParam("gridbox", "depth=10");

  //this behavior controls on depth only
  m_domain = subDomain(m_domain, "depth");

  //determines initial direction when the behavior
  //activates. -1 = down, +1 = up
  current_direc = -1;

  //makes sure the parameters get set
  upper_set = false;
  lower_set = false;

  //minimum safe altitude
  alt_min = 0.0;

  //subscribe to vehicle depth info
  addInfoVars("NAV_DEPTH, NAV_ALTITUDE");
}

/******************************************************************************
 *                                                                            *
 *  Method:setParam(string,string)                                            *
 *                                                                            *
 *  Description: The setParam method reads the initial behaior parameters     *
 *  from the behavior file and approriately sets the internal variables.      *
 *                                                                            *
 ******************************************************************************/


bool BHV_YoYo::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);
     
  if(param == "upper") {
    upper = atof(val.c_str());
    upper_set = true;
    return(true);
  }
  if(param == "lower") {
    lower = atof(val.c_str());
    lower_set = true;
    return(true);
  }  
  if(param == "min_altitude") {
    alt_min = atof(val.c_str());
    alt_set = true;
    return(true);
  }  
  
  return(false);
}

/******************************************************************************
 *                                                                            *
 *  Method: onRunState()                                                       *
 *                                                                            *
 *  Description: The onRunState method creates and returns the objective       *
 *  function for the BHV_YoYo behavior                                        *
 *                                                                            *
 ******************************************************************************/

IvPFunction *BHV_YoYo::onRunState() 
{
  double peak_utility = 100;

  int    depthIndex  = m_domain.getIndex("depth");

  if(depthIndex == -1) 
    {
     postEMessage("BHV_YoYo: No 'depth' variable in the helm domain");
     return(0);
    }

  if(!upper_set || !lower_set)
    {
     postEMessage("BHV_YoYo: yoyo limits not set");
     return(0);
    }

  bool ok,ok2;
  //query the current depth from the MOOS DB
  double current_depth = m_info_buffer->dQuery("NAV_DEPTH", ok);
  
   if(!ok)
    {
     postEMessage("BHV_YoYo: no depth info available");
     return(0);
    } 

   //if we set a min altitude, query the current altitude
  if (alt_set)
    {
      current_alt = m_info_buffer->dQuery("NAV_ALTITUDE", ok2);
       if(!ok2)
	 {
	   postEMessage("BHV_YoYo: no altitude info available");
	   return(0);
	 } 
    }

  //keep going in the same direction
  if (current_direc == -1)
    desired_depth = lower;
  else if (current_direc == 1)
    desired_depth = upper;

  //until we hit a limit, then toggle direction
  if (current_depth <= upper)
    {
    desired_depth = lower;
    current_direc = -1;
    }
  else if (current_depth >= lower)
    {
    desired_depth = upper;
    current_direc = 1;
    }
  
  //unless we hit the minimum altitude first, then toggle
  if (current_alt <= alt_min)
    {
      current_direc = 1;
      desired_depth = upper;
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


