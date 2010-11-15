/*****************************************************************************
 * AUTHOR:      Christopher Gagner
 * DATE:        July 29, 2010
 * FILE:        alogcheck_main.cpp
 * DESCRIPTION: Checks an alog for specified conditions and reports if those
 *              conditions have been satisfied.
 ****************************************************************************/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
#include "MBUtils.h"
#include "LogChecker.h"


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
void printHelp(int exit_status = FAIL);
int main(int argc, char** argv);


/**
 * Print the version information for the utility. This function always exits
 * with a SUCCESS return status.
 */
void printVersion(){
    vector<string> svector = getReleaseInfo( g_name.c_str() );
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
    printf("\nSynopsis:\n"
           "   alogcheck is an application that checks if a specified condition has been\n"
           "   satisfied in an alog file.\n");
    printf("\nUsage:\n");
    printf("   %s in.log [OPTIONS]\n", g_name.c_str() );
    printf("\nOptions:\n");

    printf("   -s, --start [COND]      - Delays checking other conditions until these\n"
           "                             conditions has been satisfied. If this flag is used\n"
           "                             multiple times, the conditions will be conjoined.\n");
    printf("   -e, --end [COND]        - Stops checking other conditions once these\n"
           "                             conditions has been satisfied. If this flag is used\n"
           "                             multiple times, the conditions will be conjoined.\n");
    printf("   -p, --pass [COND]       - Conditions that if found will result in the check\n"
           "                             passing. If this flag is used multiple times, the\n"
           "                             conditions will be conjoined.\n");
    printf("   -f, --fail [COND]       - Conditions that if found will result in the check\n"
           "                             failing. If this flag is used multiple times, the\n"
           "                             conditions will be conjoined.\n");
    printf("   -i, --input [file]      - Reads in arguments from the specified file. All\n"
           "                             arguments that take in conditions can be used\n"
           "                             in the input file.\n");
    printf("   -o, --output [file]     - Prints the output from the checker to a file\n");
    printf("   --verbose               - Sets the checker to be verbose\n");
    printf("\nGlobal options:\n");
    printf("   -h, --help              - Displays the help contents\n");
    printf("   -v, --version           - Displays the version information\n");
    printf("\nAdditional Variables:\n");
    printf("   ALOG_TIMESTAMP          - The current timestamp in the alog file. This\n"
           "                             variable can be used in logic conditions.\n");
    printf("\nHelpful Hints:\n");
    printf("   1. The results of a check are based on if ALL pass conditions have been\n"
           "      satisfied and NONE of the fail conditions have been satisfied after\n"
           "      the start conditions have been satisfied and before the end conditions\n"
           "      have been satisfied. If start and end conditions are not specified, the\n"
           "      entire log will be checked.\n"
           "   2. Condition strings should be wrapped in quotes since some logic expressions\n"
           "      may be interpreted by the shell instead of being passed in as arguments.\n"
           "      For instance, on Linux if \"NAV_X>100\" is not wrapped in quotes, the\n"
           "      application output will be redirected to a file named '100'.\n"
           "   3. When using the logical-AND or logical-OR operators, expressions MUST use\n"
           "      parentheses: (NAV_X<0) OR (NAV_X>100)\n");
    printf("\nExample:\n");
    printf("   alogcheck --start \"ALOG_TIMESTAMP > 20\" --end \"ALOG_TIMESTAMP > 500\"\n"
           "     --pass \"DEPLOY==false\" --pass \"MISSION_COMPLETE=true\" \n"
           "     --fail \"((NAV_X > 100) OR (NAV_X < 0)) OR ((NAV_Y < 0) OR (NAV_Y > 100))\"\n"
           "     alpha_mission.alog\n\n");

    exit(exit_status);
} // END printHelp


/**
 * Main function
 */
int main(int argc, char** argv) {


    // Check for the version flag
    if (scanArgs(argc, argv, "-v", "--version", "-version")) {
        printVersion();
    } // END check for the version flag

    // Check for the help flag
    if (scanArgs(argc, argv, "-h", "--help", "-help")) {
        printHelp(SUCCESS);
    } // END check for the help flag

    string alog_file = "";
    string input_file = "";
    string output_file = "";
    LogChecker m_checker;
    
    // Itterate over all of the arguments and check for valid flags
    for(int i=1; i< argc; i++){
        string argi = argv[i];
        if( strEnds(argi, ".alog") ){
            alog_file = argi;
        } else if(argi == "-i" || argi == "--input") {
            // An input file is desired - check for another argument
            if( i+1 < argc ){
                // Set the input file to the next argument
                input_file = argv[++i];
            } else {
                // There is an incorrect number of arguments
                printHelp();
            } // END check for another argument
        } else if(argi == "-o" || argi == "--output") {
            // An output file is desired - check for another argument
            if( i+1 < argc ){
                // Set the output file to the next argument
                output_file = argv[++i];
            } else {
                // There is an incorrect number of arguments
                printHelp();
            } // END check for another argument
        } else if(argi == "-f" || argi == "--fail") {
            // A Fail flag has been specified- check for another argument
            if( i+1 < argc ){
                string flag = argv[++i];
                // Try to add the fail flag to the checker
                if( !m_checker.addFailFlag( flag ) ){
                    printf("ERROR: Bad Fail condition: %s\n", flag.c_str() );
                    return FAIL;
                } // END Check add fail flag
            } else {
                // There is an incorrect number of arguments
                printHelp();
            } // END check for another argument
        }else if(argi == "-p" || argi == "--pass") {
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
                printHelp();
            } // END check for another argument
        }else if(argi == "-s" || argi == "--start") {
            // A start flag has been specified - check for another argument
            if( i+1 < argc ){
                string flag = argv[++i];
                // Try to add the start flag to the checker
                if( !m_checker.addStartFlag( flag ) ){
                    printf("ERROR: Bad Start condition: %s\n", flag.c_str() );
                    return FAIL;
                } // END check add Start flag
            } else {
                // There is an incorrect number of arguments
                printHelp();
            } // END check for another argument
        } else if(argi == "-e" || argi == "--end") {
            // An end flag has been specified - check for another argument
            if( i+1 < argc ){
                string flag = argv[++i];
                // Try to add the end flag to the checker
                if( !m_checker.addEndFlag( flag ) ){
                    printf("ERROR: Bad Start condition: %s\n", flag.c_str() );
                    return FAIL;
                } // END check add end flag
            } else {
                // There is an incorrect number of arguments
                printHelp();
            } // END check for another argument
        } else if (argi == "--verbose"){
            // The verbose flag has been specified - Set checker to verbose
            m_checker.setVerbose(true);
        } else {
            // Unknow argument
            printf("Unknow argument: %s\n", argi.c_str() );
            printHelp();
        } // END check arguments
    } // END for-loop over all arguments


    // Check if the log file is empty
    if( alog_file.empty() ){
        printf("No alog file given - exiting\n");
        return FAIL;
    } // END check logfile

    // Check if we need to parse the input file
    if( ! input_file.empty() ){
        if( !m_checker.parseInputFile(input_file) ){
            printf("ERROR! Could not read input file: %s\n",
                    input_file.c_str());
        } // END check parse input file
    } // END check input file

    #ifdef DEBUG
    printf("Running in Debug mode\n");
    #endif

    // Run the checker on the logfile
    if( m_checker.check(alog_file, output_file) ) {
        printf("PASSED\n");
        return SUCCESS;
    } else {
        printf("FAILED\n");
        return FAIL;
    } // Check the return of the checker

} // END of main


/*****************************************************************************
 * END of alogcheck_main.cpp
 ****************************************************************************/
