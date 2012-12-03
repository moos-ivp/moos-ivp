/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPContactBehavior.cpp                               */
/*    DATE: Apr 3rd 2010 Separated/generalized from indiv. bhvs  */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <iostream>
#include <cmath>
#include <cstdlib>
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "AOF_AvoidCollision.h"
#include "IvPContactBehavior.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "CPA_Utils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

IvPContactBehavior::IvPContactBehavior(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  // Behavior will only post a warning - not error - if no contact info
  m_on_no_contact_ok  = true;

  m_extrapolate  = true;
  m_decay_start  = 15;
  m_decay_end    = 30;
  m_extrapolator.setDecay(m_decay_start, m_decay_end);
  m_time_on_leg  = 60;

  // Initialize state variables
  m_osx = 0;
  m_osy = 0;
  m_osh = 0;
  m_osv = 0;
  m_cnx = 0;
  m_cny = 0;
  m_cnh = 0;
  m_cnv = 0;
  m_cnutc = 0;

  m_contact_range = 0;
  m_relevance     = 0;

  m_bearing_line_show = false;
  m_bearing_line_info = "relevance";
}

//-----------------------------------------------------------
// Procedure: setParam

bool IvPContactBehavior::setParam(string param, string param_val) 
{
  double dval = atof(param_val.c_str());
  bool non_neg_number = (isNumber(param_val) && (dval >= 0));
  
  if((param == "contact") ||     // preferred
     (param == "them")) {        // deprecated 4/2010
    m_contact = toupper(param_val);
    return(true);
  }  
  else if (param == "extrapolate")
    return(setBooleanOnString(m_extrapolate, param_val));
  else if(param == "decay") {
    string left  = biteStringX(param_val, ',');
    string right = param_val;
    if(isNumber(left) && isNumber(right)) {
      double start = atof(left.c_str());
      double end   = atof(right.c_str());
      if((start >= 0) && (start <= end)) {
	m_decay_start = start;
	m_decay_end   = end;
	m_extrapolator.setDecay(start,end);
	return(true);
      }
    }
  }  
  else if(param == "on_no_contact_ok") 
    return(setBooleanOnString(m_on_no_contact_ok, param_val));

  else if(param == "decay_end") {        // deprecated 4/2010
    if(isNumber(param_val)) {
      m_decay_end = dval;
      return(true);
    }
  }  
  else if(param == "time_on_leg") {
    if(!non_neg_number)
      return(false);
    m_time_on_leg = dval;
    return(true);
  }  
  // bearing_lines = white:0, green:0.65, yellow:0.8, red:1.0
  else if(param == "bearing_line_config") {
    m_bearing_line_show = false;
    vector<string> svector = parseString(param_val, ',');
    unsigned int i, vsize = svector.size();
    bool valid_components = true;
    for(i=0; i<vsize; i++) {
      string left  = tolower(stripBlankEnds(biteString(svector[i],':')));
      string right = stripBlankEnds(svector[i]);
      if((left == "info") && (right == "range")) 
	m_bearing_line_info = "range";
      else if((left == "info") && (right == "relevance")) 
	m_bearing_line_info = "relevance";
      else if(isColor(left) && isNumber(right) && valid_components) {
	m_bearing_line_colors.push_back(left);
	double thresh_value = atof(right.c_str());
	m_bearing_line_thresh.push_back(thresh_value);
	m_bearing_line_show = true;
      }
      else
	valid_components = false;
    }
    return(true);
  }  
  return(false);
}

//-----------------------------------------------------------
// Procedure: updatePlatformInfo
//   Purpose: Update the following member variables:
//             m_osx: Current ownship x position (meters) 
//             m_osy: Current ownship y position (meters) 
//             m_osh: Current ownship heading (degrees 0-359)
//             m_osv: Current ownship speed (meters) 
//             m_cnx: Current contact x position (meters) 
//             m_cny: Current contact y position (meters) 
//             m_cnh: Current contact heading (degrees 0-359)
//             m_cnv: Current contact speed (meters) 
//             m_cnu: UTC time of last contact report

bool IvPContactBehavior::updatePlatformInfo()
{
  if(m_contact == "") {
    postEMessage("contact ID not set.");
    return(0);
  }

  bool ok = true;

  // First ascertain the current ownship position and trajectory.
  if(ok) m_osx = getBufferDoubleVal("NAV_X", ok);
  if(ok) m_osy = getBufferDoubleVal("NAV_Y", ok);
  if(ok) m_osh = getBufferDoubleVal("NAV_HEADING", ok);
  if(ok) m_osv = getBufferDoubleVal("NAV_SPEED", ok);
  if(!ok) {
    postEMessage("ownship x,y,heading, or speed info not found.");
    return(false);
  }
  m_osh = angle360(m_osh);
  m_cnh = angle360(m_cnh);

  // Next ascertaint the current contact position and trajectory.
  if(ok) m_cnx = getBufferDoubleVal(m_contact+"_NAV_X", ok);
  if(ok) m_cny = getBufferDoubleVal(m_contact+"_NAV_Y", ok);
  if(ok) m_cnh = getBufferDoubleVal(m_contact+"_NAV_HEADING", ok);
  if(ok) m_cnv = getBufferDoubleVal(m_contact+"_NAV_SPEED",   ok);
  if(ok) m_cnutc = getBufferDoubleVal(m_contact+"_NAV_UTC", ok);
  if(!ok) {    
    string msg = m_contact + " x/y/heading/speed info not found";
    if(m_on_no_contact_ok)
      postWMessage(msg);
    else
      postEMessage(msg);
    return(false);
  }
  
  // Next set the current range in meters to the contact.
  m_contact_range = hypot((m_osx-m_cnx), (m_osy-m_cny));

  // If extrapolation is turned off, we're done with the update.
  if(!m_extrapolate)
    return(true);

  // Even if mark_time is zero and thus "fresh", still derive the 
  // the contact position from the extrapolator since the UTC time
  // of the position in this report may still be substantially 
  // behind the current own-platform UTC time.

  m_extrapolator.setPosition(m_cnx, m_cny, m_cnv, m_cnh, m_cnutc);
  
  double curr_time = getBufferCurrTime();
  double new_cnx, new_cny;
  ok = m_extrapolator.getPosition(new_cnx, new_cny, curr_time);

  if(ok) {
    m_cnx = new_cnx;
    m_cny = new_cny;
  }
  else
    postWMessage("Incomplete Linear Extrapolation");

  return(ok);
}

//-----------------------------------------------------------
// Procedure: postViewableBearingLine

void IvPContactBehavior::postViewableBearingLine()
{
  if(!m_bearing_line_show)
    return;

  double index_value;
  if(m_bearing_line_info == "range")
    index_value = m_contact_range;
  else if(m_bearing_line_info == "relevance")
    index_value = m_relevance;
  else
    return;

  string color = "";
  unsigned int i, vsize = m_bearing_line_colors.size();
  for(i=0; (i<vsize)&&(color==""); i++) {
    if(index_value <= m_bearing_line_thresh[i])
      color = m_bearing_line_colors[i];
  }
  if(color == "") 
    color = "blank";
  postMessage("COLOR_CHOICE", color);
  postMessage("COLOR_RANGE", m_contact_range);
  postMessage("COLOR_IVALUE", index_value);

  m_bearing_line.clear(); 
  m_bearing_line.set_active(true);
  m_bearing_line.add_vertex(m_osx, m_osy);
  m_bearing_line.add_vertex(m_cnx, m_cny);
  m_bearing_line.set_label(m_us_name + "_" + m_descriptor);
  //m_bearing_line.set_color("label", "invisible");
  m_bearing_line.set_color("label", "white");
  m_bearing_line.set_color("edge", color);

  postMessage("VIEW_SEGLIST", m_bearing_line.get_spec());
}


//-----------------------------------------------------------
// Procedure: postErasableBearingLine

void IvPContactBehavior::postErasableBearingLine()
{
  m_bearing_line.set_active(false);
  postMessage("VIEW_SEGLIST", m_bearing_line.get_spec());
}


