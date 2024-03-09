/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include "OpenURL.h"
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
    else if(argi=="--debug") {
      pickpos.setDebug();
      handled = true;
    }
    else if((argi == "--multiline") || (argi=="--ml") || (argi=="-ml"))
      handled = pickpos.setMultiLine();
    else if((argi == "--reverse_names") || (argi=="-r"))
      handled = pickpos.setReverseNames();
    else if(strBegins(argi, "--amt="))
      handled = pickpos.setPickAmt(argi.substr(6));
    else if(strBegins(argi, "--vix="))
      handled = pickpos.setVNameStartIX(argi.substr(6));
    else if(strBegins(argi, "--posfile="))
      handled = pickpos.addPosFile(argi.substr(10));
    else if(strBegins(argi, "--file="))
      handled = pickpos.setResultFile(argi.substr(7));
    else if(strBegins(argi, "--polygon="))
      handled = pickpos.addPolygon(argi.substr(10));
    else if(strBegins(argi, "--circle="))
      handled = pickpos.setCircle(argi.substr(9));
    else if(argi == "--poly=pavlab") // check first for homecourt
      handled = pickpos.addPolygon("-30,-15:-28,-20:35,5:32,13");
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

    else if((argi == "--group_names_one") || (argi == "-g1"))
      handled = pickpos.setVNameCacheOne();
    else if((argi == "--group_names_two") || (argi == "-g2"))
      handled = pickpos.setVNameCacheTwo();
    else if((argi == "--group_names_three") || (argi == "-g3"))
      handled = pickpos.setVNameCacheThree();
    else if((argi == "--group_names_four") || (argi == "-g4"))
      handled = pickpos.setVNameCacheFour();
    
    else if(argi == "--vnames")
      handled = pickpos.setVNames();
    else if(strBegins(argi, "--vnames="))
      handled = pickpos.setVNames(argi.substr(9));

    else if(argi == "--indices")
      handled = pickpos.setIndices();
    else if(argi == "--colors")
      handled = pickpos.setColors();
    else if(strBegins(argi, "--colors="))
      handled = pickpos.setColors(argi.substr(9));
    else if(strBegins(argi, "--grps="))
      handled = pickpos.setGroups(argi.substr(7));
    else if(argi == "--reuse")
      handled = pickpos.setReuse();
    else if(argi == "--hdrs") {
      pickpos.enableHeaders();
      handled = true;
    }
    else if((argi == "-w") || (argi == "--web") || (argi == "-web"))
      openURLX("https://oceanai.mit.edu/ivpman/apps/pickpos");
      
    if(!handled) {
      cout << "Unhandled arg: " << argi << endl;
      return(1);
    }
  }

  pickpos.setArgSummary(arg_summary);
  pickpos.setVerbose(verbose);
  pickpos.pick();

  return(0);
}










