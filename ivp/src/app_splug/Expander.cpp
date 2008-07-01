/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Expander.cpp                                         */
/*    DATE: May 28th, 2005                                       */
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
#include "Expander.h"

using namespace std;

//--------------------------------------------------------
// Constructor

Expander::Expander(string given_infile, string given_outfile)
{
  infile  = given_infile;
  outfile = given_outfile;
  m_force = false;  
}

//--------------------------------------------------------
// Procedure: expand
//     Notes: key functions, chompString and stripBlankEnds
//            are from MBUtils.h

bool Expander::expand()
{
  bool ok = true;
  int  bad_line = -1;

  vector<string> fvector = fileBuffer(infile);
  int vsize = fvector.size();
  if(vsize == 0)
    return(false);

  for(int i=0; ((i<vsize) && ok); i++) {
    string line = fvector[i];
    line = stripBlankEnds(line);
    line = findReplace(line, '\t', ' ');
    vector<string> svector = chompString(line, ' ');
    if(svector[0] == "#include") {
      string file_str = stripBlankEnds(svector[1]);
      vector<string> ivector = fileBuffer(file_str);
      int isize = ivector.size();
      if(isize == 0) {
	ok = false;
	bad_line = i+1;
      }
      for(int j=0; j<isize; j++)
	addNewLine(ivector[j]);
    }
    else if(svector[0] == "#define") {
      svector[1] = stripBlankEnds(svector[1]);
      svector[1] = compactConsecutive(svector[1], ' ');
      vector<string> mvector = parseString(svector[1], ' ');
      if(mvector.size() == 2) {
	addMacro(mvector[0], mvector[1]);
	//	string key = "$(" + mvector[0] + ")";
	//	macros[key] = mvector[1];
      }
      else {
	ok = false;
	bad_line = i+1;
      }
    }
    else
      addNewLine(fvector[i]);
  }      
  
  if(!ok) {
    cout << "Aborted: The file " << infile; 
    cout << "contains dubious entry on line " << bad_line << endl;
  }

  return(true);
}
  
//--------------------------------------------------------
// Procedure: addMacro()

void Expander::addMacro(string key, string value, bool over_ok)
{
  key   = "$(" + stripBlankEnds(key) + ")";
  value = stripBlankEnds(value);

  if(over_ok) {
    macros[key] = value;
    return;
  }
  
  // Don't allow over-writes of previous mappings
  map<string, string>::iterator p;
  p = macros.find(key);
  if(p != macros.end())
    return;
  else
    macros[key] = value;
}


//--------------------------------------------------------
// Procedure: verifyInfile()

bool Expander::verifyInfile()
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
// Procedure: writeOutput

bool Expander::writeOutput()
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
    while(!done && !m_force) {
      cout << "Warning: File " << outfile << " already exists.";
      
      char answer = 'y';
      if(!m_force) {
	cout <<  " Replace?(y/n)" << endl;
	answer = getCharNoWait();
      }

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

  for(unsigned int i=0; i<newlines.size(); i++)
    fprintf(f, "%s\n", newlines[i].c_str());

  fclose(f);

  if (!m_force)
      cout << "...successfully completed" << endl;
  
  return(true);
}

//--------------------------------------------------------
// Procedure: addNewLine

void Expander::addNewLine(string newline)
{
  map<string, string>::iterator p;

  for(p = macros.begin(); p != macros.end(); p++) {
    string key = p->first;
    string val = p->second;
    newline = findReplace(newline, key, val);
  }

  string res = containsMacro(newline);

  if(res != "") {
    cout << "Warning: The following line may contain an undefined macro:" << endl;
    cout << "> " << res << endl;
  }
  
  newlines.push_back(newline);
}

//--------------------------------------------------------
// Procedure: containsMacro()

// TO BE IMPLEMENTED - identify any undefined macros in a line
 
string Expander::containsMacro(string line)
{
  int pos = line.find("$(");
  if(pos != -1)
    return(line);
  else
    return("");
}

