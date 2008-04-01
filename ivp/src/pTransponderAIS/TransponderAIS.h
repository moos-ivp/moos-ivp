/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TransponderAIS.cpp                                   */
/*    DATE: Feb 9th 2006                                         */
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

#ifndef TRANSPONDER_AIS_HEADER
#define TRANSPONDER_AIS_HEADER

#include <iostream>
#include <string>
#include "MOOSLib.h"
#include <vector>

// tes 9-12-07 for CMOOSGeodesy class
#include "MOOSGeodesy.h"

class TransponderAIS : public CMOOSApp
{
public:
  TransponderAIS();
  virtual ~TransponderAIS() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
  bool handleIncomingAISReport(const std::string&);
  bool handleIncomingNaFConMessage(const std::string&);
  void updateContactList(std::string, double, double, double, double, double, double);
  bool prevContactInfo(std::string, double*, double*, double*, double*, double*, double*);
  void postContactList();
  std::string assembleAIS(std::string,std::string,std::string,std::string,\
			  std::string,std::string,std::string,std::string,\
			  std::string,std::string,std::string);

protected:
  double      m_db_uptime;
  double      m_start_time;
  double      m_nav_x;
  double      m_nav_y;
  double      m_nav_heading;
  double      m_nav_speed;
  double      m_nav_depth;
  std::string m_vessel_name;
  std::string m_vessel_type;

  std::vector<std::string> m_contact_list;
  std::vector<double>      m_contact_time;
  std::vector<double>      m_contact_utc;
  std::vector<double>      m_contact_x;
  std::vector<double>      m_contact_y;
  std::vector<double>      m_contact_spd;
  std::vector<double>      m_contact_hdg;
  std::vector<double>      m_contact_dep;
  
  // tes 9-12-07
  bool m_parseNaFCon;
  // vector to specify whether to publish for a given ID
  std::vector<bool> naFConPublishForID;
  // for lat long conversion
  CMOOSGeodesy m_geodesy;

  // strings to hold information about NaFCon IDs
  std::vector<std::string> m_nafcon_name;
  std::vector<std::string> m_nafcon_type;
  double  m_blackout_interval;
  double  m_blackout_baseval;
  double  m_blackout_variance;
  double  m_last_post_time;

  std::string interval_history;

  };

#endif








