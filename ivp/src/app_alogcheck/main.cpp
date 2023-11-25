/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: alogcheck_main.cpp                                   */
/*    DATE: July 29, 2010                                        */
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

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "LogChecker.h"
#include "LogChecker_Info.h"


#ifndef FAIL
#define FAIL -1
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif


using namespace std;

string g_name = "alogcheck";

// Declare the needed functions
void printVersion();

int main(int argc, char** argv);


/**
 * Print the version information for the utility. This function always exits
 * with a SUCCESS return status.
 */
void printVersion() {
  showReleaseInfo(g_name, "gpl");
  exit(SUCCESS);
} 

int main(int argc, char** argv) 
{
  // Check for the version flag
  if(scanArgs(argc, argv, "-v", "--version", "-version")) 
    printVersion();
  
  // Check for the help flag
  if(scanArgs(argc, argv, "-h", "--help", "-help"))
    showHelpAndExit();
  
  string alog_file   = "";
  string input_file  = "";
  string output_file = "";
  LogChecker m_checker;
  
  // Itterate over all of the arguments and check for valid flags
  for(int i=1; i< argc; i++){
    string argi = argv[i];
    if( strEnds(argi, ".alog") ){
      alog_file = argi;
    } 

    else if(argi == "-i" || argi == "--input") {
      // An input file is desired - check for another argument
      if( i+1 < argc ) {
	// Set the input file to the next argument
	input_file = argv[++i];
      } else {
	// There is an incorrect number of arguments
	showHelpAndExit();
      } // END check for another argument
    } 

    else if(argi == "-o" || argi == "--output") {
      // An output file is desired - check for another argument
      if((i+1) < argc) {
	// Set the output file to the next argument
	output_file = argv[++i];
      } 
      else {
	// There is an incorrect number of arguments
	showHelpAndExit();
      } // END check for another argument
    } 

    else if(argi == "-f" || argi == "--fail") {
      // A Fail flag has been specified- check for another argument
      if( i+1 < argc ){
	string flag = argv[++i];
	// Try to add the fail flag to the checker
	if( !m_checker.addFailFlag( flag ) ){
	  printf("ERROR: Bad Fail condition: %s\n", flag.c_str() );
	  return FAIL;
	} // END Check add fail flag
      } 
      else 
	showHelpAndExit(); // incorrect number of args
    }

    else if(argi == "-p" || argi == "--pass") {
      // A Pass flag has been specified - check for another argument
      if( i+1 < argc ){
	string flag = argv[++i];
	// Try to add the pass flag to the checker
	if( !m_checker.addPassFlag( flag ) ){
	  printf("ERROR: Bad Pass condition: %s\n", flag.c_str() );
	  return FAIL;
	} // END check add pass flag
      } else {
	// There is an incorrect number of arguments
	showHelpAndExit();
      } // END check for another argument
    }

    else if(argi == "-s" || argi == "--start") {
      // A start flag has been specified - check for another argument
      if(i+1 < argc) {
	string flag = argv[++i];
	// Try to add the start flag to the checker
	if( !m_checker.addStartFlag( flag ) ){
	  printf("ERROR: Bad Start condition: %s\n", flag.c_str() );
	  return FAIL;
	} // END check add Start flag
      } else {
	// There is an incorrect number of arguments
	showHelpAndExit();
      } // END check for another argument
    } 

    else if(argi == "-e" || argi == "--end") {
      // An end flag has been specified - check for another argument
      if( i+1 < argc ){
	string flag = argv[++i];
	// Try to add the end flag to the checker
	if(!m_checker.addEndFlag( flag)){
	  printf("ERROR: Bad Start condition: %s\n", flag.c_str() );
	  return FAIL;
	} // END check add end flag
            } else {
                // There is an incorrect number of arguments
                showHelpAndExit();
            } // END check for another argument
        } else if (argi == "--verbose"){
            // The verbose flag has been specified - Set checker to verbose
            m_checker.setVerbose(true);
        } else {
            // Unknow argument
            printf("Unknow argument: %s\n", argi.c_str() );
            showHelpAndExit();
        } 
    } 


    // Check if the log file is empty
    if(alog_file.empty()) {
      printf("No alog file given - exiting\n");
      return(FAIL);
    } 
    
    // Check if we need to parse the input file
    if(!input_file.empty() ) {
      if(!m_checker.parseInputFile(input_file)) {
	printf("ERROR! Could not read input file: %s\n",
	       input_file.c_str());
      } 
    } 

#ifdef DEBUG
    printf("Running in Debug mode\n");
#endif
    
    // Run the checker on the logfile
    if(m_checker.check(alog_file, output_file) ) {
      printf("PASSED\n");
      return SUCCESS;
    } 
    else {
      printf("FAILED\n");
      return(FAIL);
    } 
    
} 


/*****************************************************************************
 * END of alogcheck_main.cpp
 ****************************************************************************/










