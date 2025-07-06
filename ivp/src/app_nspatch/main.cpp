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
    else if((argi=="--version") || (argi=="-version")) {
      showReleaseInfo("nspatch", "gpl");
      return(0);
    }
    else if((argi=="--verbose") || (argi == "-v"))
      papp.setVerbose();

    else if(strEnds(argi, ".xmoos"))
      papp.addXMoosFile(argi);

    else if(strEnds(argi, ".xbhv"))
      papp.addXBhvFile(argi);

    else if(strBegins(argi, "--stem=")) {
      string stem_file = argi.substr(7);
      if(strEnds(stem_file, ".moos"))
	papp.setStemMoosFile(stem_file);
      else if(strEnds(stem_file, ".bhv"))
	papp.setStemBhvFile(stem_file);
    }
      
    else if(strBegins(argi, "--targ=")) {
      string targ_file = argi.substr(7);
      if(strEnds(targ_file, ".moos") ||
	 strEnds(targ_file, ".moosx"))
	papp.setTargMoosFile(targ_file);
      else if(strEnds(targ_file, ".bhv") ||
	      strEnds(targ_file, ".bhvx"))
	papp.setTargBhvFile(targ_file);
    }
      
    if(!handled) {
      cout << "Unhandled command line argument: " << argi << endl;
      cout << "Use --help for usage. Exiting.   " << endl;
      exit(1);
    }
  }
    
  papp.applyPatch();

  return(0);
}

//------------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "Usage: " << endl;
  cout << "  nspatch stem_file patch_file targ_file [OPTIONS]          " << endl;
  cout << "                                                            " << endl;
  cout << "Synopsis:                                                   " << endl;
  cout << "  Apply a given patch file to a given stem file, to make a  " << endl;
  cout << "  new target file. Supported file types are mission (.moos) " << endl;
  cout << "  files or behavior (.bhv) files. The objective is to make a" << endl;
  cout << "  new mission from an existing (stem) mission by providing  " << endl;
  cout << "  the patch file (difference) applied to overwrite or       " << endl;
  cout << "  augment portions of the stem mission.                     " << endl;
  cout << "                                                            " << endl;
  cout << "  Mandatory arguments: a stem file, one or more patch files " << endl;
  cout << "  and a target file. nspatch will work with either MOOS or  " << endl;
  cout << "  behavior files, but not both, in any given invocation.    " << endl;
  cout << "                                                            " << endl;
  cout << "  Note: Files ending in .xmoos are interpreted to be patch  " << endl;
  cout << "        files in .moos format. Files ending in .xbhv are    " << endl;
  cout << "        interpreted to be behavior files.                   " << endl;
  cout << "                                                            " << endl;
  cout << "Options:                                                    " << endl;
  cout << "  -h,--help       Displays this help message                " << endl;
  cout << "  -v,--version    Display current release version           " << endl;
  cout << "  --verbose       Write verbose output.                     " << endl;
  cout << "                                                            " << endl;
  cout << "  --stem=stem.moos  Stem moos or behavior file              " << endl;
  cout << "  --targ=targ.moos  Target moos or behavior file            " << endl;
  cout << "                                                            " << endl;
  cout << "Returns:                                                    " << endl;
  cout << "  0 if ok                                                   " << endl;
  cout << "  1 if not ok                                               " << endl;
  cout << "                                                            " << endl;
  cout << "Further Notes:                                              " << endl;
  cout << "  (1) The order of arguments is irrelevent.                 " << endl;
  cout << "Examples:                                                   " << endl;
  cout << "  $ nspatch --stem=stem.moos patch.xmoos --targ=targ.moosx  " << endl;
  cout << "  $ nspatch --stem=stem.bhv patch.xbhv --targ=targ.bhvx     " << endl;
  cout << endl;
  exit(0);
}
