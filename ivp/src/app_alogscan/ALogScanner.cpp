/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ALogScanner.cpp                                      */
/*    DATE: June 3rd, 2008                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "TermUtils.h"
#include "MBUtils.h"
#include "FileBuffer.h"
#include "ALogScanner.h"

using namespace std;

//--------------------------------------------------------
// Constructor

ALogScanner::ALogScanner()
{
  m_infile  = "";
  m_outfile = "";
  m_file    = 0;
}

//--------------------------------------------------------
// Procedure: scan
//     Notes: 

bool ALogScanner::scan()
{
  unsigned int lines = 0;
  while(m_file) {
    string str = getNextLine();
    lines++;

    vector<string> avect, bvect, cvect, dvect;
    string timestamp = biteString(str, ' ');
    string variable  = biteString(str, ' ');
    string wsource   = biteString(str, ' ');
    string varvalue  = biteString(str, ' ');
    
    int chars = varvalue.length();
    
    if((timestamp != "") && (timestamp[0] != '%')) {
      map<string,int>::iterator p;
      p = vmap.find(variable);
      if(p != vmap.end()) {
	int index = p->second;
	m_var_count[index]++;
	m_var_chars[index] += chars;
      }
      else {
	m_var_names.push_back(variable);
	m_var_count.push_back(1);
	m_var_chars.push_back(chars);
	vmap[variable] = m_var_names.size()-1;
      }
    }
    else
      cout << "Not Processed: line " << lines << 
	" timestamp: [" << timestamp << "]" << endl;
  }

  unsigned int vsize = m_var_names.size();
  for(unsigned int i=0; i<vsize; i++) {
    printf("[%-17s] %d %d\n", m_var_names[i].c_str(), m_var_count[i],
	   m_var_chars[i]);
  }


  return(true);
}

//--------------------------------------------------------
// Procedure: getNextLine
//     Notes: 

string ALogScanner::getNextLine()
{
  if(!m_file)
    return("");
  
  const int MAX_LINE_LENGTH = 50000;

  bool last_char_was_white = false;
  bool nonwhites_read = false;
  int  myint   = '\0';
  int  buffix  = 0;
  int  buffwix = 0;
  bool EOL     = false;
  char buff[MAX_LINE_LENGTH];

  // The buffix and buffwix indices are kept separate so that
  // when an EOL/EOF is reached the terminating NULL character
  // can be placed directly after the last non-whitespace char.

  // The first_nonwhite_read bool is kept so that leading white
  // space is not included in the returned line.

  // The buffwix, first_nonwhite_read, and last_char_was_white
  // tricks are done to ensure there are no whitespace chars
  // on either end of the line, and there are not back-to-back
  // white-spaces in the returned line. Thus it is ready for 
  // parsing on white space by the user.

  while((!EOL) && (buffix < MAX_LINE_LENGTH)) {
    myint = fgetc(m_file);
    unsigned char mychar = myint;
    switch(myint) {
    case EOF:
      fclose(m_file);
      m_file = 0;
    case '\n':
      buff[buffwix] = '\0';  // attach terminating NULL
      EOL = true;
      break;
    case ' ':               // blank char
      if((!last_char_was_white) && (nonwhites_read)) {
	buff[buffix] = ' ';  
	buffix++;
      }
      last_char_was_white = true;
      break;
    case '\t':
      if((!last_char_was_white) && (nonwhites_read)) {
	buff[buffix] = ' ';  // Convert tabs to blank chars
	buffix++;
      }
      last_char_was_white = true;
      break;
    default:
      last_char_was_white = false;
      nonwhites_read = true;
      buff[buffix] = mychar;
      buffix++;
      buffwix = buffix;;
    }
  }
  string str = buff;
  return(str);
}




//--------------------------------------------------------
// Procedure: clip
//     Notes: key functions, parse() and stripBlankEnds()
//            are from MBUtils.h

bool ALogScanner::clip(double min_time, double max_time)
{
#if 0
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
#endif
  return(true);
}
  
//--------------------------------------------------------
// Procedure: openALogFile

bool ALogScanner::openALogFile(string alogfile)
{
  m_file = fopen(alogfile.c_str(), "r");
  if(!m_file)
    return(false);
  else
    return(true);
}

//--------------------------------------------------------
// Procedure: writeOutput

bool ALogScanner::writeOutput()
{
  //  Abort condition: Output file exists but cannot
  //  be overwritten. tests:  fopen(r), !fopen(r+)
#if 0
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
#endif
  return(true);
}

