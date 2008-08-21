/*****************************************************************/
/*    NAME: Henrik Schmidt and Michael Benjamin                  */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Attractor.cpp                                    */
/*    DATE: June 2008                                            */
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
#include "AOF_AttractorCPA.h"
#include "BHV_Attractor.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

BHV_Attractor::BHV_Attractor(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_cutrange");
  this->setParam("build_info", "uniform_box =discrete@course:2,speed:3");
  this->setParam("build_info", "uniform_grid=discrete@course:8,speed:6");


  // These parameters really should be set in the behavior file, but are
  // left here for now to smoothen the transition (Aug 10, 2008, mikerb)
  this->setParam("runflag",  "ATTRACTOR_ACTIVE=1");
  this->setParam("idleflag", "ATTRACTOR_ACTIVE=0");

  this->setParam("activeflag",   "ATTRACTED=1");
  this->setParam("inactiveflag", "ATTRACTED=0");
  
  m_domain = subDomain(m_domain, "course,speed");

  // If equal: 100% priority above, 0% priority below
  // If not equal: Linear scale between 0-100 in between

  m_min_priority_range = 0;
  m_max_priority_range = 0;

  m_min_util_cpa_dist = 100;
  m_max_util_cpa_dist = 0; 

  m_extrapolate  = true;
  m_patience     = 0;
  m_giveup_range = 0;   // meters - zero means never give up
  m_time_on_leg  = 15;  // seconds

  strength = 1.0;

  m_decay_start = 5;
  m_decay_end   = 10;

  m_extrapolator.setDecay(m_decay_start, m_decay_end);

  m_contact_name = "";

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Attractor::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParam(g_param, g_val))
    return(true);

  if((g_param == "them") || (g_param == "contact")) 
    {
      m_contact_name = toupper(g_val);
      updateContactList(); 
      return(true);
    }  
  else if(g_param == "dist_priority_interval") {
    g_val = stripBlankEnds(g_val);
    vector<string> svector = parseString(g_val, ',');
    if(svector.size() != 2)
      return(false);
    if(!isNumber(svector[0]) || !isNumber(svector[1]))
      return(false);
    
    double dval1 = atof(svector[0].c_str());
    double dval2 = atof(svector[1].c_str());
    if((dval1 < 0) || (dval2 < 0) || (dval1 > dval2))
      return(false);
    m_min_priority_range = dval1;
    m_max_priority_range = dval2;
    return(true);
  }  
  else if(g_param == "cpa_utility_interval") {
    g_val = stripBlankEnds(g_val);
    vector<string> svector = parseString(g_val, ',');
    if(svector.size() != 2)
      return(false);
    if(!isNumber(svector[0]) || !isNumber(svector[1]))
      return(false);

    double dval1 = atof(svector[0].c_str());
    double dval2 = atof(svector[1].c_str());
    if((dval1 < 0) || (dval2 < 0) || (dval1 >= dval2))
      return(false);
    m_max_util_cpa_dist = dval1;
    m_min_util_cpa_dist = dval2;
    return(true);
  }  
  else if(g_param == "time_on_leg") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_time_on_leg = dval;
    return(true);
  }  
  else if(g_param == "giveup_range") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    m_giveup_range = dval;
    return(true);
  }  

  else if(g_param == "patience") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (dval > 100) || (!isNumber(g_val)))
      return(false);
    m_patience = dval;
    return(true);
  }  

  else if(g_param == "strength") 
    {
      double dval = atof(g_val.c_str());
      if((dval < 0) || (dval > 100) || (!isNumber(g_val)))
	return(false);
      strength = dval;
      return(true);
    }  
  else if(g_param == "decay") 
    {
      vector<string> svector = parseString(g_val, ',');
      if(svector.size() == 2) {
	svector[0] = stripBlankEnds(svector[0]);
	svector[1] = stripBlankEnds(svector[1]);
	if(isNumber(svector[0]) && isNumber(svector[1])) {
	  double start = atof(svector[0].c_str());
	  double end   = atof(svector[1].c_str());
	  if((start >= 0) && (start <= end)) {
	    m_decay_start = start;
	    m_decay_end   = end;
	    m_extrapolator.setDecay(start,end);
	    return(true);
	  }
	}
      }
    }  
  else if(g_param == "decay_end") 
    {
      if(isNumber(g_val)) {
	m_decay_end = atof(g_val.c_str());
	
	return(true);
      }
    }  

  return(false);
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_Attractor::onIdleState()
{
  // Below commented out now since we can accomlish this now with 
  // runflags, idleflags, activeflags, inactiveflags

  // postMessage("ATTRACTOR_ACTIVE", 0);
  // postMessage("ATTRACTED", 0);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Attractor::onRunState() 
{
  // Below handled now with active/inactive flags - mikerb, Aug10,08
  // postMessage("ATTRACTOR_ACTIVE", 1);

  if(m_contact_name == "") 
    {
      postEMessage("contact ID not set.");
      return(0);
    }
  else
    {
      updateContactList();
    }

  // Set m_osx, m_osy, m_osh, m_osv, m_cnx, m_cny, m_cnh, m_cnv
  if(!updateInfoIn())
    return(0);

  // Calculate the relevance first. If zero-relevance, we won't
  // bother to create the objective function.
  double relevance = getRelevance(m_osx, m_osy, m_cnx, m_cny);

  if(relevance <= 0) {
    // Below handled now with active/inactive flags - mikerb, Aug10,08
    //postMessage("ATTRACTED", 0);
    return(0);
  }

  // Below handled now with active/inactive flags - mikerb, Aug10,08
  //postMessage("ATTRACTED", 1);

  double dist = hypot((m_osx - m_cnx), (m_osy - m_cny));

  IvPFunction *ipf = 0;
  if (dist > m_min_priority_range)
    {  AOF_AttractorCPA aof(m_domain);
      aof.setParam("cnlat", m_cny);
      aof.setParam("cnlon", m_cnx);
      aof.setParam("cncrs", m_cnh);
      aof.setParam("cnspd", m_cnv);
      aof.setParam("oslat", m_osy);
      aof.setParam("oslon", m_osx);
      aof.setParam("tol", m_time_on_leg);
      aof.setParam("patience", m_patience);
      aof.setParam("min_util_cpa_dist", m_min_util_cpa_dist);
      aof.setParam("max_util_cpa_dist", m_max_util_cpa_dist);
      bool ok1 = aof.initialize();
      
      if(!ok1) {
	postWMessage("Error in initializing AOF_AttractorCPA.");
	return(0);
      }
      
      OF_Reflector reflector(&aof);
      reflector.create(m_build_info);
      if(reflector.hasErrors())
	postWMessage(reflector.getErrors());
      else
	ipf = reflector.extractOF();
    }
  else
    {
      ZAIC_PEAK hdg_zaic(m_domain, "course");
      hdg_zaic.setSummit(m_cnh);
      hdg_zaic.setValueWrap(true);
      hdg_zaic.setPeakWidth(120);
      hdg_zaic.setBaseWidth(60);
      hdg_zaic.setSummitDelta(10.0);
      hdg_zaic.setMinMaxUtil(0,100);
      IvPFunction *hdg_ipf = hdg_zaic.extractOF();
      
      ZAIC_PEAK spd_zaic(m_domain, "speed");
      
      spd_zaic.setSummit(m_cnv);
      spd_zaic.setPeakWidth(0.1);
      spd_zaic.setBaseWidth(2.0);
      spd_zaic.setSummitDelta(50.0); 
      
      IvPFunction *spd_ipf = spd_zaic.extractOF();
      
      OF_Coupler coupler;
      ipf = coupler.couple(hdg_ipf, spd_ipf);
    }

  // Check for properly created IvPFunction before operating on it.
  if(ipf) {
    ipf->getPDMap()->normalize(0.0, 100.0);
    ipf->setPWT(relevance * m_priority_wt);
  }

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: updateInfoIn()
//   Purpose: Update info need by the behavior from the info_buffer.
//            Error or warning messages can be posted.
//   Returns: true if no vital info is missing from the info_buffer.
//            false otherwise.
//      Note: By posting an EMessage, this sets the state_ok member
//            variable to false which will communicate the gravity
//            of the situation to the helm.

bool BHV_Attractor::updateInfoIn()
{
  bool ok1, ok2, ok3, ok4, ok5, ok6, ok7, ok8, ok9;
 
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok3);
  m_osv = getBufferDoubleVal("NAV_SPEED", ok4);

  m_cnx = getBufferDoubleVal(m_contact_name+"_NAV_X", ok5);
  m_cny = getBufferDoubleVal(m_contact_name+"_NAV_Y", ok6);
  m_cnh = getBufferDoubleVal(m_contact_name+"_NAV_HEADING", ok7);
  m_cnv = getBufferDoubleVal(m_contact_name+"_NAV_SPEED", ok8);
  m_cnutc = getBufferDoubleVal(m_contact_name+"_NAV_UTC", ok9);

  if(!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || 
     !ok6 || !ok7 || !ok8 || !ok9)
    return(false);
  
  double curr_time = getBufferCurrTime();
  // double mark_time = getBufferTimeVal(m_contact_name+"_NAV_X");

  postIntMessage("ATTRACTOR_CONTACT_TIME", m_cnutc);
  postIntMessage("ATTRACTOR_DELTA_TIME", curr_time-m_cnutc);

  if(!m_extrapolate)
    return(true);

  // if(mark_time == 0)
    m_extrapolator.setPosition(m_cnx, m_cny, m_cnv, m_cnh, m_cnutc);

  // Even if mark_time is zero and thus "fresh", still derive the 
  // the contact position from the extrapolator since the UTC time
  // of the position in this report may still be substantially 
  // behind the current own-platform UTC time.

  double new_cnx, new_cny;
  bool ok = m_extrapolator.getPosition(new_cnx, new_cny, curr_time);

  if(ok) {
    m_cnx = new_cnx;
    m_cny = new_cny;
    return(true);
  }
  else {
    postWMessage("Incomplete Linear Extrapolation");
    return(false);
  }
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_Attractor::getRelevance(double osX, double osY,
				  double cnX, double cnY)
{
  double range_max = m_max_priority_range;
  double range_min = m_min_priority_range;

  // Should be caught when setting the parameters, but check again
  if((range_max < 0) || (range_min < 0) || (range_min > range_max)) {
    postWMessage("Priority Range Error");
    return(0);
  }
  
  double dist = hypot((osX - cnX), (osY - cnY));
  postIntMessage("ATTRACTOR_RANGE_TO_CONTACT",dist );

  if((m_giveup_range > 0) && (dist > m_giveup_range))
    return(0);

  
  double total_range = range_max - range_min;
  //double exp_wgt= 1.0-exp(-10*(dist-range_min)/range_max);
  //double exp_wgt = 1;
  double pct = 1;
  // if total_range==0 one of the two cases will result in a return

  if (dist >= range_max)
    pct = 1+strength;
  else
    if (dist < range_min)
      pct = dist/range_min;
    else 
      if (total_range > 0)
	pct = (1 + strength * (dist-range_min) / total_range);
      else
	pct = 1;

  // Changed to be ATTACTOR_RELEVANCE instead of TRAIL_RELEVANCE and 
  // to post as integer between 0-100, not 0-1.0 to reduce postings.
  // mikerb Aug 09, 2008
  postIntMessage("ATRACTOR_RELEVANCE", pct*100);
  return(pct);
}

//-----------------------------------------------------------------
// Procedure: updateContactList
//   Purpose: Maintain a list of known unique contact names, and subscrinbe to NAV info

void BHV_Attractor::updateContactList()
{
  unsigned int i;
  
  m_contact_name = toupper(m_contact_name);

  for(i=0; i<m_contact_list.size(); i++) {
    if(m_contact_list[i] == m_contact_name) {
      return;
    }
  }
  
  m_contact_list.push_back(m_contact_name);
  addInfoVars(m_contact_name+"_NAV_X");
  addInfoVars(m_contact_name+"_NAV_Y");
  addInfoVars(m_contact_name+"_NAV_SPEED");
  addInfoVars(m_contact_name+"_NAV_HEADING");
  addInfoVars(m_contact_name+"_NAV_UTC");

}









