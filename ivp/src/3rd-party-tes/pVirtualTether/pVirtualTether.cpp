// t. schneider tes@mit.edu 10.2.07
// laboratory for autonomous marine sensing systems
// massachusetts institute of technology 
// 
// this is pVirtualTether.cpp 
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

#include "pVirtualTether.h"

// multiplier amount to extend the range by if TETHER_OVERRIDE is set true
// for example, RANGE_EXTENSION = 0.2 will extend the range by 20% over
// the distance between the vehicle and the contact at the time of TETHER_OVERRIDE
// being set true
#define RANGE_EXTENSION 0.2

using namespace std;

CpVirtualTether::CpVirtualTether()
{
}
CpVirtualTether::~CpVirtualTether()
{
}

// OnNewMail: called when new mail (previously registered for)
// has arrived.
bool CpVirtualTether::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::reverse_iterator p;
  
    for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
        CMOOSMsg &msg = *p;
    
        string key = msg.m_sKey;
        string sdata = msg.m_sVal;
        double ddata = msg.m_dfVal;

        if(MOOSStrCmp(key, "AIS_REPORT"))
        {
            string which_contact;
            if (!(MOOSValFromString(which_contact, sdata, "NAME")))
            {
                MOOSTrace("pVirtualTether: Bad message. No contact name found.\n");
                return false;
            }
            if(MOOSStrCmp(which_contact, m_contact))
            {
                double contact_x;
                double contact_y;

                // parse out the X and Y of the contact
                vector<string>ais = parseString(sdata, ',');
                contact_x = atof(ais[3].substr(2, ais[3].size()).c_str());
                contact_y = atof(ais[4].substr(2, ais[4].size()).c_str());
            
                double dx = contact_x - m_nav_x;
                double dy = contact_y - m_nav_y;
            
                // distance squared between vehicle and contact
                m_lsquare = dx*dx + dy*dy;

                MOOSTrace("dx: ");
                MOOSTrace(doubleToString(dx));
                MOOSTrace("\n");

                MOOSTrace("dy: ");
                MOOSTrace(doubleToString(dy));
                MOOSTrace("\n");

                MOOSTrace("l: ");
                MOOSTrace(doubleToString(sqrt(m_lsquare)));
                MOOSTrace("\n");


                // check that the vehicle is in the range still
                if (m_lsquare > m_range*m_range && m_grace < MOOSTime())
                {
                    m_override = true;
                    m_pos_ok = false;
                    // MOOS_MANUAL_OVERIDE mispelled on purpose
                    m_Comms.Notify("MOOS_MANUAL_OVERIDE", "true", MOOSTime());
                }
            
                m_last_report_time = MOOSTime();
            }
        } 
        else if(MOOSStrCmp(key, "NAV_X"))
        {
            m_nav_x = ddata;
        }
        else if(MOOSStrCmp(key, "NAV_Y"))
        {
            m_nav_y = ddata;
        }

        // override or overide, to be consistent with paul's mispelling
        else if(MOOSStrCmp(key, "TETHER_OVERRIDE") || MOOSStrCmp(key, "TETHER_OVERIDE"))
        {
            if(MOOSStrCmp(sdata, "true"))
            {
                double l = sqrt(m_lsquare);
            
                m_range = l + RANGE_EXTENSION*l;
                if (m_range < m_original_range)
                    m_range = m_original_range;
                m_pos_ok = true;
                // helm override
                m_override = false;
                m_Comms.Notify("MOOS_MANUAL_OVERIDE", "false", MOOSTime());
            }   
        
            else if(MOOSStrCmp(sdata,"false"))
            {
                m_range = m_original_range;
            }
        }    
        else
        {
            MOOSTrace("pVirtualTether: Unknown msg [%s]\n",msg.m_sKey.c_str());
        }
    }
    return true;
}

// OnConnectToServer: called when connection to the MOOSDB is
// complete
bool CpVirtualTether::OnConnectToServer()
{
    DoRegistrations();

    // set report time as connection time, so that the contact
    // is given fair time to establish before the helm is shut off
    m_last_report_time = MOOSTime();
    return true;
}

// Iterate: called AppTick times per second
bool CpVirtualTether::Iterate()
{ 
    double moos_time = MOOSTime();
    // check to make sure time is ok
    if (((m_last_report_time + m_timeout) < moos_time) && m_timeout > 0 && m_grace < moos_time)
    {
        if (!m_override)
        {
            m_override = true;
            m_Comms.Notify("MOOS_MANUAL_OVERIDE", "true", MOOSTime()); // nope!
        }
    }
    else if(m_override && m_pos_ok)
    {
        m_override = false;
        m_Comms.Notify("MOOS_MANUAL_OVERIDE", "false", MOOSTime()); // reset since we have new messages
    }

    MOOSTrace("range: ");
    MOOSTrace(doubleToString(m_range));
    MOOSTrace("\n");

    return true;
}

// OnStartUp: called when program is run
bool CpVirtualTether::OnStartUp()
{       

    m_override = false;
    m_last_report_time = MOOSTime();
    m_pos_ok = true;
  
    if(!m_MissionReader.GetConfigurationParam("contact", m_contact))
        return MOOSFail("pVirtualTether: Must specify 'contact' in .moos block\n");
    
    if(!m_MissionReader.GetConfigurationParam("range", m_original_range))
        return MOOSFail("pVirtualTether: Must specify 'range' in .moos block\n");
  
    m_range = m_original_range;

    m_timeout = 0;
    if(!m_MissionReader.GetConfigurationParam("timeout", m_timeout))
        MOOSTrace("pVirtualTether: Warning: no timeout specified, so no timeout will occur. \n");

    m_grace = 10;
    m_MissionReader.GetConfigurationParam("grace", m_grace);
    m_grace += MOOSTime();
  
    DoRegistrations();
    return true;
}

// DoRegistrations: register for mail we want
void CpVirtualTether::DoRegistrations()
{
    m_Comms.Register("AIS_REPORT", 0);
    m_Comms.Register("NAV_X", 0);
    m_Comms.Register("NAV_Y", 0);
    m_Comms.Register("TETHER_OVERRIDE",0);
    m_Comms.Register("TETHER_OVERIDE",0);
    return;
}
