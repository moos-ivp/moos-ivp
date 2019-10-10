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
      cout << "Usage:                                            " << endl;
      cout << "  utest file.utf [OPTIONS]                        " << endl;
      cout << "                                                  " << endl;
      cout << "Synopsis:                                         " << endl;
      cout << "  The utest utility will open a test (.utf) file, " << endl;
      cout << "  look for a command to test, and a set of test   " << endl;
      cout << "  cases, one per line.                            " << endl;
      cout << "  It will run the command on each test case and   " << endl;
      cout << "  compare it to the expected result for each case " << endl;
      cout << "                                                  " << endl;
      cout << "Returns (shell value in $?)                       " << endl;
      cout << "  0 if all cases are passed                       " << endl;
      cout << "  1 otherwise.                                    " << endl;
      cout << "                                                  " << endl;
      cout << "Output:                                           " << endl;
      cout << "  In non-verbose mode, nothing                    " << endl;
      cout << "  In verbose mode it will show the command used   " << endl;
      cout << "  and the result for each case                    " << endl;
      cout << "                                                  " << endl;
      cout << "Options:                                          " << endl;
      cout << "  -h, --help    Displays this help message        " << endl;
      cout << "  -v, --verbose                                   " << endl;
      cout << "                                                  " << endl;
      cout << "  --file=FILE   Test file to be used. If file ends" << endl;
      cout << "                in .utf, then --file= not needed  " << endl;
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
