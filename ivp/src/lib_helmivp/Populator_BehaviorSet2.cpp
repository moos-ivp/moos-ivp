/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_BehaviorSet.cpp                            */
/*    DATE:                                                      */
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
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "MBUtils.h"
#include "FileBuffer.h"
#include "Populator_BehaviorSet2.h"

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif


using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

Populator_BehaviorSet2::Populator_BehaviorSet2(IvPDomain g_domain,
					     InfoBuffer *g_buffer)
{
  // cout << "Populator_BehaviorSet2::Constructor()" << endl;
  // g_domain.print();
  // cout << endl << endl;

  domain             = g_domain;
  info_buffer        = g_buffer;
  define_mode        = 0;
  open_behavior_mode = false;
  ok                 = false;
}

//-------------------------------------------------------------
// Procedure: loadBehaviors

void Populator_BehaviorSet2::loadBehaviors(const string & lib_directory) 
{

}


//-------------------------------------------------------------
// Procedure: populate

BehaviorSet *Populator_BehaviorSet2::populate(set<string> bhv_files)
{
  int i;
  set<string>::const_iterator p;
  for(p=bhv_files.begin(); p!=bhv_files.end(); p++) {

    string filename = *p;

    FILE *f = fopen(filename.c_str(), "r");
    
    if(!f) 
      cout << "Could not find File: " << filename << endl;
    else {
      cout << "Successfully found file: " << filename << endl;
      fclose(f);
      
      vector<string> file_vector = fileBufferSlash(filename);
      int lineCount = file_vector.size();
    
      for(i=0; i<lineCount; i++) {
	string line = stripBlankEnds(file_vector[i]);
	
	if((line.length()!=0) && ((line)[0]!='#')) {
	  int res = handleLine(line);
	  if(!res) {
	    cout << " Problem with line " << i+1;
	    cout << " in the BehaviorSet file: " << filename << endl;
	    cout << line << endl;
	    return(0);
	  }
	}
      }
    }
  }

  if(behaviors.size() == 0) 
    return(0);
  else {
    BehaviorSet *bset = new BehaviorSet;
    for(i=0; i<behaviors.size(); i++) {
      behaviors[i]->setInfoBuffer(info_buffer);
      bset->addBehavior(behaviors[i]);
    }
    for(i=0; i<initial_vars.size(); i++)
      bset->addInitialVar(initial_vars[i]);
    for(i=0; i<default_vars.size(); i++)
      bset->addDefaultVar(default_vars[i]);
    return(bset);
  }
}

//-------------------------------------------------------------
// Procedure: populate
//      Note: A convenience function when the behaviors are 
//            given by only one file.

BehaviorSet *Populator_BehaviorSet2::populate(string filestr)
{
  set<string> bhv_files;
  bhv_files.insert(filestr);
  return(populate(bhv_files));
}

//----------------------------------------------------------
// Procedure: handleLine
//   Returns: 1 if all OK
//            0 otherwise
//
bool Populator_BehaviorSet2::handleLine(string line)
{
  // Comments are anything to the right of a "#" or "//"
  line = stripComment(line, "//");
  line = stripComment(line, "#");
  line = stripBlankEnds(line);
  if(line.size() == 0)  // Either blank or comment line
    return(true);  
  
  if(!strncasecmp("initialize", line.c_str(), 10))
    line = findReplace(line, "initialize", "initial_var = ");

  vector<string> svector = chompString(line, '=');

  string left  = stripBlankEnds(svector[0]);
  string right = stripBlankEnds(svector[1]);

  if(right == "") {
    if((left != "{") && (left != "}")) {
      cout << "PROBLEM #1" << endl;
      return(false);
    }

    if(left == "{") {
      if(define_mode == 0) {
	if(open_behavior_mode)
	  define_mode = 1;
	return(true);
      }
      else { 
	cout << "PROBLEM #2" << endl;
	return(false);
      }
    }
      
    if(left == "}") {
      open_behavior_mode = false;
      define_mode = 0;
      return(true);
    }
  }
  
  // Handle initialization lines
  string str = tolower(left);
  if((str == "initial_var") && (define_mode == 0)) {
    right = findReplace(right, ',', '=');
    vector<string> dvector = parseString(right, '=');
    if(dvector.size() != 2) {
      cout << "PROBLEM #2" << endl;
      return(false);
    }
    VarDataPair msg(dvector[0], dvector[1], "auto");
    initial_vars.push_back(msg);
    return(true);
  }

  // Handle default lines
  if((str == "default") && (define_mode == 0)) {
    vector<string> dvector = parseString(right, '=');
    if(dvector.size() != 2)
      return(false);
    VarDataPair msg(dvector[0], dvector[1], "auto");
    default_vars.push_back(msg);
    return(true);
  }
  
  if(define_mode == 0) {
    if(left != "Behavior") {
      if(open_behavior_mode)
	return(false);
      else
	return(true);
    }
    else
      open_behavior_mode = true;

    IvPBehavior *bhv = initializeBehavior(right); 

    if(bhv)
      behaviors.push_back(bhv);
    return(bhv!=0);
  }
  
  if(define_mode == 1) {
    IvPBehavior *bhv = behaviors[behaviors.size()-1];
    bool result = bhv->setParam(left.c_str(), right.c_str());
    return(result);
  }

  return(false);
}

//----------------------------------------------------------

void Populator_BehaviorSet2::load_behavior_libs(string dirname) 
{
  bhv_factory.load_directory(dirname);
}

//----------------------------------------------------------
// Procedure: initializeBehavior

IvPBehavior* Populator_BehaviorSet2::initializeBehavior(string bhv_name)
{
  if (! bhv_factory.is_known_behavior(bhv_name)) {
    return NULL;
  }

  return bhv_factory.new_behavior(bhv_name, domain);
}
