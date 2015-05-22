/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: June 5th, 2008                                       */
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

#include <string>
#include <iostream>
#include "cmath"
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "TermUtils.h"
#include "ALogClipper.h"
#include <cstdlib>
#include <cstdio>

using namespace std;

void display_usage();

//--------------------------------------------------------
// Procedure: exit_with_usage

void display_usage()
{
  cout << "Usage: " << endl;
  cout << "  alogclip in.alog mintime maxtime [out.alog] [OPTIONS]  " << endl;
  cout << "                                                         " << endl;
  cout << "Synopsis:                                                " << endl;
  cout << "  Create a new MOOS .alog file from a given .alog file   " << endl;
  cout << "  by removing entries outside a given time window.       " << endl;
  cout << "                                                         " << endl;
  cout << "Standard Arguments:                                      " << endl;
  cout << "  in.alog  - The input logfile.                          " << endl;
  cout << "  mintime  - Log entries with timestamps below mintime   " << endl;
  cout << "             will be excluded from the output file.      " << endl;
  cout << "  maxtime  - Log entries with timestamps above mintime   " << endl;
  cout << "             will be excluded from the output file.      " << endl;
  cout << "  out.alog - The newly generated output logfile. If no   " << endl;
  cout << "             file provided, output goes to stdout.       " << endl;
  cout << "                                                         " << endl;
  cout << "Options:                                                 " << endl;
  cout << "  -h,--help     Display this usage/help message.         " << endl;
  cout << "  -v,--version  Display version information.             " << endl;
  cout << "  -f,--force    Overwrite an existing output file.       " << endl;
  cout << "  -q,--quiet    Verbose report suppressed at conclusion. " << endl;
  cout << "                                                         " << endl;
  cout << "Further Notes:                                           " << endl;
  cout << "  (1) The order of arguments may vary. The first alog    " << endl;
  cout << "      file is treated as the input file, and the first   " << endl;
  cout << "      numerical value is treated as the mintime.         " << endl;
  cout << "  (2) Two numerical values, in order, must be given.     " << endl;
  cout << "  (3) See also: alogscan, alogrm, aloggrep, alogview     " << endl;
  cout << endl;
}

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
 // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("alogclip", "gpl");
    return(0);
  }

  // Look to see if user wants a quiet operation
  bool verbose = true;
  if(scanArgs(argc, argv, "-q", "--quiet", "-quiet"))
    verbose = false;
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    display_usage();
    return(0);
  }

  bool force_overwrite = false;
  // Look for the option to force an overwrite of an existing output
  // file without prompting the user.
  if(scanArgs(argc, argv, "-f", "--force", "-force"))
    force_overwrite = true;

  bool   okargs = true;
  double min_time = 0; 
  double max_time = 0;
  bool   min_time_set = false;
  bool   max_time_set = false;
  
  string alog_infile  = "";
  string alog_outfile = "";
  for(int i=1; i<argc; i++) {
    string sarg = argv[i];

    if(strContains(sarg, ".alog")) {
      if(alog_infile == "")
	alog_infile = sarg;
      else if(alog_outfile == "")
	alog_outfile = sarg;
      else // Three alog files on command line - error.
	okargs = false;
    }
    else if(isNumber(sarg)) {
      if(!min_time_set) {
	min_time = atof(sarg.c_str());
	min_time_set = true;
      }
      else if(!max_time_set) {
	max_time = atof(sarg.c_str());
	max_time_set = true;
      }
      else // Three numerical args on command line - error.
	okargs = false;
    }
  }
  
  //----------------------------------------------------------------
  // Check that all the appropriate parameters were provided, no more
  // and no less. Also check that min/max time range is valid
  if(min_time > max_time)
    okargs = false;

  if(!okargs) {
    display_usage();
    return(0);
  }

  ALogClipper clipper;


  //-----------------------------------------------------------------
  // Check that the provided files are sensible. Handle the case where
  // the output file already exists and prompt the user for a decision
  // on whether to over-write it .

  bool ok = clipper.openALogFileRead(alog_infile);
  if(!ok) {
    cout << "Input file: " << alog_infile << 
      " does not exist - exiting." << endl;
    return(0);
  }
  
  ok = clipper.openALogFileRead(alog_outfile);
  if(ok && !force_overwrite) {
    bool done = false;
    while(!done) {
      cout << "File " << alog_outfile << " exists. Replace?(y/n)" << endl;
      char answer = getCharNoWait();
      if(answer == 'n') {
	cout << "Aborted: The file " << alog_outfile;
	cout << " will not be overwritten" << endl;
	exit(0);
      }
      if(answer == 'y')
	done = true;
    }
  }
  
  clipper.openALogFileRead(alog_infile);

  if(alog_outfile != "") {
    ok = clipper.openALogFileWrite(alog_outfile);
    if(!ok) {
      cout << "Unable to create output file: " << alog_outfile << 
	" - exiting. " << endl;
      return(0);
    }
  }

  //--------------------------------------------------------------
  // Everything is fine - so start doing the job.

  if(verbose)
    cout << "\nProcessing input file " << alog_infile << " ..." << endl;
  clipper.clip(min_time, max_time);

  //--------------------------------------------------------------
  // Display the stats for the clip process
  if(!verbose)
    return(0);

  unsigned int clipped_lines_front, clipped_lines_back, kept_lines;
  unsigned int clipped_chars_front, clipped_chars_back, kept_chars;

  clipped_lines_front = clipper.getDetails("clipped_lines_front");
  clipped_lines_back  = clipper.getDetails("clipped_lines_back");
  kept_lines          = clipper.getDetails("kept_lines");

  clipped_chars_front = clipper.getDetails("clipped_chars_front");
  clipped_chars_back  = clipper.getDetails("clipped_chars_back");
  kept_chars          = clipper.getDetails("kept_chars");

  unsigned int clipped_lines_total = clipped_lines_front+clipped_lines_back;
  unsigned int clipped_chars_total = clipped_chars_front+clipped_chars_back;

  
  double clipped_lines_pct = 100.0 * (double)(clipped_lines_total) /
    (double)(clipped_lines_total + kept_lines);
  
  double clipped_chars_pct = 100.0 * (double)(clipped_chars_total) /
    (double)(clipped_chars_total + kept_chars);

  string lpct = doubleToString(clipped_lines_pct, 2);
  string cpct = doubleToString(clipped_chars_pct, 2);


  string clipped_lines_total_s = intToCommaString(clipped_lines_total);
  string clipped_lines_front_s = intToCommaString(clipped_lines_front);
  string clipped_lines_back_s  = intToCommaString(clipped_lines_back);

  string clipped_chars_total_s = intToCommaString(clipped_chars_total);
  string clipped_chars_front_s = intToCommaString(clipped_chars_front);
  string clipped_chars_back_s  = intToCommaString(clipped_chars_back);

  string digits = intToString(clipped_chars_total_s.length());

  //#ifdef _WIN32
  //string digits = intToString((int)(log10( (double)clipped_chars_total)));
  //#else
  //string digits = intToString((int)(log10(clipped_chars_total)));
  //#endif

  printf("\n\n");

  string format = "Total lines clipped:   %" + digits + "s  (%s pct)\n";
  printf(format.c_str(), clipped_lines_total_s.c_str(), lpct.c_str());

  format = "  Front lines clipped: %" + digits + "s \n";
  printf(format.c_str(), clipped_lines_front_s.c_str());
  format = "  Back  lines clipped: %" + digits + "s \n";
  printf(format.c_str(), clipped_lines_back_s.c_str());
	 

  format = "Total chars clipped:   %" + digits + "s  (%s pct)\n";
  printf(format.c_str(), clipped_chars_total_s.c_str(), cpct.c_str());

  format = "  Front chars clipped: %" + digits + "s \n";
  printf(format.c_str(), clipped_chars_front_s.c_str());

  format = "  Back  chars clipped: %" + digits + "s \n";
  printf(format.c_str(), clipped_chars_back_s.c_str());
  
}





