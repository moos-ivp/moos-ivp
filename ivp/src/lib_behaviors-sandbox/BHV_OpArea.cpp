/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_OpArea.cpp                                       */
/*    DATE: Jul 6th, 2005                                        */
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
#include "BHV_OpArea.h"
#include "AOF_OpArea.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "XYBuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_OpArea::BHV_OpArea(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_op_area");
  this->setParam("build_info", "uniform_piece=course:2,speed:3,tol=3");
  this->setParam("build_info", "uniform_grid=course:8,speed:6,tol=6");

  m_domain = subDomain(m_domain, "course,speed,tol");

  max_depth    = 0;
  depth_buffer = 0;
  poly_buffer  = 0;

  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition,tag" where the tag is an optional
//            descriptor of the waypoint.
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_OpArea::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  // Typical line: polygon  = 0,0:0,100:100,0:100,100
  if(param == "polygon") {
    polygon = stringToPoly(val);
    // Should be convex 
    return(polygon.is_convex());
  }
  else if(param == "max_depth") {
    max_depth = atof(val.c_str());
    return(true);
  }
  else if(param == "depth_buffer") {
    depth_buffer = atof(val.c_str());
    return(true);
  }
  else if(param == "poly_buffer") {
    poly_buffer = atof(val.c_str());
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState
//     Notes: Returns NULL if all safety concerns can be verified
//     Notes: Returns (OF*)1 otherwise

IvPFunction *BHV_OpArea::onRunState() 
{
  bool ok1, ok2;
  // ownship position in meters from some 0,0 reference point.
  double osX = getBufferDoubleVal("NAV_X", ok1);
  double osY = getBufferDoubleVal("NAV_Y", ok2);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship info in info_buffer.");
    return(0);
  }

  double relevance = getRelevance(osX, osY);
  if(relevance == 0)
    return(0);

  AOF_OpArea aof(m_domain, polygon);
  aof.setParam("os_lat", osY);
  aof.setParam("os_lon", osX);
  aof.setParam("buffer", poly_buffer);
  OF_Reflector reflector(&aof, 1);

  reflector.create(m_build_info);
  IvPFunction *of = reflector.extractOF();

  of->setPWT(relevance * m_priority_wt);

  return(of);
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_OpArea::getRelevance(double osX, double osY)
{
  // If we're actually outside the poly, this is probably a fatal
  // condition, (handled by another behavior), but we'll still do
  // our best to produce an OF that brings us back inside the poly.
  if(!polygon.contains(osX, osY))
    return(1.0);

  double dist = polygon.dist_to_poly(osX, osY);

  if(dist >= poly_buffer)
    return(0.0);
  else
    return(1.0);
}







