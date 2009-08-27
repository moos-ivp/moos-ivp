// Ex3/Simulator.h: interface for the  class.

#ifndef SIMULATORH
#define SIMULATORH

#include <MOOSLIB/MOOSApp.h>

class CSimulator : public CMOOSApp  
{
public:
    //standard construction and destruction
    CSimulator();
    virtual ~CSimulator();

protected:
    //where we handle new mail
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    //where we do the work
    bool Iterate();
    //called when we connect to the server
    bool OnConnectToServer();
    //called when we are starting up..
    bool OnStartUp();

    //these are some variables put here to elucidate
    //config file reading
    double m_dfBatteryVoltage;
    double m_dfHeading;
    std::string m_sBilge;
    std::string m_sVehicleName;
    


};

#endif 
