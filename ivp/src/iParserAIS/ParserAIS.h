/*****************************************************************/
/*    NAME: Phong Tran                                           */
/*    ORGN: NAVSEA Newport RI                                    */
/*    FILE: ParserAIS.h                                          */
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

#ifndef PARSER_AIS_HEADER
#define PARSER_AIS_HEADER

#include "MOOSLib.h"
#include "MOOSUtilityLib/MOOSGeodesy.h"
#include "OM1371Message.h"
#include "OM1371Message_1.h"
#include "OM1371Message_5.h"
 

class ParserAIS : public CMOOSApp
{
public:
  ParserAIS();
  virtual ~ParserAIS() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
  bool handleIncomingReport(char*);
  void PrintSummaryData(AISTargetDataType*);
protected:
  CMOOSGeodesy m_Geodesy;
  double start_time;
  char staticAIS[256];
  char dynamicAIS[256];
  OM1371Message ais_object;
  AISTargetDataType*  sAISData;
  AISTargetDataType*  dAISData;
  OM1371Message_1* msg_1;
  OM1371Message_5* msg_5;
};

#endif







