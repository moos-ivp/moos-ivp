/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp (utest)                                     */
/*    DATE: Nov 5th, 2018                                        */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "UnitTester.h"

using namespace std;

int main(int argc, char** argv) 
{
  UnitTester utester;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    bool ok = false;
    if((argi == "--help") || (argi == "-h")) {
      cout << "Usage: utest file.utf" << endl;
      return(0);
    }
    if((argi == "--verbose") || (argi == "-v"))
      ok = utester.addVerbosity();
    else if(strBegins(argi, "--file="))
      ok = utester.addTestFile(argi.substr(7));
    else if(strEnds(argi, ".utf"))
      ok = utester.addTestFile(argi);

    if(!ok) {
      cout << "Error arg: " << argi << " Exiting." << endl;
      return(1);
    }
  }

  bool all_ok = utester.runUnitTests();
  if(!all_ok)
    return(1);
  return(0);
}
