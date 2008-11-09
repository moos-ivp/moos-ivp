/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: June 5th, 2008                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include <iostream>
#include "math.h"
#include "MBUtils.h"
#include "TermUtils.h"
#include "ALogClipper.h"
#include <cstdlib>

using namespace std;

//--------------------------------------------------------
// Procedure: exit_with_usage

void display_usage()
{
  cout << "Usage: " << endl;
  cout << "  alogclip in.alog out.alog mintime maxtime" << endl;
  cout << "                                           " << endl;
  cout << "  Order of arguments may vary. The first alog file is   " << endl;
  cout << "  treated as the input file, and the 2nd is the output. " << endl;
  cout << "  The first numerical argument is treated as the mintime" << endl;
  cout << "  and the 2nd is treated as the maxtime. If more than   " << endl;
  cout << "  two alog files, or more than two numerical arguments  " << endl;
  cout << "  are provided, this usage message will be displayed.   " << endl;
}

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
 // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("alogclip");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    display_usage();
    return(0);
  }

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
    exit(0);
  }
    
  ok = clipper.openALogFileRead(alog_outfile);
  if(ok) {
    bool done = false;
    while(!done) {
      cout << "File " << alog_outfile << " exists. Replace?(y/n)" << endl;
      char answer = getCharNoWait();
      if(answer == 'n') {
	cout << "Aborted: The file " << alog_outfile;
	cout << " will not be created" << endl;
	exit(0);
      }
      if(answer == 'y')
	done = true;
    }
  }
  
  clipper.openALogFileRead(alog_infile);

  ok = clipper.openALogFileWrite(alog_outfile);
  if(!ok) {
    cout << "Unable to create output file: " << alog_outfile << 
      " - exiting. " << endl;
    exit(0);
  }

  //--------------------------------------------------------------
  // Everything is fine - so start doing the job.
    
  cout << "\nProcessing input file " << alog_infile << "..." << endl;
  clipper.clip(min_time, max_time);

  //--------------------------------------------------------------
  // Display the stats for the clip process

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

  string digits = intToString((int)(log10(clipped_chars_total)));

  printf("\n\n");

  string format = "Total lines clipped:   %" + digits + "d  (%s pct)\n";
  printf(format.c_str(), clipped_lines_total, lpct.c_str());

  format = "  Front lines clipped: %" + digits + "d \n";
  printf(format.c_str(), clipped_lines_front);
  format = "  Back  lines clipped: %" + digits + "d \n";
  printf(format.c_str(), clipped_lines_back);
	 
  format = "Total chars clipped  : %" + digits + "d  (%s pct)\n";
  printf(format.c_str(), clipped_chars_total, cpct.c_str());

  format = "  Front chars clipped: %" + digits + "d \n";
  printf(format.c_str(), clipped_chars_front);

  format = "  Back  chars clipped: %" + digits + "d \n";
  printf(format.c_str(), clipped_chars_back);
  
}






