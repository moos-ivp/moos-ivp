/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: MIT Dept of Mechanical Eng                           */
/*    FILE: main.cpp                                             */
/*    DATE: March 8th 2024                                       */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "Plucker.h"
#include "Plucker_Info.h"

using namespace std;

int main(int argc, char *argv[])
{
  Plucker plucker;

  if(argc == 1)
    showHelpAndExit();

  bool newline = false;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];

    bool handled = false;
    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      showReleaseInfoAndExit();
    else if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if(strBegins(argi, "--file="))
      handled = plucker.setFile(argi.substr(7));
    else if(strBegins(argi, "--lnum="))
      handled = plucker.setLNum(argi.substr(7));
    else if(strBegins(argi, "--ix="))
      handled = plucker.setIndex(argi.substr(5));
    else if(strBegins(argi, "--fld="))
      handled = plucker.setFld(argi.substr(6));
    else if(strBegins(argi, "--gsep="))
      handled = plucker.setGSep(argi.substr(7));
    else if(strBegins(argi, "--lsep="))
      handled = plucker.setLSep(argi.substr(7));
    else if(strBegins(argi, "--grep="))
      handled = plucker.setGrep(argi.substr(7));
    else if(strBegins(argi, "-ch"))
      handled = plucker.setComment("#");
    else if(strBegins(argi, "-cp"))
      handled = plucker.setComment("%");
    else if(strBegins(argi, "-css"))
      handled = plucker.setComment("//");
    else if(argi == "-nl") {
      newline = true;
      handled = true;
    }
    else if(isNumber(argi))
      handled = plucker.setLNum(argi);
    else
      handled = plucker.setFile(argi);
    
    if(!handled) {
      cout << "pluck: Unhandled arg: " << argi << endl;
      return(1);
    }
  }

  string result = plucker.pluck();
  if(result == "")
    return(1);

  cout << result << flush;
  if(newline)
    cout << endl;
  return(0);
}
