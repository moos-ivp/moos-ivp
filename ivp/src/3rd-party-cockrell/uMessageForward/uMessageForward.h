#ifndef uMessageForward_HEADER
#define uMessageForward_HEADER

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include <string>



class CuMessageForward : public CMOOSApp
{
 public:
  CuMessageForward();
  virtual ~CuMessageForward();

  bool Iterate();
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool OnConnectToServer();
  bool OnStartUp();
  
 protected:

  void change_dest_id(std::string nafcon_message, int which_message);
  std::string nafcon_messages3;
  std::string nafcon_messages4;
  std::string nafcon_messages5;
  bool debug;

};

#endif 

