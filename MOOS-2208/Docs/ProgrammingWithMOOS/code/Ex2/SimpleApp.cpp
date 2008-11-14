
#include "SimpleApp.h"

#include <MOOSGenLib/MOOSGenLibGlobalHelper.h>

//default constructor
CSimpleApp::CSimpleApp()
{
}
//default (virtual) destructor
CSimpleApp::~CSimpleApp()
{
}

/** 
Called by base class whenever new mail has arrived.
Place your code for handling mail (notifications that something
has changed in the MOOSDB in this function

Parameters:
    NewMail :  std::list<CMOOSMsg> reference

Return values:
    return true if everything went OK
    return false if there was problem
**/
bool CSimpleApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;
    
    for(p=NewMail.begin();p!=NewMail.end();p++)
    {
    //lets get a reference to the Message - no need for pointless copy
    CMOOSMsg & rMsg = *p;
    
    // repetitive "ifs" is one way to build a switch yard for
    // the messages
    if(MOOSStrCmp(rMsg.GetKey(),"VehicleStatus"))
    {
        //this is message is about something called "VariableX"
        OnVehicleStatus(rMsg);
    }
    else if(MOOSStrCmp(rMsg.GetKey(),"Heading"))
    {
        //this is message is about something called "VariableY"
        OnHeading(rMsg);
    }
    }

   

    return true;
}


/**
called by the base class when the application has made contact with
the MOOSDB and a channel has been opened. Place code to specify what
notifications you want to receive here.
**/
bool CSimpleApp::OnConnectToServer()
{
    //do registrations
    DoRegistrations();

    return true;
}

/** Called by the base class periodically. This is where you place code
which does the work of the application **/
bool CSimpleApp::Iterate()
{ 
    
    return true;
}

/** called by the base class before the first ::Iterate is called. Place
startup code here - especiall code whic reads configuration data from the 
mission file **/
bool CSimpleApp::OnStartUp()
{       
    //do registrations - its good practive to do this BOTH in OnStartUp and
    //in OnConnectToServer - that way if comms is lost registrations will be
    //reinstigated when the connection is remade 
    DoRegistrations();

    return true;
}


bool CSimpleApp::OnVehicleStatus(CMOOSMsg & Msg)
{
    MOOSTrace("I (%s) received a notification about \"%s\" the details are:\n",
          GetAppName().c_str(),
          Msg.GetKey().c_str());

    //if you want to see all teh details you can print a message...
    //Msg.Trace();

    if(!Msg.IsString())
    return MOOSFail("Ouch - I was promised \"VehicleStatus\" would be a string!");
    
    //OK the guy who wrote the program that publishes VehicleStatus wrote me an
    //email saying the format of the message is:
    //Status = [Good/Bad/Sunk], BatteryVoltage = <double>, Bilge=[on/off]
    //so here we parse the bits we want from the string
    std::string sStatus="Unknown";
    if(!MOOSValFromString(sStatus,Msg.GetString(),"Status"))
    MOOSTrace("warning field \"Status\" not found in VehicleStatus string %s",MOOSHERE);

    double dfBatteryVoltage=-1;
    if(!MOOSValFromString(dfBatteryVoltage,Msg.GetString(),"BatteryVoltage"))
    MOOSTrace("warning field \"BatteryVoltage\" not found in VehicleStatus string %s",MOOSHERE);

    //simple print out our findings..
    MOOSTrace("Status is \"%s\" and battery voltage is %.2fV\n",sStatus.c_str(),dfBatteryVoltage);
    
    return true;
}


bool CSimpleApp::OnHeading(CMOOSMsg & Msg)
{
    MOOSTrace("I (%s) received a notification about \"%s\" the details are:\n",
          GetAppName().c_str(),  //note GetAppName() returns the name of this application as seen by the DB
          Msg.GetKey().c_str()); //note GetKey() return the name of the variable


    //if you want to see all the details you can print a message...
    //Msg.Trace();

    //you might want to be sure that the message is in the format you were expecting
    //in this case heading comes as a single double...

    if(!Msg.IsDouble())
    return MOOSFail("Ouch - was promised \"Heading\" would be a double %s", MOOSHERE);

    double dfHeading = Msg.GetDouble();
    double dfTime = Msg.GetTime();

    MOOSTrace("The heading (according to process %s),at time %f (%f since appstart) is %f\n",
          Msg.GetSource().c_str(), //who wrote it
          dfTime,//when
          dfTime-GetAppStartTime(),//time since we started running (easier to read)
          dfHeading);//the actual heading
    
    return true;
}




void CSimpleApp::DoRegistrations()
{
    //register to be told about every change (write) to "VehicleStatus"
    m_Comms.Register("VehicleStatus",0);
    
    //register to be told about changes (writes) to "Heading" at at most
    //4 times a second
    m_Comms.Register("Heading",0.25);

    return;
}
