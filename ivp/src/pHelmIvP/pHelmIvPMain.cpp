/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
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
#include "MBUtils.h"
#include <vector>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: pHelmIvP file.moos [file.bhv]...[file.bhv]     " << endl;
    cout << "       [--help|-h] [--version|-v]                     " << endl;
    cout << "                                                      " << endl;
    cout << "[file.moos] Filename to get MOOS config parameters.   " << endl;
    cout << "[file.bhv]  Filename to get IvP Helm config paramters." << endl;
    cout << "[-v]        Output version number and exit.           " << endl;
    cout << "[-h]        Output this usage information and exit.   " << endl;
    cout << endl;
    return(0);
  }

  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("pHelmIvP");
    for(int i=0; i<svector.size(); i++)
      if(!strContains(svector[i], "ree"))
	cout << svector[i] << endl;
    return(0);
  }

  vector<string>  bhv_files;
  vector<string>  moos_files;

  for(int i=1; i<argc; i++) {
    if(strContains(argv[i], ".bhv"))
      bhv_files.push_back(argv[i]);
    else if(strContains(argv[i], ".moos"))
      moos_files.push_back(argv[i]);

  }

  if(moos_files.size() != 1) {
    cout << "A *single* .moos file must be provided. Exiting" << endl;
    exit(-1);
  }

  HelmIvP helmIvP;
  
  const char *sMissionFile = moos_files[0].c_str();

  int vsize = bhv_files.size();
  for(int i=0; i<vsize; i++) {
    helmIvP.addBehaviorFile(bhv_files[i]);
  }

  helmIvP.Run("pHelmIvP", sMissionFile);
  
  return(0);
}










