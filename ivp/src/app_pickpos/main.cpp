/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Sep 22nd 2018                                        */
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
#include "PickPos.h"
#include "PickPos_Info.h"

using namespace std;

int main(int argc, char *argv[])
{
  PickPos pickpos;

  bool    verbose = false;
  string  arg_summary = argv[0];

  if(argc == 1)
    showHelpAndExit();
  
  for(int i=1; i<argc; i++) {
    bool   handled = false;
    
    string argi = argv[i];
    arg_summary += " " + argi;

    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      showReleaseInfoAndExit();
    else if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if(argi=="--verbose") {
      verbose=true;
      handled = true;
    }
    else if((argi == "--multiline") || (argi=="--ml") || (argi=="-ml"))
      handled = pickpos.setMultiLine();
    else if(strBegins(argi, "--amt="))
      handled = pickpos.setPickAmt(argi.substr(6));
    else if(strBegins(argi, "--posfile="))
      handled = pickpos.addPosFile(argi.substr(10));
    else if(strBegins(argi, "--polygon="))
      handled = pickpos.addPolygon(argi.substr(10));
    else if(strBegins(argi, "--circle="))
      handled = pickpos.setCircle(argi.substr(9));
    else if(strBegins(argi, "--poly="))
      handled = pickpos.addPolygon(argi.substr(7));
    else if(strBegins(argi, "--buffer="))
      handled = pickpos.setBufferDist(argi.substr(9));
    else if(strBegins(argi, "--maxtries="))
      handled = pickpos.setMaxTries(argi.substr(11));
    else if(strBegins(argi, "--hdg="))
      handled = pickpos.setHdgConfig(argi.substr(6));
    else if(strBegins(argi, "--spd="))
      handled = pickpos.setSpdConfig(argi.substr(6));
    else if(strBegins(argi, "--format="))
      handled = pickpos.setOutputType(argi.substr(9));
    else if(strBegins(argi, "--hsnap="))
      handled = pickpos.setHeadingSnap(argi.substr(8));
    else if(strBegins(argi, "--ssnap="))
      handled = pickpos.setSpeedSnap(argi.substr(8));
    else if(strBegins(argi, "--psnap="))
      handled = pickpos.setPointSnap(argi.substr(8));
    else if(strBegins(argi, "--vnames"))
      handled = pickpos.setVNames();
    else if(strBegins(argi, "--grps="))
      handled = pickpos.setGroups(argi.substr(7));
    
    if(!handled) {
      cout << "Unhandled arg: " << argi << endl;
      return(0);
    }
  }

  pickpos.setVerbose(verbose);
  pickpos.pick();

  if(verbose)
    cout << "// " << arg_summary << endl;

  return(0);
}









