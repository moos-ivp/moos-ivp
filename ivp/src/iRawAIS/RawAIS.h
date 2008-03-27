/*****************************************************************/
/*    NAME: Phong Tran  */
/*    ORGN: NAVSEA Newport RI 		                         */
/*    FILE: RawAIS.h                                             */
/*    DATE: Nov 19th 2007                                        */
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


#ifndef RAW_AIS_HEADER
#define RAW_AIS_HEADER

#include "MOOSLib.h"
#include "MOOSUtilityLib/MOOSGeodesy.h"
#include "AisIncludes.h"
#include "AisTargetDataIncludes.h"
#include <string>
#define FALSE 0
#define TRUE 1

class RawAIS : public CMOOSInstrument
{
public:
  RawAIS();
  virtual ~RawAIS() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
  bool handleIncomingReport();
  bool handleIncomingReport(const std::string&);
  void PrintSummaryData(AISTargetDataType*);
  bool SetupPort();
protected:
  CMOOSGeodesy m_Geodesy;
  double report_time;
  std::string ais_report_raw;
  int fd,c, res;
  struct termios oldtio,newtio;
  struct sigaction saio;           /* definition of signal action */
  char buf[255];
  std::string m_Port;
  int nLinuxBaudRate;
  double nav_x;
  double nav_y;
  double nav_speed;
  double nav_heading;
  double nav_depth;
  AISTargetDataType* AISData;
};

#endif






