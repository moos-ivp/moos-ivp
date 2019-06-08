/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UCMD_MOOSApp.cpp                                     */
/*    DATE: July 1st, 2016                                       */
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
#include "UCMD_MOOSApp.h"
#include "MBUtils.h"
#include "ACTable.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

UCMD_MOOSApp::UCMD_MOOSApp() 
{
  m_pending_moos_events = 0;
  m_gui                 = 0; 
}

//----------------------------------------------------------------
// Procedure: setPendingEventsPipe

void UCMD_MOOSApp::setPendingEventsPipe(Threadsafe_pipe<MOOS_event>*
					pending_moos_events)
{
  m_pending_moos_events = pending_moos_events;
}

//----------------------------------------------------------------
// Procedure: OnNewMail

bool UCMD_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  if((!m_gui) || (!m_pending_moos_events))
    return(true);

  NewMail.sort();
  
  MOOS_event e;
  e.type="OnNewMail";
  e.moos_time = MOOSTime();

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &Msg = *p;
    e.mail.push_back(MOOS_event::Mail_message(Msg));
  }
  
  m_pending_moos_events->enqueue(e);
  Fl::awake();
  
  return(true);
}


//--------------------------------------------------------------
// Procedure: OnConnectToServer()
//      Note: virtual overide of base class CMOOSApp member.

bool UCMD_MOOSApp::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//-------------------------------------------------------------
// Procedure: Iterate
//      Note: virtual overide of base class CMOOSApp member.

bool UCMD_MOOSApp::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if((!m_gui) || (!m_pending_moos_events))
    return(true);  
 
  MOOS_event e;
  e.type="Iterate";
  e.moos_time = MOOSTime();

  m_pending_moos_events->enqueue(e);
  Fl::awake();
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//--------------------------------------------------------
// Procedure: OnStartUp()

bool UCMD_MOOSApp::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  if((!m_gui) || (!m_pending_moos_events))
    return(true);
  
  MOOS_event e;
  e.type="OnStartUp";
  e.moos_time = MOOSTime();

  m_pending_moos_events->enqueue(e);
  Fl::awake();

  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void UCMD_MOOSApp::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("DB_UPTIME", 0);
  Register("APPCAST", 0);
}

//----------------------------------------------------------------------
// Procedure: handleNewMail  (OnNewMail)

void UCMD_MOOSApp::handleNewMail(const MOOS_event & e)
{
#if 0
  for(size_t i=0; i<e.mail.size(); ++i) {
    CMOOSMsg msg = e.mail[i].msg;
    string key   = msg.GetKey();
    string sval  = msg.GetString();
    //double dval  = msg.GetDouble();
    string community = msg.GetCommunity();
    string source = msg.GetSource();

    if(key == "APPCAST") {
      m_appcast_repo->addAppCast(sval);
      handled_appcast = true;
    }
  }
#endif
}


//----------------------------------------------------------------------
// Procedure: handleIterate

void UCMD_MOOSApp::handleIterate(const MOOS_event& e) 
{
  //cout << "-------------------------------------------------" << endl;
  //cout << "Iteration: " << m_iteration << endl;
  //m_cmd_folio.print();

  handlePendingPostsFromGUI();
  handlePendingCommandSummary();

  //unsigned int window_val = 1;
  //if(!m_gui)
  //  window_val = 0;
  //else
  //  window_val = m_gui->isVisible();
}


//----------------------------------------------------------------------
// Procedure: handleStartUp (OnStartUp)

void UCMD_MOOSApp::handleStartUp(const MOOS_event & e) 
{
  if(!m_gui)
    return;

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  STRING_LIST::reverse_iterator p;
  for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "cmd") {
      cout << "Handling cmd line: " << value << endl;
      handled = handleConfigCmd(value);
      cout << "handled: " << boolToString(handled) << endl;
    }
    else if(param == "limited_vnames") {
      handled = handleConfigLimitedVNames(value);
    }
      
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
 
  cout << "====================================A " << endl;
  m_cmd_folio.print();
  cout << "====================================B " << endl;
  m_cmd_folio.limitedVNames(m_limited_vnames);
  m_cmd_folio.print();
  cout << "====================================C " << endl;

  if(m_gui)
    m_gui->setCommandFolio(m_cmd_folio);
  
  //m_gui->updateRadios();
  registerVariables();
}


//---------------------------------------------------------
// Procedure: handleConfigCmd
//  Examples:
//   cmd = label=loiter,   var=LOITER,      sval=true
//   cmd = label=go_deep,  var=DEPTH,       dval=400
//   cnd = label=PolyWest, var=LOITER_POLY, sval={60,-40:60,-160:150,-160}

bool UCMD_MOOSApp::handleConfigCmd(string cmd)
{
  string label, moosvar, sval, dval, receivers;
  
  vector<string> svector = parseStringQ(cmd, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string field = tolower(biteStringX(svector[i], '='));
    string value = svector[i];

    if((field == "label") && (label == ""))
      label = value;
    else if((field == "var") && (moosvar == ""))
      moosvar = value;
    else if((field == "sval") && (sval == ""))
      sval = value;
    else if((field == "dval") && (dval == "") && isNumber(value))
      dval = value;
    else if((field == "receivers") && (receivers == ""))
      receivers = value;
    else
      return(false);
  }

  if((label == "") || (moosvar == ""))
    return(false);
  if((sval == "") && (dval == ""))
    return(false);
  if((sval != "") && (dval != ""))
    return(false);

  if(isBraced(sval))
    sval = stripBraces(sval);
  
  // Part 2: Build the Command Item
  CommandItem item;
  bool ok = true;
  item.setCmdLabel(label);

  if(sval != "")
    ok = ok && item.setCmdPostStr(moosvar, sval);
  else 
    ok = ok && item.setCmdPostDbl(moosvar, atof(dval.c_str()));
  
  vector<string> rvector = parseString(receivers, ':');
  for(unsigned int i=0; i<rvector.size(); i++) {
    string receiver = stripBlankEnds(rvector[i]);
    ok = ok && item.addCmdPostReceiver(receiver);
  }

  if(!ok)
    return(false);

  // Part 3: Add the new command item
  cout << "Adding Command item:" << endl;
  bool result = m_cmd_folio.addCmdItem(item);
  cout << "result: " << boolToString(result) << endl;
  
  return(true);
}

//---------------------------------------------------------
// Procedure: handleConfigLimitedVNames
//   Purpose: If a non-empty set, then the GUI will only render
//            buttons for the listed vehicles and not the others.
//  Examples:
//   limited_vnames = henry,gus

bool UCMD_MOOSApp::handleConfigLimitedVNames(string vnames)
{
  vector<string> svector = parseString(vnames, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string vname = stripBlankEnds(svector[i]);
    m_limited_vnames.insert(vname);
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: buildReport()
//      Note: A virtual function of the AppCastingMOOSApp superclass,
//            conditionally invoked if either a terminal or appcast
//            report is needed.

bool UCMD_MOOSApp::buildReport()
{
  return(false);
}


//----------------------------------------------------------------------
// Procedure: handlePendingPostsFromGUI

void UCMD_MOOSApp::handlePendingPostsFromGUI()
{
  if(!m_gui)
    return;

  vector<CommandPost> cmd_posts = m_gui->getPendingCmdPosts();

  for(unsigned i=0; i<cmd_posts.size(); i++) {
    CommandItem cmd_item = cmd_posts[i].getCommandItem();
    string      cmd_targ = cmd_posts[i].getCommandTarg();
    bool        cmd_test = cmd_posts[i].getCommandTest();
    string      cmd_pid  = cmd_posts[i].getCommandPID();

    string moosvar = cmd_item.getCmdPostVar();
    if((cmd_targ != "local") && (cmd_targ != "shore"))
      moosvar += "_" + toupper(cmd_targ);
      
    string valtype = cmd_item.getCmdPostType();

    // Part 1: Make the posting if the posting is real. A posting with a
    // target name beginning with "test:" indicates that a posting should
    // not be made, but the posting should still go into the cmd_summary
    // to show the user what would have been posted.
    if(!cmd_test) {
      string src_aux = "pid=" + cmd_pid;
      if(valtype == "string") 
	Notify(moosvar, cmd_item.getCmdPostStr(), src_aux);
      else 
	Notify(moosvar, cmd_item.getCmdPostDbl(), src_aux);
    }
    
    // Part 2: Build the history entry
    string post_val = cmd_item.getCmdPostStr();
    if(valtype != "string")
      post_val = doubleToStringX(cmd_item.getCmdPostDbl());

    m_cmd_summary.addPosting(moosvar, post_val, cmd_pid, cmd_test);
  }

  m_gui->clearPendingCmdPosts();
}


//----------------------------------------------------------------------
// Procedure: handlePendingCommandSummary()

void UCMD_MOOSApp::handlePendingCommandSummary()
{
  if(!m_gui || !m_cmd_summary.reportPending())
    return;

  vector<string> command_report = m_cmd_summary.getCommandReport();
  m_gui->setPostSummary(command_report);
}





