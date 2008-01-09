// t. schneider tes@mit.edu 10.09.07
// laboratory for autonomous marine sensing systems
// massachusetts institute of technology 
// 
// this is pSoundSpeed.cpp 
//
// see the readme file within this directory for information
// pertaining to usage and purpose of this script.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this software.  If not, see <http://www.gnu.org/licenses/>.

#include "pSoundSpeed.h"

using namespace std;

CpSoundSpeed::CpSoundSpeed()
{
    m_do_ping = false;
    m_last_ping = 0;
}
CpSoundSpeed::~CpSoundSpeed()
{
}

// OnNewMail: called when new mail (previously registered for)
// has arrived.
bool CpSoundSpeed::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::reverse_iterator p;
  
    for(p = NewMail.rbegin(); p != NewMail.rend(); p++)
    {
        CMOOSMsg &msg = *p;
      
        string key   = msg.m_sKey;
        string rMsg = msg.m_sVal;
        double ddata = msg.m_dfVal;
        char   mtype = msg.m_cDataType;
      
        if(MOOSStrCmp(key, "NAFCON_MESSAGES"))
        {
          
            string messageType;
            MOOSValFromString(messageType, rMsg, "MessageType");
          
            if(MOOSStrCmp(messageType, "SENSOR_PROSECUTE"))
            {
                string sourceID;
                MOOSValFromString(sourceID, rMsg, "DestinationPlatformId");
                if(MOOSStrCmp(sourceID, "9"))
                {
                    double navLat;
                    double navLong;
                  
                    if(!MOOSValFromString(navLong, rMsg, "TargetLongitude"))
                        return MOOSFail("No TargetLongitude\n");
                  
                    if (!MOOSValFromString(navLat, rMsg, "TargetLatitude"))
                        return MOOSFail("No TargetLatitude\n");
                  
                  
                    m_Geodesy.LatLong2LocalGrid(navLat, navLong, m_start_y, m_start_x);
                  
                    cout << "start_x: " << doubleToString(m_start_x) << "start_y: " << doubleToString(m_start_y) << "\n";
                  
                    string bobby_go = "points=";
                    bobby_go += doubleToString(m_start_x+500);
                    bobby_go += ",";
                    bobby_go += doubleToString(m_start_y);
                  
                    m_Comms.Notify("SOUNDSPEED_UPDATES_BOBBY", bobby_go);
                    m_Comms.Notify("PROSECUTE_STATE_BOBBY", "PROSECUTE");
                  
                    string dee_go = "points=";
                    dee_go += doubleToString(m_start_x-500);
                    dee_go += ",";
                    dee_go += doubleToString(m_start_y);

                    m_Comms.Notify("SOUNDSPEED_UPDATES", dee_go);
                  
                    cout << "bobby_go: \n" << bobby_go << "\n" << "dee_go: \n" << dee_go << "\n";
                  
                    //create elanor website file
                    ofstream fout;
                  
                    fout.open("PN07_CTD_command.txt");
                  
                    fout << "SEQUENCE_NUM=1" << "\n";
                    fout << "MISSION_NUM=4" << "\n";
                    fout << "START_LAT=" << doubleToString(navLat) << "\n";
                    fout << "START_LON=" << doubleToString(navLong) << "\n";
                    fout << "CTD_DEPTH=12\n";
                    fout << "MISSION_DURATION=1800\n";

                    fout.close();
                  
                    m_Comms.Notify("WEBSITE_PUT", "PN07_CTD_command.txt");
                  
                }
            }
        }
        else if(MOOSStrCmp(key, "MODEM_PING"))
        {
            if(MOOSStrCmp(rMsg, "TRUE"))
            {
                m_do_ping = true;
                cout << "modem ping flag set true \n";
            }
            else
            {
                m_do_ping = false;
                cout << "modem ping flag set false \n";
            }
        }  
    }
    return true;
}


// OnConnectToServer: called when connection to the MOOSDB is
// complete
bool CpSoundSpeed::OnConnectToServer()
{
    // register for variables here
    registerVariables();
    return true;
}

// Iterate: called AppTick times per second
bool CpSoundSpeed::Iterate()
{ 
    double moos_time = MOOSTime();
    if(m_do_ping && ((m_last_ping + 15) < moos_time))
    {
        cout << "moostime: " << moos_time << " m_last_ping: " << m_last_ping << " \n";
        doPing();
    }
    return true;
}

// OnStartUp: called when program is run
bool CpSoundSpeed::OnStartUp()
{       
  
    double latOrigin, longOrigin;
    if (!m_MissionReader.GetValue("LatOrigin", latOrigin))
        return MOOSFail("LatOrigin not set in *.moos file.\n");
      
    if (!m_MissionReader.GetValue("LongOrigin", longOrigin))
        return MOOSFail("LongOrigin not set in *.moos file\n");
  
    // initialize m_Geodesy
    if (!m_Geodesy.Initialise(latOrigin, longOrigin))
        return MOOSFail("Geodesy init failed.\n");

    cout << "latorigin: " << doubleToString(latOrigin) << "longorigin: " << doubleToString(longOrigin) << "\n";

    // register for variables here
    registerVariables();
    return true;
}


//-----------------------------------------------------------------
// Procedure: registerVariables()

void CpSoundSpeed::registerVariables()
{
    m_Comms.Register("NAFCON_MESSAGES", 0);
    m_Comms.Register("MODEM_PING", 0);
    return;
}



//-----------------------------------------------------------------
// Procedure: doping

void CpSoundSpeed::doPing()
{
    // ping bobby
    m_Comms.Notify("MICROMODEM_COMMAND", "Command=PING,Dest=2");
    cout << "pinging bobby (id: 2) \n";
    m_last_ping = MOOSTime();

    return;
}


