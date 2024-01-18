/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Sep 28th, 2020                                       */
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

#include <string>
#include <cstdlib>
#include <iostream>
#include "MBUtils.h"
#include "TagHandler.h"
#include "ReleaseInfo.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  TagHandler handler;

  // First pass over args just to check verbosity
  bool verbose = false;
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(argi == "--verbose") {
      handler.setVerbose();
      verbose = true;
    }
  }

  // Second pass over args
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("tagrep", "gpl");
      return(0);
    }

    bool handled = true;
    if(strBegins(argi, "--file="))
      handled = handler.setInputFile(argi.substr(7));
    else if(argi == "--hdr")
      handled = handler.setHeader("same_as_tag");
    else if(strBegins(argi, "--hdr="))
      handled = handler.setHeader(argi.substr(6));
    else if((argi=="-n") || (argi=="--noblanks")) 
      handler.setNoBlankLines();
    else if((argi=="-f") || (argi=="--first")) 
      handler.setFirstLineOnly();
    else if((argi=="-k") || (argi=="--keep_tag_line")) 
      handler.setKeepTagLine();
    else {
      if(!handler.isInputFileSet())
	handled = handler.setInputFile(argi);
      else
	handled = handler.setTag(argi);
    }
    
    if(!handled) {
      if(verbose) {
	cout << "tagrep: Bad command line argument: " << argi << endl;
	cout << "Use --help for usage. Exiting.   " << endl;
      }
      exit(1);
    }
  }

  bool ok = handler.handle();
  if(!ok)
    exit(1);
  exit(0);
}
  
//------------------------------------------------------------
// Procedure: showHelpAndExit()  

void showHelpAndExit()
{
  cout << "Usage: " << endl;
  cout << "  tagrep file tag [OPTIONS]                                " << endl;
  cout << "                                                           " << endl;
  cout << "Synopsis:                                                  " << endl;
  cout << "  Given a file of text, return the portion of the file that" << endl;
  cout << "  lies between the given tag and the next tag, or the end  " << endl;
  cout << "  of the file. Used in some launch scripts.                " << endl;
  cout << "                                                           " << endl;
  cout << "Standard Arguments:                                        " << endl;
  cout << "  file - The input file.                                   " << endl;
  cout << "  tag  - The tag to use                                    " << endl;
  cout << "                                                           " << endl;
  cout << "Options:                                                   " << endl;
  cout << "  -h,--help            Displays this help message          " << endl;
  cout << "  -v,--version         Displays the current release version" << endl;
  cout << "  --noblanks, -n       Do not include blank lines          " << endl;
  cout << "  --first, -f          Just first non-blank line after tag " << endl;
  cout << "  --keep_tag_line, -k  Include/keep tag line in output     " << endl;
  cout << "  --hdr                Use a header, set to same as tag    " << endl;
  cout << "                                                           " << endl;
  cout << "Further Notes:                                             " << endl;
  cout << "  (1) Order of file and tag arguments ARE relevant         " << endl;
  cout << "  (2) Order of other switches is not relevant              " << endl;
  cout << endl;
  exit(0);
}






