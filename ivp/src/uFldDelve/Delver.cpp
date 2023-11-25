/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Delver.cpp                                           */
/*    DATE: Sep 2nd, 2022                                        */
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
#include "MBUtils.h"
#include "ACTable.h"
#include "Delver.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

Delver::Delver()
{
  // Config vars
  m_rate_frame = 10;

  // State vars
  m_node_count = 0;

  m_total_char_rate = 0;
  m_total_char_rate_node  = 0;
  m_total_char_rate_shore = 0;

  m_total_msg_rate = 0;
  m_total_msg_rate_node  = 0;
  m_total_msg_rate_shore = 0;

  m_max_total_msg_rate = 0;
  m_max_total_msg_rate_node  = 0;
  m_max_total_msg_rate_shore = 0;
}


//---------------------------------------------------------
// Procedure: OnNewMail()

bool Delver::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  updateCurrTime();
  
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key  = msg.GetKey();
    string sval = msg.GetString(); 
    double dval = msg.GetDouble();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mstr  = msg.IsString();
    bool   mdbl = msg.IsDouble();
#endif
    
    if(key == "NODE_PSHARE_VARS") 
      handleMailNodePShareVars(sval);
    else if(key == "UFSB_QBRIDGE_VARS") 
      handleMailQBridgeVars(sval);
    else if(key == "UFSB_BRIDGE_VARS") 
      handleMailBridgeVars(sval);
    else if(key == "UFSB_NODE_COUNT") 
      m_node_count = (unsigned int)(dval);

    else if(m_map_shore_recs.count(key))
      handleMailPosting(msg, true);
    else if(m_map_node_recs.count(key))
      handleMailPosting(msg, false);
    
    else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool Delver::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Delver::Iterate()
{
  AppCastingMOOSApp::Iterate();

  calcTotalRates();
  
  AppCastingMOOSApp::PostReport();

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Delver::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if((param == "rate_frame") && isNumber(value)) {
      double dval = atof(value.c_str());
      if(dval > 0) {
	m_rate_frame = dval;
	handled = true;
      }
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void Delver::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NODE_PSHARE_VARS", 0);
  Register("UFSB_BRIDGE_VARS", 0);
  Register("UFSB_QBRIDGE_VARS", 0);
  Register("UFSB_NODE_COUNT", 0);
}


//------------------------------------------------------------
// Procedure: handleMailNodePShareVars()
//   Example: NODE_REPORT,VIEW_POINT,APPCAST,REALMCAST

void Delver::handleMailNodePShareVars(string sval) 
{
  vector<string> vars = parseString(sval, ',');
  for(unsigned int i=0; i<vars.size(); i++) {
    if(!m_map_node_recs.count(vars[i])) {
      TransRecord record(vars[i]);
      record.setRateFrame(m_rate_frame);
      Register(vars[i]);
      m_map_node_recs[vars[i]] = record;
    }
  }
}

//------------------------------------------------------------
// Procedure: handleMailQBridgeVars()
//   Example: NODE_REPORT_ABE,APPCAST_REQ,REALMCAST_REQ

void Delver::handleMailQBridgeVars(string sval) 
{
  vector<string> vars = parseString(sval, ',');
  for(unsigned int i=0; i<vars.size(); i++)
    m_set_qbridge_vars.insert(vars[i]);
}

//------------------------------------------------------------
// Procedure: handleMailBridgeVars()
//   Example: NODE_REPORT_ABE,APPCAST_REQ_ABE,REALMCAST_REQ_ABE

void Delver::handleMailBridgeVars(string sval) 
{
  vector<string> vars = parseString(sval, ',');
  for(unsigned int i=0; i<vars.size(); i++) {
    if(!m_map_shore_recs.count(vars[i])) {
      TransRecord record(vars[i]);
      record.setRateFrame(m_rate_frame);
      Register(vars[i]);
      m_map_shore_recs[vars[i]] = record;
    }
  }
}

//------------------------------------------------------------
// Procedure: updateCurrTime()

void Delver::updateCurrTime()
{
  map<string,TransRecord>::iterator p;
  for(p=m_map_node_recs.begin(); p!=m_map_node_recs.end(); p++) 
    p->second.setCurrTime(m_curr_time);
  for(p=m_map_shore_recs.begin(); p!=m_map_shore_recs.end(); p++) 
    p->second.setCurrTime(m_curr_time);
}

//------------------------------------------------------------
// Procedure: calcTotalRates()

void Delver::calcTotalRates()
{
  //========================================================
  // Part A: Message Rates
  //========================================================  
  // Part 1: Calculate the current total msg_rate from shore
  m_total_msg_rate_shore = 0;
  map<string,TransRecord>::iterator p;
  for(p=m_map_shore_recs.begin(); p!=m_map_shore_recs.end(); p++) 
    m_total_msg_rate_shore += p->second.getMsgRate();
  
  if(m_total_msg_rate_shore > m_max_total_msg_rate_shore)
    m_max_total_msg_rate_shore = m_total_msg_rate_shore;

  // Part 2: Calculate the current total msg_rate from nodes
  m_total_msg_rate_node = 0;
  map<string,TransRecord>::iterator q;
  for(q=m_map_node_recs.begin(); q!=m_map_node_recs.end(); q++) 
    m_total_msg_rate_node += q->second.getMsgRate();
  
  if(m_total_msg_rate_node > m_max_total_msg_rate_node)
    m_max_total_msg_rate_node = m_total_msg_rate_node;
  
  // Part 3: Calculate combined current total msg_rate
  m_total_msg_rate = m_total_msg_rate_shore + m_total_msg_rate_node;
  if(m_total_msg_rate > m_max_total_msg_rate)
    m_max_total_msg_rate = m_total_msg_rate;

  //========================================================
  // Part B: Character Rates
  //========================================================  
  // Part 1: Calculate the current total char_rate from shore
  m_total_char_rate_shore = 0;
  for(p=m_map_shore_recs.begin(); p!=m_map_shore_recs.end(); p++) 
    m_total_char_rate_shore += p->second.getCharRate();
  
  // Part 2: Calculate the current total char_rate from nodes
  m_total_char_rate_node = 0;
  for(q=m_map_node_recs.begin(); q!=m_map_node_recs.end(); q++) 
    m_total_char_rate_node += q->second.getCharRate();
  
  // Part 3: Calculate combined current total char_rate
  m_total_char_rate = m_total_char_rate_shore + m_total_char_rate_node;
  
}

//------------------------------------------------------------
// Procedure: handleMailPosting()

void Delver::handleMailPosting(const CMOOSMsg& msg, bool shore) 
{
  string key   = msg.GetKey();
  string sval  = msg.GetString(); 
  bool   mdbl  = msg.IsDouble();
  bool   mstr  = msg.IsString();
  string comm  = msg.GetCommunity();
  string msrc  = msg.GetSource();
  double mtime = msg.GetTime();

  unsigned int len = key.length();
  len += msrc.length();
  len += comm.length();
  if(mdbl)
    len += 4;
  else if(mstr)
    len += sval.length();

  // If this posting ends with _ALL, then pShare would be configured
  // to send this to all nodes.
  if(strEnds(key, "_ALL"))
    len *= m_node_count;
  
  if(shore)
    m_map_shore_recs[key].addMsg(mtime, msrc, len);  
  else {
    if(comm != "shoreside")
      m_map_node_recs[key].addMsg(mtime, msrc, len);
  }
}

//------------------------------------------------------------
// Procedure: buildReport()

bool Delver::buildReport() 
{
  bool show_virgins = false;

  string tot_mrate = doubleToStringX(getTotalMsgRate(),0);
  string tot_crate = doubleToStringX(getTotalCharRate(),0);
  string rate_frame = doubleToStringX(m_rate_frame, 1);
  
  m_msgs << "Rate Frame: " << rate_frame << endl;
  m_msgs << "Current Bi-Dictional Rate: " << tot_mrate;
  m_msgs << "(" << tot_crate << ")" << endl << endl;
  
  string smrate = doubleToStringX(getTotalMsgRateNode(),0);
  string scrate = doubleToStringX(getTotalCharRateNode(),0);
  
  m_msgs << "Node To Shore:  Rate: " << smrate;
  m_msgs << "(" << scrate << ")" << endl << endl;
  m_msgs << "-----------------------------------------------" << endl;
  ACTable actab(6);
  actab << "         | Latest | Total | Total | Msg  | Chars";
  actab << "Variable | Source | Msgs  | Chars | Rate | Rate ";
  actab.addHeaderLines();

  map<string,TransRecord>::iterator p;
  for(p=m_map_node_recs.begin(); p!=m_map_node_recs.end(); p++) {
    string varname = p->first;
    TransRecord trans_rec = p->second;

    if((trans_rec.getMsgCount() > 0) || show_virgins) {
      string latest_src = trans_rec.getLatestSource();
      double msg_count  = trans_rec.getMsgCount();
      double char_count = trans_rec.getCharCount();
      double msg_rate   = trans_rec.getMsgRate();
      double char_rate  = trans_rec.getCharRate();
      string msg_count_str  = doubleToStringX(msg_count,0);
      string char_count_str = doubleToStringX(char_count,0);
      string msg_rate_str   = doubleToStringX(msg_rate,2);
      string char_rate_str  = doubleToStringX(char_rate,2);
      
      actab << varname << latest_src;
      actab << msg_count_str;
      actab << char_count_str;
      actab << msg_rate_str;
      actab << char_rate_str;
    }
  }
  
  m_msgs << actab.getFormattedString();

  string nmrate = doubleToStringX(getTotalMsgRateShore(),0);
  string ncrate = doubleToStringX(getTotalCharRateShore(),0);
  
  //string rate = doubleToStringX(getTotalMsgRateShore(),0);
  //string mrate = doubleToStringX(getMaxTotalMsgRateShore(),0);
  
  m_msgs << endl << endl;
  m_msgs << "Shore To Nodes:  Rate: " << nmrate << "(" << ncrate << ")" << endl;
  m_msgs << "-----------------------------------------------" << endl;
  ACTable actab2(6);
  actab2 << "         | Latest | Total | Total | Msg  | Chars";
  actab2 << "Variable | Source | Msgs  | Chars | Rate | Rate";
  actab2.addHeaderLines();

  map<string,TransRecord>::iterator q;
  for(q=m_map_shore_recs.begin(); q!=m_map_shore_recs.end(); q++) {
    string varname = q->first;
    TransRecord trans_rec = q->second;

    if((trans_rec.getMsgCount() > 0) || show_virgins) {
      string latest_src = trans_rec.getLatestSource();
      double msg_count  = trans_rec.getMsgCount();
      double char_count = trans_rec.getCharCount();
      double msg_rate   = trans_rec.getMsgRate();
      double char_rate  = trans_rec.getCharRate();
      string msg_count_str  = doubleToStringX(msg_count,0);
      string char_count_str = doubleToStringX(char_count,0);
      string msg_rate_str   = doubleToStringX(msg_rate,2);
      string char_rate_str  = doubleToStringX(char_rate,2);
      
      actab2 << varname << latest_src;
      actab2 << msg_count_str;
      actab2 << char_count_str;
      actab2 << msg_rate_str;
      actab2 << char_rate_str;
    }
  }

  m_msgs << actab2.getFormattedString();

  return(true);
}

