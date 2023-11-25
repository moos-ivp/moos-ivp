/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Dec 29th 2015 Happy Birthday Dan!                    */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "OpenURL.h"
#include "QueryDB.h"
#include "QueryDB_Info.h"

using namespace std;

int main(int argc, char *argv[])
{
  QueryDB query;
  bool unique_name = false;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      showReleaseInfoAndExit();
    else if((argi=="-e") || (argi=="--example") || (argi=="-example"))
      showExampleConfigAndExit();
    else if((argi == "-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi == "-i") || (argi == "--interface"))
      showInterfaceAndExit();
    
    bool handled = true;
    if(strBegins(argi, "--wait="))
      query.setConfigWaitTime(argi.substr(7));
    else if(strBegins(argi, "--host="))
      query.setServerHost(argi.substr(7));
    else if(strBegins(argi, "--port="))      
      handled = query.setServerPort(argi.substr(7));
    else if(strBegins(argi, "--condition="))
      handled = query.addPassCondition(argi.substr(12));
    else if(strBegins(argi, "--pass_condition="))
      handled = query.addPassCondition(argi.substr(17));
    else if(strBegins(argi, "--fail_condition="))
      handled = query.addFailCondition(argi.substr(17));
    else if(strBegins(argi, "--check_var="))
      handled = query.addConfigCheckVar(argi.substr(12));
    else if((argi == "--unique") || (argi == "-u"))
      unique_name = true;
    else if(argi == "--csv")
      handled = query.setConfigCheckVarFormat("csv");
    else if(argi == "--wsv")
      handled = query.setConfigCheckVarFormat("wsv");
    else if(argi == "--esv")
      handled = query.setConfigCheckVarFormat("esv");
    else if((argi == "--vo") || (argi == "--vo"))
      handled = query.setConfigCheckVarFormat("esv");
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++")) 
      handled = query.setMissionFile(argi);
    else if((argi == "-w") || (argi == "--web") || (argi == "-web"))
      openURLX("https://oceanai.mit.edu/ivpman/apps/uQueryDB");
    else
      handled = false;
    
    if(!handled) {
      cout << "uQueryDB Unhandled arg: " << argi << endl;
      exit(1);
    }
  }

  // If the mission file is not provided, we prompt the user if the 
  // server_host or server_port information is not on command line.
  string mission_file = query.getMissionFile();
  if(mission_file == "") {
    char buff[1000];
    // If server_host info was not on the command line, prompt here.
    if(query.getServerHost() == "") {
      string server_host = "localhost";
      cout << "Enter IP address:  [localhost] ";
      if(fgets(buff, 999, stdin) != NULL) {
	if(buff[0] != '\n') {
	  server_host = buff;    
	}
      }
      query.setServerHost(server_host);
    }

    // If server_port info was not on the command line, prompt here.
    if(query.getServerPort() == 0) {
      string server_port = "9000";
      cout << "Enter Port number: [9000] ";
      if(fgets(buff, 999, stdin) != NULL) {
	if(buff[0] != '\n') {
	  server_port = buff; 
	}
      }
      query.setServerPort(server_port);
    }
    
    cout << "Mission File was not provided. " << endl;
    cout << "  server_host  = " << query.getServerHost() << endl;
    cout << "  server_port  = " << query.getServerPort() << endl;
    query.setConfigCommsLocally(true);
  }

  string app_name = "uQueryDB";
  if(unique_name) {
    srand(time(NULL));
    int rand_int = rand() % 10000;
    string rand_str = intToString(rand_int);
    app_name += "_" + rand_str;
  }

  query.Run(app_name.c_str(), mission_file.c_str(), argc, argv);

  return(1);
}

