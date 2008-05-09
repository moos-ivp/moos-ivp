/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_HCutRange.cpp                                     */
/*    DATE: May 10th 2005                                        */
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
#include "AOF_HCutRangeCPA.h"
#include "BHV_HCutRange.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_HCutRange::BHV_HCutRange(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_cutrange");
  this->setParam("build_info", "uniform_piece=course:3,speed:2");
  this->setParam("build_info", "uniform_grid=course:9,speed:6");

  m_domain = subDomain(m_domain, "course,speed,tol");

  range_max = 0;
  range_min = 50;
  obsolete = 0;
  my_contact = false;
  cnCRS = 0;
  cnSPD = 0;
  cnX   = 0;
  cnY   = 0;
  cnTime   = 0;

  addInfoVars("NAV_X");
  addInfoVars("NAV_Y");
  addInfoVars("NAV_SPEED");
  addInfoVars("NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_HCutRange::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParamCommon(g_param, g_val))
    return(true);

  if((g_param == "them") ||(g_param == "contact")) 
    {
      m_them_name = g_val;
      them_id = atoi(g_val.c_str());
      return(true);
    }  
  else if(g_param == "max_range") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    range_max = dval;
    return(true);
  }  
  else if(g_param == "min_range") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    range_min = dval;
    return(true);
  }  
  else if (g_param == "obsolete")
    {
      obsolete = atof(g_val.c_str());
      return(true);
    }
  return(false);
}

//-----------------------------------------------------------
// Procedure: onIdleState
//      Note: This function overrides the onIdleState() virtual
//            function defined for the IvPBehavior superclass
//            This function will be executed by the helm each
//            time the behavior FAILS to meet its run conditions.

void BHV_HCutRange::onIdleState()
{
  // Do your thing here
  my_contact = false;
  cnCRS = 0;
  cnSPD = 0;
  cnX   = 0;
  cnY   = 0;
  cnTime   = 0;
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_HCutRange::onRunState() 
{
  if(m_them_name == "") {
    postEMessage("contact name not set.");
    return(0);
  }

  bool ok1, ok2;

  //get current contact state
  string tState = getBufferStringVal("COMMUNITY_STAT", ok1);
  

  if(!ok1) 
    {
      //     postEMessage("contact info not found.");
      return(0);
    }

  new_state = decode(tState);

  my_contact = (contact_id == them_id);

  if ( my_contact )
    {
      cnCRS = contact_heading;
      cnSPD = contact_speed;
      cnX   = contact_x;
      cnY   = contact_y;
      cnTime   = contact_time;
    }


  //  double cnCRS = getBufferDoubleVal(them_name, "NAV_HEADING", &ok1);
  //  double cnSPD = getBufferDoubleVal(them_name, "NAV_SPEED", &ok2);
  // if(!ok1 || !ok2) {
  //  postEMessage("contact course/speed info not found.");
  //  return(0);
  //  }

  double osCRS = getBufferDoubleVal("NAV_HEADING", ok1);
  double osSPD = getBufferDoubleVal("NAV_SPEED", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship course/speed info not found.");
    return(0);
  }

  if(cnCRS < 0) cnCRS += 360.0;

  //  double cnX = getBufferDoubleVal(them_name, "NAV_X", &ok2);
  //  double cnY = getBufferDoubleVal(them_name, "NAV_Y", &ok1);
  // if(!ok1 || !ok2) {
  //  postEMessage("contact x/y info not found.");
  //  return(0);
  //}

  double osX = getBufferDoubleVal("NAV_X", ok2);
  double osY = getBufferDoubleVal("NAV_Y", ok1);
  if(!ok1 || !ok2) {
    postEMessage("ownship x/y info not found.");
    return(0);
  }


  // Calculate the relevance first. If zero-relevance, we won't
  // bother to create the objective function.
  double relevance = getRelevance(osX, osY, cnX, cnY);

  if(relevance <= 0)
    return(0);

  AOF_HCutRangeCPA aof(m_domain);
  aof.setParam("cnlat", cnY);
  aof.setParam("cnlon", cnX);
  aof.setParam("cncrs", cnCRS);
  aof.setParam("cnspd", cnSPD);
  aof.setParam("oslat", osY);
  aof.setParam("oslon", osX);
  aof.initialize();

  OF_Reflector reflector(&aof, 1);

  reflector.create(m_build_info);

  IvPFunction *of = reflector.extractOF();

  of->getPDMap()->normalize(0.0, 100.0);

  of->setPWT(relevance * m_priority_wt);

  return(of);
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_HCutRange::getRelevance(double osX, double osY,
				  double cnX, double cnY)
{
  double curr_time = getBufferCurrTime();
  if (((curr_time - contact_time) < obsolete) || (obsolete == 0))
    {
      double total_range = range_max - range_min;
      if(total_range <= 0)  // This would likely be an error
	return(0);

      double dist = hypot((osX - cnX), (osY - cnY));
      
      if(dist > range_max)
	return(0.0);
      if(dist < range_min)
	return(0.0);

      double pct = (dist - range_min) / total_range;
      return(pct);
    }
  else
    return(0.0);

}

int BHV_HCutRange::decode(string status)
{
  vector<string> svector;
  vector<string> svector2;
  
  
  // Parse the community state string for "contact"  
  svector = parseString(status, ',');
  for(unsigned int i=0; i<svector.size(); i++) 
    {
      svector2 = parseString(svector[i], '=');
      if(svector2.size() != 2) 
	{
	  postEMessage("error,BHV_HShadow: Invalid community string");
	  return(0);
	}
      
      string left  = stripBlankEnds(svector2[0]);
      string right = stripBlankEnds(svector2[1]);
      if(left == "node")     contact_id       = atoi(right.c_str());
      if(left == "x")        contact_x        = atof(right.c_str());
      if(left == "y")        contact_y        = atof(right.c_str());
      if(left == "heading")  contact_heading  = atof(right.c_str());
      if(left == "speed")    contact_speed    = atof(right.c_str());
      if(left == "time")     contact_time     = atof(right.c_str());
    }
  
  return(state);
}






