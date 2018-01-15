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
    
    if((arg == "-v") || (arg == "--verbose"))
      verbose = true;
    
    if((arg == "-p") || (arg == "--print"))
      print = true;
    
    if(strEnds(arg, ".mfs") || strEnds(arg, ".gfs")) 
      populator.addManifestFile(arg);
    if(strEnds(arg, ".loc"))
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


