// Ex1/SimpleApp.h: interface for the CSimpleApp class.

#ifndef SIMPLEAPPH
#define SIMPLEAPPH

#include <MOOSLIB/MOOSApp.h>

class CSimpleApp : public CMOOSApp  
{
public:
    //standard construction and destruction
    CSimpleApp();
    virtual ~CSimpleApp();

protected:
    //where we handle new mail
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    //where we do the work
    bool Iterate();
    //called when we connect to the server
    bool OnConnectToServer();
    //called when we are starting up..
    bool OnStartUp();

};

#endif 
