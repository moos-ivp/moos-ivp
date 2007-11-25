/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LogClipper.cpp                                       */
/*    DATE: Nov 5th, 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <stdio.h>
#include <iostream>
#include "TermUtils.h"
#include "MBUtils.h"
#include "FileBuffer.h"
#include "LogClipper.h"

using namespace std;

//--------------------------------------------------------
// Constructor

LogClipper::LogClipper(string g_infile, string g_outfile)
{
  infile    = g_infile;
  outfile   = g_outfile;
}

//--------------------------------------------------------
// Procedure: clip
//     Notes: key functions, parse() and stripBlankEnds()
//            are from MBUtils.h

bool LogClipper::clip(double min_time, double max_time)
{
  // if clipping both ends, ensure min is less than max
  if((min_time != 0) && (max_time != 0) && (min_time >= max_time))
    return(false);

  // Indicate clipping with negative time as an error
  if((min_time < 0) || (max_time < 0))
    return(false);

  double min_index = 0;
  double max_index = 0;

  vector<string> inputlines = fileBuffer(infile);
  int i, vsize = inputlines.size();
  if(vsize == 0)
    return(false);

  for(i=0; i<vsize; i++) {
    inputlines[i] = findReplace(inputlines[i], '\t', ' ');
    inputlines[i] = stripBlankEnds(inputlines[i]);
  }

  // First pass: Find the min/max indices
  //   min_index: the first data line to be at least min_time
  //   max_index: the last data line to be under max_time
  for(i=0; i<vsize; i++) {
    string line = inputlines[i];
    if(line[0] != '%') {
      if(line.size() > 0) {
	vector<string> svector = parseString(line, ' ');
	double ltime = atof(svector[0].c_str());
	if((min_time != 0) && (ltime != 0))
	  if((ltime >= min_time) && !min_index)
	    min_index = i;

	if((max_time != 0) && (ltime != 0))
	  if(ltime <= max_time)
	    max_index = i;
      }
    }
  }
  
  cout << "min_index:" << min_index << endl;
  cout << "max_index:" << max_index << endl;
  
  // Second Pass: prune out the unwanted lines and store
  //    the newlines in a new vector of strings.
  //    Three types of lines: (a) blank lines, (b) lines that 
  //    begin with '%', and (c) lines that have data.
  for(i=0; i<vsize; i++) {
    string line = inputlines[i];
    // If the line is a comment, preserve it
    if(inputlines[i][0] == '%') {
      if(!max_index || i <= max_index)
	newlines.push_back(inputlines[i]);
    }
    else {
      // If line is inside the index range
      if(!min_index || (i >= min_index)) {
	if(!max_index || (i <= max_index))
	  newlines.push_back(inputlines[i]);
      }
      else {
	// If the line is blank, preseve it
	if(line.size() == 0) 
	  newlines.push_back(inputlines[i]);
      }
    }
  }

  return(true);
}
  
//--------------------------------------------------------
// Procedure: verify_infile()

bool LogClipper::verify_infile()
{
  // First abort condition: Cannot read the input file.
  FILE *f = fopen(infile.c_str(), "r");
  if(f)
    fclose(f);
  else {
    cout << "Aborted: " << infile << " cannot be opened. " << endl;
    return(false);
  }
  return(true);
}

//--------------------------------------------------------
// Procedure: write_output

bool LogClipper::write_output()
{
  //  Abort condition: Output file exists but cannot
  //  be overwritten. tests:  fopen(r), !fopen(r+)

  FILE *f = fopen(outfile.c_str(), "r");
  if(f) {
    fclose(f); 
    f = fopen(outfile.c_str(), "r+");
    if(f)
      fclose(f);
    else {
      cout << "Aborted: The file " << outfile;
      cout << " already exists and cannot be overwritten." << endl;
      return(false);
    }
  }
    
  // Abort condition: Output file exists, and CAN be overwritten
  // but the user declines to do so. tests: fopen(r+)

  f = fopen(outfile.c_str(), "r+");
  if(f) {
    fclose(f);
    bool done = false;
    while(!done) {
      cout << "File " << outfile << " exists. Replace?(y/n)" << endl;
      char answer = getCharNoWait();
      if(answer == 'n') {
	cout << "Aborted: The file " << outfile;
	cout << " will not be created" << endl;
	return(false);
      }
      if(answer == 'y')
	done = true;
    }
  }

  // Fourth abort condition: Output file cannot be written to
  // created or overwritten. Tests: !fopen(w), 
  f = fopen(outfile.c_str(), "w");
  if(!f) {
    cout << "Aborted: The file" << outfile;
    cout << " cannot be written to" << endl;
    return(false);
  }

  for(int i=0; i<newlines.size(); i++)
    fprintf(f, "%s\n", newlines[i].c_str());

  fclose(f);

  return(true);
}

