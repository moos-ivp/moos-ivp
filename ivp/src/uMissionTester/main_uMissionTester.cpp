/*****************************************************************************
 * AUTHOR:      Christopher Gagner
 * DATE:        June 21, 2010
 * FILE:        main_uMissionTester.cpp
 * DESCRIPTION: The main C++ file for the uMissionTester utility. This
 *              utility is used to start/stop a mission using pAntler.
 ****************************************************************************/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
#include "MBUtils.h"
#include "MissionTester.h"
#include "MissionTesterThreadPool.h"

#ifndef FAIL
#define FAIL -1
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif

using namespace std;

string g_name = "uMissionTester";

const char* SUMMARY =
"uMissionTester is a utility that gives users the ability to start a mission,\n"
"and stop the mission automatically after a timeout period or after an end\n"
"condition has been satisfied.\n";

// Declare the needed functions
void printSummary(){printf( "\n%s\n", SUMMARY );};
void printVersion();
void printHelp(int exit_status = FAIL);
int main(int argc, char** argv);

/**
 * Print the version information for the utility. This function always exits
 * with a SUCCESS return status.
 */
void printVersion(){
    vector<string> svector = getReleaseInfo("uMissionTester");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;

    exit(SUCCESS);
} // END printVersion

/**
 * Print the help information for the utility. This function always exits
 * and returns the specified status.
 * @param exit_status Value to return
 */
void printHelp(int exit_status){
    printSummary();
    printf("Usage:\n");
    printf("\tuMissionTester file.moos [OPTIONS]\n");
    printf("\nOptions:\n");
    printf("\t--help      Displays the help contents\n");
    printf("\t--version   Displays the version information\n");
    exit(exit_status);
} // END printHelp

/**
 * Main function for uMissionTester utility. Handles parsing arguments and
 * starting/stopping the mission.
 */
int main(int argc, char** argv){

    // Check for the version flag
    if (scanArgs(argc, argv, "-v", "--version", "-version")) {
        printVersion();
    }

    // Check for the help flag
    if (scanArgs(argc, argv, "-h", "--help", "-help")) {
        printHelp(SUCCESS);
    }

    vector<string> mission_files;

    // Itterate through the remaining arguments looking for the mission file.
    for (int i = 1; i < argc; i++) {
        string argi = tolower(argv[i]);
        if (strEnds(argi, ".moos") || strEnds(argi, ".moos++")) {
            mission_files.push_back( argv[i] );
        } else {
            printf( "WARNING: Unknown argument %s\n", argv[i] );
        }// END check current argument

    } // END for-loop

    // If no mission file has been entered, print an error message and the 
    // help contents
    if( mission_files.empty() ){
        printf("**********************************************************\n");
        printf("ERROR! You must specify a mission file!\n");
        printf("**********************************************************\n");
        printHelp(FAIL);
    }

    // If only one mission file is specifed, just run the application
    if( mission_files.size() == 1){
        MissionTester m_mission_tester;
        m_mission_tester.Run(g_name.c_str(), mission_files.back().c_str());
    } else {
        // If more than one mission file is specifed, start multiple
        // applications 
        MissionTesterThreadPool m_pool(g_name, mission_files);
        m_pool.start();
        m_pool.wait();
    } // END check mission_files.size()

} // END of main



/*****************************************************************************
 *                       END of main_uMissionTester.cpp
 ****************************************************************************/

