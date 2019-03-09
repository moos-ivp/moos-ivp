/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Jan 9th, 2018                                        */
/*****************************************************************/

#include <cstdlib>
#include <string>
#include <iostream>
#include "LOCTester.h"
#include "MBUtils.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  bool verbose = false;

  string loc_file1, loc_file2;
  
  for(int i=1; i<argc; i++) {
    string arg = argv[i];
    
    if((arg == "-h") || (arg == "--help") || (arg == "-help"))
      showHelpAndExit();
    
    if(strEnds(arg, ".loc")) {
      if(loc_file1 == "")
	loc_file1 = arg;
      else if(loc_file2 == "")
	loc_file2 = arg;
      else {
	cout << "Only two .loc files should be provided. Exiting now." << endl;
	return(1);
      }
    }
  }
  
  cout << "=================== LOC TESTER =====================" << endl;
  cout << "File 1: [" << loc_file1 << "]" << endl;
  cout << "File 2: [" << loc_file2 << "]" << endl;

  if((loc_file1 == "") || (loc_file2 == "")) {
    cout << "Two .loc files must be provided. Exiting now." << endl;
    return(1);
  }
  
  LOCTester tester;

  bool ok;
  ok = tester.setFile(loc_file1, 0);
  if(!ok) {
    cout << "Problem opening or reading file1: [" << loc_file1 << endl;
    return(1);
  }

  ok = tester.setFile(loc_file2, 1);
  if(!ok) {
    cout << "Problem opening or reading file1: [" << loc_file2 << endl;
    return(1);
  }

  tester.test();
  return(0);
}

//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "Usage: manifest_loc [OPTIONS] file1.loc  file2.loc         " << endl;
  cout << "===========================================================" << endl;
  cout << "                                                           " << endl;
  cout << "SYNOPSIS:                                                  " << endl;
  cout << "-----------------------------------                        " << endl;
  cout << "  manifest_loc is a utility for checking two lines-of-code " << endl;
  cout << "  (.loc) files and reporting the differences. It will      " << endl;
  cout << "  any two same-named modules with different counts. It will" << endl;
  cout << "  also report any module in one file not known to the other" << endl;
  cout << "  file                                                     " << endl;
  cout << "                                                           " << endl;
  cout << "Options:                                                   " << endl;
  cout << "  --help, -h                                               " << endl;
  cout << "      Display this help message                            " << endl;
  cout << "                                                           " << endl;
  cout << "Examples:                                                  " << endl;
  cout << "  $ manifest_loc file1.loc file2.loc                       " << endl;
  cout << "                                                           " << endl;
  exit(0);
}
