// t. schneider tes@mit.edu 9.30.07
// laboratory for autonomous marine sensing systems
// massachusetts institute of technology 
// 
// portions of code adapted from Echovar.cpp (Mike Benjamin)
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

#include "iWebsite.h"

using namespace std;

CiWebsite::CiWebsite()
{
}
CiWebsite::~CiWebsite()
{
}


// OnNewMail: called when new mail (previously registered for)
// has arrived.
bool CiWebsite::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::reverse_iterator p;
    
    for(p = NewMail.rbegin(); p != NewMail.rend(); p++)
    {
        CMOOSMsg &msg = *p;

        string key = msg.m_sKey;
        string sdata = msg.m_sVal;
        
        if(MOOSStrCmp(key, "WEBSITE_PUT"))
        {
            if (!scpPut(sdata) && MOOSStrCmp(m_verbosity, "verbose"))
                MOOSTrace("iWebsite: Failed scp.\n");
        }
        else if (MOOSStrCmp(m_verbosity, "verbose")) 
        {
            MOOSTrace("iWebsite: Unknown msg [%s]\n",msg.m_sKey.c_str());
        }
    }



    return true;
}

// OnConnectToServer: called when connection to the MOOSDB is
// complete
bool CiWebsite::OnConnectToServer()
{
    DoRegistrations();
    return true;
}

// Iterate: called AppTick times per second
bool CiWebsite::Iterate()
{ 
    // is it time to do a fetch?
    double now = MOOSTime();
    if (now < m_next_fetch)
    {
        if (MOOSStrCmp(m_verbosity, "verbose"))
        {
            MOOSTrace("iWebsite: fetch in ");
            MOOSTrace(doubleToString(m_next_fetch-now));
            MOOSTrace(".\n");       
        }
        else if (MOOSStrCmp(m_verbosity, "terse"))
            MOOSTrace("$");

        return true; // no, it isn't
    }

    // build a string that will be passed to the unix / linux
    // wget command to retrieve the file.
    string wget_command;
    if (MOOSStrCmp(m_verbosity, "verbose"))
        wget_command = "wget -O ";
    else
        wget_command = "wget -qO ";
    wget_command += m_local_cache;
    wget_command += " ";
    wget_command += m_server_address;

    if (MOOSStrCmp(m_verbosity, "verbose"))
    {
        MOOSTrace("iWebsite: Sending wget command:\n");
        MOOSTrace(wget_command);
        MOOSTrace("\n");
    }

    // execute linux system command and test for success
    if(!system(wget_command.c_str()))
    {
        if (MOOSStrCmp(m_verbosity, "verbose"))
        {
            MOOSTrace("iWebsite: wget successful.\n");      
        }
        else if (MOOSStrCmp(m_verbosity, "terse"))
            MOOSTrace("<");
    }
    else
    {
        if (MOOSStrCmp(m_verbosity, "verbose"))
        {
            MOOSTrace("iWebsite: wget failed.\n");
        }
        else if (MOOSStrCmp(m_verbosity, "terse"))
            MOOSTrace("!");
      
        return false; //try again on next iteration
    }

    if(ProcessFetchedFile())
        m_next_fetch = now + m_wait; // success, now set wait time until next fetch
    else
        return false;

    return true;
}

// OnStartUp: called when program is run
bool CiWebsite::OnStartUp()
{       
    //parse ye ol' configuration file
    m_local_cache = "./iWebsite.tmp";
    m_wait = 600;
    m_verbosity = "terse";
    m_sequence_string = "sequence";
    m_ignore_sequence = false;
    m_last_seq = 0;

    list<string> sParams;
    if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
        list<string>::iterator p;
        for(p = sParams.begin(); p!=sParams.end(); p++) {

            string sLine    = stripBlankEnds(*p);
            string sVarName = stripBlankEnds(MOOSChomp(sLine, "="));

            if(MOOSStrCmp(sVarName, "down_address"))
                m_server_address = sLine;

            if(MOOSStrCmp(sVarName, "up_address"))
                m_server_up_address = sLine;

            if(MOOSStrCmp(sVarName, "up_user"))
                m_server_up_user = sLine;

            if(MOOSStrCmp(sVarName, "cache"))
                m_local_cache = sLine;

            if(MOOSStrCmp(sVarName, "wait"))
                m_wait = atof(sLine.c_str());      

            if(MOOSStrCmp(sVarName, "verbosity"))
                m_verbosity = sLine;

            if(MOOSStrCmp(sVarName, "sequence_string"))
                m_sequence_string = sLine;

            if(MOOSStrCmp(sVarName, "ignore_sequence"))
            {
                if(MOOSStrCmp(sLine, "true"))
                    m_ignore_sequence = true;
            }
      
            if(MOOSStrCmp(sVarName, "echo"))
            {
                sLine = findReplace(sLine, "->", ">");
                vector<string> svector = parseString(sLine, '>');
                if(svector.size() != 2)
                    return(false);
                string sLeft  = stripBlankEnds(svector[0]);
                string sRight = stripBlankEnds(svector[1]);
                if(!addMapping(sLeft, sRight)) 
                    return(false);
            }
      
        }


        ofstream fout;
        fout.open(m_local_cache.c_str());
        fout << "";
        fout.close();
  
        if (m_server_address.size() == 0)
            return MOOSFail("iWebsite: The mandatory server 'address' config parameter not specified.\n");

    }

    m_next_fetch = MOOSTime();

    return true;
}

// DoRegistrations: register for MOOS variables we 
// want to hear about (receive mail for)
void CiWebsite::DoRegistrations()
{
    m_Comms.Register("WEBSITE_PUT", 0);

    return;
}

// ProcessFetchedFile: takes the file fetched from the website
// and stored at m_local_cache and performs the parsing
// and subsequent publishing to the MOOSDB requested from 
// the configuration options
bool CiWebsite::ProcessFetchedFile()
{
  
    // test if file is valid
    if(!okFileToRead(m_local_cache))
        return false;

    // create a file stream and open the locally cached file
    ifstream fin;
    fin.open(m_local_cache.c_str());
    string line;

    // search for sequence number
    if(!m_ignore_sequence)
    {
        while(!fin.eof())
        {
            getline(fin, line);
            line = stripBlankEnds(line);
          
            if(line.size() > 0)
            {
                vector<string> b_line;
                b_line = chompString(line, '=');

                if(b_line.size() == 2)
                {
                    string key = stripBlankEnds(b_line[0]);
                    string value = stripBlankEnds(b_line[1]);  

                    if(MOOSStrCmp(key, m_sequence_string))
                    {
                        double cur_seq = atof(value.c_str());

                        // if not a larger sequence number, just return without echoing
                        if (cur_seq <= m_last_seq)
                            return true;
                        m_last_seq = cur_seq;
                        break;
                    }
                }
            }
        }
    }
  
    fin.seekg(0, ios::beg);

    while(!fin.eof())
    {  
        getline(fin, line);
        line = stripBlankEnds(line);
        if(line.size() > 0)
        {
            //test for comment ("#")
            if(!(line[0] == '#'))
            {
                // not a comment, go ahead and echo it
                vector<string> b_line;
                b_line = chompString(line, '=');
                if(b_line.size() == 2)
                {
                    string key = stripBlankEnds(b_line[0]);
                    string value = stripBlankEnds(b_line[1]);
                  
                    map<string,string>::iterator p;
                    p = var_map.find(key);
                    if(p == var_map.end())
                    {
                        if((m_ignore_sequence || !MOOSStrCmp(key, m_sequence_string)) && MOOSStrCmp(m_verbosity, "verbose"))
                            MOOSTrace("iWebsite: unknown line \n[%s]\n", line.c_str());
                    }
                    else 
                    {
                        string new_key = p->second;
                        if(value[0] == '"') // string data
                            m_Comms.Notify(new_key, value.substr(1, value.size()-2));
                        else // assume double data
                            m_Comms.Notify(new_key, atof(value.c_str()));
                    }         
                }
            }
        }
    }
    fin.close(); 

    if (MOOSStrCmp(m_verbosity, "terse"))
        MOOSTrace("@");

    return true;
}


// addMapping: adds the mapping of one variable on to another
// for use of echoing variables from the website file to the MOOSDB
// this procedure is directly used from Echovar.cpp (Mike Benjamin)

bool CiWebsite::addMapping(string src, string targ)
{
    if((src == "") || (targ == ""))
        return(false);
  
    map<string,string>::iterator p;
    p = var_map.find(src);

    if(p == var_map.end())
        var_map[src] = targ;
    else
        return(false);

    return(true);
}

// scpPut: invokes scp to attempt to place a file (pointed to by 'what') on a remote site
bool CiWebsite::scpPut(string what)
{
    if ((what == "") || (m_server_up_address == "") || (m_server_up_user == ""))
        return false;

    // build string for scp command line
    // scp file user@server:location
    string scp_command;
    if(MOOSStrCmp(m_verbosity, "verbose"))
        scp_command = "scp -o Compression=yes -o ConnectionAttempts=10 ";
    else
        scp_command = "scp -q -o Compression=yes -o ConnectionAttempts=10 ";
    scp_command += what;
    scp_command += " ";
    scp_command += m_server_up_user;
    scp_command += "@";
    scp_command += m_server_up_address;

    system(scp_command.c_str());

    if(MOOSStrCmp(m_verbosity, "terse"))
        MOOSTrace(">");  
      
    return true;

}
