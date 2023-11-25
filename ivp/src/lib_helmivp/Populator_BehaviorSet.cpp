/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Populator_BehaviorSet.cpp                            */
/*    DATE: Jan 27 2005                                          */
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
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "MBUtils.h"
#include "FileBuffer.h"
#include "Populator_BehaviorSet.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

Populator_BehaviorSet::Populator_BehaviorSet(IvPDomain g_domain,
					     InfoBuffer *g_buffer)
{
  m_domain        = g_domain;
  m_info_buffer   = g_buffer;
  m_parse_mode    = "top";

  m_bhv_dir_not_found_ok = false;

  m_mode_set.setInfoBuffer(g_buffer);
}

//-------------------------------------------------------------
// Procedure: populate

BehaviorSet *Populator_BehaviorSet::populate(set<string> bhv_files)
{
  // Augment the BehaviorSet with 


  cout << "Number of behavior files: " << bhv_files.size() << endl;
  unsigned int  line_ix;
  set<string>::const_iterator p;
  for(p=bhv_files.begin(); p!=bhv_files.end(); p++) {
    string filename = *p;
    FILE *f = fopen(filename.c_str(), "r");
    cout << "Processing Behavior File: " << filename << "  START" << endl;

    if(!f) {
      string msg = "Could not find File: " + filename;
      cerr << "    " << msg << endl;
      addConfigWarning(msg);
    } 
    else {
      cout << "    Successfully found file: " << filename << endl;
      fclose(f);
      
      vector<string> file_vector = fileBufferSlash(filename);
      unsigned int lineCount = file_vector.size();
    
      for(line_ix=0; line_ix<lineCount; line_ix++) {
	string line = stripBlankEnds(file_vector[line_ix]);

	bool is_comment = false;
	line = stripBlankEnds(line);
	if(!strncmp("//", line.c_str(), 2))
	  is_comment = true;
	if(!strncmp("#", line.c_str(), 1))
	  is_comment = true;
	
	// Begin Brace-Separate.  If the beginning or end of a line
	// has a brace, pretend as if it was given on two separate
	// lines.
	unsigned int len = line.length();	
	string pre_line, post_line;
	if(!is_comment) {
	  unsigned left_braces = charCount(line, '{');
	  unsigned right_braces = charCount(line, '{');
	  if((len>1) && ((line.at(0) == '{') || (line.at(0) == '}'))) {
	    pre_line += line.at(0);
	    line.erase(0,1);
	  }
	  len = line.length();	
	  if(len > 1) {
	    if(line.at(len-1) == '{') {
	      post_line += line.at(len-1);
	      line.erase(len-1,1);
	    }
	    if((right_braces > left_braces) && (line.at(len-1) == '}')) {
	      post_line += line.at(len-1);
	      line.erase(len-1,1);
	    }
	  }
	}
	// End Brace-Separate.
	  
	bool ok = true;
	if(pre_line != "")
	  ok = ok && handleLine(filename, pre_line, line_ix);
	ok = ok && handleLine(filename, line, line_ix);
	if(post_line != "")
	  ok = ok && handleLine(filename, post_line, line_ix);
	
	//cout << "After line " << i+1 << " mode:[" << m_parse_mode
	//<< "]" << endl; cout << "(" << line << ")" << endl;
	
	if(!ok) {
	  string msg = "Problem with line " + uintToString(line_ix+1);
	  msg +=  " in the BehaviorSet file: " + filename;
	  //msg +=  " Pre_line: [" + pre_line + "]";
	  //msg +=  " Post_line: [" + post_line + "]";
	  msg +=  " Line: [" + line + "]";
	  cerr << "   " << msg << endl;
	  addConfigWarning(msg);
	  return(0);
	}

      }
    }
    cout << "Processing Behavior File: " << filename << "  END" << endl;
  }

  // Build the behaviorset with specs and try to instantiate all the
  // behaviors. If some fail instantiation, abort the behaviorset.
  BehaviorSet *bset = new BehaviorSet;
  bset->setDomain(m_domain);
  bset->setOwnship(m_ownship);

  // Inform the behavior set of the directories to look for dynamically
  // loaded behaviors specified in the moos configuration block
  
  unsigned int i;
  for(i=0; i<m_dir_names.size(); i++) {
    bool result = bset->addBehaviorDir(m_dir_names[i]);
    if(!result && !m_bhv_dir_not_found_ok) {
      addConfigWarning("Could not find behavior directory: " + m_dir_names[i]);
      addConfigWarning("Use bhv_dir_not_found_ok=true to surpress this warning");
    }
  }
  
  for(i=0; i<m_behavior_specs.size(); i++) { 
    bset->addBehaviorSpec(m_behavior_specs[i]);
    m_behavior_specs[i].print();    // mikerb-jan04
  }
  bool ok = bset->buildBehaviorsFromSpecs();

  addConfigWarnings(bset->getWarnings());
  
  if(!ok) {
    delete(bset);
    return(0);
  }    
  
  // Given that all the behaviors were able to be instantiated from
  // their specs, fill out the rest of the behaviorset and return it.
  bset->connectInfoBuffer(m_info_buffer);
  for(i=0; i<initial_vars.size(); i++)
    bset->addInitialVar(initial_vars[i]);
  for(i=0; i<default_vars.size(); i++)
    bset->addDefaultVar(default_vars[i]);
  bset->setModeSet(m_mode_set);
  string sval = m_mode_set.getStringDescription();
  cout << "mode description: " << sval << endl;
  
  return(bset);
}

//-------------------------------------------------------------
// Procedure: populate
//      Note: A convenience function when the behaviors are 
//            given by only one file.

BehaviorSet *Populator_BehaviorSet::populate(string filestr)
{
  set<string> bhv_files;
  bhv_files.insert(filestr);
  return(populate(bhv_files));
}

//----------------------------------------------------------
// Procedure: printBehaviorSpecs()

void Populator_BehaviorSet::printBehaviorSpecs() const
{
  unsigned int i, vsize = m_behavior_specs.size();
  for(i=0; i<vsize; i++) {
    m_behavior_specs[i].print();
  }
}

//----------------------------------------------------------
// Procedure: handleLine
//   Returns: true  if all OK
//            false otherwise
//
bool Populator_BehaviorSet::handleLine(string filename, string line, 
				       unsigned int line_num)
{
  // Comments are anything to the right of a "#" or "//"
  line = stripComment(line, "//");
  //line = stripComment(line, "#");   mikerb-jan04
  line = stripBlankEnds(line);

  // HANDLE A BLANK LINE or COMMMENT LINE 
  if(line.size() == 0)  { 
    if(m_parse_mode == "misc-defined-ish") 
      m_parse_mode = "top";
    else if(m_parse_mode == "set-defined-ish") {
      m_parse_mode = "top";
      m_mode_set.addEntry(m_mode_entry);
      m_mode_entry.clear();
    }
    return(true);  
  }
  
  // Handle LEFT BRACE
  if(line == "{") {
    if(m_parse_mode == "top")
      m_parse_mode = "misc-defining";
    else if(m_parse_mode == "bhv-declared")
      m_parse_mode = "bhv-defining";
    else if(m_parse_mode == "set-declared")
      m_parse_mode = "set-defining";
    else {
      string msg = "File:" + filename + " Line:" + uintToString(line_num);
      msg += "Unexpected open brace '{'";
      cerr << msg << endl;
      addConfigWarning(msg);
      return(false);
    }
    return(true);
  }

  // Handle RIGHT BRACE
  if(line == "}") {
    if(m_parse_mode == "misc-defining")
      m_parse_mode = "misc-defined-ish";
    else if(m_parse_mode == "bhv-defining") {
      m_parse_mode = "top";
      m_curr_bhv_spec.setFileName(filename);
      m_behavior_specs.push_back(m_curr_bhv_spec);
      m_curr_bhv_spec.clear();
    }
    else if(m_parse_mode == "set-defining")
      m_parse_mode = "set-defined-ish";
    else { 
      string msg = "File:" + filename + " Line:" + uintToString(line_num);
      msg += "Unexpected close brace '}'";
      cerr << msg << endl;
      addConfigWarning(msg);
      return(false);
    }
    return(true);
  }
    
  bool ish_mode = false;
  if((m_parse_mode == "misc-defined-ish") || 
     (m_parse_mode == "set-defined-ish"))
    ish_mode = true;
    
  // In each of the strncasecmp lines below we pattern match on the
  // string PLUS ONE BLANK, to allow for the possibility that a 
  // symbol such as "set", if it were alone on a line, might actually
  // be something picked up by one of the ish_modes.
  if((m_parse_mode == "top") || (ish_mode == true)) {
    if((!strncasecmp("initialize ", line.c_str(), 11)) ||
       (!strncasecmp("initialize_ ", line.c_str(), 12))) {
      if(m_parse_mode == "set-defined-ish")
	closeSetMode();
      m_parse_mode = "top";
      string init_str = biteString(line, ' ');
      string init_val = stripBlankEnds(line);
      vector<string> init_vector = parseStringQ(line, ',');
      unsigned int iv, ivsize = init_vector.size();
      for(iv=0; iv<ivsize; iv++) {
	string pair = stripBlankEnds(init_vector[iv]);
	string left = stripBlankEnds(biteString(pair, '='));
	string right = stripBlankEnds(pair);
	if((left=="") || strContainsWhite(left) || (right==""))
	  return(false);
	VarDataPair msg(left, right, "auto");
	if(init_str == "initialize_")
	  msg.set_key("defer");
	else
	  msg.set_key("post");
	initial_vars.push_back(msg);
      }
      return(true);
    }
    else if(!strncasecmp("behavior", line.c_str(), 8)) {
      if(m_parse_mode == "set-defined-ish")
	closeSetMode();
      string bhv_str  = biteString(line, '=');
      string bhv_name = stripBlankEnds(line);
      m_curr_bhv_spec.setBehaviorKind(bhv_name, line_num);
      m_parse_mode = "bhv-declared";
      return(true);
    }    
    else if(!strncasecmp("set ", line.c_str(), 4)) {
      if(m_parse_mode == "set-defined-ish")
	closeSetMode();
      string set_str = tolower(biteString(line, ' '));
      string set_val = stripBlankEnds(line);
      m_mode_entry.clear();
      string mode_var = stripBlankEnds(biteString(set_val, '='));
      string mode_val = stripBlankEnds(set_val);
      if((mode_var != "") && (mode_val != ""))
	m_mode_entry.setHead(mode_var, mode_val);
      else
	return(false);
      m_parse_mode = "set-declared";
      return(true);
    }
  }

  // If we're in a misc-defining or -ish mode, ignore any non-empty
  // line as the possible trailing entry(ies).
  if((m_parse_mode == "misc-defining") || 
     (m_parse_mode == "misc-defining-ish"))
    return(true);
 
  if(m_parse_mode == "set-defined-ish") {
    bool ok = m_mode_entry.setElseValue(line);
    if(ok) 
      closeSetMode();
    m_parse_mode = "top";
    return(ok);
  }
  
  if(m_parse_mode == "bhv-defining") {
    m_curr_bhv_spec.addBehaviorConfig(line, line_num);
    return(true);
  }

  if(m_parse_mode == "set-defining")  {
    string a_condition_string = line;
    bool ok = m_mode_entry.addCondition(a_condition_string);
    return(ok);
  }

  return(false);
}

//----------------------------------------------------------
// Procedure: closeSetMode

void Populator_BehaviorSet::closeSetMode()
{
  m_mode_set.addEntry(m_mode_entry);
  m_mode_entry.clear();
}

//----------------------------------------------------------
// Procedure: addConfigWarning()

void Populator_BehaviorSet::addConfigWarning(const string& warning)
{
  m_config_warnings.push_back(warning);
}

//----------------------------------------------------------
// Procedure: addConfigWarnings()

void Populator_BehaviorSet::addConfigWarnings(vector<string> warnings)
{
  for(unsigned int i=0; i<warnings.size(); i++)
    m_config_warnings.push_back(warnings[i]);
}





