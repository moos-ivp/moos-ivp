/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Dec 14th, 2012 (Sandy Hook)                          */
/*    DATE: Dec 17th, 2017 (Syndey) Resumed development          */
/*****************************************************************/

#include <cstdlib>
#include <string>
#include <iostream>
#include "Populator_ManifestSet.h"
#include "ManifestHandler.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  Populator_ManifestSet populator;
  
  bool verbose = false;
  bool print   = false;
  
  for(int i=1; i<argc; i++) {
    string arg = argv[i];
    
    if((arg == "-h") || (arg == "--help") || (arg == "-help"))
      showHelpAndExit();
    else if((arg == "-v") || (arg == "--version") || (arg == "-version")) {
      showReleaseInfo("manifest_wiki", "gpl");
      return(0);
    }

    else if(arg == "--verbose")
      verbose = true;
    else if((arg == "-p") || (arg == "--print"))
      print = true;
    else if(strEnds(arg, ".mfs") || strEnds(arg, ".gfs")) 
      populator.addManifestFile(arg);
    else if(strEnds(arg, ".loc"))
      populator.addLOCFile(arg);
  }

  populator.setExitOnFailure(false);
  populator.setVerbose(verbose);
  populator.populate();
  ManifestSet mset = populator.getManifestSet();

  if(print)
    mset.print();

  ManifestHandler handler;
  handler.setManifestSet(mset);
  handler.setVerbose(verbose);

  handler.genAllManifestPages();
  handler.genGroupManifestPages();
  
  return(0);
}

//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << endl;
  cout << "Usage: manifest_wiki file.mfs file.gfs file.loc [OPTIONS]  " << endl;
  cout << "                                                           " << endl;
  cout << "Synopsis:                                                  " << endl;
  cout << "  Generate wiki web page content given the manifest and    " << endl;
  cout << "  lines-of-code statistic files. A web page for each module" << endl;
  cout << "  and each group will be created. Cross-correlation of     " << endl;
  cout << "  groups, dependencies and lines-of-code are calculated and" << endl;
  cout << "  reflected in the web page content.                       " << endl;
  cout << "                                                    " << endl;
  cout << "Options:                                                   " << endl;
  cout << "  --help, -h           Display this help message           " << endl;
  cout << "  --verbose,           Enable verbose output               " << endl;
  cout << "  --version, -v,       Display the release version         " << endl;
  cout << "  --print, -p,         Print Manifest to terminal          " << endl;
  cout << "                                                           " << endl;
  cout << "Example:                                                   " << endl;
  cout << " $ manifest_wiki *.mfs *.loc *.gfs                         " << endl;
  cout << " $ manifest_wiki *.mfs *.loc *.gfs -p                      " << endl;
  cout << "                                                           " << endl;
  cout << "Further notes:                                             " << endl;
  cout << " (1) Order of files is irrelevant.                         " << endl;
  cout << " (2) See also manifest_test                                " << endl;
  exit(0);
}
