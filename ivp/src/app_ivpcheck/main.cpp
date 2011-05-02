/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: April 18th, 2011                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include <cstdlib>
#include <iostream>
#include "MBUtils.h"
#include "IvPChecker.h"
#include "IC_GUI.h"
#include "Populator_IPFBundleSeries.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("ivpcheck");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }

  bool verbose = true;
  if(scanArgs(argc, argv, "--verbose", "-verbose"))
    verbose = true;
  if(scanArgs(argc, argv, "--quiet", "-quiet", "-q"))
    verbose = false;
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: " << endl;
    cout << "  ivpcheck in.alog iteration [out.alog] [OPTIONS]    " << endl;
    cout << "                                                     " << endl;
    cout << "Synopsis:                                            " << endl;
    cout << "  Read in IvP functions from a given alog file for   " << endl;
    cout << "  a given helm iteration and solve the corresponding " << endl;
    cout << "  IvP problem.  " << endl;
    cout << "                                                     " << endl;
    cout << "Standard Arguments:                                  " << endl;
    cout << "  in.alog  - The input logfile.                      " << endl;
    cout << "  out.alog - The newly generated logfile. If no      " << endl;
    cout << "             file provided, output goes to stdout.   " << endl;
    cout << "                                                     " << endl;
    cout << "Options:                                             " << endl;
    cout << "  -h,--help     Displays this help message           " << endl;
    cout << "  -v,--version  Displays the current release version " << endl;
    cout << endl;
    return(0);
  }

  unsigned int iteration = 0;
  string alogfile_in;
  string alogfile_out;
  bool   check_brute = true;

  for(int i=1; i<argc; i++) {
    string sarg = argv[i];
    if(sarg == "--nobrute")
      check_brute = false;
    else if(strContains(sarg, ".alog")) {
      if(alogfile_in == "")
	alogfile_in = sarg;
      else 
	alogfile_out = sarg;
    }
    else
      iteration = atof(argv[i]);
  }
  
  if(alogfile_in == "") {
    cout << "No alog file given - exiting" << endl;
    exit(0);
  }
  else if(verbose)
    cout << "Processing on file : " << alogfile_in << endl;

  Populator_IPFBundleSeries populator;
  populator.setIterationRange(iteration, iteration);

  bool ok = populator.readFile(alogfile_in);
  if(!ok) {
    cout << "Problem reading the given .alog file. Exiting." << endl;
    exit(0);
  }
  
  IPF_BundleSeries bundle_series = populator.getIPF_BundleSeries();
  if(bundle_series.size() == 0) {
    cout << "Empty bundle series. Exiting." << endl;
    exit(0);
  }

  IC_GUI *gui  = new IC_GUI(1000, 700, "ivpcheck-viewer");

  gui->getViewer()->setIPF_BundleSeries(bundle_series);
  gui->getViewer()->setCheckBrute(check_brute);

  gui->getViewer()->resetQuadSet();
  gui->getViewer()->redraw();
  gui->updateFields();
  
  gui->getViewer()->checkSolutions();

  return Fl::run();
}

