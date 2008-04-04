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

#ifndef ASCM_PK_HEADER
#define ASCM_PK_HEADER

#include "MOOSLib.h"
#include "MOOSUtilityLib/MOOSGeodesy.h"
#include "AscmContact.h"
#include <iostream>
#include <vector>

class MOOS_ASCM_PK : public CMOOSApp
{
public:
  MOOS_ASCM_PK();
  virtual ~MOOS_ASCM_PK() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
  bool handleIncomingReport(char*);
  void composePK_SOL_Msg(AscmContact *);
protected:
  CMOOSGeodesy m_Geodesy;
  double start_time;
  char dynamicAIS[256];
  bool isRead;
  bool isWritten;
  std::vector<AscmContact> ascmContactList;
};

#endif







