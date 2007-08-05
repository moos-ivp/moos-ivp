/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TMS.h                                                */
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

#ifndef TMS_HEADER
#define TMS_HEADER

#include <vector>
#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"

class TMS : public CMOOSApp  
{
 public:
  TMS();
  virtual ~TMS() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

  void handleCommand(char);
  void addVariable(std::string, bool=false);

 protected:
  void registerVariables();
  
  void updateVariable(CMOOSMsg& msg);
  void updateVarConfig(std::string, bool);
  void updateVarVal(std::string, std::string);
  void updateVarType(std::string, std::string);
  void updateVarSource(std::string, std::string);
  void updateVarTime(std::string, std::string);
  void updateVarCommunity(std::string, std::string);
  
  void printHelp();
  void printReport();

  bool isConfigVar(std::string);

 protected:
  std::vector<std::string> var_names;
  std::vector<bool>        var_config;
  std::vector<std::string> var_vals;
  std::vector<std::string> var_type;
  std::vector<std::string> var_source;
  std::vector<std::string> var_time;
  std::vector<std::string> var_community;

  bool display_allvars_toggle;
  bool display_allvars_pending_val;
  bool display_allvars;
  bool display_source;
  bool display_time;
  bool display_community;
  bool display_virgins;
  bool display_help;

  bool config_list_empty;
  int  limit_string_amt;
  bool limit_string;
  bool paused;
  int  iteration;
};

#endif 






