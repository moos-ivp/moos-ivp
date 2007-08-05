/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_SSR.cpp                                    */
/*    DATE: Mar 6th 2005 Sun at Bruggers (lao lao lai le)        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "Populator_SSR.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

Populator_SSR::Populator_SSR() 
{
  ssr = 0;
}

//-------------------------------------------------------------
// Procedure: Destructor

Populator_SSR::~Populator_SSR()
{
  if(ssr) delete(ssr);
}

//-------------------------------------------------------------
// Procedure: populate

bool Populator_SSR::populate(const char *filestr)
{
  if(FILE *f=fopen(filestr, "r")) {
    cout << "Successfully found file: " << filestr << endl;
    fclose(f);
  }
  else {
    cout << "Could not find File: " << filestr << endl;
    return(false);
  }
  if(ssr) delete(ssr);
  ssr = new SSR;


  vector<string> file_vector = fileBuffer(filestr);
  int lineCount = file_vector.size();
  
  for(int i=0; i < lineCount; i++) {
    string line = stripBlankEnds(file_vector[i]);
    
    if((line.length()!=0) && ((line)[0]!='#')) {
      int res = handleLine(line);
      if(!res) {
	cout << " Problem with line " << i+1;
	cout << " in the SSR file" << endl;
	cout << line << endl  << endl;
	delete(ssr);
	ssr = 0;
	return(false);
      }
    }
  }

  return(true);
}

//----------------------------------------------------------
// Procedure: handleLine
//   Returns: 1 if all OK
//            0 otherwise
//
// type|name|tag> tagData

bool Populator_SSR::handleLine(string line)
{
  // Comments begin w/ '#' - return indication that line handled
  if(line[0] == '#')  
    return(1);

  vector<string> svector = parseString(line, '|');
  if(svector.size() != 2)
    return(0);

  string entry_name = svector[0];
  string entry_data = svector[1];

  svector = parseString(entry_data, '>');

  string tag = svector[0];
  string tag_data = svector[1];
  
  bool ok = handleEntry(entry_name, tag, tag_data);
  return(ok);
}


//----------------------------------------------------------
// Procedure: handleEntry
//   Returns: 1 if all OK
//            0 otherwise

bool Populator_SSR::handleEntry(string name, string tag, string tag_data)
{
  bool ok = true;

  name     = stripBlankEnds(name);
  tag      = stripBlankEnds(tag);
  tag_data = stripBlankEnds(tag_data);

  int index = -1;
  for(int i=0; i<ssr->getCount(); i++) {
    if(ssr->h_nam[i] == name)
      index = i;
  }
  if(index == -1) {
    ssr->h_nam.push_back(name);
    ssr->h_dom.push_back("");
    ssr->h_bhv.push_back("");
    ssr->h_lon.push_back("");
    ssr->h_lat.push_back("");
    ssr->h_crs.push_back("");
    ssr->h_spd.push_back("");
    ssr->h_pid.push_back("");
    ssr->h_ver.push_back("");
    index = ssr->getCount()-1;
  }

  if(tag == "dom")   ssr->h_dom[index] = tag_data;
  if(tag == "bhv")   ssr->h_bhv[index] = tag_data;
  if(tag == "lon")   ssr->h_lon[index] = tag_data;
  if(tag == "lat")   ssr->h_lat[index] = tag_data;
  if(tag == "crs")   ssr->h_crs[index] = tag_data;
  if(tag == "spd")   ssr->h_spd[index] = tag_data;
  if(tag == "pid")   ssr->h_pid[index] = tag_data;
  if(tag == "ver")   ssr->h_ver[index] = tag_data;

  if((tag != "dom")  && (tag != "bhv")  && (tag != "lon")  && 
     (tag != "lat")  && (tag != "crs")  && (tag != "spd")  &&
     (tag != "pid")  && (tag != "ver"))
    ok = false;

  return(ok);
}




