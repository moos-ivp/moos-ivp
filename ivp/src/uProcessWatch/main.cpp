/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: May 27th 2007 (MINUS-07)                             */
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

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "ProcessWatch.h"
#include "MBUtils.h"

using namespace std;

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("uProcessWatch");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }

  // Look for a request for help or usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    MOOSTrace("Usage: uProcessWatch moosfile.moos   \n");
    return(0);
  }

  string sMissionFile = "ProcessWatch.moos";
  string sMOOSName = "uProcessWatch";

  switch(argc) {
  case 3:
    sMOOSName = argv[2];
  case 2:
    sMissionFile = argv[1];
  }
  
  ProcessWatch ProcessWatch;
  
  ProcessWatch.Run(sMOOSName.c_str(), sMissionFile.c_str());
  
  return(0);
}


