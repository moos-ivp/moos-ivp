/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: May 28th, 2005                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <stdio.h>
#include <cstring>
#include <string>
#include <iostream>
#include "Expander.h"
#include "MBUtils.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  bool help_requested    = false;
  bool version_requested = false;
  for(int i=1; i<argc; i++) {
    string sarg = argv[i];
    if((sarg=="-v") || (sarg=="--version") || (sarg=="-version")) 
      version_requested = true;
    else if((sarg=="-h") || (sarg=="--help") || (sarg=="-help")) 
      help_requested = true;
  }

  if(help_requested) {
    cout << "Usage: splug filename newfilename [-f, --force][MACRO=VAL]" << endl;
    return(0);
  }

  //----------------------------------------------------------------
  // Check if version output is requested
  if(version_requested) {
    vector<string> svector = getReleaseInfo("splug");
    for(int i=0; i<svector.size(); i++)
      cout << svector[i] << endl;
    return(0);
  }

  Expander expander(argv[1], argv[2]);

  for(int i=3; i<argc; i++) {
    string arg = argv[i];
    if((arg == "-f") || (arg == "--force"))
      expander.setForce(true);
    vector<string> svector = parseString(argv[i], '=');
    if(svector.size() == 2) {
      string left  = stripBlankEnds(svector[0]);
      string right = stripBlankEnds(svector[1]);
      expander.addMacro(left, right);
    }
  }

  if(expander.verifyInfile())
    if(expander.expand())
        expander.writeOutput();
  
  return(0);
}
