/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PipeWay.cpp                                          */
/*    DATE: Dec 21st 2020                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <iterator>
#include "MBUtils.h"
#include "PipeWay.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

PipeWay::PipeWay()
{
  // Init config variables 
  m_show_source = true;
  m_show_community = true;
  m_show_subscriptions = true;
  m_show_masked = true;

  m_wrap_content = false;
  m_trunc_content = false;
  m_time_format_utc = false;

  m_duration = 0;

  // force_refresh of unchanged watch variables
  m_force_refresh = false;
  
  // Init config variables 
  m_modified = true;
  
  m_expire_time = 0;
}

//---------------------------------------------------------
// Procedure: setStartExpTime()
//   Purpose: Set the expiration time based on the client configured
//            duration, plus a given start time and time warp.

void PipeWay::setStartExpTime(double start_time, double warp)
{
  // Want duration to be in real time, so mult by warp
  double duration = m_duration * warp;

  m_expire_time = start_time + duration;  
}

//---------------------------------------------------------
// Procedure: getContentDescriptor()

string PipeWay::getContentDescriptor(unsigned int trunclen) const
{
  if(!valid())
    return("null pipeway");
  
  string descriptor = m_channel;
  if(descriptor == "") {
    set<string>::iterator p;
    for(p=m_vars.begin(); p!=m_vars.end(); p++) {
      if(descriptor != "")
	descriptor += ",";
      descriptor += *p;
    }
    descriptor = "vars=" + descriptor;
  }

  if(descriptor.length() > trunclen)
    descriptor = descriptor.substr(0, trunclen);

  return(descriptor);
}

//---------------------------------------------------------
// Procedure: modified()

bool PipeWay::modified()
{
  bool modified = m_modified;
  m_modified = false;

  return(modified);
}

//---------------------------------------------------------
// Procedure: valid()

bool PipeWay::valid() const
{
  if((m_channel == "") && (m_vars.size() == 0))
    return(false);
  if(m_duration == 0)
    return(false);
  if(m_client == "")
    return(false);

  return(true);
}

//---------------------------------------------------------
// Procedure: timeUntilExpire()

double PipeWay::timeUntilExpire(double curr_time) const
{
  return(m_expire_time - curr_time);
}

//---------------------------------------------------------
// Procedure: Overload operator ==

bool PipeWay::operator==(const PipeWay& pipeway)
{
  if(m_client != pipeway.getClient())
    return(false);
  if(m_channel != pipeway.getChannel())
    return(false);

  if(m_show_source != pipeway.showSource())
    return(false);
  if(m_show_community != pipeway.showCommunity())
    return(false);
  if(m_show_subscriptions != pipeway.showSubscriptions())
    return(false);
  if(m_show_masked != pipeway.showMasked())
    return(false);

  if(m_wrap_content != pipeway.wrapContent())
    return(false);
  if(m_trunc_content != pipeway.truncContent())
    return(false);
  if(m_duration != pipeway.getDuration())
    return(false);
  if(m_time_format_utc != pipeway.timeFormatUTC())
    return(false);
  
  if(m_force_refresh != pipeway.forceRefresh())
    return(false);
  
  if(m_vars != pipeway.getVars())
    return(false);

  return(true);
}

//---------------------------------------------------------
// Procedure: Overload operator !=

bool PipeWay::operator!=(const PipeWay& pipeway)
{
  return(!(*this == pipeway));
}

//---------------------------------------------------------
// Procedure: string2PipeWay()
//   Example: channel=pHelmIvP,duration=3,mask,nosrc,nocom,nosubs,wrap,trunc
//   Example: vars=DEPLOY,duration=3,nosrc,nocom,nosubs,wrap,trunc
//   Example: vars=DEPLOY:RETURN:STATION,duration=3

PipeWay string2PipeWay(string str)
{
  PipeWay null_pipeway;
  PipeWay good_pipeway;

  vector<string> parts = parseString(str, ',');  
  for(unsigned int i=0; i<parts.size(); i++) {
    string param = tolower(biteStringX(parts[i], '='));
    string value = parts[i];
    
    if(param == "client") 
      good_pipeway.setClient(value);
    else if(param == "channel") 
      good_pipeway.setChannel(value);
    else if((param == "duration") && isNumber(value))
      good_pipeway.setDuration(atof(value.c_str()));
    else if(param == "vars") {
      vector<string> vars = parseString(value, ':');
      for(unsigned int j=0; j<vars.size(); j++) 
	good_pipeway.addVar(vars[j]);
    }
    else if(param == "nosrc")
      good_pipeway.setShowSource(false);
    else if(param == "nocom")
      good_pipeway.setShowCommunity(false);
    else if(param == "nosubs")
      good_pipeway.setShowSubscriptions(false);
    else if(param == "mask")
      good_pipeway.setShowMasked(false);
    else if(param == "wrap")
      good_pipeway.setWrapContent(true);
    else if(param == "trunc")
      good_pipeway.setTruncContent(true);
    else if(param == "utc")
      good_pipeway.setTimeFormatUTC(true);
    else if(param == "force")
      good_pipeway.setForceRefresh(true);
    else
      return(null_pipeway);
  }

  return(good_pipeway);
}




