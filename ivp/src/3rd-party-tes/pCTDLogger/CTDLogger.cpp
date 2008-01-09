#include "CTDLogger.h"
#include "MBUtils.h"
#include <math.h>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

/**************************************************************************
 *   CTDLogger::CTDLogger()                                                *
 *                                                                         *
 *   Constructor for the CTDLogger Object                                  *
 ***************************************************************************/
CTDLogger::CTDLogger()
{
  
}

/**************************************************************************
 *   CTDLogger::~CTDLogger()                                               *
 *                                                                         *
 *   Destructor for the CTDLogger Object                                   *
 ***************************************************************************/

CTDLogger::~CTDLogger()
{
}

bool CTDLogger::OnStartUp()
{

    if(!m_MissionReader.GetValue("LogPath", logPath)) {
        MOOSTrace("pCTDLogger: Warning: no log path, using '.'\n");
        logPath = "./";
    }
  
    return(true);
}

bool CTDLogger::OnConnectToServer()
{  
    m_Comms.Register("CTD_TCPSV",0);
    m_Comms.Register("GPS_LATITUDE",0);
    m_Comms.Register("GPS_LONGITUDE",0);
    m_Comms.Register("CTD_LOG",0);
    m_Comms.Register("SENSOR_MISSION",0);
    return(true);
}


bool CTDLogger::Iterate()
{
  
    return(true);
}


//-------------------------------------------------------------
// Procedure: OnNewMail

bool CTDLogger::OnNewMail(MOOSMSG_LIST &NewMail)

{  
    MOOSMSG_LIST::iterator p;

    for(p = NewMail.begin(); p != NewMail.end(); p++) {
        CMOOSMsg &Msg = *p;

        if(Msg.m_sKey == "GPS_LATITUDE")
        {       
            Current_Lat = Msg.m_dfVal;        
        }      
        else if(Msg.m_sKey == "GPS_LONGITUDE")
        {       
            Current_Lon = Msg.m_dfVal;        
        }  
        else if(Msg.m_sKey == "SENSOR_MISSION")
        {
            m_mission_number = int(Msg.m_dfVal);
        }

        else if(Msg.m_sKey == "CTD_LOG")
        {       
            string mess = Msg.m_sVal;
            if(MOOSStrCmp(mess, "TRUE"))
            {
                //close any cuurently open files
                if (fout.is_open())
                    fout.close();
                //open a new file
                string log_file = "log_"+ MOOSGetTimeStampString() + "_m"+ intToString(m_mission_number)  + ".ctdlog";
                m_file_path = logPath + log_file;

                fout.open(m_file_path.c_str());
                if (!fout.is_open())
                {
                    logPath = ".";
                    m_file_path = logPath + log_file;
                    fout.open(m_file_path.c_str());
                }
                // print header information
                // fout << "# time, depth, temperature, conductivity, pressure, salinity, velocity, latitude, longitude \n"; 
            }
            else
            {
                //close any currently open file
                if (fout.is_open())
                {
                    fout.close();
                    m_Comms.Notify("WEBSITE_PUT", m_file_path);
                }
            }
           
        } 
        else if(Msg.m_sKey == "CTD_TCPSV")
        {       
            //if a file is open, write string to file    
            if (fout.is_open())
                fout << Msg.m_sVal << "," << setprecision(9) << Current_Lat << "," << setprecision(9) << Current_Lon << "\n";
        }
        else
            MOOSTrace("Unrecognized command: [%s]\n",Msg.m_sKey.c_str());
    }
  
    return(true);
}
