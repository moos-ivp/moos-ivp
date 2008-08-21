/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: FiltHandler.cpp                                      */
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
#include "FiltHandler.h"
#include "LogUtils.h"

using namespace std;


//--------------------------------------------------------
// Constructor

FiltHandler::FiltHandler()
{
  m_file_in       = 0;
  m_file_out      = 0;
  m_chuck_strings = false;
  m_chuck_numbers = false;
  m_logstart      = -1;
  m_timeshift     = false;
}

//--------------------------------------------------------
// Procedure: setParam
//     Notes: 

bool FiltHandler::setParam(const string& param, const string& value)
{
  if(param == "nostrings")
    return(setBooleanOnString(m_chuck_strings, value));
  else if(param == "nonumbers")
    return(setBooleanOnString(m_chuck_numbers, value));
  else if(param == "timeshift")
    return(setBooleanOnString(m_timeshift, value));
  else if(param == "newkey") {
    addVectorKey(m_keys, m_pmatch, value);
    return(true);
  }
  return(false);
}


//--------------------------------------------------------
// Procedure: handle
//     Notes: 

void FiltHandler::handle(const string& alogfile, const string& new_alogfile)
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
  
  int  count = 0;
  bool done  = false;
  while(!done) {
    count++;
    string line_raw = getNextRawLine(m_file_in);

    if(count == 4) 
      m_logstart = getLogStart(line_raw);      

    if((line_raw == "eof") || (line_raw == "err"))
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

      if((!match || (count < 6)) && (line_raw != "")) {
	string data_field = getDataEntry(line_raw);
	if(m_chuck_strings && (varname != "") && !isNumber(data_field))
	  addVectorKey(m_keys, m_pmatch, varname);
	else if(m_chuck_numbers && (varname != "") && isNumber(data_field))
	  addVectorKey(m_keys, m_pmatch, varname);
	else {
	  stripInsigDigits(line_raw);
	  if(m_timeshift)
	    shiftTimeStamp(line_raw, m_logstart);
	  if(m_file_out)
	    fprintf(m_file_out, "%s\n", line_raw.c_str());
	  else
	    cout << line_raw << endl;
	}
      }
    }
  }
  if(m_file_out)
    fclose(m_file_out);
  m_file_out = 0;
}

