/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: pHelmIvP.cpp                                         */
/*    DATE: Oct 12th 2004                                        */
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

#include <iostream>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "HelmIvP.h"
#include "ReleaseInfo.h"
#include "MBUtils.h"
#include "HELM_ExampleConfig.h"
#include <vector>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: pHelmIvP file.moos [file.bhv]...[file.bhv]      " << endl;
    cout << "                                                       " << endl;
    cout << "Options:                                               " << endl;
    cout << "  --alias=<ProcessName>                                " << endl;
    cout << "      Launch pHelmIvP with the given process name      " << endl;
    cout << "      rather than pHelmIvP.                            " << endl;
    cout << "  --example, -e                                        " << endl;
    cout << "      Display example MOOS configuration block         " << endl;
    cout << "  --help, -h                                           " << endl;
    cout << "      Display this help message.                       " << endl;
    cout << "  --version,-v                                         " << endl;
    cout << "      Display the release version of uTimerScript.     " << endl;
    cout << "Notes:                                                 " << endl;
    cout << "  One or more behavior files may be provided, or none. " << endl;
    cout << "  If none provided, the behavior file may be indicated " << endl;
    cout << "  within the .moos file in the helm config block.      " << endl;
    cout << endl;
    return(0);
  }

  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("pHelmIvP", "none");
    return(0);
  }

  // Look for a request for example configuration
  if(scanArgs(argc, argv, "-e", "--example", "-example")) {
    showExampleConfig();
    return(0);
  }

  vector<string>  bhv_files;
  vector<string>  moos_files;

  string run_command = argv[0];

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strContains(argi, ".bhv"))
      bhv_files.push_back(argv[i]);
    else if(strEnds(argi, ".moos"))
      moos_files.push_back(argv[i]);
    else if(strEnds(argi, ".moos++"))
      moos_files.push_back(argv[i]);
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
  }

  if(moos_files.size() != 1) {
    cout << "A *single* .moos file must be provided. Exiting" << endl;
    exit(-1);
  }

  HelmIvP helmIvP;
  
  const char *sMissionFile = moos_files[0].c_str();

  unsigned int i, vsize = bhv_files.size();
  for(i=0; i<vsize; i++)
    helmIvP.addBehaviorFile(bhv_files[i]);

  helmIvP.Run(run_command.c_str(), sMissionFile);
  
  return(0);
}











