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
  m_infile  = given_infile;
  m_outfile = given_outfile;
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

  bool skip_lines = false;

  vector<string> fvector = fileBuffer(m_infile);
  int vsize = fvector.size();
  if(vsize == 0)
    return(false);

  for(int i=0; ((i<vsize) && ok); i++) {
    string line = fvector[i];
    line = stripBlankEnds(line);
    line = findReplace(line, '\t', ' ');
    vector<string> svector = chompString(line, ' ');
    if(!skip_lines && (svector[0] == "#ifdef")) {
      string clause = "$("+stripBlankEnds(svector[1])+")";
      if(m_macros[clause] == "")
	skip_lines = true;
    }
    else if(!skip_lines && (svector[0] == "#ifndef")) {
      string clause = "$("+stripBlankEnds(svector[1])+")";
      if(m_macros[clause] != "")
	skip_lines = true;
    }
    else if(svector[0] == "#endif")
      skip_lines = false;
    else if(!skip_lines && (svector[0] == "#include")) {
      string file_str = stripBlankEnds(svector[1]);
      string full_file_str = findFileInPath(file_str);
      vector<string> ivector = fileBuffer(full_file_str);
      int isize = ivector.size();
      if(isize == 0) {
	ok = false;
	bad_line = i+1;
      }
      for(int j=0; j<isize; j++)
	addNewLine(ivector[j]);
    }
    else if(!skip_lines && (svector[0] == "#define")) {
      svector[1] = stripBlankEnds(svector[1]);
      svector[1] = compactConsecutive(svector[1], ' ');
      vector<string> mvector = parseString(svector[1], ' ');
      if(mvector.size() == 2)
	addMacro(mvector[0], mvector[1]);
      else {
	ok = false;
	bad_line = i+1;
      }
    }
    else if(!skip_lines)
      addNewLine(fvector[i]);
  }      
  
  if(!ok) {
    cout << "Aborted: The file " << m_infile; 
    cout << "contains dubious entry on line " << bad_line << ":" << endl;
    cout << fvector[bad_line-1] << endl;
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
    m_macros[key] = value;
    return;
  }
  
  m_macros[key] = value;

#if 0
  // Don't allow over-writes of previous mappings
  map<string, string>::iterator p;
  p = m_macros.find(key);
  if(p != m_macros.end())
    return;
  else
    m_macros[key] = value;
#endif
}


//--------------------------------------------------------
// Procedure: verifyInfile()

bool Expander::verifyInfile()
{
  // First abort condition: Cannot read the input file.
  FILE *f = fopen(m_infile.c_str(), "r");
  if(f)
    fclose(f);
  else {
    cout << "Aborted: " << m_infile << " cannot be opened. " << endl;
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

  FILE *f = fopen(m_outfile.c_str(), "r");
  if(f) {
    fclose(f); 
    f = fopen(m_outfile.c_str(), "r+");
    if(f)
      fclose(f);
    else {
      cout << "Aborted: The file " << m_outfile;
      cout << " already exists and cannot be overwritten." << endl;
      return(false);
    }
  }
    
  // Abort condition: Output file exists, and CAN be overwritten
  // but the user declines to do so. tests: fopen(r+)

  f = fopen(m_outfile.c_str(), "r+");
  if(f) {
    fclose(f);
    bool done = false;
    while(!done && !m_force) {
      cout << "Warning: File " << m_outfile << " already exists.";
      
      char answer = 'y';
      if(!m_force) {
	cout <<  " Replace?(y/n)" << endl;
	answer = getCharNoWait();
      }

      if(answer == 'n') {
	cout << "Aborted: The file " << m_outfile;
	cout << " will not be created" << endl;
	return(false);
      }
      if(answer == 'y')
	done = true;
    }
  }

  // Fourth abort condition: Output file cannot be written to
  // created or overwritten. Tests: !fopen(w), 

  f = fopen(m_outfile.c_str(), "w");
  if(!f) {
    cout << "Aborted: The file" << m_outfile;
    cout << " cannot be written to" << endl;
    return(false);
  }

  for(unsigned int i=0; i<m_newlines.size(); i++)
    fprintf(f, "%s\n", m_newlines[i].c_str());

  fclose(f);

  if (!m_force)
      cout << "...successfully completed" << endl;
  
  return(true);
}

//--------------------------------------------------------
// Procedure: addPath

void Expander::addPath(string str)
{
  vector<string> svector = parseString(str, ':');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string new_entry = stripBlankEnds(svector[i]);
    m_path.push_back(new_entry);
  }
}



//--------------------------------------------------------
// Procedure: addNewLine

void Expander::addNewLine(string newline)
{
  map<string, string>::iterator p;

  for(p = m_macros.begin(); p != m_macros.end(); p++) {
    string key = p->first;
    string val = p->second;

    // Assuming key is of the form "$(FOOBAR)"
    string pkey = key;
    if((pkey.length() > 0) && (pkey.at(0) == '$'))
      pkey.at(0) = '%';

    if(strContains(newline, pkey))
      newline = findReplace(newline, pkey, toupper(val));

    if(strContains(newline, key))
      newline = findReplace(newline, key, val);
  }

  string res = containsMacro(newline);

  if(res != "") {
    cout << "Warning: The following line of " << m_infile
	 << " creating" << m_outfile
	 << " may contain an undefined macro::" << endl;
    cout << "> " << res << endl;
  }
  
  m_newlines.push_back(newline);
}

//--------------------------------------------------------
// Procedure: containsMacro()

string Expander::containsMacro(string line)
{
  int pos = line.find("$(");
  if(pos != -1)
    return(line);
  else {
    pos = line.find("%(");
    if(pos != -1)
      return(line);
    else
      return("");
  }
}

//--------------------------------------------------------
// Procedure: findFileInPath
//      Note: Looking in the current directory (./) is only done if
//            either it was explicitly added to the path, or if the
//            path itself is empty.

string Expander::findFileInPath(string filename)
{
  unsigned int i, vsize = m_path.size();
  if(vsize == 0)
    return(filename);

  for(i=0; i<vsize; i++) {
    string full_name = (m_path[i] + "/" + filename);
    FILE *f = fopen(full_name.c_str(), "r");
    if(f) {
      fclose(f);
      return(full_name);
    }
  }
  return("");
}

