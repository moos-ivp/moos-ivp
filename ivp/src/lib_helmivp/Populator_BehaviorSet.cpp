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

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "MBUtils.h"
#include "FileBuffer.h"
#include "Populator_BehaviorSet.h"

// CORE Behaviors
#include "BHV_Waypoint.h"
#include "BHV_Loiter.h"
#include "BHV_OpRegion.h"
#include "BHV_ConstantDepth.h"
#include "BHV_ConstantHeading.h"
#include "BHV_MaintainHeading.h"
#include "BHV_ConstantSpeed.h"
#include "BHV_PeriodicSpeed.h"
#include "BHV_PeriodicSurface.h"
#include "BHV_Trail.h"
#include "BHV_Shadow.h"
#include "BHV_Timer.h"
#include "BHV_HSLine.h"
#include "BHV_HeadingChange.h"
#include "BHV_StationKeep.h"
#include "BHV_RStationKeep.h"
#include "BHV_CutRange.h"
#include "BHV_AvoidCollision.h"
#include "BHV_AvoidObstacles.h"
#include "BHV_GoToDepth.h"
#include "BHV_MemoryTurnLimit.h"
#include "BHV_Hysteresis.h"
#include "BHV_Attractor.h"
#include "BHV_RubberBand.h"
// Mikes new collision fix
#include "BHV_HeadingBias.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

Populator_BehaviorSet::Populator_BehaviorSet(IvPDomain g_domain,
					     InfoBuffer *g_buffer)
{
  // cout << "Populator_BehaviorSet::Constructor()" << endl;
  // g_domain.print();
  // cout << endl << endl;

  m_domain             = g_domain;
  m_info_buffer        = g_buffer;
  m_parse_mode         = "top";

  m_mode_set.setInfoBuffer(g_buffer);
}

//-------------------------------------------------------------
// Procedure: populate

BehaviorSet *Populator_BehaviorSet::populate(set<string> bhv_files)
{
  cout << "Number of behavior files: " << bhv_files.size() << endl;
  unsigned int i;
  set<string>::const_iterator p;
  for(p=bhv_files.begin(); p!=bhv_files.end(); p++) {

    string filename = *p;

    FILE *f = fopen(filename.c_str(), "r");
    
    cout << "Processing Behavior File: " << filename << "  START" << endl;

    if(!f) 
      cout << "    Could not find File: " << filename << endl;
    else {
      cout << "    Successfully found file: " << filename << endl;
      fclose(f);
      
      vector<string> file_vector = fileBufferSlash(filename);
      unsigned int lineCount = file_vector.size();
    
      for(i=0; i<lineCount; i++) {
	string line = stripBlankEnds(file_vector[i]);

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
	  if((len>1) && ((line.at(0) == '{') || (line.at(0) == '}'))) {
	    pre_line += line.at(0);
	    line.erase(0,1);
	  }
	  len = line.length();	
	  if((len>1) && ((line.at(len-1) == '{') || (line.at(len-1) == '}'))) {
	    post_line += line.at(len-1);
	    line.erase(len-1,1);
	  }
	}
	// End Brace-Separate.
	  
	bool ok = true;
	if(pre_line != "")
	  ok = ok && handleLine(pre_line);
	ok = ok && handleLine(line);
	if(post_line != "")
	  ok = ok && handleLine(post_line);
	
	//cout << "After line " << i+1 << " mode:[" << m_parse_mode
	//<< "]" << endl; cout << "(" << line << ")" << endl;
	
	if(!ok) {
	  cout << "    Problem with line " << i+1;
	  cout << "    in the BehaviorSet file: " << filename << endl;
	  cout << line << endl;
	  return(0);
	}

      }
    }
    cout << "Processing Behavior File: " << filename << "  END" << endl;
  }


  if(behaviors.size() == 0) 
    return(0);
  else {
    BehaviorSet *bset = new BehaviorSet;
    for(i=0; i<behaviors.size(); i++) {
      behaviors[i]->setInfoBuffer(m_info_buffer);
      bset->addBehavior(behaviors[i]);
    }
    for(i=0; i<initial_vars.size(); i++)
      bset->addInitialVar(initial_vars[i]);
    for(i=0; i<default_vars.size(); i++)
      bset->addDefaultVar(default_vars[i]);
    bset->setModeSet(m_mode_set);
#if 1
    string sval = m_mode_set.getStringDescription();
    cout << "mode description: " << sval << endl;
#endif    
    //m_mode_set.print();
    return(bset);
  }  
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
// Procedure: handleLine
//   Returns: 1 if all OK
//            0 otherwise
//
bool Populator_BehaviorSet::handleLine(string line)
{
  // Comments are anything to the right of a "#" or "//"
  line = stripComment(line, "//");
  line = stripComment(line, "#");
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
      cout << "Unexpected open brace '{'" << endl;
      return(false);
    }
    return(true);
  }

  // Handle RIGHT BRACE
  if(line == "}") {
    if(m_parse_mode == "misc-defining")
      m_parse_mode = "misc-defined-ish";
    else if(m_parse_mode == "bhv-defining")
      m_parse_mode = "top";
    else if(m_parse_mode == "set-defining")
      m_parse_mode = "set-defined-ish";
    else { 
      cout << "Unexpected close brace '}'" << endl;
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
    if(!strncasecmp("initialize ", line.c_str(), 11)) {
      if(m_parse_mode == "set-defined-ish")
	closeSetMode();
      m_parse_mode = "top";
      string init_str = biteString(line, ' ');
      string init_val = stripBlankEnds(line);
      vector<string> dvector = parseString(init_val, '=');
      if(dvector.size() != 2) {
	return(false);
      }
      VarDataPair msg(dvector[0], dvector[1], "auto");
      initial_vars.push_back(msg);
      return(true);
    }
    else if(!strncasecmp("behavior ", line.c_str(), 9)) {
      if(m_parse_mode == "set-defined-ish")
	closeSetMode();
      string bhv_str  = biteString(line, '=');
      string bhv_name = stripBlankEnds(line);
      IvPBehavior *bhv = initializeBehavior(bhv_name); 
      if(bhv) {
	behaviors.push_back(bhv);
	m_parse_mode = "bhv-declared";
	return(true);
      }
      return(false);
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
    string left  = stripBlankEnds(tolower(biteString(line, '=')));
    string right = stripBlankEnds(line); 
    IvPBehavior *bhv = behaviors[behaviors.size()-1];
    bool result = bhv->IvPBehavior::setParam(left.c_str(), right.c_str());
    if(!result)
      result = bhv->setParam(left.c_str(), right.c_str());
    return(result);
  }

  if(m_parse_mode == "set-defining")  {
    string a_condition_string = line;
    bool ok = m_mode_entry.addCondition(a_condition_string);
    return(ok);
  }
  return(false);
}


//----------------------------------------------------------
// Procedure: initializeBehavior

IvPBehavior* Populator_BehaviorSet::initializeBehavior(string bhv_name)
{
  IvPBehavior *bhv = 0;      

  if(bhv_name == "BHV_OpRegion")
    bhv = new BHV_OpRegion(m_domain);
  else if(bhv_name == "BHV_Waypoint")   
    bhv = new BHV_Waypoint(m_domain);
  else if(bhv_name == "BHV_ConstantSpeed")     
    bhv = new BHV_ConstantSpeed(m_domain);
  else if(bhv_name == "BHV_Trail")      
    bhv = new BHV_Trail(m_domain);
  else if(bhv_name == "BHV_ConstantDepth")      
    bhv = new BHV_ConstantDepth(m_domain);
  else if(bhv_name == "BHV_ConstantHeading")      
    bhv = new BHV_ConstantHeading(m_domain);
  else if(bhv_name == "BHV_MaintainHeading")      
    bhv = new BHV_MaintainHeading(m_domain);
  else if(bhv_name == "BHV_Loiter")     
    bhv = new BHV_Loiter(m_domain);
  else if(bhv_name == "BHV_StationKeep")     
    bhv = new BHV_StationKeep(m_domain);
  else if(bhv_name == "BHV_RStationKeep")     
    bhv = new BHV_RStationKeep(m_domain);
  else if(bhv_name == "BHV_Timer")     
    bhv = new BHV_Timer(m_domain);
  else if(bhv_name == "BHV_HSLine")     
    bhv = new BHV_HSLine(m_domain);
  else if(bhv_name == "BHV_HeadingChange")     
    bhv = new BHV_HeadingChange(m_domain);
  else if(bhv_name == "BHV_Shadow")     
    bhv = new BHV_Shadow(m_domain);
  else if(bhv_name == "BHV_CutRange")   
    bhv = new BHV_CutRange(m_domain);
  else if(bhv_name == "BHV_AvoidCollision") 
    bhv = new BHV_AvoidCollision(m_domain);
  else if(bhv_name == "BHV_AvoidObstacles") 
    bhv = new BHV_AvoidObstacles(m_domain);
  else if(bhv_name == "BHV_PeriodicSpeed") 
    bhv = new BHV_PeriodicSpeed(m_domain);
  else if(bhv_name == "BHV_PeriodicSurface") 
    bhv = new BHV_PeriodicSurface(m_domain);
  else if(bhv_name == "BHV_GoToDepth")      
    bhv = new BHV_GoToDepth(m_domain);
  else if(bhv_name == "BHV_MemoryTurnLimit")      
    bhv = new BHV_MemoryTurnLimit(m_domain);
  else if(bhv_name == "BHV_Hysteresis")      
    bhv = new BHV_Hysteresis(m_domain);
  else if(bhv_name == "BHV_Attractor")      
    bhv = new BHV_Attractor(m_domain);
  else if(bhv_name == "BHV_RubberBand")      
    bhv = new BHV_RubberBand(m_domain);
  else if(bhv_name == "BHV_HeadingBias")     
    bhv = new BHV_HeadingBias(m_domain);

  return(bhv);
}


//----------------------------------------------------------
// Procedure: closeSetMode

void Populator_BehaviorSet::closeSetMode()
{
  m_mode_set.addEntry(m_mode_entry);
  m_mode_entry.clear();
}
