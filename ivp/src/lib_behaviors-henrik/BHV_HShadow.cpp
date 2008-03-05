/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_HShadow.cpp                                      */
/*    DATE: May 10th 2005                                        */
/*    MODIFIED: Henrik Schmidt, MIT                              */
/*    DATE: Apr. 13, 2007                                        */
/*    Changed to using COLLABORATOR_STAT instead of MOOSBridge   */
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
#include <string>
#include <iostream>
#include <math.h>
#include "MBUtils.h"
#include "AngleUtils.h"
#include "AOF_HShadow.h"
#include "BHV_HShadow.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_HShadow::BHV_HShadow(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_shadow");
  this->setParam("build_info", "uniform_piece=course:2,speed:3");
  this->setParam("build_info", "uniform_grid=course:8,speed:6");
  
  m_domain = subDomain(m_domain, "course,speed");

  max_range = 0;
  my_contact = false;
  cnCRS = 0;
  cnSPD = 0;
  cnX   = 0;
  cnY   = 0;
  cnTime   = 0;
  obsolete = 0;


  addInfoVars("NAV_X");
  addInfoVars("NAV_Y");
  addInfoVars("NAV_SPEED");
  addInfoVars("NAV_HEADING");
  addInfoVars("COMMUNITY_STAT");
  addInfoVars("DB_TIME");
}

//-----------------------------------------------------------
// Procedure: onIdleState
//      Note: This function overrides the onIdleState() virtual
//            function defined for the IvPBehavior superclass
//            This function will be executed by the helm each
//            time the behavior FAILS to meet its run conditions.

void BHV_HShadow::onIdleState()
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
// Procedure: setParam

bool BHV_HShadow::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParamCommon(g_param, g_val))
    return(true);

  if((g_param == "them") ||(g_param == "contact")) 
    {
      them_name = g_val;
      them_id = atoi(g_val.c_str());
      return(true);
    }  
  else if(g_param == "max_range") {
    max_range = atof(g_val.c_str());
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
// Procedure: onRunState

IvPFunction *BHV_HShadow::onRunState() 
{
  if(them_name == "") {
    postEMessage("contact name not set.");
    return(0);
  }


  bool ok1, ok2;

  //get current contact state
  string tState = getBufferStringVal("COMMUNITY_STAT", ok1);
  

  if(!ok1) {
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
  // double cnSPD = getBufferDoubleVal(them_name, "NAV_SPEED", &ok2);
  // if(!ok1 || !ok2) {

  double osCRS = getBufferDoubleVal("NAV_HEADING", ok1);
  double osSPD = getBufferDoubleVal("NAV_SPEED", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship course/speed info not found.");
    return(0);
  }

  if(cnCRS < 0) cnCRS += 360.0;

  //  double cnY = getBufferDoubleVal(them_name, "NAV_Y", &ok1);
  // double cnX = getBufferDoubleVal(them_name, "NAV_X", &ok2);
  // if(!ok1 || !ok2) {
  //  postEMessage("contact x/y info not found.");
  //  return(0);
  // }

  double osY = getBufferDoubleVal("NAV_Y", ok1);
  double osX = getBufferDoubleVal("NAV_X", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship x/y info not found.");
    return(0);
  }

  // Calculate the relevance first. If zero-relevance, we won't
  // bother to create the objective function.
  double relevance = getRelevance(osY, osX, cnY, cnX);

  if(relevance <= 0)
    return(0);

  AOF_HShadow aof(m_domain);
  aof.setParam("cn_crs", cnCRS);
  aof.setParam("cn_spd", cnSPD);
  aof.initialize();

  OF_Reflector reflector(&aof, 1);

  reflector.create(m_build_info);

  IvPFunction *of = reflector.extractOF(true);

  of->setPWT(relevance * m_priority_wt);

  return(of);
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_HShadow::getRelevance(double osx, double osy,
				 double cnx, double cny)
{
  bool ok1;
  double curr_time = getBufferDoubleVal("DB_TIME", ok1);
  if (((curr_time - contact_time) < obsolete) || (obsolete == 0))
    {
      if(max_range == 0)
	return(1.0);
      
      double contact_range = hypot((osx-cnx), (osy-cny));
      if(contact_range < max_range)
	return(1.0);
      else
	return(0.0);
    }
  else
    return(0.0);
}


int BHV_HShadow::decode(string status)
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







