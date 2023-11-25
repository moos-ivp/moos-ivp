/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: InfoCastSettings.h                                   */
/*    DATE: Dec 5th, 2020                                        */
/*    NOTE: Migrated from VPlug_AppCastSettings.h                */
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

#ifndef INFOCAST_SETTINGS_HEADER
#define INFOCAST_SETTINGS_HEADER

#include <vector>
#include <map>
#include <string>
#include "ColorPack.h"

class InfoCastSettings {
public:
  InfoCastSettings();
  virtual ~InfoCastSettings() {}

 public: 
  bool setInfoCastViewable(std::string str)
  {return(setBooleanOnString(m_infocast_viewable, str));}
  
  bool setFullScreen(std::string str)
  {return(setBooleanOnString(m_full_screen, str));}

  bool setContentMode(std::string);
  bool setRefreshMode(std::string);

  bool setInfoCastFontSize(std::string);
  bool setNodesFontSize(std::string);
  bool setProcsFontSize(std::string);

  bool setInfoCastHeight(std::string);
  bool setInfoCastWidth(std::string);

  bool setAppCastColorScheme(std::string);
  bool setRealmCastColorScheme(std::string);

  bool setShowRealmCastSource(std::string);
  bool setShowRealmCastCommunity(std::string);
  bool setShowRealmCastSubs(std::string);
  bool setShowRealmCastMasked(std::string);
  bool setWrapRealmCastContent(std::string);
  bool setTruncRealmCastContent(std::string);
  bool setRealmCastTimeFormatUTC(std::string);
  
 public: // Getters
  
  bool getInfoCastViewable() const {return(m_infocast_viewable);}
  bool getFullScreen() const       {return(m_full_screen);}

  std::string  getContentMode() const {return(m_content_mode);}
  std::string  getRefreshMode() const {return(m_refresh_mode);}

  std::string  getInfoCastFontSize() const {return(m_infocast_font_size);}
  std::string  getNodesFontSize() const    {return(m_nodes_font_size);}
  std::string  getProcsFontSize() const    {return(m_procs_font_size);}

  std::string getAppCastColorScheme() const   {return(m_appcast_color_scheme);}
  std::string getRealmCastColorScheme() const {return(m_realmcast_color_scheme);}

  double getInfoCastHeight() const  {return(m_infocast_height);}
  double getInfoCastWidth() const   {return(m_infocast_width);}

  bool getShowRealmCastSource() const    {return(m_show_rc_source);}
  bool getShowRealmCastCommunity() const {return(m_show_rc_community);}
  bool getShowRealmCastSubs() const      {return(m_show_rc_subscriptions);}
  bool getShowRealmCastMasked() const    {return(m_show_rc_masked);}
  bool getWrapRealmCastContent() const   {return(m_wrap_rc_content);}
  bool getTruncRealmCastContent() const  {return(m_trunc_rc_content);}
  bool getRealmCastTimeFormatUTC() const {return(m_time_format_utc);}
  
 protected:
  std::string fontChange(std::string, std::string);
  
protected:
  std::string m_content_mode;      // appcast or realmcast
  std::string m_refresh_mode;
  bool        m_infocast_viewable;
  bool        m_full_screen;


  std::string m_infocast_font_size;
  std::string m_procs_font_size;
  std::string m_nodes_font_size;
  
  double m_infocast_height;
  double m_infocast_width;

  std::string m_appcast_color_scheme;
  std::string m_realmcast_color_scheme;

  // Specific to realmcast content
  bool  m_show_rc_source;
  bool  m_show_rc_community;
  bool  m_show_rc_subscriptions;
  bool  m_show_rc_masked;
  bool  m_wrap_rc_content;
  bool  m_trunc_rc_content;
  bool  m_time_format_utc;
};

#endif

