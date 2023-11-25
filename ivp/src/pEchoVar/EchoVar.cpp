/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: EchoVar.cpp                                          */
/*    DATE: July 22 2006                                         */
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

#include <cstring>
#include <iterator>
#include "EchoVar.h"
#include "MBUtils.h"
#include "ACTable.h"

using namespace std;

//-----------------------------------------------------------------
// Constructor

EchoVar::EchoVar()
{
  // State variables
  m_conditions_met = true;
  m_no_cycles      = false;

  // Configuration variables

  // If false, all incoming mail relevant to echoing, or flipping,
  // is ignored when conditions not met
  m_hold_messages_during_pause = true;

  // If true only echo the most recently received source posting
  m_echo_latest_only = false;
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool EchoVar::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  if(m_no_cycles == false)
    return(false);

  // First go through the mail and determine if the current crop
  // of mail affects the LogicBuffer.
  if(m_logic_buffer.size() > 0) {
    MOOSMSG_LIST::iterator p;
    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
      
      string key   = msg.GetKey();
      string sdata = msg.GetString();
      double ddata = msg.GetDouble();
      
      if(msg.IsDouble())
	m_logic_buffer.updateInfoBuffer(key, ddata);
      else if(msg.IsString())
	m_logic_buffer.updateInfoBuffer(key, sdata);
    }
  }

  // If the logic conditions are not satisfied, ignore the rest of 
  // the mail.
  //m_conditions_met = m_logic_buffer.checkConditions();

  if((!m_conditions_met)  && !m_hold_messages_during_pause)
    return(true);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    holdMessage(msg);
  }

  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool EchoVar::OnConnectToServer()
{
  // register for variables here
  registerVariables();
  return(true);
}

//-----------------------------------------------------------------
// Procedure: Iterate()
//      Note: Happens AppTick times per second

bool EchoVar::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(m_conditions_met && m_no_cycles)
    releaseMessages();

  AppCastingMOOSApp::PostReport();
  return(true);
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool EchoVar::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  CMOOSApp::OnStartUp();

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(true);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::reverse_iterator p;
    for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
      string orig  = *p;
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;

      bool handled = false;
      if(!strncmp(param.c_str(), "flip", 4)) 
	handled = handleFlipEntry(param, value);
      else if(param == "echo") {
	bool boolswitch = false;
	if(strContains(value, "!->")) {
	  value = findReplace(value, "!->", ">");
	  boolswitch = true;
	}
	value = findReplace(value, "->", ">");
	vector<string> svector = parseString(value, '>');
	if(svector.size() != 2)
	  return(false);
	string left  = stripBlankEnds(svector[0]);
	string right = stripBlankEnds(svector[1]);
	handled = addMapping(left, right, boolswitch);
      }
      else if(param == "condition") 
	handled = m_logic_buffer.addNewCondition(value);
      else if(param == "echo_latest_only") 
	handled = setBooleanOnString(m_echo_latest_only, value);
      else if(param == "hold_messages") 
	handled = setBooleanOnString(m_hold_messages_during_pause, value);
      
      if(!handled)
	reportUnhandledConfigWarning(orig);
    }
  }
  
  m_no_cycles = noCycles();
  if(!m_no_cycles) {
    reportConfigWarning("An echo cycle was detected.");
    reportRunWarning("An echo cycle was detected.");
    return(true);
  }

  unsigned int i, vsize = m_eflippers.size();
  for(i=0; i<vsize; i++) 
    m_eflippers[i].print();

  registerVariables();
  return(true);
}

//-----------------------------------------------------------------
// Procedure: registerVariables()

void EchoVar::registerVariables()
{  
  AppCastingMOOSApp::RegisterVariables();

  for(unsigned int i=0; i<m_unique_sources.size(); i++)
    Register(m_unique_sources[i], 0);
  
  for(unsigned int j=0; j<m_eflippers.size(); j++)
    Register(m_eflippers[j].getSourceVar(), 0);


  // Register for all variables found in all conditions.
  vector<string> all_vars = m_logic_buffer.getAllVars();
  for(unsigned int i=0; i<all_vars.size(); i++)
    Register(all_vars[i], 0);

}

//-----------------------------------------------------------------
// Procedure: addMapping

bool EchoVar::addMapping(string src, string targ, bool boolswitch)
{
  if((src == "") || (targ == ""))
    return(false);
  
  bool new_pair = true;
  bool new_src  = true;
  
  unsigned int i, vsize = m_var_source.size();
  for(i=0; i<vsize; i++) {
    if(m_var_source[i] == src) {
      new_src = false;
      if(m_var_target[i] == targ)
	new_pair = false;
    }
  }
   
  if(new_pair) {
    m_var_source.push_back(src);
    m_var_target.push_back(targ);
    m_boolswitch.push_back(boolswitch);
  }
  
  if(new_src)
    m_unique_sources.push_back(src);
  
  return(true);
}


//-----------------------------------------------------------------
// Procedure: noCycles

bool EchoVar::noCycles()
{
  unsigned int  i, vsize    = m_unique_sources.size(); 
  unsigned int  j, map_size = m_var_source.size();;

  vector<string> key_vector;
  vector<string> new_vector;
  for(i=0; i<vsize; i++) {
    key_vector.clear();
    
    for(j=0; j<map_size; j++)
      if(m_unique_sources[i] == m_var_source[j])
	if(!vectorContains(key_vector, m_var_target[j]))
	  key_vector.push_back(m_var_target[j]);

    new_vector = expand(key_vector);
    
    if(vectorContains(new_vector, m_unique_sources[i]))
      return(false);
  }
  return(true);
}



//-----------------------------------------------------------------
// Procedure: expand

vector<string> EchoVar::expand(vector<string> key_vector)
{
  unsigned int i, vsize = key_vector.size();
  unsigned int j, map_size = m_var_source.size();;
  for(i=0; i<vsize; i++) {
    string key = key_vector[i];
    for(j=0; j<map_size; j++)
      if(key == m_var_source[j])
	if(!vectorContains(key_vector, m_var_target[j]))
	  key_vector.push_back(m_var_target[j]);
  }

  if(key_vector.size() == vsize)
    return(key_vector);
  else
    return(expand(key_vector));
}

//-----------------------------------------------------------------
// Procedure: handleFlipEntry

bool EchoVar::handleFlipEntry(string sKey, string sLine)
{
  vector<string> svector = parseString(sKey, ':');
  unsigned int vsize = svector.size();
  if(vsize != 2) 
    return(false);
  string tag = stripBlankEnds(svector[0]);
  string key = stripBlankEnds(svector[1]);
  if(tolower(tag) != "flip")
    return(false);

  // Determine the index of the given flipper reference
  int index = -1;

  unsigned int i, esize = m_eflippers.size();
  for(i=0; i<esize; i++) {
    if(key == m_eflippers[i].getKey())
      index = i;
  }
  
  // If pre-existing EFlipper not found, create a new one
  if(index == -1) {
    EFlipper new_flipper;
    new_flipper.setParam("key", key);
    m_eflippers.push_back(new_flipper);
    m_eflip_count.push_back(0);
    index = esize;
  }
    
  if(!strncmp(sLine.c_str(), "source_variable", 15)) {
    string left = biteString(sLine, '=');
    string right = stripBlankEnds(sLine);
    bool ok = m_eflippers[index].setParam("source_variable", right);
    return(ok);
  }
  if(!strncmp(sLine.c_str(), "dest_variable", 13)) {
    string left = biteString(sLine, '=');
    string right = stripBlankEnds(sLine);
    bool ok = m_eflippers[index].setParam("dest_variable", right);
    return(ok);
  }
  if(!strncmp(sLine.c_str(), "source_separator", 16)) {
    string left = biteString(sLine, '=');
    string right = stripBlankEnds(sLine);
    bool ok = m_eflippers[index].setParam("source_separator", right);
    return(ok);
  }
  if(!strncmp(sLine.c_str(), "dest_separator", 14)) {
    string left = biteString(sLine, '=');
    string right = stripBlankEnds(sLine);
    bool ok = m_eflippers[index].setParam("dest_separator", right);
    return(ok);
  }
  if(!strncmp(sLine.c_str(), "filter", 6)) {
    string left = biteString(sLine, '=');
    string right = stripBlankEnds(sLine);
    bool ok = m_eflippers[index].setParam("filter", right);
    return(ok);
  }
  if(strContains(sLine, "==")) {
    bool ok = m_eflippers[index].setParam("filter", sLine);
    return(ok);
  }
  if(!strncmp(sLine.c_str(), "component", 9)) {
    string left = biteString(sLine, '=');
    string right = stripBlankEnds(sLine);
    bool ok = m_eflippers[index].setParam("component", right);
    return(ok);
  }
  if(strContains(sLine, "->")) {
    bool ok = m_eflippers[index].setParam("component", sLine);
    return(ok);
  }

  return(false);
}


//-----------------------------------------------------------------
// Procedure: holdMessage
//   Purpose: Hold a given message in a list ordered by time of
//              receipt oldest to newest. 
//            Prior to adding the new message to the end of the list,
//              the list is scanned and if there is an older message
//              matching the MOOS variable (key), then that message is
//              removed from the list. 
//            When the list is posted, only the most recent value of 
//              each variable will be posted.
//            This pruning of duplicate messages only applies when the
//              logic conditions are not met.


void EchoVar::holdMessage(CMOOSMsg msg)
{
  string key = msg.GetKey();

  m_map_hits[key]++;
  
  if(!m_conditions_met || m_echo_latest_only) {
    list<CMOOSMsg>::iterator p;
    for(p=m_held_messages.begin(); p!=m_held_messages.end();) {
      CMOOSMsg imsg = *p;
      if(imsg.GetKey() == key)
	p = m_held_messages.erase(p);
      else
	++p;
    }
  }
  
  m_held_messages.push_back(msg);
}


//-----------------------------------------------------------------
// Procedure: releaseMessages

void EchoVar::releaseMessages()
{
  list<CMOOSMsg>::iterator p;
  for(p=m_held_messages.begin(); p!=m_held_messages.end(); p++) {
    CMOOSMsg msg = *p;
    
    string key   = msg.GetKey();
    string sdata = msg.GetString();
    double ddata = msg.GetDouble();
    
    unsigned int i, vsize = m_var_source.size();
    for(i=0; i<vsize; i++) {
      if(key == m_var_source[i]) {
	string new_key = m_var_target[i];	
	string srctarg = key + ":" + new_key;
	m_map_posts[srctarg]++;
	if(msg.IsDouble())
	  Notify(new_key, ddata);
	else if(msg.IsString()) {
	  if(m_boolswitch[i]) {
	    if(sdata == "true")
	      sdata = "false";
	    else if(sdata == "false")
	      sdata = "true";
	    else if(sdata == "TRUE")
	      sdata = "FALSE";
	    else if(sdata == "FALSE")
	      sdata = "TRUE";
	    else if(sdata == "False")
	      sdata = "True";
	    else if(sdata == "True")
	      sdata = "False";
	    else if(tolower(sdata) == "true")
	      sdata = "false";
	    else if(tolower(sdata) == "false")
	      sdata = "true";
	  }
	  Notify(new_key, sdata);
	}
      }
    }
    
    unsigned int j, fsize = m_eflippers.size();
    for(j=0; j<fsize; j++) {
      if(key == m_eflippers[j].getSourceVar()) {
	m_eflip_count[j]++;
	string flip_result = m_eflippers[j].flip(sdata);
	if(flip_result != "")
	  Notify(m_eflippers[j].getDestVar(), flip_result);
      }
    }
  }
  m_held_messages.clear();
}

//---------------------------------------------------------
// Procedure: buildReport()
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.
//   Example:
//
//   conditions_met:   true
//   hold_messages:    true
//   echo_latest_only: true
//
//   ==========================================================
//   Echoes:
//   ==========================================================
//                       
//       Source            Dest      Hits  Posts    
//       -----------  ---  -----     ----  -----
//             USM_X  -->  NAV_X     23    11 
//             USM_Y  -->  NAV_Y     23    11
//             USM_X  -->  NAV_XPOS  23    11
//             USM_Y  -->  NAV_YPOS  23    11
//       USM_HEADING  -->  NAV_X     22     9
//         USM_SPEED  -->  NAV_X     24    10
//   
//   ==========================================================
//   Flips:           
//   ==========================================================
//                              Src  Dest          Old    New
//   Key  Hits   Source  Dest   Sep  Sep   Filter  Field  Field
//   ---  ----   ------  ----   ---  ----  ------  -----  -----
//   1    17     ALPHA   BRAVO  ,    ,     type=1  xpos   x
//                                                 ypos   y


bool EchoVar::buildReport()
{
  unsigned int echo_cnt = m_var_source.size();

  m_msgs << "conditions_met:   " << boolToString(m_conditions_met) << endl;
  m_msgs << "hold_messages:    " << boolToString(m_hold_messages_during_pause) << endl;
  m_msgs << "echo_latest_only: " << boolToString(m_echo_latest_only) << endl << endl;

  m_msgs << "==================================================" << endl;
  m_msgs << "Echoes: (" << echo_cnt << ")" << endl;
  m_msgs << "==================================================" << endl << endl;

  if(echo_cnt > 0) {
    ACTable actab(5,2);
    actab << "Source | | Dest | Hits | Posts \n";
    actab.addHeaderLines();
    for(unsigned int i=0; i<echo_cnt; i++) {
      string source  = m_var_source[i];
      string srctarg = source + ":" + m_var_target[i];
      string hits    = uintToString(m_map_hits[source]);
      string posts   = uintToString(m_map_posts[srctarg]);
      actab << m_var_source[i] << "-->" << m_var_target[i] << hits << posts;;
    }
    m_msgs << actab.getFormattedString();
  }
  m_msgs << endl << endl;

  unsigned int flip_cnt = m_eflippers.size();

  m_msgs << "==================================================" << endl;
  m_msgs << "Flips: (" << flip_cnt << ")" << endl;
  m_msgs << "==================================================" << endl << endl;

  if(flip_cnt > 0) {
    ACTable actab(9,2);
    actab << "    |      |        |      | Src | Dest|        | Old   | New   \n";
    actab << "Key | Hits | Source | Dest | Sep | Sep | Filter | Field | Field \n";
    actab.addHeaderLines();
    for(unsigned int i=0; i<flip_cnt; i++) {
      string key  = m_eflippers[i].getKey();
      string hits = uintToString(m_eflip_count[i]);
      string src  = m_eflippers[i].getSourceVar();
      string dest = m_eflippers[i].getDestVar();
      string src_sep = m_eflippers[i].getSourceSep();
      string dest_sep = m_eflippers[i].getDestSep();
      string filters = m_eflippers[i].getFilters();

      string components = m_eflippers[i].getComponents();
      vector<string> svector = parseString(components, ',');
      for(unsigned int j=0; j<svector.size(); j++) {
	string old_fld = biteStringX(svector[j], ':');
	string new_fld = svector[j];
	actab << key << hits << src << dest << src_sep << dest_sep << filters;
	actab << old_fld << new_fld;
      }
    }
    m_msgs << actab.getFormattedString();
  }

  return(true);
}









