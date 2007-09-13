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

protected:
  double start_time;
  double nav_x;
  double nav_y;
  double nav_heading;
  double nav_speed;
  double nav_depth;
  std::string vessel_name;
  std::string vessel_type;
  
  // tes 9-12-07
  bool parseNaFCon;
  // array of bools stating whether to publish for a given ID
  bool naFConPublishForID[32];
  // for lat long conversion
  CMOOSGeodesy m_Geodesy;

};

#endif







