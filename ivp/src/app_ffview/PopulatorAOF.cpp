/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PopulatorAOF.cpp                                     */
/*    DATE: Jan 14th 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <math.h>
#include "IvPDomain.h"
#include "AOF_Rings.h"
#include "AOF_Ring.h"
#include "AOF_Linear.h"
#include "AOF_AvoidCollision.h"
#include "PopulatorAOF.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: populate

AOF *PopulatorAOF::populate(string filename)
{
  FILE *f = fopen(filename.c_str(), "r");
    
  if(!f) {
    cout << "Could not find File: " << filename << endl;
    return(0);
  }

  cout << "Successfully found file: " << filename << endl;
  fclose(f);
  
  vector<string> file_vector = fileBuffer(filename);
  int lineCount = file_vector.size();
    
  for(int i=0; i<lineCount; i++) {
    string line = stripBlankEnds(file_vector[i]);
    
    if((line.length()!=0) && ((line)[0]!='#')) {
      bool res = handleLine(line);
      if(!res) {
	cout << " Problem with line " << i+1;
	cout << " in the file: " << filename << endl;
	cout << line << endl;
	if(aof)
	  delete(aof);
	return(0);
      }
    }
  }

  if(aof && (aof->initialize() == false)) {
    cout << "Problem with AOF initialization!!!!" << endl;
    delete(aof);
    return(0);
  }

  return(aof);
}


//-------------------------------------------------------------
// Procedure: handleLine

bool PopulatorAOF::handleLine(string line)
{
  // Comments are anything to the right of a "#" or "//"
  line = stripComment(line, "//");
  line = stripComment(line, "#");
  line = stripBlankEnds(line);

  if(line.size() == 0)  // Either blank or comment line
    return(true);  
  
  vector<string> svector = parseString(line, '=');
  int vsize = svector.size();

  string left, right, domain_str;

  left  = stripBlankEnds(svector[0]);
  if(vsize > 1)
    right = stripBlankEnds(svector[1]);
  if(vsize > 2)
    domain_str = stripBlankEnds(svector[2]);

  if(right == "") {
    if((left != "{") && (left != "}"))
      return(false);

    if(left == "{") {
      if(define_mode == 0) {
	define_mode = 1;
	return(true);
      }
      else
	return(false);
    }
    
    if(left == "}") {
      if(define_mode == 1) {
	define_mode = 0;
	if(aof)
	  aof->initialize();
	return(true);
      }
      else
	return(false);
    }
  }
  
  if(define_mode == 0) {
    if(left != "AOF")
      return(false);
    
    bool ok = buildDomain(domain_str);
    if(!ok)
      return(false);

    if(right == "AOF_Rings")
      aof = new AOF_Rings(domain);
    else if(right == "AOF_Ring")
      aof = new AOF_Ring(domain);
    else if(right == "AOF_AvoidCollision")
      aof = new AOF_AvoidCollision(domain);
    else if(right == "AOF_Linear")
      aof = new AOF_Linear(domain);
    return(aof!=0);
  }
  
  if(define_mode == 1) {
    // First see if the parameter is a string parameter
    bool result = aof->setParam(left, right);
    // If it fails, try it as a "double" paramater
    if(!result) {
      double right_val = atof(right.c_str());
      result = aof->setParam(left, right_val);
    }
    return(result);
  }

  return(false);
}


//-------------------------------------------------------------
// Procedure: buildDomain
//      Note: 

bool PopulatorAOF::buildDomain(string domain_descriptor)
{
  // First clear the existing domain;
  IvPDomain new_domain;
  domain = new_domain;

  domain_descriptor = stripBlankEnds(domain_descriptor);

  vector<string> svector = parseString(domain_descriptor, ':');
  int vsize = svector.size();
  if(vsize == 0)
    return(false);
  for(int i=0; i<vsize; i++) {
    string dstr = stripBlankEnds(svector[i]);
    vector<string> svector2 = parseString(dstr, ',');
    int vsize2 = svector2.size();
    if(vsize2 != 4)
      return(false);
    string dname   = svector2[0];
    string dlow    = svector2[1];
    string dhigh   = svector2[2];
    string dpoints = svector2[3];
    double f_dlow    = atof(dlow.c_str());
    double f_dhigh   = atof(dhigh.c_str());
    int    i_dpoints = atoi(dpoints.c_str());

    bool ok = domain.addDomain(dname.c_str(), f_dlow, f_dhigh, i_dpoints);
    if(!ok)
      return(false);
  }
  return(true);
}





