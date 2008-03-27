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
#include <cstring>
#include <list>
#include <iterator>
#include <iostream>
#include <ext/hash_map>
#include "ParserAIS.h"
#include "MBUtils.h"

#define HASH __gnu_cxx 
using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

ParserAIS::ParserAIS()
{
  sAISData = NULL;
  dAISData = NULL;
  msg_1 = NULL;
  msg_5 = NULL;
}
int num_sent =0;
int sent_num = 0;
int seq_num  = 0;
//HASH::hash_map<int, string> msg_list;
//-----------------------------------------------------------------
// Procedure: OnNewMail


bool ParserAIS::OnNewMail(MOOSMSG_LIST &NewMail)
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
    
    if(key == "AIS_REPORT_RAW" && strlen(sdata) > 0) {
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

bool ParserAIS::handleIncomingReport(char* sdata)
{
  //MOOSTrace("ON handleIncomingReport: %s \n", sdata);
  short msg_type = ais_object.GetMessageType(sdata);
  //MOOSTrace("ParserAIS: Parsing AIS_Report_Raw: message Type: %d \n", msg_type);
  
  double X_local, Y_local;
  
  if(msg_type == 1 || msg_type == 2 || msg_type == 3)
  {
      if(msg_1 == NULL && dAISData == NULL)
      {
          
	  dAISData = new AISTargetDataType;
          msg_1 = new OM1371Message_1(dAISData);
          ;
          if(msg_1->SetNMEAString(sdata, strlen(sdata), 0))
  	  {
            //MOOSTrace("msg_1->SetNMEAString is OK\n");
            if(msg_1->DecodeNMEAString())
            {
            /**/
            if(!m_Geodesy.LatLong2LocalGrid(dAISData->dynamic_data.latitude,
					   dAISData->dynamic_data.longitude,
					   Y_local, X_local))
	    {
		cout<<"ERROR: m_Geodesy.LatLong2LocalGrid "<<endl;
	    }
            /**/
            //PrintSummaryData(dAISData);
	    else
	    {
                /**/
                double moos_time = MOOSTime();
                string utc_time = dstringCompact(doubleToString(moos_time,3));
                strcpy(dynamicAIS,"NAME="); 
                strcat(dynamicAIS,dstringCompact(intToString(dAISData->static_data.mmsi_number)).c_str());
                strcat(dynamicAIS,",");
                strcat(dynamicAIS,"TYPE=Ship,");
                strcat(dynamicAIS,"MOOS_TIME=");
                strcat(dynamicAIS,dstringCompact(doubleToString(moos_time)).c_str());
                strcat(dynamicAIS,",");
                strcat(dynamicAIS,"UTC_TIME=");
                strcat(dynamicAIS,utc_time.c_str());
                strcat(dynamicAIS,",");
                strcat(dynamicAIS,"X="); 
                strcat(dynamicAIS,dstringCompact(doubleToString(X_local)).c_str());
                strcat(dynamicAIS,",");
   		strcat(dynamicAIS,"Y="); 
                strcat(dynamicAIS,dstringCompact(doubleToString(Y_local)).c_str());
                strcat(dynamicAIS,",");
                strcat(dynamicAIS,"LAT="); 
                strcat(dynamicAIS,dstringCompact(doubleToString(dAISData->dynamic_data.latitude)).c_str());
                strcat(dynamicAIS,",");
		strcat(dynamicAIS,"LON="); 
                strcat(dynamicAIS,dstringCompact(doubleToString(dAISData->dynamic_data.longitude)).c_str());
                strcat(dynamicAIS,",");
		strcat(dynamicAIS,"SPD="); 
                strcat(dynamicAIS,dstringCompact(doubleToString(dAISData->dynamic_data.speed_over_ground*0.5144)).c_str());
                strcat(dynamicAIS,",");
		strcat(dynamicAIS,"HDG="); 
                strcat(dynamicAIS,dstringCompact(doubleToString(dAISData->dynamic_data.course_over_ground)).c_str());
                strcat(dynamicAIS,",");
		strcat(dynamicAIS,"DEPTH="); 
                strcat(dynamicAIS,dstringCompact(doubleToString(dAISData->dynamic_data.depth)).c_str());
                /**/
	    }
	  
            }
            MOOSTrace("Notify AIS_REPORT: %s\n",dynamicAIS);
            if(strlen(dynamicAIS) > 0)
            {
   	        m_Comms.Notify("AIS_REPORT", dynamicAIS);
            }
	  }
         memset(dynamicAIS, 0, sizeof dynamicAIS);
         delete dAISData;
         dAISData = NULL;
         delete msg_1;
         msg_1 = NULL;
       }
  }
  else if(msg_type == 5)
  {

        if(num_sent == 2 && sent_num ==1)
	{
            if(sAISData == NULL)
            {
		sAISData = new AISTargetDataType;
            }
            
	    msg_5 = new OM1371Message_5(sAISData);
	    msg_5->SetNMEAString(sdata, strlen(sdata), 0);	
	}
	
  }
  else if(msg_type == -2 && num_sent == 2 && sent_num ==2)
  {
	if(msg_5 !=NULL && sAISData != NULL)
        {
	   msg_5->SetNMEAString(sdata, strlen(sdata), 1);
           msg_5->DecodeNMEAString();
           
  
          // if(sAISData->static_data.name != NULL || sAISData->static_data.name != "")
          // 	strcpy(staticAIS, sAISData->static_data.name);
           strcpy(staticAIS,"MMSI="); 
           strcat(staticAIS,dstringCompact(intToString(sAISData->static_data.mmsi_number)).c_str());
           strcat(staticAIS,",");
           strcat(staticAIS,"NAME=");
           strcat(staticAIS,sAISData->static_data.name);
           
           MOOSTrace("Notify AIS_STATIC_REPORT_LOCAL: %s\n",staticAIS);
           m_Comms.Notify("AIS_STATIC_REPORT_LOCAL", staticAIS);
         
           delete sAISData;
           sAISData = NULL;
           delete msg_5;
           msg_5 = NULL;
           memset(staticAIS, 0, sizeof staticAIS);
        }
        
  }
  else
  {
       MOOSTrace("Error parsing AIS data message Type: %d \n", msg_type);
  }

  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool ParserAIS::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
  m_Comms.Register("AIS_REPORT_RAW", 0);
  return(true);
}

//-----------------------------------------------------------------
// Procedure: Iterate()
//      Note: Happens AppTick times per second

bool ParserAIS::Iterate()
{

  return(true);
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool ParserAIS::OnStartUp()
{
  CMOOSApp::OnStartUp();

  MOOSTrace("iParserAIS starting....\n");

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

void ParserAIS::PrintSummaryData(AISTargetDataType* AISData)
{
	// Override in derived classes.  Shouldn't call this version of the function
	// but just in case... Print out MMSI
        cout<<"ParserAIS::PrintSummaryData"<<endl;
	printf("MMSI: %ld Name: %s Dest: %s  Lat:%7.2f Lon:%7.2f Crs:%7.2f Spd:%7.2f \n\n",
	AISData->static_data.mmsi_number,
        AISData->static_data.name,
        AISData->static_data.destination,
	AISData->dynamic_data.latitude, 
	AISData->dynamic_data.longitude,
	AISData->dynamic_data.course_over_ground,
	AISData->dynamic_data.speed_over_ground
	);
}






