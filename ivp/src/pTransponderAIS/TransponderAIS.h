/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    NaFCon Functionality: Toby Schneider tes@mit.edu           */
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

#include <string>
#include "MOOSLib.h"

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
  bool handleIncomingCSReport(const std::string&);
  bool handleIncomingNaFConMessage(const std::string&);
  void updateContactList(std::string);
  void postContactList();


protected:
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
  
  // tes 9-12-07
  bool m_parseNaFCon;
  // 32 bits to specify whether to publish for a given ID
  unsigned long int naFConPublishForID;
  // for lat long conversion
  CMOOSGeodesy m_Geodesy;


  double  m_blackout_interval;
  double  m_last_post_time;


};

#endif







