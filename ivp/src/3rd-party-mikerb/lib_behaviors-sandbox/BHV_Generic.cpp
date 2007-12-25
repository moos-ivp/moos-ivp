/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Generic.cpp                                      */
/*    DATE: July 26th 2005 In Elba w/ M.Grunde, P.Newman         */
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

// This behavior is intended to serve as a template for developing
// A new behavior. 


#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h>
#include "GeomUtils.h"
#include "BHV_Generic.h"
#include "AOF_WPT2D.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Generic::BHV_Generic(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_generic");

  m_domain = subDomain(m_domain, "course,speed");

  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Generic::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParamCommon(g_param, g_val))
    return(true);

  if(g_param == "foobar") {
    foobar = atoi(g_val.c_str());
    return(true);
  }  

  return(false);
}

//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_Generic::produceOF() 
{
  bool ok1, ok2;
  // ownship position in meters from some 0,0 reference point.
  double osX = m_info_buffer->dQuery("NAV_X", ok1);
  double osY = m_info_buffer->dQuery("NAV_Y", ok2);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship info in info_buffer.");
    return(0);
  }

  return(0);
}







