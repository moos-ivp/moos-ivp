/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MessageHandler.cpp                                   */
/*    DATE: Jan 30th 2012                                        */
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

#include <iterator>
#include "MessageHandler.h"
#include "MBUtils.h"
#include "NodeMessage.h"
#include "NodeMessageUtils.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor

MessageHandler::MessageHandler()
{
  // Initialize state variables
  m_reports    = 0;
  m_newmail    = false;

  m_total_messages_rcvd     = 0; 
  m_rejected_messages_rcvd  = 0;  // Not to this destination
  m_valid_messages_rcvd     = 0;  // Not valid msg structure
  m_last_message_rcvd       = 0;
  
  // Initialize config variables
  m_strict_addressing   = false;
  m_appcast_trunc_msg   = 75;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool MessageHandler::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval   = msg.GetString(); 

    if(key == "NODE_MESSAGE") {
      handleMailNodeMessage(sval);
      m_newmail = true;
    }
    else
      reportRunWarning("Unhandled mail: " + key);

  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool MessageHandler::OnConnectToServer()
{
  registerVariables();  
  return(true);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool MessageHandler::Iterate()
{
  AppCastingMOOSApp::Iterate();
  
  if(((m_iteration%100)==1) || m_newmail)
    postMsgSummary();
  m_newmail = false;

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool MessageHandler::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

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
    if(param == "strict_addressing")
      handled = setBooleanOnString(m_strict_addressing, value);

    else if((param == "appcast_trunc_msg") && isNumber(value)) {
      int ival = atoi(value.c_str());
      if(ival < 0)
	ival = 0;
      m_appcast_trunc_msg = (unsigned int)(ival);
      handled = true;
    }
    
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  registerVariables();
  return(true);
}


//------------------------------------------------------------
// Procedure: registerVariables

void MessageHandler::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  m_Comms.Register("NODE_MESSAGE", 0);
}


//------------------------------------------------------------
// Procedure: handleMailNodeMessage

bool MessageHandler::handleMailNodeMessage(const string& msg)
{
  // Part 1: Updating the Statistics and Checking for Validity
  m_total_messages_rcvd++;
  m_last_message_rcvd = m_curr_time;

  NodeMessage message = string2NodeMessage(msg);

  if(!message.valid()) {
    // List of invalid messages stored solely for user debug 
    // viewing at the console window.
    m_last_invalid_msgs.push_back(msg);
    if(m_last_invalid_msgs.size() > 5) 
      m_last_invalid_msgs.pop_front();
    return(false);
  }

  m_valid_messages_rcvd++;

  string src_node   = message.getSourceNode();
  string dest_node  = message.getDestNode();
  string dest_group = message.getDestGroup();
  string var_name   = message.getVarName();
  double var_dval   = message.getDoubleVal();
  string var_sval   = message.getStringVal();
  if(isQuoted(var_sval))
    var_sval = stripQuotes(var_sval);
  
  bool is_string = true;
  if(var_sval == "")
    is_string = false;

  m_map_msg_total[src_node]++;
  m_map_msg_tstamp[src_node] = m_curr_time;
  m_map_msg_var[src_node]    = var_name;

  if(is_string)
    m_map_msg_value[src_node] = var_sval;
  else
    m_map_msg_value[src_node] = doubleToStringX(var_dval,6);

  // List of "last" messages store solely for user debug viewing at
  // the console window.
  m_last_valid_msgs.push_back(msg);
  if(m_last_valid_msgs.size() > 5) 
    m_last_valid_msgs.pop_front();

  // Part 2: Filtering
  //         Recall a message with no dest_node or dest_group is an
  //         invalid. Those bad messages already rejected above.

  bool reject = false;

  // Part 2a: Filter out explicit mail with mismatch address
  if((dest_node != "") && (dest_node != "all") && (dest_node != m_host_community))
    reject = true;

  // Part 2b: Filter broad mailings if strict_addressing enabled
  if(m_strict_addressing && ((dest_node == "") || (dest_node == "all")))
    reject = true;

  if(reject) {
    m_rejected_messages_rcvd++;        
    m_last_rejected_msgs.push_back(msg);
    if(m_last_rejected_msgs.size() > 5) 
      m_last_rejected_msgs.pop_front();
    return(false);
  }


  // Part 3: Handling and Posting the Message
  if(is_string) 
    Notify(var_name, var_sval, src_node);
  else
    Notify(var_name, var_dval, src_node);

  return(true);
}


//------------------------------------------------------------
// Procedure: postMsgSummary
//   Purpose: Post a totals summary of all messages receive so far.
//            UMH_SUMMARY_MSGS = total=14,valid=12,rejected=2

void MessageHandler::postMsgSummary()
{
  string str;
  str += "total="     + uintToString(m_total_messages_rcvd);
  str += ",valid="    + uintToString(m_valid_messages_rcvd);
  str += ",rejected=" + uintToString(m_rejected_messages_rcvd);

  Notify("UMH_SUMMARY_MSGS", str);
}

//------------------------------------------------------------
// Procedure: buildReport
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.
// 
//   Overall Totals Summary                   
//   ======================================   
//      Total Received Valid: 0
//                   Invalid: 0
//                  Rejected: 0
//       Time since last Msg: N/A
//                                          
//   Per Source Node Summary                
//   ====================================== 
//     Source  Total  Elapsed  Variable  Value
//     ------  -----  -------  --------  -----
//                                              
//   Last Few Messages: (from oldest to newest) 
//   ======================================     
//   Valid Mgs: 
//     NONE  
//   Invalid Mgs: 
//     NONE  
//   Rejected Mgs: 
//     NONE  
    
bool MessageHandler::buildReport()
{
  m_reports++;

  m_msgs << "Overall Totals Summary"                 << endl;
  m_msgs << "======================================" << endl;

  unsigned int invalid = m_total_messages_rcvd - m_valid_messages_rcvd;

  string last_msg = "N/A";
  if(m_last_message_rcvd > 0) {
    double elapsed = m_curr_time - m_last_message_rcvd;
    last_msg = doubleToString(elapsed, 1);
  }

  m_msgs << "   Total Received Valid: " << m_total_messages_rcvd << endl;
  m_msgs << "                Invalid: " << invalid << endl;
  m_msgs << "               Rejected: " << m_rejected_messages_rcvd << endl;
  m_msgs << "    Time since last Msg: " << last_msg << endl;
  m_msgs << endl;
  m_msgs << "Per Source Node Summary"                << endl;
  m_msgs << "======================================" << endl;

  ACTable actab(5,2); // 5 columns, 2 blank separator
  actab << "Source | Total | Elapsed | Variable | Value";
  actab.addHeaderLines();

  map<string, unsigned int>::iterator p;
  for(p=m_map_msg_total.begin(); p!=m_map_msg_total.end(); p++) {
    string src      = p->first;
    string total    = uintToString(p->second);
    string elapsed  = doubleToString((m_curr_time - m_map_msg_tstamp[src]),1);
    string msg_var  = m_map_msg_var[src];
    string msg_val  = m_map_msg_value[src];
    actab << src << total << elapsed << msg_var << msg_val;
  }

  m_msgs << actab.getFormattedString();

  list<string>::iterator p2;
  m_msgs << endl << endl;
  m_msgs << "Last Few Messages: (oldest to newest) " << endl;
  m_msgs << "======================================" << endl;
  m_msgs << "Valid Mgs: " << endl;
  if(m_last_valid_msgs.size() == 0)
    m_msgs << "  NONE  " << endl;
  else {
    for(p2 = m_last_valid_msgs.begin(); p2 != m_last_valid_msgs.end(); p2++) {
      string msg = *p2;
      string trunc_msg = msg;
      if(m_appcast_trunc_msg > 0) {
	trunc_msg = truncString(msg, m_appcast_trunc_msg);
	if(trunc_msg.length() != msg.length())
	  trunc_msg += "...";
      }
      m_msgs << "  " << trunc_msg << endl;
    }
  }

  m_msgs << "Invalid Mgs: " << endl;
  if(m_last_invalid_msgs.size() == 0)
    m_msgs << "  NONE  " << endl;
  else {
    for(p2 = m_last_invalid_msgs.begin(); p2 != m_last_invalid_msgs.end(); p2++) {
      string msg = *p2;
      m_msgs << "  " << truncString(msg, 70) << endl;
    }
  }

  m_msgs << "Rejected Mgs: " << endl;
  if(m_last_rejected_msgs.size() == 0)
    m_msgs << "  NONE  " << endl;
  else {
    for(p2 = m_last_rejected_msgs.begin(); p2 != m_last_rejected_msgs.end(); p2++) {
      string msg = *p2;
      m_msgs << "  " << truncString(msg, 70) << endl;
    }
  }
  
  return(true);
}








