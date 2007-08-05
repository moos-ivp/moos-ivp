#include <string.h>
#include "XYPolygon.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace std;

class CTarget: public CMOOSApp  
{
public:
  CTarget();
  virtual ~CTarget();
  
protected:
  bool Iterate();
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool OnConnectToServer();
  bool OnStartUp();	
  
  XYPolygon OpArea;
  
  double xp,yp;
};

