/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: MessageHandler.h                                     */
/*   DATE: Jan 30th 2012                                        */
/****************************************************************/

#ifndef UFLD_MESSAGE_HANDLER_HEADER
#define UFLD_MESSAGE_HANDLER_HEADER

#include <string>
#include <map>
#include "MOOSLib.h"

class MessageHandler : public CMOOSApp
{
 public:
  MessageHandler();
  virtual ~MessageHandler() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
  void registerVariables();
  bool handleMailNodeMessage(const std::string&);
  void printReport();
  void postMsgSummary();
  void updateMaxLens();

 protected: // Config vars
  bool         m_verbose;
  bool         m_strict_addressing;
  
 protected: // State vars
  double       m_curr_time;
  double       m_time_warp;
  double       m_last_report_time;
  unsigned int m_iterations;
  unsigned int m_reports;
  bool         m_newmail;
  std::string  m_host_community;

  unsigned int m_total_messages_rcvd;
  unsigned int m_valid_messages_rcvd;
  unsigned int m_rejected_messages_rcvd;
  double       m_last_message_rcvd;

  std::map<std::string, unsigned int> m_map_msg_total;
  std::map<std::string, double>       m_map_msg_tstamp;
  std::map<std::string, std::string>  m_map_msg_var;
  std::map<std::string, std::string>  m_map_msg_value;

  unsigned int m_max_len_msg_src;
  unsigned int m_max_len_msg_total;
  unsigned int m_max_len_msg_tstamp;
  unsigned int m_max_len_msg_var;
  unsigned int m_max_len_msg_value;

  std::list<std::string>  m_last_valid_msgs;
  std::list<std::string>  m_last_invalid_msgs;
  std::list<std::string>  m_last_rejected_msgs;
};

#endif 
