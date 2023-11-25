/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Jan 9th, 2018                                        */
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

#include <cstdlib>
#include <string>
#include <iostream>
#include "Populator_ManifestSet.h"
#include "ManifestTester.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  Populator_ManifestSet populator;
  
  bool verbose  = false;
  bool print    = false;
  bool warnings = false;
  bool depends  = false;
  bool stats    = false;

  string ignore_libs;
  unsigned int file_count = 0;
  
  for(int i=1; i<argc; i++) {
    string arg = argv[i];
    
    if((arg == "-h") || (arg == "--help") || (arg == "-help"))
      showHelpAndExit();
    else if((arg == "-v") || (arg == "--version") || (arg == "-version")) {
      showReleaseInfo("manifest_test", "gpl");
      return(0);
    }
    if(arg == "--verbose")
      verbose = true;
    else if(strBegins(arg, "--ignore_libs="))
      ignore_libs = arg.substr(14);
    else if((arg == "-p") || (arg == "--print"))
      print = true;
    else if((arg == "-w") || (arg == "--warnings"))
      warnings = true;
    else if((arg == "-d") || (arg == "--depends"))
      depends = true;
    else if((arg == "-s") || (arg == "--stats"))
      stats = true;
    else if((arg == "-a") || (arg == "--all")) {
      depends = true;
      warnings = true;
      stats = true;
    }
    
    if(strEnds(arg, ".mfs") || strEnds(arg, ".gfs")) {
      file_count++;
      populator.addManifestFile(arg);
    }
    if(strEnds(arg, ".loc")) {
      file_count++;
      populator.addLOCFile(arg);
    }
  }

  cout << "=================== TEST BEGIN ========================" << endl;
  populator.setVerbose(verbose);
  bool ok = populator.populate();
  
  if(!ok) {
    cout << "Phase 1: All files readable with valid syntax:     FAIL" << endl;
    cout << "=================== TEST COMPLETE =====================" << endl;
    exit(1);
  }
  else {
    cout << "Phase 1: All files readable with valid syntax:     PASS" << endl;
    if(verbose)
      cout << "         " << file_count << " files checked.       " << endl;
  }
  
  ManifestSet mset = populator.getManifestSet();
  
  if(print) {
    mset.print();
    exit(0);
  }
    
  ManifestTester tester;
  tester.setTestWarnings(warnings);
  tester.setTestDepends(depends);
  tester.setTestStats(stats);
  tester.setManifestSet(mset);
  tester.setVerbose(verbose);
  tester.setIgnoreLibs(ignore_libs);

  bool all_tests_pass = tester.test();

  cout << "=================== TEST COMPLETE =====================" << endl;
  if(!all_tests_pass)
    return(1);
  return(0);
}

//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "Usage: manifest_test [OPTIONS] file.mfs  file.loc          " << endl;
  cout << "===========================================================" << endl;
  cout << "                                                           " << endl;
  cout << "SYNOPSIS:                                                  " << endl;
  cout << "-----------------------------------                        " << endl;
  cout << "  manifest_test is a utility for checking the correctness  " << endl;
  cout << "  of one or more manifest (.mfs) files, and one or more    " << endl;
  cout << "  lines of code (.loc) files.                              " << endl;
  cout << "                                                           " << endl;
  cout << "Options:                                                   " << endl;
  cout << "  --help, -h                                               " << endl;
  cout << "      Display this help message                            " << endl;
  cout << "  --verbose                                                " << endl;
  cout << "      Set to run in verbose mode                           " << endl;
  cout << "  --version, -v                                            " << endl;
  cout << "      Show release version of manifest_test                " << endl;
  cout << "  --warnings, -w                                           " << endl;
  cout << "      Show manifest warnings of missing optional fields    " << endl;
  cout << "  --depends, -d                                            " << endl;
  cout << "      Show manifest warnings of unknown dependencies       " << endl;
  cout << "  --stats, -s                                              " << endl;
  cout << "      Show manifest warnings of missing lines-of-code stats " << endl;
  cout << "  --all, -a                                                " << endl;
  cout << "      Show manifest warnings of all types                  " << endl;
  cout << "                                                           " << endl;
  cout << "Notes:                                                     " << endl;
  cout << "  (1) The order of arguments is not significant            " << endl;
  cout << "  (2) Lines of code (.loc) files are optional              " << endl;
  cout << "                                                           " << endl;
  cout << "Examples:                                                  " << endl;
  cout << "  $ manifest_test *.mfs                                    " << endl;
  cout << "  $ manifest_test *.mfs *.loc ../others/*.mfs              " << endl;
  cout << "                                                           " << endl;
  exit(0);
}

