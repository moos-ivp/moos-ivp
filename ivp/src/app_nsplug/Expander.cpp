/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Expander.cpp                                         */
/*    DATE: May 28th, 2005                                       */
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
#include <cstdlib>
#include <string>
#include <iostream>
#include "TermUtils.h"
#include "MBUtils.h"
#include "FileBuffer.h"
#include "Expander.h"
#include "ColorParse.h" // For debugging

using namespace std;

//--------------------------------------------------------
// Constructor()

Expander::Expander(string given_infile, string given_outfile)
{
  m_infile   = given_infile;
  m_outfile  = given_outfile;
  m_force    = false;  
  m_strict   = false;
  m_terminal = false;
  
  m_max_subs_per_line = 100;
  m_initial_filenames.push_back(given_infile);
  m_path.push_back(".");
  m_pmode.push_back("top");

  m_partial_expand_ok = false;

  m_interactive = false;
  m_impatient = false;
}

//--------------------------------------------------------
// Procedure: expand()
//      Note: High level entry point for recursion

bool Expander::expand()
{
  //printMacros(m_initial_macros);
  bool result;
  vector<string> fvector = expandFile(m_infile, m_initial_macros, 
				      m_initial_filenames,
				      m_inctag, result);

  if(result == false) {
    cout << "#  Aborting without writing the new file." << endl;
    return(false);
  }
  else
    m_newlines = fvector;
  
  return(true);
}
  
//--------------------------------------------------------
// Procedure: expandFile()
//     Notes: key functions, chompString and stripBlankEnds
//            are from MBUtils.h
//      Note: inctag added Sep 20th, 2020 to support lines like
//            #include filename <inctag> which will only include
//            part of the named file

vector<string> Expander::expandFile(string filename, 
				    map<string, string>& macros, 
				    vector<string> filenames, 
				    string inctag, bool& result)
{
  vector<string> return_vector;
  vector<string> empty_vector;

  vector<string> fvector = fileBuffer(filename);

  unsigned int i, vsize = fvector.size();
  if(vsize == 0) {
    cout << "#  Warning: The file " << filename << " was empty." << endl;    
    result = true;
    return(empty_vector);
  }

  inctag = stripChevrons(inctag);

  string curr_tag;
  
  for(i=0; i<vsize; i++) {

    string line = stripBlankEnds(findReplace(fvector[i], '\t', ' '));
    string line_orig = line;
    string left = biteStringX(line, ' ');
    string rest = line;

    // Begin tag support, added Sep 2020
    if(strBegins(line_orig, "<tag>")) {
      curr_tag = stripChevrons(line_orig.substr(5));
      continue;
    }
    if((inctag != "") && (inctag != curr_tag))
      continue;
    if((inctag != "") && strBegins(line_orig, "#"))
      continue;
    
    // End tag support, added Sep 2020
    
    //------------------------------------------------------------
    if(left == "#ifdef") {
      bool ifdef = checkIfDef(rest, macros, i+1);
      if(!ifdef)
	pushMode("ifdefno");
      else
	pushMode("ifdefyes");
    }
    
    //------------------------------------------------------------
    else if(left == "#elseifdef") {
      if((currMode() != "ifdefyes") && (currMode() != "ifdefno") &&
	 (currMode() != "ifdefnomore")) {
	cout << "#  Error in file " << filename << " line:" << i+1 << endl;
	cout << "#  Dangling #elseifdef" << endl;
	result = false;
	return(empty_vector);
      }
      
      // Being in the ifdefyes mode means one of the "above" ifdef cases
      // matched and the current #elseifdef is moot.
      if(currMode() == "ifdefyes") {
	currMode("ifdefnomore");
      }
      else if(currMode() == "ifdefno") {
	bool ifdef = checkIfDef(rest, macros, i+1);
	if(ifdef)
	  currMode("ifdefyes");
      }
    }

    //------------------------------------------------------------
    else if(left == "#else") {
      if(currMode() == "top") {
	cout << "#  Error in file " << filename << " line:" << i+1 << endl;
	cout << "#  Dangling #else" << endl;
	result = false;
	return(empty_vector);
      }

      // Being in the ifdefyes mode means one of the "above" ifdef cases
      // matched and the current #else is moot.
      if((currMode() == "ifdefyes") || (currMode() == "ifndefyes"))
	currMode("elseno");
      else if((currMode() == "ifdefno") || (currMode() == "ifndefno"))
	currMode("elseyes");
    }

    //------------------------------------------------------------
    else if(left == "#ifndef") {
      bool ifndef = checkIfNDef(rest, macros);
      if(!ifndef)
	pushMode("ifndefno");
      else
	pushMode("ifndefyes");
    }

    //-------------------------------------------------------------
    else if(left == "#endif") {
      if(currMode() == "top") {
	cout << "#  Error in file " << filename << " line:" << i+1 << endl;
	cout << "#  #endif detected with no open #ifdef or #ifndef" << endl;
	result = false;
	return(empty_vector);
      }
      popMode();
    }

    //--------------------------------------------------------------
    else if(!skipLines() && (left == "#include")) {
      applyMacrosToLine(rest, macros, i+1);

      string file_str = stripBlankEnds(rest);

      // Begin check for file <tag>
      string inctag;
      string file_str_copy = file_str;
      string last_chunk = rbiteStringX(file_str_copy, ' ');
      if(isChevroned(last_chunk)) {
	inctag = last_chunk;
	file_str = file_str_copy;
      }
      // End check for file <tag>
	
	
      if(isQuoted(file_str))
	file_str = stripQuotes(file_str);
      string full_file_str = findFileInPath(file_str);

      if(!verifyInfile(full_file_str)) {
	cout << "#  Error in file " << filename << " line:" << i+1 << endl;
	cout << "#  The #include file \"" << file_str << "\" not found." << endl;
	result = false;
	return(empty_vector);
      }
	
      if(vectorContains(filenames, full_file_str)) {
	cout << "#  Error in file " << filename << " line:" << i+1 << endl;
	cout << "#  A #include LOOP was detected - " << endl;
	cout << "#  trying to #include file " << file_str << endl;
	result = false;
	return(empty_vector);
      }
      vector<string> ilines = expandFile(full_file_str, macros, 
					 filenames, inctag, result);
      if(!result) {
	cout << "#  (Included from file " << filename << ")" << endl;
	return(empty_vector);
      }

      return_vector = mergeVectors(return_vector, ilines);
    }

    //----------------------------------------------------------------
    else if(!skipLines() && (left == "#define")) {
      rest = compactConsecutive(rest, ' ');
      string macro = biteStringX(rest, ' ');
      string value = rest;

      // Support "#define $(FOO) BAR" as well as "#define FOO BAR"
      int mlen = macro.length();
      if((mlen > 3) && (macro.at(0)=='$') &&
	 (macro.at(1)=='(') && (macro.at(mlen-1)==')'))
	macro = macro.substr(2, mlen-3);
	 
      if(macro == "") {
	cout << "#  Error in file " << filename << " line:" << i+1 << endl;
	cout << "#  A #define line must have an argument" << endl;
	result = false;
	return(empty_vector);
      }
      else {
	if(value == "") 
	  value = "<defined>";
	
	if(macros.count(macro) == 1) {
	  cout << termColor("blue") << "Warning: \"" << macro;
	  cout << "\" redefined from \"" << macros[macro] << "\" to \"";
	  cout << value << "\". Line #" << i+1 << " in file: " << filename;
	  cout << termColor() << endl;
	}
	macros[macro] = value;
      }
    }
    else if(!skipLines()) {
      applyMacrosToLine(fvector[i], macros, i+1);
      return_vector.push_back(fvector[i]);
    }   

#if 0 // BEGIN DEBUGGING OUTPUT BLOCK
    string pline = fvector[i];
    if(pline.length() > 0)
      pline.at(pline.length()-1) = '\0';

    cout << "Line[" << i+1 << "]: " << pline << flush;
    cout << termColor("blue");
    cout << "   (" << currMode() << ")   ";

    if(skipLines())
      cout << termColor("red") << "skip" << termColor();
    else
      cout << termColor("green") << "ok" << termColor();
    cout << endl;
#endif // END DEBUGGING OUTPUT BLOCK
  }   
  

  result = true;
  return(return_vector);
}
  
//--------------------------------------------------------
// Procedure: addMacro()

void Expander::addMacro(string key, string value, bool over_ok)
{
  m_initial_macros[key] = value;
}


//--------------------------------------------------------
// Procedure: verifyInfile()

bool Expander::verifyInfile()
{
  return(verifyInfile(m_infile));
}

//--------------------------------------------------------
// Procedure: verifyInfile()

bool Expander::verifyInfile(const string& filename)
{
  FILE *f = fopen(filename.c_str(), "r");
  if(f)
    fclose(f);
  else
    return(false);
  return(true);
}

//--------------------------------------------------------
// Procedure: writeOutput()

bool Expander::writeOutput()
{
  if(m_terminal) {
    for(unsigned int i=0; i<m_newlines.size(); i++)
      printf("%s\n", m_newlines[i].c_str());
    return(true);
  }
  
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
	cout <<  " Replace? (y/N)" << endl;
	answer = getCharNoWait();
      }

      if(answer == 'y')
	done = true;
      else if((answer == 'n') || ((int)(answer) == 10)) {
	cout << "Aborted: The file " << m_outfile;
	cout << " will not be created" << endl;
	return(false);
      }
    }
  }

  // Fourth abort condition: Output file cannot be written to
  // created or overwritten. Tests: !fopen(w), 

  f = fopen(m_outfile.c_str(), "w");
  if(!f) {
    cout << "Aborted: The file [" << m_outfile;
    cout << "] cannot be written to" << endl;
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
// Procedure: addPath()

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
// Procedure: applyMacrosToLine()

bool Expander::applyMacrosToLine(string& line, 
				 map<string, string> macros,
				 unsigned int line_num)
{
  map<string, string>::iterator p;

  if(stripBlankEnds(line) == "")
    return(true);
  
  string newline = line;
  for(p = macros.begin(); p != macros.end(); p++) {
    string macro = p->first;
    string key = "$(" + macro + ")";
    string val = p->second;
    if(val == "<defined>")
      val = "";

    // Assuming key is of the form "$(FOOBAR)"
    string pkey = key;
    if((pkey.length() > 0) && (pkey.at(0) == '$'))
      pkey.at(0) = '%';

    string tmp_newline = newline;
    
    if(val == "")
      tmp_newline = reduceMacrosToBase(tmp_newline, "=", macro);
    else {
      tmp_newline = reduceMacrosToBase(tmp_newline, ":=", macro);
      tmp_newline = reduceMacrosToBase(tmp_newline, "=", macro);
    }
    
    tmp_newline = findReplace(tmp_newline, pkey, toupper(val));
    newline = findReplace(tmp_newline, key, val);
  }

  newline = expandMacrosWithDefault(newline);
  
  string res = containsMacro(newline);

  if(res != "") {
    bool isCommented = false;
    if(strBegins(stripBlankEnds(line), "//"))
      isCommented = true;
    if(!isCommented) {
      cout << termColor("magenta");
      cout << "Warning: The following line of " << m_infile;
      cout << "  (creating " << m_outfile << ")" << endl;
      cout << "  may contain an undefined macro on Line: " << line_num << endl;
      cout << "> " << res << termColor() << endl;
      if(m_interactive) {
	bool answered = false;
	while(!answered) {
	  cout << "Continue? [Y/n]" << endl;
	  char c = getCharNoWait();
	  //cout << "char:[" << (int)c << "]" << endl;
	  //cout << "char:[" << c << "]" << endl;
	  if((c=='n') || (c=='N') || (c==((int)(3))))   // ASCII 03 is control-c
	    exit(1);
	  if((c=='Y') || (c=='y') || (c==((int)(10))))  // ASCII 10 is ENTER
	    answered = true;
	  else if(m_impatient)
	    exit(1);
	}
      }
    }

    if(!isCommented && m_strict)
      exit(EXIT_FAILURE);
    
    if(!m_partial_expand_ok)
      return(false);
  }

  line = newline;
  return(true);
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
// Procedure: findFileInPath()
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

//--------------------------------------------------------
// Procedure: checkIfDef()

bool Expander::checkIfDef(string entry, map<string, string> macros,
			  unsigned int line_num)
{
  bool disj = strContains(entry, "||");
  bool conj = strContains(entry, "&&");
    
  if(disj && conj) {
    cout << termColor("red");
    cout << "Warning: The following line of " << m_infile;
    cout << "  (creating " << m_outfile << ")" << endl;
    cout << "  contains a mixed-logic #ifdef on line: " << line_num << endl;
    cout << "> #ifdef " << entry << endl;
    if(m_strict) {
      cout << "Since strict==true, exiting now..." << termColor() << endl;
      exit(EXIT_FAILURE);
    }
    else {
      cout << "Since strict==false, this line simply evaluates to false.";
      cout << termColor() << endl;
      return(false);
    }
  }

  if(conj)
    return(checkIfDefConj(entry, macros));

  return(checkIfDefDisj(entry, macros));
}

//--------------------------------------------------------
// Procedure: checkIfDefDisj()

bool Expander::checkIfDefDisj(string entry, map<string, string> macros)
{
  // Assume this ifdef does not hold unless one of the ifdef 
  // macro-value pairs holds. The #ifdef construct supports the
  // disjunction of macro-value pairs.
  bool ifdef = false;

  vector<string> kvector = parseString(entry, "||"); 
  unsigned int k, ksize = kvector.size();
  for(k=0; ((k<ksize) && !ifdef); k++) {
    kvector[k] = stripBlankEnds(kvector[k]);
    string macro_name  = biteStringX(kvector[k], ' ');
    string macro_value = kvector[k];

    if(macros.count(macro_name) == 1) {
      if(macro_value == "") {
	if(macros[macro_name] != "")
	  ifdef = true;
      }
      else if(macros[macro_name] == macro_value)
	ifdef = true;
      else if(macros[macro_name] == macros[macro_value])
	ifdef = true;
    }
  }
  
  return(ifdef);
}

//--------------------------------------------------------
// Procedure: checkIfDefConj()

bool Expander::checkIfDefConj(string entry, map<string, string> macros)
{
  // Assume this ifdef does not hold unless all of the ifdef 
  // macro-value pairs holds. The #ifdef construct supports the
  // conjunction of macro-value pairs.
  bool ifdef = true;

  vector<string> kvector = parseString(entry, "&&"); 
  unsigned int k, ksize = kvector.size();
  for(k=0; ((k<ksize) && ifdef); k++) {
    kvector[k] = stripBlankEnds(kvector[k]);
    string macro_name  = biteStringX(kvector[k], ' ');
    string macro_value = kvector[k];
    if(macro_value == "") {
      //  if(macros[macro_name] == "")
      if(macros.count(macro_name) == 0)
	ifdef = false;
    }
    else if((macros[macro_name] != macro_value) &&
	    (macros[macro_name] != macros[macro_value]))
      ifdef = false;
  }
  
  return(ifdef);
}

//--------------------------------------------------------
// Procedure: checkIfNDef()

bool Expander::checkIfNDef(string entry, map<string, string> macros)
{
  // If "entry" is empty then declare this ifndef to be false
  // If it is non-empty make sure each component is found.
  bool ifndef = false;
  if(entry != "")
    ifndef = true;
  
  // Now go thru each clause, ensure that each is not defined
  bool done = false;
  while(ifndef && !done) {
    string clause = biteStringX(entry, ' ');
    if(clause == "")
      done = true;
    else {
      if(macros[clause] != "")
	ifndef = false;
    }
  }
  return(ifndef);
}

//--------------------------------------------------------
// Procedure: currMode(string)

void Expander::currMode(string new_mode)
{
  unsigned int vsize = m_pmode.size();
  if(vsize > 1)
    m_pmode[vsize-1] = new_mode;
}

//--------------------------------------------------------
// Procedure: pushMode(string)

void Expander::pushMode(string new_mode)
{
    m_pmode.push_back(new_mode);
}

//--------------------------------------------------------
// Procedure: currMode()

string Expander::currMode()
{
  unsigned int vsize = m_pmode.size();
  if(vsize > 0)
    return(m_pmode[vsize-1]);
  else
    return("error - stack underflow");
}

//--------------------------------------------------------
// Procedure: popMode()

bool Expander::popMode()
{
    unsigned int vsize = m_pmode.size();
    if(vsize > 1) {
        m_pmode.pop_back();
        return(true);
    }
  else
    return(false);
}

//--------------------------------------------------------
// Procedure: skipLines()
//      Notr: Determines if, given the current stack of modes,
//            whether a currently considered line should be
//            skipped.

bool Expander::skipLines()
{
  string curr_mode = currMode();
  if(modeStackContains("ifdefno"))
    return(true);
  if(modeStackContains("ifdefnomore"))
    return(true);
  if(modeStackContains("elseno"))
    return(true);
  if(modeStackContains("ifndefno"))
    return(true);

  if(curr_mode == "top")
    return(false);
  if(curr_mode == "ifdefyes")
    return(false);
  if(curr_mode == "elseyes")
    return(false);
  if(curr_mode == "ifndefyes")
    return(false);
  return(true);
}

//--------------------------------------------------------
// Procedure: modeStackContains()
//      Note: Returns true if the given string is anywhere
//            in the stack of saved modes.

bool Expander::modeStackContains(string str)
{
  unsigned int i, vsize = m_pmode.size();
  for(i=0; i<vsize; i++) {
    if(m_pmode[i] == str)
      return(true);
  }
  return(false);
}

//--------------------------------------------------------
// Procedure: printModeStack()
//      Note: For debugging

void Expander::printModeStack()
{
  unsigned int i, vsize = m_pmode.size();
  for(i=0; i<vsize; i++) {
    cout << "[" << i << "]" << m_pmode[i] << " ";
  }
  cout << endl;
}

//--------------------------------------------------------
// Procedure: printMacros()
//      Note: For debugging

void Expander::printMacros(map<string,string> macros)
{
  cout << "------------- total:" << macros.size() << endl;
  unsigned int index = 0;
  map<string,string>::iterator p;
  for(p=macros.begin(); p!=macros.end(); p++) {
    string macro = p->first;
    string value = p->second;
    cout << "[" << index << "]: ";
    cout << macro << "=" << value << endl;
    index++;
  }
  cout << endl;
}

//--------------------------------------------------------
// Procedure: addMacroFile()

bool Expander::addMacroFile(string filestr)
{
  // Sanity checks
  if(!okFileToRead(filestr))
    return(false);

  vector<string> lines = fileBuffer(filestr);
  if(lines.size() == 0)
    return(false);

  // ===================================================
  // Part 2: 
  // Handle each line/macro and create a new, additional
  // macro map. Apply all macros from previously loaded
  // macros files, to each new line from this file.
  // ===================================================
  map<string, string> new_macros;
  for(unsigned int i=0; i<lines.size(); i++) {
    string line = stripBlankEnds(lines[i]);

    if(line == "")
      continue;
    if(strBegins(line, "#") || strBegins(line, "//"))
      continue;
    
    string orig = line;
    string param = biteStringX(line, '=');
    string value = line;
    if(isQuoted(value))
      value = stripQuotes(value);

    // Sanity check a given line for null parts or dupls.
    string issue;    
    if((param == "") || (value == ""))
      issue = "null param or value: [" + orig + "]";
    if(m_initial_macros.count(param) != 0) {
      if(value != m_initial_macros[param])
	issue = "ambiguous macro: [" + orig + "]";
    }
    if(new_macros.count(param) != 0) {
      if(value != new_macros[param])
	issue = "ambiguous macro: [" + orig + "]";
    }
    // Sanity check end

    if(issue == "") {
      applyMacrosToLine(value, m_initial_macros, 0);      
      new_macros[param] = value;
    }
    else {
      cout << "Err in macro file around line " << i;
      cout << "Line:[" << orig << "]" << endl;
    }
  }

  // ===================================================
  // Part 3: Merge the two maps

  //printMacros(m_initial_macros);
  //printMacros(new_macros);
  
  map<string,string>::iterator p;
  for(p=new_macros.begin(); p!=new_macros.end(); p++) {
    string key = p->first;
    string val = p->second;
    m_initial_macros[key] = val;
  }

  return(true);
}


