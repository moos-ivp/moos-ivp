/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GrepHandler.cpp                                      */
/*    DATE: August 6th, 2008                                     */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "MBUtils.h"
#include "GrepHandler.h"
#include "LogUtils.h"
#include "TermUtils.h"

using namespace std;


//--------------------------------------------------------
// Constructor

GrepHandler::GrepHandler()
{
  m_file_in  = 0;
  m_file_out = 0;

  m_lines_removed  = 0;
  m_lines_retained = 0;
  m_chars_removed  = 0;
  m_chars_retained = 0;

  m_file_overwrite = false;
}

//--------------------------------------------------------
// Procedure: handle
//     Notes: 

bool GrepHandler::handle(const string& alogfile, const string& new_alogfile)
{
  if(alogfile == new_alogfile) {
    cout << "Input and output .alog files cannot be the same. " << endl;
    cout << "Exiting now." << endl;
    return(false);
  }

  m_file_in = fopen(alogfile.c_str(), "r");
  if(!m_file_in) {
    cout << "input not found or unable to open - exiting" << endl;
    return(false);
  }
  
  if(new_alogfile != "") {
    m_file_out = fopen(new_alogfile.c_str(), "r");
    if(m_file_out && !m_file_overwrite) {
      bool done = false;
      while(!done) {
	cout << new_alogfile << " already exists. Replace? (y/n [n])" << endl;
	char answer = getCharNoWait();
	if((answer != 'y') && (answer != 'Y')){
	  cout << "Aborted: The file " << new_alogfile;
	  cout << " will not be overwritten." << endl;
	  return(false);
	}
	if(answer == 'y')
	  done = true;
      }
    }
    m_file_out = fopen(new_alogfile.c_str(), "w");
  }
  
  bool done = false;
  while(!done) {
    string line_raw = getNextRawLine(m_file_in);
    
    bool   line_is_comment = false;
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      line_is_comment = true;

    if(line_raw == "eof") 
      done = true;
    else {
      string varname = getVarName(line_raw);
      string srcname = getSourceNameNoAux(line_raw);

      //string data = getDataEntry(line_raw);
      //cout << "data: [" << data << "]" << endl;

      int ksize = m_keys.size();
      bool match = false;
      for(int i=0; ((i<ksize) && !match); i++) {
	if((varname == m_keys[i]) || (srcname == m_keys[i]))
	  match = true;
	else if(m_pmatch[i] && (strContains(varname, m_keys[i]) ||
				strContains(srcname, m_keys[i])))
	  match = true;
      }

      if(match || line_is_comment) {
	if(m_file_out)
	  fprintf(m_file_out, "%s\n", line_raw.c_str());
	else
	  cout << line_raw << endl;
      }
      
      if(match && !line_is_comment) {
	m_lines_retained++;
	m_chars_retained += line_raw.length();
	m_vars_retained.insert(varname);
      }

      if(!match && !line_is_comment) {
	m_lines_removed++;
	m_chars_removed += line_raw.length();
	m_vars_removed.insert(varname);
      }
    }
  }

  if(m_file_out)
    fclose(m_file_out);
  m_file_out = 0;

  return(true);
}

//--------------------------------------------------------
// Procedure: addKey
//     Notes: 

void GrepHandler::addKey(string key)
{
  bool pmatch = false;
  int len = key.length();
  if(key.at(len-1) == '*') {
    pmatch = true;
    key.erase(len-1, 1);
  }
  
  int  ksize = m_keys.size();
  bool prior = false;
  int  prior_ix = -1;
  for(int i=0; i<ksize; i++) {
    if(key == m_keys[i]) {
      prior = true;
      prior_ix = i;
    }
  }
  
  if(!prior) {
    m_keys.push_back(key);
    m_pmatch.push_back(pmatch);
  }

  if(prior && pmatch && !m_pmatch[prior_ix])
    m_pmatch[prior_ix] = true;
}


//--------------------------------------------------------
// Procedure: getMatchedKeys()
//     Notes: 

vector<string> GrepHandler::getMatchedKeys()
{
  vector<string> rvector;

  unsigned int i, vsize = m_keys.size();
  for(i=0; i<vsize; i++) {
    if(m_pmatch[i])
      rvector.push_back(m_keys[i]);
  }
  return(rvector);
}


//--------------------------------------------------------
// Procedure: getUnMatchedKeys()
//     Notes: 

vector<string> GrepHandler::getUnMatchedKeys()
{
  vector<string> rvector;

  unsigned int i, vsize = m_keys.size();
  for(i=0; i<vsize; i++) {
    if(!m_pmatch[i])
      rvector.push_back(m_keys[i]);
  }
  return(rvector);
}


//--------------------------------------------------------
// Procedure: printReport
//     Notes: 

void GrepHandler::printReport()
{
  double total_lines = m_lines_retained + m_lines_removed;
  double total_chars = m_chars_retained + m_chars_removed;

  double pct_lines_retained = (m_lines_retained / total_lines);
  double pct_lines_removed  = (m_lines_removed  / total_lines);
  double pct_chars_retained = (m_chars_retained / total_chars);
  double pct_chars_removed  = (m_chars_removed  / total_chars);

  cout << "  Total lines retained: " << doubleToString(m_lines_retained,0);
  cout << " (" << doubleToString((100*pct_lines_retained),2) << "%)" << endl;
  
  cout << "  Total lines excluded: " << doubleToString(m_lines_removed,0);
  cout << " (" << doubleToString((100*pct_lines_removed),2) << "%)" << endl;

  cout << "  Total chars retained: " << doubleToString(m_chars_retained,0);
  cout << " (" << doubleToString((100*pct_chars_retained),2) << "%)" << endl;

  cout << "  Total chars excluded: " << doubleToString(m_chars_removed,0);
  cout << " (" << doubleToString((100*pct_chars_removed),2) << "%)" << endl;

  cout << "    Variables retained: (" << m_vars_retained.size() << ") "; 
  set<string>::iterator p;
  for(p=m_vars_retained.begin(); p!=m_vars_retained.end(); p++) {
    string varname = *p;
    if(p!=m_vars_retained.begin())
      cout << ", ";
    cout << varname;
  }
  cout << endl;
}


