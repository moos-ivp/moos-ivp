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
  XMS();
  virtual ~XMS() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

  void handleCommand(char);
  void addVariable(std::string);

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

 protected:
  std::vector<std::string> var_names;
  std::vector<std::string> var_vals;
  std::vector<std::string> var_type;
  std::vector<std::string> var_source;
  std::vector<std::string> var_time;
  std::vector<std::string> var_community;

  bool display_help;
  bool display_source;
  bool display_time;
  bool display_community;
  bool paused;
  bool update_requested;
  bool help_requested;
  int  iteration;
};

#endif 





