#ifndef MOOSDUMP
#define MOOSDUMP

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "MOOSLib.h"

using namespace std;

class MOOSDump : public CMOOSApp  
{
 public:
  MOOSDump() {};
  virtual ~MOOSDump() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
protected:
  string m_sWatchFor;
  string m_sSend;
  string m_sSendData;
  double m_dfSendPeriod;
  double m_dfLastSend;
};

#endif 
