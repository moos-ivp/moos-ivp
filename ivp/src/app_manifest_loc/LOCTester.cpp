/************************************************************/
/*   NAME: Michael Benjamin                                 */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA */
/*   FILE: LOCTester.cpp                                    */
/*   DATE: Jan 19th, 2018                                   */
/************************************************************/

#include <cstdio>
#include <iostream>
#include "FileBuffer.h"
#include "LOCTester.h"
#include "MBUtils.h"
#include "ACBlock.h"

using namespace std;

//----------------------------------------------------------
// Constructor

LOCTester::LOCTester() 
{
  m_verbose  = false;
}

//----------------------------------------------------------
// Procedure: setFile()

bool LOCTester::setFile(string filename, unsigned int ix)
{
  if(m_verbose)
    cout << "Handling LOC File: " << filename << endl;

  if(ix > 1)
    return(false);
  
  vector<string> lines = fileBuffer(filename);
  if(lines.size() == 0)
    return(false);

  for(unsigned int i=0; i<lines.size(); i++) {
    string line = stripBlankEnds(lines[i]);
    if(strBegins(line, "//"))
      continue;
    if(line == "")
      continue;

    vector<string> svector = parseString(line, ',');

    string module_name, loc_str, foc_str, wkyrs_str;
    
    for(unsigned int j=0; j<svector.size(); j++) {
      string param = tolower(biteStringX(svector[j], '='));
      string value = svector[j];

      if(param == "module")
	module_name = value;
      else if((param == "loc") && isNumber(value))
	loc_str = value;
      else if((param == "foc") && isNumber(value))
	foc_str = value;
      else if((param == "wkyrs") && isNumber(value))
	wkyrs_str = value;
      else {
	cout << "Invalid param on line " << i+1 << ": " << param << endl;
	return(false);
      }
    }	
    string sval = "loc=" + loc_str + ", foc=" + foc_str + ", wkyrs=" + wkyrs_str;

    if(ix == 0) {
      m_file1 = filename;
      m_loc1[module_name] = sval;
    }
    else {
      m_file2 = filename;
      m_loc2[module_name] = sval;      
    }
  }
  
  return(true);
}


//----------------------------------------------------------
// Procedure: test()

bool LOCTester::test()
{
  unsigned int total_matches = 0;
  string matches;
  cout << "Modules SAME                                             " << endl;
  cout << "=========================================================" << endl;
  map<string,string>::iterator p;
  for(p=m_loc1.begin(); p != m_loc1.end(); p++) {
    string module = p->first;
    string stats = p->second;
    
    if(m_loc2.count(module) && (m_loc2[module] == stats)) {
      if(matches != "")
	matches += ",";
      matches += module;
      total_matches++;
    }
  }
  ACBlock mblock("", matches, 70);
  vector<string> svector = mblock.getFormattedLines();
  for(unsigned int i=0; i<svector.size(); i++) {
    cout << svector[i] << endl;
  }

  cout << endl;
  cout << "Modules in " << m_file1 << " but not in " << m_file2 << endl;
  cout << "=========================================================" << endl;
  for(p=m_loc1.begin(); p != m_loc1.end(); p++) {
    string module = p->first;
    if(!m_loc2.count(module))
      cout << module << endl;
  }
  
  cout << endl;
  cout << "Modules in " << m_file2 << " but not in " << m_file1 << endl;
  cout << "=========================================================" << endl;
  for(p=m_loc2.begin(); p != m_loc2.end(); p++) {
    string module = p->first;
    if(!m_loc1.count(module))
      cout << module << endl;
  }


  unsigned int max_filename_len = m_file1.length();
  if(m_file2.length() > max_filename_len)
    max_filename_len = m_file2.length();

  string file1 = padString(m_file1, max_filename_len);
  string file2 = padString(m_file2, max_filename_len);
  
  cout << endl;
  cout << "Modules in both files but with different stats           " << endl;
  cout << "=========================================================" << endl;
  for(p=m_loc1.begin(); p != m_loc1.end(); p++) {
    string module = p->first;
    string stats = p->second;
    
    if(m_loc2.count(module) && (m_loc2[module] != stats)) {
      cout << module << endl;
      cout << "  [" << file1 << "]: " << m_loc1[module] << endl; 
      cout << "  [" << file2 << "]: " << m_loc2[module] << endl; 
    }
  }

  return(true);
}

//----------------------------------------------------------
// Procedure: print()

void LOCTester::print() const
{
  map<string,string>::const_iterator p;


  cout << "Set 1:                                           " << endl;
  cout << "=================================================" << endl;
  for(p=m_loc1.begin(); p!=m_loc1.end(); p++) {
    cout << "[" << p->first << "]: " << p->second << endl;
  }

  cout << "Set 2:                                           " << endl;
  cout << "=================================================" << endl;
  for(p=m_loc2.begin(); p!=m_loc2.end(); p++) {
    cout << "[" << p->first << "]: " << p->second << endl;
  }
}
