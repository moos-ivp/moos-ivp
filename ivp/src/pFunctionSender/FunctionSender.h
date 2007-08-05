/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI - MIT Dept. Mechanical Engineering */
/*    FILE: FunctionSender.h                                     */
/*    DATE: April 10 2006                                        */
/*****************************************************************/

#ifndef FUNCTION_SENDER_HEADER
#define FUNCTION_SENDER_HEADER

#include "MOOSLib.h"
#include "IvPFunction.h"

class FunctionSender : public CMOOSApp
{
public:
  FunctionSender();
  virtual ~FunctionSender() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void registerVariables();

 protected:
  int     piece_count;
  int     iteration;
  double  start_time;

  int loc_x;
  int loc_y;
  int delta;
  
  std::string m_str;

};

#endif 

