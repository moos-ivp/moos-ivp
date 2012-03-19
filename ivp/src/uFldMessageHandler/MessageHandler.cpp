/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: MessageHandler.cpp                                   */
/*   DATE: Jan 30th 2012                                        */
/****************************************************************/

#include <iterator>
#include "MessageHandler.h"
#include "MBUtils.h"
#include "NodeMessage.h"
#include "NodeMessageUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

MessageHandler::MessageHandler()
{
  // Initialize state variables
  m_curr_time  = 0;
  m_iterations = 0;
  m_reports    = 0;
  m_newmail    = false;

  m_total_messages_rcvd     = 0; 
  m_rejected_messages_rcvd  = 0;  // Not to this destination
  m_valid_messages_rcvd     = 0;  // Not valid msg structure
  m_last_message_rcvd       = 0;
  
  m_last_report_time        = 0;

  m_max_len_msg_src    = 6;  // Source
  m_max_len_msg_total  = 5;  // Total
  m_max_len_msg_tstamp = 7;  // Elapsed
  m_max_len_msg_var    = 8;  // Variable
  m_max_len_msg_value  = 5;  // Value

  // Initialize config variables
  m_verbose             = true;
  m_strict_addressing   = false;

  m_time_warp = 1;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool MessageHandler::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
	
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 

    //double dval  = msg.GetDouble();
    //double mtime = msg.GetTime();
    //bool   mdbl  = msg.IsDouble();
    //bool   mstr  = msg.IsString();
    //string msrc  = msg.GetSource();

    if(key == "NODE_MESSAGE") {
      cout << "msg!!!!!!!!!!!!!+" << sval << endl;
      handleMailNodeMessage(sval);
      m_newmail = true;
    }
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
  m_iterations++;
  m_curr_time = MOOSTime();



  if(m_newmail)
    updateMaxLens();
  
  if(((m_iterations%100)==1) || m_newmail)
    postMsgSummary();
  
  // Begin handling the Terminal Report 

  if(!m_verbose)
    cout << "*" << flush;
  else {
    double warp_elapsed_time = m_curr_time - m_last_report_time;
    double real_elapsed_time = warp_elapsed_time;
    if(m_time_warp > 0)
      real_elapsed_time = warp_elapsed_time / m_time_warp;
    if(real_elapsed_time > 0.5) {
      printReport();
      m_last_report_time = m_curr_time;
    }
  }

  m_newmail = false;
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool MessageHandler::OnStartUp()
{
  // Get the MOOSDB Community Information
  if(!m_MissionReader.GetValue("COMMUNITY", m_host_community))
    MOOSTrace("Community name not found in mission file!\n");
  else
    MOOSTrace("Community: %s\n", m_host_community.c_str());

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);

  STRING_LIST::reverse_iterator p;
  for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
    string line  = *p;
    string param = stripBlankEnds(toupper(biteString(line, '=')));
    string value = stripBlankEnds(line);
    
    if(param == "STRICT_ADDRESSING")
      setBooleanOnString(m_strict_addressing, value);
    else if(param == "VERBOSE")
      setBooleanOnString(m_verbose, value);
    
  }

  registerVariables();

  m_time_warp = GetMOOSTimeWarp();
  return(true);
}


//------------------------------------------------------------
// Procedure: registerVariables

void MessageHandler::registerVariables()
{
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
    m_Comms.Notify(var_name, var_sval, src_node);
  else
    m_Comms.Notify(var_name, var_dval, src_node);

  return(true);
}


//------------------------------------------------------------
// Procedure: printReport
//   Purpose: 

void MessageHandler::printReport()
{
  m_reports++;
  unsigned int colpad = 2;

  cout << endl << endl <<endl << endl << endl << endl;
  cout << "*****************************************" << endl;
  cout << "uFldMessageHandler Summary: (" << m_host_community << ")";
  cout << "(" << m_reports << ")" << endl;
  cout << "*****************************************" << endl;
  cout << "                                         " << endl;
  cout << "Overall Totals Summary                   " << endl;
  cout << "======================================   " << endl;

  unsigned int invalid = m_total_messages_rcvd - m_valid_messages_rcvd;

  string last_msg = "N/A";
  if(m_last_message_rcvd > 0) {
    double elapsed = m_curr_time - m_last_message_rcvd;
    last_msg = doubleToString(elapsed, 1);
  }

  cout << "   Total Received Valid: " << m_total_messages_rcvd << endl;
  cout << "                Invalid: " << invalid << endl;
  cout << "               Rejected: " << m_rejected_messages_rcvd << endl;
  cout << "    Time since last Msg: " << last_msg << endl;

  cout << endl;
  cout << "Per Source Node Summary                  " << endl;
  cout << "======================================   " << endl;

  cout << padString("Source",   m_max_len_msg_src    + colpad);
  cout << padString("Total",    m_max_len_msg_total  + colpad);
  cout << padString("Elapsed",  m_max_len_msg_tstamp + colpad);
  cout << padString("Variable", m_max_len_msg_var    + colpad);
  cout << padString("Value",    m_max_len_msg_value  + colpad) << endl;

  cout << padString("------",   m_max_len_msg_src    + colpad);
  cout << padString("-----",    m_max_len_msg_total  + colpad);
  cout << padString("-------",  m_max_len_msg_tstamp + colpad);
  cout << padString("--------", m_max_len_msg_var    + colpad);
  cout << padString("-----",    m_max_len_msg_value  + colpad) << endl;

  map<string, unsigned int>::iterator p;
  for(p=m_map_msg_total.begin(); p!=m_map_msg_total.end(); p++) {
    string src          = p->first;
    string str_total    = uintToString(p->second);
    string str_elapsed  = doubleToString((m_curr_time - m_map_msg_tstamp[src]),1);
    string str_msg_var  = m_map_msg_var[src];
    string str_msg_val  = m_map_msg_value[src];

    string pad_src      = padString(src,         m_max_len_msg_src    + colpad);
    string pad_total    = padString(str_total,   m_max_len_msg_total  + colpad);
    string pad_elapsed  = padString(str_elapsed, m_max_len_msg_tstamp + colpad);
    string pad_msg_var  = padString(str_msg_var, m_max_len_msg_var    + colpad);
    string pad_msg_val  = padString(str_msg_val, m_max_len_msg_value  + colpad);

    cout << pad_src << pad_total + pad_elapsed + pad_msg_var + pad_msg_val << endl;
  }

  list<string>::iterator p2;
  cout << endl;
  cout << "Last Few Messages: (from oldest to newest) " << endl;
  cout << "======================================     " << endl;
  cout << "Valid Mgs: " << endl;
  if(m_last_valid_msgs.size() == 0)
    cout << "  NONE" << endl;
  else {
    for(p2 = m_last_valid_msgs.begin(); p2 != m_last_valid_msgs.end(); p2++) {
      string msg = *p2;
      cout << "  " << truncString(msg, 70) << endl;
    }
  }

  cout << "Invalid Mgs: " << endl;
  if(m_last_invalid_msgs.size() == 0)
    cout << "  NONE" << endl;
  else {
    for(p2 = m_last_invalid_msgs.begin(); p2 != m_last_invalid_msgs.end(); p2++) {
      string msg = *p2;
      cout << "  " << truncString(msg, 70) << endl;
    }
  }

  cout << "Rejected Mgs: " << endl;
  if(m_last_rejected_msgs.size() == 0)
    cout << "  NONE" << endl;
  else {
    for(p2 = m_last_rejected_msgs.begin(); p2 != m_last_rejected_msgs.end(); p2++) {
      string msg = *p2;
      cout << "  " << truncString(msg, 70) << endl;
    }
  }

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

  m_Comms.Notify("UMH_SUMMARY_MSGS", str);
}



//------------------------------------------------------------
// Procedure: updateMaxLens
//   Purpose: 

void MessageHandler::updateMaxLens()
{
  m_max_len_msg_src    = 6;  // Source
  m_max_len_msg_total  = 5;  // Total
  m_max_len_msg_tstamp = 7;  // Elapsed
  m_max_len_msg_var    = 8;  // Variable
  m_max_len_msg_value  = 5;  // Value

  map<string, unsigned int>::iterator p;
  for(p=m_map_msg_total.begin(); p!=m_map_msg_total.end(); p++) {
    string src = p->first;
    unsigned int total = p->second;
    string stotal = uintToString(total);

    double elapsed = m_curr_time - m_map_msg_tstamp[src];
    string selapsed = doubleToString(elapsed,1);
    string msg_var  = m_map_msg_var[src];
    string msg_val  = m_map_msg_value[src];

    if(src.length() > m_max_len_msg_src)
      m_max_len_msg_src = src.length();

    if(stotal.length() > m_max_len_msg_total)
      m_max_len_msg_total = stotal.length();

    if(selapsed.length() > m_max_len_msg_tstamp)
      m_max_len_msg_tstamp = selapsed.length();

    if(msg_var.length() > m_max_len_msg_var)
      m_max_len_msg_var = msg_var.length();

    if(msg_val.length() > m_max_len_msg_value)
      m_max_len_msg_value = msg_val.length();
  }
}
