/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: QueryDB.cpp                                          */
/*    DATE: Dec 29th 2015                                        */
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

#include "QueryDB.h"
#include "MBUtils.h"

using namespace std;

extern bool MOOSAPP_OnConnect(void*);
extern bool MOOSAPP_OnDisconnect(void*);

//------------------------------------------------------------
// Constructor

QueryDB::QueryDB(string g_server_host, long int g_server_port)
{
  m_wait_time     = 10;
  m_start_time    = 0; 
  m_iteration     = 0; 
  m_verbose       = true;
  m_sServerHost   = g_server_host; 
  m_lServerPort   = g_server_port;

  m_configure_comms_locally = false;
  m_info_buffer    = new InfoBuffer;
}

//------------------------------------------------------------
// Procedure: Iterate()

bool QueryDB::Iterate()
{
  m_iteration++;
  if(m_verbose)
    printReport();
  
  // First check if the logic condition is true. If so we want to exit
  // with ZERO which indicates SUCCESS. 
  if(checkCondition()) {
    if(m_verbose)
      cout << "Logic condition succeeded: exit(0)" << endl;
    exit(0);
  }
  
  // If all variables involved in the logic condition have been updated
  // through MOOS mail, then we can declare FAILURE by exiting with ONE
  if(allMailReceived()) {
    if(m_verbose) 
      cout << "Logic condition failed, all mail received: exit(1)" << endl;
    exit(1);
  }

  // Otherwise, the condition may yet still prove to be true. We may
  // just need to wait until further mail has arrived. But we can only
  // wait as long as allowed by m_wait_time.
  double elapsed_time = MOOSTime() - m_start_time;
  if(m_verbose)
    cout << "Elapsed time: " << elapsed_time << endl;
  if(elapsed_time > m_wait_time) {
    if(m_verbose) 
      cout << "Logic condition failed by timeout: exit(1)" << endl;    
    exit(1);
  }

  return(true);
}

//------------------------------------------------------------
// Procedure: OnNewMail()

bool QueryDB::OnNewMail(MOOSMSG_LIST &NewMail)
{    
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    updateInfoBuffer(msg);
   }

  return(true);
}

//------------------------------------------------------------
// Procedure: OnStartUp()

bool QueryDB::OnStartUp()
{
  CMOOSApp::OnStartUp();

  m_start_time = MOOSTime();
  
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool QueryDB::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: setLogicCondition

bool QueryDB::setLogicCondition(string str)
{
  LogicCondition new_condition;
  bool ok = new_condition.setCondition(str);
  if(!ok)
    return(false);

  m_logic_condition = new_condition;

  return(true);
}


//------------------------------------------------------------
// Procedure: registerVariables

void QueryDB::registerVariables()
{
  vector<string> vars = m_logic_condition.getVarNames();
  for(unsigned int i=0; i<vars.size(); i++) 
    Register(vars[i], 0);

  Register("DB_UPTIME", 0);
  Register("DB_TIME", 0);
}

//------------------------------------------------------------
// Procedure: updateInfoBuffer()

bool QueryDB::updateInfoBuffer(CMOOSMsg &msg)
{
  string key = msg.GetKey();
  string sdata = msg.GetString();
  double ddata = msg.GetDouble();

  if(msg.IsDouble())
    return(m_info_buffer->setValue(key, ddata));
  else if(msg.IsString()) 
    return(m_info_buffer->setValue(key, sdata));
  return(false);
}

//------------------------------------------------------------
// Procedure: ConfigureComms
//      Note: Overload the MOOSApp::ConfigureComms implementation
//            which would have grabbed the port/host info from the
//            .moos file instead.

bool QueryDB::ConfigureComms()
{
  //cout << "QueryDB::ConfigureComms:" << endl;
  //cout << "  m_sServerHost: " << M_Sserverhost << endl;
  //cout << "  m_lServErport: " << m_lServerPort << endl;

  if(!m_configure_comms_locally) 
    return(CMOOSApp::ConfigureComms());

  //register a callback for On Connect
  m_Comms.SetOnConnectCallBack(MOOSAPP_OnConnect, this);
  
  //and one for the disconnect callback
  m_Comms.SetOnDisconnectCallBack(MOOSAPP_OnDisconnect, this);
  
  //start the comms client....
  if(m_sMOOSName.empty())
    m_sMOOSName = m_sAppName;
  
  m_nCommsFreq = 10;

  m_Comms.Run(m_sServerHost.c_str(), 
	      m_lServerPort,
	      m_sMOOSName.c_str(), 
	      m_nCommsFreq);
  
  return(true);
}



//-----------------------------------------------------------
// Procedure: checkCondition()

bool QueryDB::checkCondition()
{
  if(!m_info_buffer) 
    return(false);

  // Phase 1: get all the variable names from the logic condition.
  vector<string> vars = m_logic_condition.getVarNames();

  // Phase 2: get values of all variables from the info_buffer and 
  // propogate these values down to all the logic conditions.
  for(unsigned int i=0; i<vars.size(); i++) {
    string varname = vars[i];
    bool   ok_s, ok_d;
    string s_result = m_info_buffer->sQuery(varname, ok_s);
    double d_result = m_info_buffer->dQuery(varname, ok_d);
    if(ok_s)
      m_logic_condition.setVarVal(varname, s_result);
    if(ok_d)
      m_logic_condition.setVarVal(varname, d_result);
  }

  bool satisfied = m_logic_condition.eval();
  return(satisfied);
}




//------------------------------------------------------------
// Procedure: allMailReceived()
//   Purpose: Get all variables involved in the logic condition and
//            check if mail has been received for all variables by 
//            checking if each variable is known to the info_buffer.

bool QueryDB::allMailReceived() const
{
  if(!m_info_buffer)
    return(false);
  
  vector<string> vars = m_logic_condition.getVarNames();
  for(unsigned int i=0; i<vars.size(); i++) {
    bool known = m_info_buffer->isKnown(vars[i]);
    if(!known)
      return(false);
  }
  return(true);
}

//------------------------------------------------------------
// Procedure: printReport()

void QueryDB::printReport()
{
  printf("====================================\n");
  printf("Iteration: %i \n", m_iteration);
  printf("Condition: %s \n", m_logic_condition.getRawCondition().c_str());
  vector<string> vars = m_logic_condition.getVarNames();
  for(unsigned int i=0; i<vars.size(); i++) {
    bool ok_str, ok_dbl;
    string sval = m_info_buffer->sQuery(vars[i], ok_str);
    double dval = m_info_buffer->dQuery(vars[i], ok_dbl);
    printf("Var:  %-18s", vars[i].c_str());
    if(ok_str)
      printf(" [%s] \n", sval.c_str());
    else if(ok_dbl)
      printf(" [%f] \n", dval);
    else
      printf(" [---] \n");
  }
}




