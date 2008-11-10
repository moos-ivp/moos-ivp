/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XMS.h                                                */
/*    DATE: Mar 23rd 2006                                        */
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

#ifndef XMS_HEADER
#define XMS_HEADER

#include <vector>
#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"

class XMS : public CMOOSApp  
{
 public:
  XMS(std::string server_host="localhost", long int server_port=9000);
  virtual ~XMS() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
  void handleCommand(char);
  void addVariables(std::string);
  bool addVariable(std::string);
  void ignoreVars(bool v)       {m_ignore_vars = v;};
  void setPaused(bool v)        {m_paused = v;};
  void setDispVirgins(bool v)   {m_display_virgins = v;};
  void setDispTime(bool v)      {m_display_time = v;};
  void setDispSource(bool v)    {m_display_source = v;};
  void setDispCommunity(bool v) {m_display_community = v;};
  void setDispEmptyStrings(bool v) {m_display_empty_strings = v;};
  void setDispAll(bool v)       {m_display_all = v;};
 
  void setConfigureCommsLocally(bool v) {m_configure_comms_locally=v;};

 protected:
  void registerVariables();
  
  void updateVariable(CMOOSMsg& msg);
  void updateVarVal(std::string, std::string);
  void updateVarType(std::string, std::string);
  void updateVarSource(std::string, std::string);
  void updateVarTime(std::string, std::string);
  void updateVarCommunity(std::string, std::string);
  
  void printHelp();
  void printReport();
  
  void refreshAllVarsList();

  // An overloading of the CMOOSApp ConfigureComms function
  bool ConfigureComms();

 protected:
  std::vector<std::string> var_names;
  std::vector<std::string> var_vals;
  std::vector<std::string> var_type;
  std::vector<std::string> var_source;
  std::vector<std::string> var_time;
  std::vector<std::string> var_community;
  
  std::vector<std::string> orig_var_names;
  std::vector<std::string> orig_var_vals;
  std::vector<std::string> orig_var_type;
  std::vector<std::string> orig_var_source;
  std::vector<std::string> orig_var_time;
  std::vector<std::string> orig_var_community;
  
  bool   m_display_help;
  bool   m_display_source;
  bool   m_display_time;
  bool   m_display_community;
  bool   m_paused;
  bool   m_update_requested;
  bool   m_help_requested;
  int    m_iteration;
  
  bool   m_display_virgins;
  bool   m_display_empty_strings;
  bool   m_configure_comms_locally;
  
  bool   m_ignore_vars;
  double m_db_start_time;
  
  std::string m_filter;
  
  bool   m_display_all;
  double m_last_all_refresh;    
};


#endif 






