/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GrabHandler.cpp                                      */
/*    DATE: August 6th, 2008                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include <math.h>
#include "MBUtils.h"
#include "ALogScanner.h"
#include "GrabHandler.h"
#include "LogUtils.h"

using namespace std;


//--------------------------------------------------------
// Procedure: handle
//     Notes: 

void GrabHandler::handle(const string& alogfile, const string& new_alogfile)
{
  ALogScanner scanner;

  m_file_in = fopen(alogfile.c_str(), "r");
  if(!m_file_in) {
    cout << "input not found or unable to open - exiting" << endl;
    exit(0);
  }
  
  if(new_alogfile != "") {
    m_file_out = fopen(new_alogfile.c_str(), "r");
    if(m_file_out) {
      cout << new_alogfile << " already exists - exiting now." << endl;
      fclose(m_file_out);
      exit(0);
    }
    m_file_out = fopen(new_alogfile.c_str(), "w");
  }
  
  int count = 0;
  bool done = false;
  while(!done) {
    count++;
    string line_raw = getNextRawLine(m_file_in);

    if(line_raw == "eof") 
      done = true;
    else {
      string varname = getVarName(line_raw);

      int ksize = m_keys.size();
      bool match = false;
      for(int i=0; ((i<ksize) && !match); i++) {
	if(varname == m_keys[i])
	  match = true;
	else if(m_pmatch[i] && strContains(varname, m_keys[i]))
	  match = true;
      }

      if((match || (count < 6) && (line_raw != ""))) {
	if(m_file_out)
	  fprintf(m_file_out, "%s\n", line_raw.c_str());
	else
	  cout << line_raw << endl;
      }
    }
  }
  if(m_file_out)
    fclose(m_file_out);
  m_file_out = 0;
}

//--------------------------------------------------------
// Procedure: addKey
//     Notes: 

void GrabHandler::addKey(string key)
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


