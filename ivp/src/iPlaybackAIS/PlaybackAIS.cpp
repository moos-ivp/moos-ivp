/*****************************************************************/
/*    NAME: Phong Tran                                           */
/*    ORGN: NAVSEA Newport RI                                    */
/*    FILE: PlaybackAIS.cpp                                           */
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

#include <string>
#include <vector>
#include <list>
#include <iterator>
#include <iostream>
#include "PlaybackAIS.h"
#include "FileBuffer.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

PlaybackAIS::PlaybackAIS()
{
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool PlaybackAIS::OnNewMail(MOOSMSG_LIST &NewMail)
{
 
  return(true);
}

//-----------------------------------------------------------------
// Procedure: handleIncomingReport()


//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool PlaybackAIS::OnConnectToServer()
{
  return(true);
}

//-----------------------------------------------------------------
// Procedure: Iterate()
//      Note: Happens AppTick times per second

bool PlaybackAIS::Iterate()
{
  //report_time	
  //string timeinfo = dstringCompact(doubleToString(MOOSTime()));
  //string ais_raw_message = timeinfo + "  " + ais_report_raw;
  if(idx < ais_Reports.size() )
  {
   	string ais_raw_message = ais_Reports.at(idx);
        if(ais_raw_message.size() > 0)
        {
  	   MOOSTrace("Notifying: " + ais_raw_message +"\n");
  	   m_Comms.Notify("AIS_REPORT_RAW", ais_raw_message);
           idx++;
        sleep(5);
        }
  }
  return(true);
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool PlaybackAIS::OnStartUp()
{
  CMOOSApp::OnStartUp();

  MOOSTrace("iPlaybackAIS reading AIS file....\n");
  string sTmp;// = "aisplayback.data";
  if(m_MissionReader.GetConfigurationParam("FILE",sTmp))
  {
	if(!sTmp.empty())
        {
	   ais_Reports = fileBuffer(sTmp,0);
	   idx = 0;
	}
        else
	{
	    MOOSTrace("iPlaybackAIS fail to open AIS file....\n");
	}
  }
  else
  {
      MOOSTrace("GetConfigurationParam fail to get AIS file....\n");
  }
  
  MOOSTrace("iPlaybackAIS starting....\n");

  return(true);
}








