/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: InfoCastSettings.cpp                                 */
/*    DATE: Dec 5th, 2020                                        */
/*    NOTE: Migrated from VPlug_AppCastSettings.cpp              */
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
#include <cstdlib>
#include "InfoCastSettings.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

InfoCastSettings::InfoCastSettings()
{
  m_content_mode = "appcast";
  m_refresh_mode = "events";
  m_infocast_viewable = true;
  m_full_screen = false;

  m_infocast_font_size = "medium";
  m_procs_font_size = "large";
  m_nodes_font_size = "large";

  m_infocast_height = 70;
  m_infocast_width  = 30;  

  m_appcast_color_scheme = "indigo";
  m_realmcast_color_scheme = "hillside";

  m_show_rc_source = true;
  m_show_rc_community = true;
  m_show_rc_masked = true;
  m_show_rc_subscriptions = true;

  m_wrap_rc_content = false;
  m_trunc_rc_content = false;

  m_time_format_utc = false;
}

//-------------------------------------------------------------
// Procedure: setAppCastColorScheme()

bool InfoCastSettings::setAppCastColorScheme(string str)
{
  str = tolower(stripBlankEnds(str));

  if(str == "default") {
    m_appcast_color_scheme = "indigo";
    return(true);
  }
  
  if((str != "white") && (str != "indigo") &&
     (str != "beige") && (str != "toggle"))
    return(false);

  if(str == "toggle") {
    if(m_appcast_color_scheme == "white")
      m_appcast_color_scheme = "indigo";
    else if(m_appcast_color_scheme == "indigo")
      m_appcast_color_scheme = "beige";
    else if(m_appcast_color_scheme == "beige")
      m_appcast_color_scheme = "white";
  }
  else
    m_appcast_color_scheme = str;

  return(true);
}

//-------------------------------------------------------------
// Procedure: setRealmCastColorScheme()

bool InfoCastSettings::setRealmCastColorScheme(string str)
{
  str = tolower(stripBlankEnds(str));
  
  if(str == "default") {
    m_appcast_color_scheme = "hillside";
    return(true);
  }
  
  if((str != "white") && (str != "indigo") && (str != "hillside") &&
     (str != "beige") && (str != "toggle"))
    return(false);

  if(str == "toggle") {
    if(m_realmcast_color_scheme == "white")
      m_realmcast_color_scheme = "hillside";

    else if(m_realmcast_color_scheme == "hillside")
      m_realmcast_color_scheme = "beige";

    else if(m_realmcast_color_scheme == "beige")
      m_realmcast_color_scheme = "indigo";

    else if(m_realmcast_color_scheme == "indigo")
      m_realmcast_color_scheme = "white";
  }
  else
    m_realmcast_color_scheme = str;

  return(true);
}

//-------------------------------------------------------------
// Procedure: setInfoCastHeight()

bool InfoCastSettings::setInfoCastHeight(string str)
{
  if(!isNumber(str) && !strBegins(str, "delta:"))
    return(false);
  
  if(strBegins(str, "delta:")) {
    biteStringX(str, ':');
    if(!isNumber(str))
      return(false);
    double delta = atof(str.c_str());
    m_infocast_height += delta;
  }
  else {
    double dval = atof(str.c_str());
    m_infocast_height = snapToStep(dval, 5.0);
  }

  m_infocast_height = vclip(m_infocast_height, 30, 90);
  return(true);
}

//-------------------------------------------------------------
// Procedure: setInfoCastWidth()

bool InfoCastSettings::setInfoCastWidth(string str)
{
  if(!isNumber(str) && !strBegins(str, "delta:"))
    return(false);
  
  if(strBegins(str, "delta:")) {
    biteStringX(str, ':');
    if(!isNumber(str))
      return(false);
    double delta = atof(str.c_str());
    m_infocast_width += delta;
  }
  else {
    double dval = atof(str.c_str());
    m_infocast_width = snapToStep(dval, 5.0);
  }

  m_infocast_width = vclip(m_infocast_width, 20, 70);
  return(true);
}

//-------------------------------------------------------------
// Procedure: setContentMode()

bool InfoCastSettings::setContentMode(string str)
{
  str = tolower(str);

  if((str != "realmcast") && (str != "appcast") && (str != "toggle"))
    return(false);
  
  if(str == "toggle") {
    if(m_content_mode == "appcast")
      m_content_mode = "realmcast";
    else if(m_content_mode == "realmcast")
      m_content_mode = "appcast";
  }
  else 
    m_content_mode = str;

  return(true);
}

//-------------------------------------------------------------
// Procedure: setRefreshMode()

bool InfoCastSettings::setRefreshMode(string str)
{
  str = tolower(str);
  
  if((str != "streaming") && (str != "events") && (str != "paused"))
    return(false);
  
  m_refresh_mode = str;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setShowRealmCastSource()

bool InfoCastSettings::setShowRealmCastSource(string str)
{
  return(setBooleanOnString(m_show_rc_source, str));
}

//-------------------------------------------------------------
// Procedure: setShowRealmCastCommunity()

bool InfoCastSettings::setShowRealmCastCommunity(string str)
{
  return(setBooleanOnString(m_show_rc_community, str));
}

//-------------------------------------------------------------
// Procedure: setShowRealmCastSubs()

bool InfoCastSettings::setShowRealmCastSubs(string str)
{
  return(setBooleanOnString(m_show_rc_subscriptions, str));
}

//-------------------------------------------------------------
// Procedure: setShowRealmCastMasked()

bool InfoCastSettings::setShowRealmCastMasked(string str)
{
  return(setBooleanOnString(m_show_rc_masked, str));
}

//-------------------------------------------------------------
// Procedure: setWrapRealmCastContent()

bool InfoCastSettings::setWrapRealmCastContent(string str)
{
  return(setBooleanOnString(m_wrap_rc_content, str));
}

//-------------------------------------------------------------
// Procedure: setTruncRealmCastContent()

bool InfoCastSettings::setTruncRealmCastContent(string str)
{
  return(setBooleanOnString(m_trunc_rc_content, str));
}

//-------------------------------------------------------------
// Procedure: setRealmCastTimeFormatUTC()

bool InfoCastSettings::setRealmCastTimeFormatUTC(string str)
{
  return(setBooleanOnString(m_time_format_utc, str));
}


//-------------------------------------------------------------
// Procedure: fontChange()

string InfoCastSettings::fontChange(string old, string str)
{
  str = tolower(str);
  if((str != "xsmall") && (str != "small")  && (str != "medium") &&
     (str != "large")  && (str != "xlarge") && (str != "toggle") &&
     (str != "bigger") && (str != "smaller"))
    return("error");

  if((str == "toggle") || (str == "bigger")) {
    if(old == "xsmall")
      return("small");
    else if(old == "small")
      return("medium");
    else if(old == "medium")
      return("large");
    else if(old == "large")
      return("xlarge");
    else if((old == "xlarge") && (str == "toggle"))
      return("xsmall");
  }
  
  else if(str == "smaller") {
    if(old == "xlarge")
      return("large");
    else if(old == "large")
      return("medium");
    else if(old == "medium")
      return("small");
    else if(old == "small")
      return("xsmall");
  }

  else
    return(str);
  
  return(old);
}


//-------------------------------------------------------------
// Procedure: setInfoCastFontSize()

bool InfoCastSettings::setInfoCastFontSize(string str)
{
  string new_val = fontChange(m_infocast_font_size, str);
  if(new_val == "error")
    return(false);

  m_infocast_font_size = new_val;
  return(true);
}


//-------------------------------------------------------------
// Procedure: setNodesFontSize()

bool InfoCastSettings::setNodesFontSize(string str)
{
  string new_val = fontChange(m_nodes_font_size, str);
  if(new_val == "error")
    return(false);
  
  m_nodes_font_size = new_val;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setProcsFontSize()

bool InfoCastSettings::setProcsFontSize(string str)
{
  string new_val = fontChange(m_procs_font_size, str);
  if(new_val == "error")
    return(false);
  
  m_procs_font_size = new_val;
  return(true);
}


