/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MessageHandler.h                                     */
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

#ifndef UFLD_MESSAGE_HANDLER_HEADER
#define UFLD_MESSAGE_HANDLER_HEADER

#include <string>
#include <map>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class MessageHandler : public AppCastingMOOSApp
{
 public:
  MessageHandler();
  virtual ~MessageHandler() {}

 public: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
  bool buildReport();

 protected:
  void registerVariables();
  bool handleMailNodeMessage(const std::string&);
  void postMsgSummary();

 protected: // Config vars
  bool         m_strict_addressing;
  unsigned int m_appcast_trunc_msg;
  
 protected: // State vars
  unsigned int m_reports;
  bool         m_newmail;

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








