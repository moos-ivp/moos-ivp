/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: May 28th, 2005                                       */
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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include "ColorParse.h"
#include "Expander.h"
#include "Expander_Info.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  Expander expander;

  bool input_file_provided  = false;
  bool output_file_provided = false;

  for(int i=1; i<argc; i++) {
    string arg = argv[i];

    if((arg=="-v") || (arg=="--version") || (arg=="-version"))
      showReleaseInfoAndExit();
    else if((arg == "-h") || (arg == "--help") || (arg=="-help"))
      showHelpAndExit();
    else if((arg == "-m") || (arg == "--manual"))
      showManualAndExit();
    else if((arg == "-f") || (arg == "--force"))
      expander.setForce(true);
    else if(strBegins(arg, "--path=")) 
      expander.addPath(arg.substr(7));
    else if(((arg=="-p") || (arg=="--path")) && (i<(argc-1))) 
      expander.addPath(argv[i+1]);
    else if(((arg=="-s") || (arg=="--strict")))
      expander.setStrict(true);
    else if(((arg=="-l") || (arg=="--lenient")))
      expander.setPartialsOK(true);
    else if(((arg=="-i") || (arg=="--interactive")))
      expander.setInteractive(true);
    else if(((arg=="-t") || (arg=="--terminal")))
      expander.setTerminal(true);

    // If none of the above switch options explicitly match...
    else if(i == 1) {
      expander.setInFile(argv[1]);
      input_file_provided = true;
    }
    else if(i == 2) {
      expander.setOutFile(argv[2]);
      output_file_provided = true;
    }
    else if(strContains(arg, '=')) {
      string left  = biteStringX(arg, '=');
      string right = arg;
      expander.addMacro(left, right);
    }
    else
      expander.addMacro(arg, "<defined>");
  }

  if(!input_file_provided) {
    cout << "Aborted: An input file must be provided." << endl;
    exit(EXIT_FAILURE);
  }

  if(!output_file_provided) {
    cout << "Aborted: An output file must be provided" << endl;
    exit(EXIT_FAILURE);
  }

  if(expander.verifyInfile()) {
    if(expander.expand())
      expander.writeOutput();
    else
      exit(EXIT_FAILURE);
  }
  else {
    cout << "Aborted: " << argv[1] << " cannot be opened. " << endl;
    exit(EXIT_FAILURE);
  }
  
  return(0);
}









