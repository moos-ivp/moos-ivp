/*****************************************************************/
/*    NAME: Phong Tran                                           */
/*    ORGN: NAVSEA Newport RI                                    */
/*    FILE: ParserAIS.cpp                                        */
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

#include <MOOSLIB/MOOSLib.h>
#include <MOOSGenLib/MOOSGenLib.h>
#include <string>
//#include <cstring>
#include <list>
#include <iterator>
#include <iostream>
#include <ext/hash_map>
#include "MOOS_ASCM_PK.h"
#include "MBUtils.h"
#include "PkContacts.h"
#define HASH __gnu_cxx 
using namespace std;
using std::string;
//-----------------------------------------------------------------
// Procedure: Constructor

MOOS_ASCM_PK::MOOS_ASCM_PK()
{
  isWritten = true;
  isRead = false;
}
int num_sent =0;
int sent_num = 0;
int seq_num  = 0;
//HASH::hash_map<int, string> msg_list;
//-----------------------------------------------------------------
// Procedure: OnNewMail


bool MOOS_ASCM_PK::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;
  
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.m_sKey;
    string community = msg.m_sOriginatingCommunity;
    if(msg.m_sVal.size() == 0)
    {
       return true;
    }
    char* sdata = new char[(msg.m_sVal.size()+1)];
    strcpy(sdata, msg.m_sVal.c_str() );
    vector<string> svector = parseString(msg.m_sVal, ',');
    
    num_sent = atoi(svector[1].c_str());
    sent_num = atoi(svector[2].c_str());
    seq_num  = atoi(svector[3].c_str());
    double ddata = msg.m_dfVal;
    char   mtype = msg.m_cDataType;
    
    if(key == "AIS_REPORT" && !isRead) {
      //MOOSTrace("ON Received AIS_REPORT_RAW: %s \n", sdata);	
        
      bool ok = handleIncomingReport(sdata);
      if(!ok) 
	MOOSTrace("ParserAIS: Un-Parsed AIS-Report.\n");
    }
    else {
      MOOSTrace("TransponderAIS: Unknown msg [%s]\n",msg.m_sKey.c_str());
    }
  }
  return(true);
}

//-----------------------------------------------------------------
// Procedure: handleIncomingReport()

bool MOOS_ASCM_PK::handleIncomingReport(char* sdata)
{
 
  string sVal = sdata;
  double dfX  = 0;
  double dfY  = 0; 
  double dfLat = 0;
  double dfLon = 0;
  double dfSpeed     = 0;
  double dfHeading   = 0;
  double dfDepth     = 0;
  string vessel_name = "";
  string vessel_type = "";
  double aouMajor    = 0;
  double aouMinor    = 0;
  double aouOrient   = 0;
  double lTime       = 0;

  bool bTime = tokParse(sVal, "MOOS_TIME",   ',', '=', lTime);
  bool bVName = tokParse(sVal, "NAME",   ',', '=', vessel_name);
  bool bVType = tokParse(sVal, "TYPE",   ',', '=', vessel_type);
  bool bLat     = tokParse(sVal, "LAT",      ',', '=', dfLat);
  bool bLon     = tokParse(sVal, "LON",      ',', '=', dfLon);
  bool bX     = tokParse(sVal, "X",      ',', '=', dfX);
  bool bY     = tokParse(sVal, "Y",      ',', '=', dfY);
  bool bSpeed = tokParse(sVal, "SPD",    ',', '=', dfSpeed);
  bool bHeading = tokParse(sVal, "HDG",  ',', '=', dfHeading);
  bool bDepth = tokParse(sVal, "DEPTH",  ',', '=', dfDepth);
  bool bAouMajor = tokParse(sVal, "AOU_SEMI_MAJOR",    ',', '=', aouMajor);
  bool bAouMinor = tokParse(sVal, "AOU_SEMI_MINOR",  ',', '=', aouMinor);
  bool bAouOrient = tokParse(sVal, "AOU_ORIENT",  ',', '=', aouOrient);

  if(bVName)
  {
     AscmContact contact;
     char* lType = new char[(vessel_type.size()+1)];
     strcpy(lType, vessel_type.c_str() );
     contact.setType(lType);
     char* lId = new char[(vessel_name.size()+1)];
     strcpy(lId, vessel_name.c_str() );
     contact.setId(lId);
     contact.setTime( lTime );
     contact.setLatLon( dfLat, dfLon );
     contact.setXY( dfX, dfY );
     contact.setSpeed( dfSpeed );
     contact.setHeading( dfHeading );
     contact.setDepth( dfDepth );
     contact.setAou( aouMajor, aouMinor, aouOrient );
     int i = 0;
     isWritten = true;
     for(; i< ascmContactList.size(); i++)
     {
        if(vessel_name.compare(ascmContactList.at(i).getId()) == 0)
        {
	    ascmContactList.at(i) = contact;
            break;
        }
     }

     if(i == ascmContactList.size())
     {
	ascmContactList.push_back(contact);
     }
  }
  isWritten = false;
  
  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool MOOS_ASCM_PK::OnConnectToServer()
{
  
  m_Comms.Register("AIS_REPORT", 0);
  return(true);
}

//-----------------------------------------------------------------
// Procedure: Iterate()
//      Note: Happens AppTick times per second

bool MOOS_ASCM_PK::Iterate()
{
  //MOOSTrace("Total contacts %d \n", ascmContactList.size());
  sleep(2);
  
  if(!isWritten)
  {
    double lDeltaTime = 0;
    double moos_time = MOOSTime();
    isRead = true;
    std::string pk_sol;
    for(int i = 0; i< ascmContactList.size(); i++)
     {
	AscmContact *lPkedContact = NULL;

	lDeltaTime = moos_time - ascmContactList.at(i).getTime();
	lPkedContact = PkContacts::pkContact( &ascmContactList.at(i), lDeltaTime );
	composePK_SOL_Msg(lPkedContact);
        pk_sol.append(dynamicAIS);
        delete lPkedContact;
     }

     MOOSTrace("Notify PK_SOL: ========================================================================\n" + pk_sol);
    if(strlen(dynamicAIS) > 0)
    {
        m_Comms.Notify("PK_SOL", pk_sol);
    }
  }
  isRead = false;
  return(true);
}

void MOOS_ASCM_PK::composePK_SOL_Msg(AscmContact *pPkedContact)
{
	double X_local, Y_local;
	if(!m_Geodesy.LatLong2LocalGrid(pPkedContact->getLatitude(),
					pPkedContact->getLongitude(),
					Y_local, X_local))
	{
		cout<<"ERROR: m_Geodesy.LatLong2LocalGrid "<<endl;
	}	
        
	strcpy(dynamicAIS,"REPORT_TYPE = AIS_REPORT");
	strcat(dynamicAIS,",");
	strcat(dynamicAIS,"NAME="); 
        strcat(dynamicAIS,pPkedContact->getId());
        strcat(dynamicAIS,",");
        strcat(dynamicAIS,"TYPE=");
	strcat(dynamicAIS,pPkedContact->getType());
	strcat(dynamicAIS,",");
        strcat(dynamicAIS,"MOOS_TIME=");
        strcat(dynamicAIS,dstringCompact(doubleToString(pPkedContact->getTime())).c_str());
        strcat(dynamicAIS,",");
        strcat(dynamicAIS,"X="); 
        strcat(dynamicAIS,dstringCompact(doubleToString(X_local)).c_str());
        strcat(dynamicAIS,",");
   	strcat(dynamicAIS,"Y="); 
        strcat(dynamicAIS,dstringCompact(doubleToString(Y_local)).c_str());
        strcat(dynamicAIS,",");
        strcat(dynamicAIS,"LAT="); 
        strcat(dynamicAIS,dstringCompact(doubleToString(pPkedContact->getLatitude())).c_str());
        strcat(dynamicAIS,",");
	strcat(dynamicAIS,"LON="); 
        strcat(dynamicAIS,dstringCompact(doubleToString(pPkedContact->getLongitude())).c_str());
        strcat(dynamicAIS,",");
	strcat(dynamicAIS,"SPD="); 
        strcat(dynamicAIS,dstringCompact(doubleToString(pPkedContact->getSpeed())).c_str());
        strcat(dynamicAIS,",");
	strcat(dynamicAIS,"HDG="); 
        strcat(dynamicAIS,dstringCompact(doubleToString(pPkedContact->getHeading())).c_str());
        strcat(dynamicAIS,",");
	strcat(dynamicAIS,"DEPTH="); 
        strcat(dynamicAIS,dstringCompact(doubleToString(pPkedContact->getDepth())).c_str());
	strcat(dynamicAIS,",");
	strcat(dynamicAIS,"AOU_SEMI_MAJOR="); 
        strcat(dynamicAIS,dstringCompact(doubleToString(4.0)).c_str());
	strcat(dynamicAIS,",");
	strcat(dynamicAIS,"AOU_SEMI_MINOR="); 
        strcat(dynamicAIS,dstringCompact(doubleToString(2.0)).c_str());
	strcat(dynamicAIS,",");
	strcat(dynamicAIS,"AOU_ORIENT="); 
        strcat(dynamicAIS,dstringCompact(doubleToString(pPkedContact->getHeading())).c_str());
        strcat(dynamicAIS,";\n");
}
//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool MOOS_ASCM_PK::OnStartUp()
{
  CMOOSApp::OnStartUp();

  MOOSTrace("MOOS_ASCM_PK starting....\n");

  start_time = MOOSTime();
  
	//set up Geodetic conversions
	double dfLatOrigin;
	double dfLongOrigin;
  /**/
	string sVal;
	if(m_MissionReader.GetValue("LatOrigin",sVal))
	{
		dfLatOrigin = atof(sVal.c_str());
	}
	else
	{		
		MOOSTrace("LatOrigin not set - FAIL\n");

		return false;

	}

	if(m_MissionReader.GetValue("LongOrigin",sVal))
	{
		dfLongOrigin = atof(sVal.c_str());
	}
	else
	{
		MOOSTrace("LongOrigin not set - FAIL\n");

		return false;
	}

	if(!m_Geodesy.Initialise(dfLatOrigin,dfLongOrigin))
	{
		MOOSTrace("Geodesy Init failed - FAIL\n");

		return false;
	}
 /**/
  //printf("X:%2.2f Y:%2.2f\n", dfLatOrigin, dfLongOrigin);
  return(true);
}







