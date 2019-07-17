/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
#include <cstring>
#include <vector>
#include "MBUtils.h"
#include "QueryDB.h"
#include "QueryDB_Info.h"
#include "LogicCondition.h"

using namespace std;

int main(int argc, char *argv[])
{
  string mission_file;
  string condition;
  bool   quiet = false;      
  
  string server_host = "";  // localhost
  int    server_port = 0;   // 9000
  double wait_time   = 10;  // seconds
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    //cout << "argv[" << i << "]:[" << argi << "]" << endl;
    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      showReleaseInfoAndExit();
    else if((argi=="-e") || (argi=="--example") || (argi=="-example"))
      showExampleConfigAndExit();
    else if((argi == "-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi == "-i") || (argi == "--interface"))
      showInterfaceAndExit();
    else if((argi == "-q") || (argi == "--quiet"))
      quiet = true;
    else if(strBegins(argi, "--host=") && (server_host==""))
      server_host = argi.substr(7);
    else if(strBegins(argi, "--port="))      
      server_port = atoi(argi.substr(7).c_str());
    else if(strBegins(argi, "--wait="))      
      wait_time = atof(argi.substr(7).c_str());
    else if(strBegins(argi, "--condition=") && (condition==""))      
      condition = argi.substr(12);
    else if((strEnds(argi, ".moos")||strEnds(argi, ".moos++")) && (mission_file==""))
      mission_file = argv[i];    
    else {
      cout << "uQueryDB Unhandled arg: " << argi << endl;
      exit(1);
    }
  }

  // If the mission file is not provided, we prompt the user if the 
  // server_host or server_port information is not on command line.
  if(mission_file == "") {
    char buff[1000];
    // If server_host info was not on the command line, prompt here.
    if(server_host == "") {
      server_host = "localhost";
      cout << "Enter IP address:  [localhost] ";
      if(fgets(buff, 999, stdin) != NULL) {
	if(buff[0] != '\n') {
	  server_host     = buff;    
	}
      }
    }
    // If server_port info was not on the command line, prompt here.
    if(server_port == 0) {
      cout << "Enter Port number: [9000] ";
      server_port = 9000;
      if(fgets(buff, 999, stdin) != NULL) {
	if(buff[0] != '\n') {
	  server_port     = atoi(buff); 
	}
      }
    }
  }

  if(mission_file != "") {
    if(!okFileToRead(mission_file)) {
      cout << "Unable to read mission file: " << mission_file << endl;
      cout << "Exiting now as failure, return value=1" << endl;
      exit(1);
    }
  }
  
  QueryDB query(server_host, server_port);
  
  if(mission_file == "") {
    cout << "Mission File not provided. " << endl;
    cout << "  server_host  = " << server_host << endl;
    cout << "  server_port  = " << server_port << endl;
    query.setConfigCommsLocally(true);
  }
  else {
    if(!quiet)
      cout << "Mission File was provided: " << mission_file << endl;
  }

  if(condition == "") {
    cout << "A condition must be specified. Exiting now with value 1.";
    cout << endl;
    return(1);
  }
  
  if(!quiet)
    cout << "Condition: [" << condition << "]" << endl;

  query.setLogicCondition(condition);
  query.setVerbose(!quiet);
  query.setWaitTime(wait_time);
  query.Run("uQueryDB", mission_file.c_str(), argc, argv);

  return(1);
}
