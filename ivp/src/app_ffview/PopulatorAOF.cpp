/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PopulatorAOF.cpp                                     */
/*    DATE: Jan 14th 2006                                        */
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

#include <iostream>
#include <cmath>
#include <cstdlib>
#include "IvPDomain.h"
#include "AOF_Rings.h"
#include "AOF_Ring.h"
#include "AOF_Gaussian.h"
#include "AOF_Linear.h"
#include "AOF_Quadratic.h"
#include "AOF_AvoidCollision.h"
#include "PopulatorAOF.h"
#include "MBUtils.h"
#include "FileBuffer.h"
#include "AOFFactory.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: populate

AOF *PopulatorAOF::populate(string filename)
{
  vector<string> file_vector = fileBuffer(filename);
  if(file_vector.size() == 0)
    cout << "Invalid or empty file: " << filename << ". Exiting" << endl;
  
  for(unsigned int i=0; i<file_vector.size(); i++) {
    string line = stripBlankEnds(file_vector[i]);
    
    if((line.size()==0) || strBegins(line, "//") || strBegins(line, "#"))
      continue;

    bool ok_line = handleLine(line);
    if(!ok_line) {
      cout << " Problem with line " << i+1;
      cout << " in the file: " << filename << endl;
      cout << line << endl;
      if(aof)
	delete(aof);
      return(0);
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
  
  string left  = stripBlankEnds(biteString(line, '='));
  string right = stripBlankEnds(line);

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
    
    string aof_type   = stripBlankEnds(biteString(right, '='));
    string domain_str = stripBlankEnds(right);
    bool ok = buildDomain(domain_str);
    if(!ok)
      return(false);
    
    if(aof_type == "AOF_Rings")
      aof = new AOF_Rings(domain);
    else if(aof_type == "AOF_Ring")
      aof = new AOF_Ring(domain);
    else if(aof_type == "AOF_Gaussian")
      aof = new AOF_Gaussian(domain);
    else if(aof_type == "AOF_Linear")
      aof = new AOF_Linear(domain);
    else if(aof_type == "AOF_Quadratic")
      aof = new AOF_Quadratic(domain);
    else {
      const string envVar = "IVP_AOF_DIRS";
      const char * dirs = getenv(envVar.c_str());
      if (! dirs) {
        cerr << "The AOF type \"" << aof_type << "\" wasn't recognized as"
                " a built-in AOF name.  But ffview couldn't attempt to"
                " dynamically load that AOF either, because the \"" << envVar
             << "\" environment variable isn't set." << endl;
      }
      else {
        AOFFactory factory;
        factory.loadEnvVarDirectories(envVar, true);

        aof = factory.new_aof(aof_type, domain);
        if (! aof) {
          cerr << "The AOF type \"" << aof_type << "\" wasn't recognized as"
                  " either a built-in or" << endl
               << "dynamically loaded AOF type." << endl
               << endl;
        }
      }
      
      // This will return NULL if 'aof_type' isn't known to the factory.
    }

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















