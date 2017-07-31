/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: pHelmIvP.cpp                                         */
/*    DATE: Oct 12th 2004                                        */
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
#include <vector>
#include <string>
#include "MBUtils.h"
#include "HelmIvP.h"
#include "HelmIvP_Info.h"

using namespace std;

int main(int argc, char *argv[])
{
  string mission_file;
  string run_command = argv[0];
  string verbose_setting;

  vector<string>  bhv_files;

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
    else if(strBegins(argi, "--verbose=")) 
      verbose_setting = argi.substr(10);
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    else if(strEnds(argi, ".bhv"))
      bhv_files.push_back(argv[i]);
    else if(i==2)
      run_command = argi;
  }
  
  if(mission_file == "")
    showHelpAndExit();
  
  HelmIvP helmIvP;

  if(verbose_setting != "") {
    bool ok = helmIvP.setVerbosity(verbose_setting);
    if(!ok) {
      cout << "Illegal verbose setting. Exiting now.";
      return(0);
    }
  }
    
  unsigned int k, ksize = bhv_files.size();
  for(k=0; k<ksize; k++)
    helmIvP.addBehaviorFile(bhv_files[k]);

  //  helmIvP.Run(run_command.c_str(), mission_file.c_str(), argc, argv);
  helmIvP.Run(run_command.c_str(), mission_file.c_str());
  
  return(0);
}




