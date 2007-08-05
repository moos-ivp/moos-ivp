/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TermCommand.h                                        */
/*    DATE: June 26 2007                                         */
/*****************************************************************/

#ifndef TERM_COMMAND_HEADER
#define TERM_COMMAND_HEADER

#include <vector>
#include <string>
#include "MOOSLib.h"


class TermCommand : public CMOOSApp
{
public:
  TermCommand();
  virtual ~TermCommand() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
  void handleCharInput(char);

protected:
  void addCommand(std::string);
  void printMapping();
  void registerVariables();
  void postCommand(int);

  std::vector<int> getPartialKeyMatches();
  int              getFullKeyMatch();

  void tabExpand();

protected:

  std::vector<std::string>  m_var_key;
  std::vector<std::string>  m_var_name;
  std::vector<std::string>  m_var_type;
  std::vector<std::string>  m_var_val;

  int  m_iteration;

  std::string m_cmd_buffer;
};

#endif
