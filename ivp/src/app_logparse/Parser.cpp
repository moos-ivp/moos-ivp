/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Parser.cpp                                           */
/*    DATE: May 31st, 2005                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <stdio.h>
#include <string>
#include <iostream>
#include "TermUtils.h"
#include "MBUtils.h"
#include "FileBuffer.h"
#include "Parser.h"

using namespace std;

//--------------------------------------------------------
// Constructor

Parser::Parser(string g_infile, string g_outfile, string g_columns)
{
  infile  = g_infile;
  outfile = g_outfile;
  columns = g_columns;
}

//--------------------------------------------------------
// Procedure: parse
//     Notes: key functions, chompString and stripBlankEnds
//            are from MBUtils.h

#if 1
bool Parser::parse()
{
  inputlines = fileBuffer(infile);
  int vsize = inputlines.size();
  if(vsize == 0)
    return(false);

  columns = findReplace(columns, '\t', ' ');
  columns = compactConsecutive(columns, ' ');

  bool ok = true;
  int  i, j;

  vector<string> cvector = parseString(columns, ' ');
  for(i=0; i<cvector.size(); i++)
    cindex.push_back(atoi(cvector[i].c_str()));

  vector<string> datalines;

  for(i=0; i<inputlines.size(); i++) {
    if(inputlines[i][0] != '%') {
      string testline = stripBlankEnds(inputlines[i]);
      if(testline.size() > 0)
	datalines.push_back(inputlines[i]);
    }
  }

  for(i=0; i<datalines.size(); i++) {
    //cout << "[" << i << "] " << datalines[i] << endl; 
    string line = findReplace(datalines[i], '\t', ' ');
    line = compactConsecutive(datalines[i], ' ');
    
    string newline = "";
    vector<string> svector = parseString(line, ' ');
    bool all_nans = true;
    for(j=0; j<cindex.size(); j++) {
      //int ix = cindex[j] - 1;
      int ix = cindex[j];
      cout << "[" << j << "] " << cindex[j] << " - " << svector[ix] << endl; 
      if((ix >= 0) && (ix <=svector.size())) {
	newline += svector[ix];
	if(j < cindex.size()-1)
	  newline += " ";
	if(svector[ix] != "NaN")
	  all_nans = false;
      }
    }
    if(!all_nans) {
      newlines.push_back(newline);
    }
  }
  return(true);
}
#endif


#if 0
bool Parser::parse()
{
  inputlines = fileBuffer(infile);
  int vsize = inputlines.size();
  if(vsize == 0)
    return(false);

  columns = findReplace(columns, '\t', ' ');
  columns = compactConsecutive(columns, ' ');

  bool ok = true;
  int  i, j;

  vector<string> cvector = parseString(columns, ' ');
  for(i=0; i<cvector.size(); i++)
    cindex.push_back(atoi(cvector[i].c_str()));

  vector<string> datalines;

  for(i=0; i<inputlines.size(); i++) {
    if(inputlines[i][0] != '%') {
      string testline = stripBlankEnds(inputlines[i]);
      if(testline.size() > 0)
	datalines.push_back(inputlines[i]);
    }
  }

  polyline = "polygon = ";
  for(i=0; i<datalines.size(); i++) {
    string line = findReplace(datalines[i], '\t', ' ');
    line = compactConsecutive(datalines[i], ' ');

    string entry = "";
    vector<string> svector = parseString(line, ' ');
    bool all_nans = true;
    for(j=0; j<cindex.size(); j++) {
      int ix = cindex[j] - 1;
      if((ix >= 0) && (ix <=svector.size())) {
	entry += svector[ix];
	if(j < cindex.size()-1)
	  entry += ",";
	if(ix && (svector[ix] != "NaN"))
	  all_nans = false;
      }
    }
    if(!all_nans)
      polyline += entry + ":";
  }
    
  return(true);
}
#endif
  
//--------------------------------------------------------
// Procedure: verify_infile()

bool Parser::verify_infile()
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

bool Parser::write_output()
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

#if 1
  for(int i=0; i<newlines.size(); i++)
    fprintf(f, "%s\n", newlines[i].c_str());
#endif
#if 0
    fprintf(f, "%s\n", polyline.c_str());
#endif    

  fclose(f);

  return(true);
}

