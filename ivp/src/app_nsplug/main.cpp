/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: May 28th, 2005                                       */
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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include "Expander.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("nsplug", "gpl");
    return(0);
  }
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: nsplug filename newfilename [-f, --force][MACRO=VAL]" << endl;
    return(0);
  }

  Expander expander(argv[1], argv[2]);

  for(int i=3; i<argc; i++) {
    string arg = argv[i];
    if((arg == "-f") || (arg == "--force"))
      expander.setForce(true);
    else if(((arg=="-p") || (arg=="--path")) && (i<(argc-1)))
      expander.addPath(argv[i+1]);
    else if(((arg=="-s") || (arg=="--strict")))
      expander.setStrict(true);
    else if(strContains(arg, '=')) {
      string left  = stripBlankEnds(biteString(arg, '='));
      string right = stripBlankEnds(arg);
      expander.addMacro(left, right);
    }
    else
      expander.addMacro(arg, "<defined>");
  }

  if(expander.verifyInfile()) {
    if(expander.expand())
        expander.writeOutput();
    else
        exit(EXIT_FAILURE);
    
  }
  else
  {
      cout << "Aborted: " << argv[1] << " cannot be opened. " << endl;
      exit(EXIT_FAILURE);
  }
  
  return(0);
}

