/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng, MIT Cambridge MA             */
/*    FILE: main.cpp                                             */
/*    DATE: May 7th, 2025                                        */
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

#include <cstdlib>
#include <iostream>
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "Populator_MOOSFile.h"
#include "PatchApplicator.h"
#include "Patch_Info.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main()

int main(int argc, char *argv[])
{
  PatchApplicator papp;

  bool handled = true;
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-m") || (argi == "--man") || (argi=="-man"))
      showManualAndExit();
    else if((argi=="--version") || (argi=="-version")) {
      showReleaseInfo("nspatch", "gpl");
      return(0);
    }
    else if((argi=="--verbose") || (argi == "-v"))
      papp.setVerbose();

    // ---------------------------------------------------
    // Part 2: Ways of specifying a MOOS or BHV stem file
    // ---------------------------------------------------
    // Multipe ways of specifying a MOOS stem file
    else if(strBegins(argi, "--mstem="))
      papp.setStemMoosFile(argi.substr(8));
    else if(strBegins(argi, "-ms="))
      papp.setStemMoosFile(argi.substr(4));
    else if(strBegins(argi, "--stem=")) { 
      if(strEnds(argi, ".moos"))
	papp.setStemMoosFile(argi.substr(7));
    }

    // Multipe ways of specifying a BHV stem file
    else if(strBegins(argi, "--bstem="))
      papp.setStemBhvFile(argi.substr(8));
    else if(strBegins(argi, "-bs="))
      papp.setStemBhvFile(argi.substr(4));
    else if(strBegins(argi, "--stem=")) {
      if(strEnds(argi, ".bhv"))
	papp.setStemBhvFile(argi.substr(7));
    }

    // ---------------------------------------------------
    // Part 2: Ways of specifying a MOOS or BHV patch file
    // ---------------------------------------------------
    // Multiple ways of specifying a MOOS patch file
    else if(strEnds(argi, ".xmoos"))
      papp.addXMoosFile(argi);
    else if(strBegins(argi, "--mpatch="))
      papp.addXMoosFile(argi.substr(9));
    else if(strBegins(argi, "-mp="))
      papp.addXMoosFile(argi.substr(4));

    // Multiple ways of specifying a BHV patch file
    else if(strEnds(argi, ".xbhv"))
      papp.addXBhvFile(argi);
    else if(strBegins(argi, "--bpatch="))
      papp.addXBhvFile(argi.substr(9));
    else if(strBegins(argi, "-bp="))
      papp.addXBhvFile(argi.substr(4));

    // ---------------------------------------------------
    // Part 2: Ways of specifying a MOOS or BHV targ file
    // ---------------------------------------------------
    // Multiple ways of specifying a MOOS targ file
    else if(strBegins(argi, "--mtarg="))
      papp.setTargMoosFile(argi.substr(8));
    else if(strBegins(argi, "-mt="))
      papp.setTargMoosFile(argi.substr(4));
    else if(strBegins(argi, "--targ=")) {
      string targ_file = argi.substr(7);
      if(strEnds(targ_file, ".moos") ||
	 strEnds(targ_file, ".moosx"))
	papp.setTargMoosFile(targ_file);
    }

    // Multiple ways of specifying a BHV targ file
    else if(strBegins(argi, "--btarg="))
      papp.setTargBhvFile(argi.substr(8));
    else if(strBegins(argi, "-bt="))
      papp.setTargBhvFile(argi.substr(4));
    else if(strBegins(argi, "--targ=")) {
      string targ_file = argi.substr(7);
      if(strEnds(targ_file, ".bhv") ||
	 strEnds(targ_file, ".bhvx"))
	papp.setTargBhvFile(targ_file);
    }
      
    else if(strBegins(argi, "--vkey=")) {
      string vkey_str = argi.substr(7);
      papp.setVKey(vkey_str);
    }
      
    // If arg ends in .moos, there is ambiguity as to whether the user
    // wants this to be a stem or targ file. If the stem file is not
    // set, treat as a stem. Otherwise treat as targ moos file.
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moosx")) {
      if(!papp.hasStemMoos())
	papp.setStemMoosFile(argi);
      else
	papp.setTargMoosFile(argi);
    }

    // If arg ends in .bhv, there is ambiguity as to whether the user
    // wants this to be a stem or targ file. If the stem bhv file is
    // not set, treat as a stem. Otherwise treat as targ bhv file.
    else if(strEnds(argi, ".bhv") || strEnds(argi, ".bhvx")) {
      if(!papp.hasStemBhv())
	papp.setStemBhvFile(argi);
      else
	papp.setTargBhvFile(argi);
    }
    else
      handled = false;
    
    if(!handled) {
      cout << "Unhandled command line argument: " << argi << endl;
      cout << "Use --help for usage. Exiting.   " << endl;
      exit(1);
    }
  }

  bool ok = papp.applyPatch();

  if(!ok)
    return(1); // failure

  return(0); // success
}
