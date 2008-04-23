/*****************************************************************/
/*    NAME: Phong Tran                                           */
/*    ORGN: NAVSEA Newport RI                                    */
/*    FILE: RawAIS.cpp                                           */
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
#include <sstream>
#include <iomanip>
#include <assert.h>

#include <string>
#include <list>
#include <iterator>
#include <iostream>
#include "RawAIS.h"
#include "MBUtils.h"

#include "AisIncludes.h"
#include "AisTargetDataIncludes.h"
#include "OM1371Message.h"
#include "OM1371Message_1.h"

using namespace std;

 volatile int STOP=FALSE; 
        
void signal_handler_IO (int status);   /* definition of signal handler */
int wait_flag=TRUE;                    /* TRUE while no signal received */
string runMode;    
int test = 0;    
//-----------------------------------------------------------------
// Procedure: Constructor

RawAIS::RawAIS()
{
  ais_report_raw = "";
  AISTargetDataType* AISData = new AISTargetDataType;
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool RawAIS::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;


  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.m_sKey;
    string community = msg.m_sOriginatingCommunity;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;
    char   mtype = msg.m_cDataType;
    //MOOSTrace("OnNewMail " + sdata );
    if(key == "MarineSimToAIS"){
      bool ok = handleIncomingReport(sdata);
      if(!ok) 
	MOOSTrace("iRawAIS: Un-Parsed AIS-Report-Sim.\n");
    }
    else {
      MOOSTrace("TransponderAIS: Unknown msg [%s]\n",msg.m_sKey.c_str());
    }
  }
  return(true);
}

//-----------------------------------------------------------------
// Procedure: handleIncomingReport()
bool RawAIS::handleIncomingReport()
{

  AISData->static_data.mmsi_number = 123456789;
  //AISData->static_data.name,
  //AISData->static_data.destination,
 // m_Geodesy.LocalGrid2LatLong(nav_x, nav_y, AISData->dynamic_data.latitude, AISData->dynamic_data.longitude);
  AISData->dynamic_data.latitude = 0.9;
  AISData->dynamic_data.longitude = 1.8;
  AISData->dynamic_data.course_over_ground = 7;//nav_heading;
  AISData->dynamic_data.speed_over_ground = 10;//nav_speed;
  AISData->dynamic_data.depth = 0;//nav_depth;
 
  OM1371Message_1 message_1(AISData);
  message_1.FormatNMEAString(0, false);
  int num_messages = message_1.GetNumberOfNMEAStrings();

  char nmea_message[AIS_MAX_NMEA_STRING_SIZE];	// local storage
  for(int i = 0; i < num_messages; i++)
  {
	// Get message into local space
	message_1.GetNMEAString(nmea_message, AIS_MAX_NMEA_STRING_SIZE, i);

        MOOSTrace("handleIncomingReport() Sending AIS Raw Message: %s\n", nmea_message);
        m_Comms.Notify("AIS_REPORT_RAW", nmea_message);
  }

  return(true);
}
bool RawAIS::handleIncomingReport(const std::string& sdata)
{
  int    mmsi;          bool vmmsi_set   = false;
  double nav_x_val;     bool nav_x_set   = false;
  double nav_y_val;     bool nav_y_set   = false;
  double nav_spd_val;   bool nav_spd_set = false;
  double nav_hdg_val;   bool nav_hdg_set = false;
  double nav_dep_val;   bool nav_dep_set = false;
  vector<string> svector = parseString(sdata, ',');
  int vsize = svector.size();
  for(int i=0; i<vsize; i++) {
    vector<string> svector2 = parseString(svector[i], '=');
    if(svector2.size() == 2) {
      string left = stripBlankEnds(svector2[0]);
      string right = stripBlankEnds(svector2[1]);
      
      if(left=="MMSI") {
	mmsi = atoi(right.c_str());
	vmmsi_set = true;
      }
      if(left == "X") {
	nav_x_val = atof(right.c_str());
	nav_x_set = true;
      }
      if(left == "Y") {
	nav_y_val = atof(right.c_str());
	nav_y_set = true;
      }
      if(left == "SPEED") {
	nav_spd_val = atof(right.c_str());
	nav_spd_set = true;
      }
      if(left == "HEADING") {
	nav_hdg_val = atof(right.c_str());
	nav_hdg_set = true;
      }
      if(left == "DEPTH") {
	nav_dep_val = atof(right.c_str());
	nav_dep_set = true;
      }
    }
  }

  if(!vmmsi_set || !nav_x_set || !nav_y_set ||
     !nav_spd_set || !nav_hdg_set || !nav_dep_set)
    return(false);

  
  AISData->static_data.mmsi_number = mmsi;
  m_Geodesy.LocalGrid2LatLong(nav_x_val, nav_y_val, AISData->dynamic_data.latitude, AISData->dynamic_data.longitude);
   
  AISData->dynamic_data.course_over_ground = nav_hdg_val;
  AISData->dynamic_data.speed_over_ground = nav_spd_val;
  AISData->dynamic_data.depth = nav_dep_val;
  
  PrintSummaryData(AISData);
  OM1371Message_1 message_1(AISData);
  message_1.FormatNMEAString(0, false);
  int num_messages = message_1.GetNumberOfNMEAStrings();
  
  char nmea_message[AIS_MAX_NMEA_STRING_SIZE];	// local storage
  for(int i = 0; i < num_messages; i++)
  {
	// Get message into local space
	message_1.GetNMEAString(nmea_message, AIS_MAX_NMEA_STRING_SIZE, i);

        MOOSTrace("handleIncomingReport(sdata) Sending AIS Raw Message: %s \n", nmea_message);
        m_Comms.Notify("AIS_REPORT_RAW", nmea_message);
  }
  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool RawAIS::OnConnectToServer()
{
  
  m_Comms.Register("MarineSimToAIS", 0);
 
  return(true);
}

//-----------------------------------------------------------------
// Procedure: Iterate()
//      Note: Happens AppTick times per second

bool RawAIS::Iterate()
{
  
  if(runMode == "true")
   {
     cout<<"Waiting for serial input....\n";
     usleep(100000);
     //if (wait_flag==FALSE) 
     //{ 
       res = read(fd,buf,255);
       buf[res]=0;
       printf("receive: %s size %d\n", buf, res);
       if (res < 1)
       { 
           //STOP=TRUE; /* stop loop if only a CR was input */
          tcsetattr(fd,TCSANOW,&oldtio);
       }
       wait_flag = TRUE;      /* wait for new input */
       if(res > 1){
       //printf("receive: %s size %d\n", buf, res);
       m_Comms.Notify("AIS_REPORT_RAW", buf);
       }
     //}
  }
  else{
        if(test<5)
	//handleIncomingReport();
     test++;
  }
  
  return(true);
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool RawAIS::OnStartUp()
{
  CMOOSInstrument::OnStartUp();
  MOOSTrace("iRawAIS starting....\n");
/////////// Sim incoming data  ////////////////
double dfLatOrigin;
double dfLongOrigin;

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


///////////// Serial incoming data ////////////////
   m_MissionReader.GetValue("LiveMode",runMode);
   if(runMode == "true")
   {
      MOOSTrace("iRawAIS running at Live Mode....\n");
      if(!m_MissionReader.GetValue("PORT",m_Port))
      {
	m_Port = "/dev/ttyS0";
  
      }
      string baudrate;
      if(m_MissionReader.GetValue("BAUDRATE",baudrate))
      {
       switch(atoi(baudrate.c_str()) )
       {
        case 115200:    nLinuxBaudRate = B115200;       break;
        case 38400: 	nLinuxBaudRate = B38400;  	break;
        case 19200: 	nLinuxBaudRate = B19200;  	break;
        case 9600:  	nLinuxBaudRate = B9600;   	break;
        case 4800:	    nLinuxBaudRate = B4800;  	break;
        case 2400:	    nLinuxBaudRate = B2400;  	break;
        case 1200:	    nLinuxBaudRate = B1200;  	break;
        case 600:	    nLinuxBaudRate = B600;  	break;
        case 300:	    nLinuxBaudRate = B300;  	break;
        default :   
	        printf("unsupported baud rate\n");
	        return false;
	        break;
      
       }
      }
      else{
        nLinuxBaudRate = B38400;
      }
  //try to open 
     if(!SetupPort())
     {
       return false;
     }
  }
  MOOSTrace("iRawAIS started\n");
  return(true);
}

bool RawAIS::SetupPort()
{
/* open the device to be non-blocking (read will return immediatly) */
        fd = open(m_Port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd <0) 
	{    perror(m_Port.c_str()); 
            return(FALSE); 
        }
        
        /* install the signal handler before making the device asynchronous */
        saio.sa_handler = signal_handler_IO;
        //saio.sa_mask = 0;
        saio.sa_flags = 0;
        saio.sa_restorer = NULL;
        sigaction(SIGIO,&saio,NULL);
          
        /* allow the process to receive SIGIO */
        fcntl(fd, F_SETOWN, getpid());
        /* Make the file descriptor asynchronous (the manual page says only 
           O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
        fcntl(fd, F_SETFL, FASYNC);
        
        tcgetattr(fd,&oldtio); /* save current port settings */
        /* set new port settings for canonical input processing */
        newtio.c_cflag = nLinuxBaudRate | CRTSCTS | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR | ICRNL;
        newtio.c_oflag = 0;
        newtio.c_lflag = ICANON;
        newtio.c_cc[VMIN]=1;
        newtio.c_cc[VTIME]=0;
        tcflush(fd, TCIFLUSH);
        tcsetattr(fd,TCSANOW,&newtio);
   return TRUE;
}

  
      /***************************************************************************
      * signal handler. sets wait_flag to FALSE, to indicate above loop that     *
      * characters have been received.                                           *
      ***************************************************************************/
        
      void signal_handler_IO (int status)
      {
         printf("received SIGIO signal.\n");
         wait_flag = FALSE;
      }


void RawAIS::PrintSummaryData(AISTargetDataType* AISData)
{
	// Override in derived classes.  Shouldn't call this version of the function
	// but just in case... Print out MMSI
        cout<<"ParserAIS::PrintSummaryData"<<endl;
	printf("MMSI: %ld Lat:%7.2f Lon:%7.2f Crs:%7.2f Spd:%7.2f \n\n",
	AISData->static_data.mmsi_number,
	AISData->dynamic_data.latitude, 
	AISData->dynamic_data.longitude,
	AISData->dynamic_data.course_over_ground,
	AISData->dynamic_data.speed_over_ground
	);
}
