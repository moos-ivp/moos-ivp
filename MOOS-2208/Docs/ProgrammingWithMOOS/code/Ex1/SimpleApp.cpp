
#include "SimpleApp.h"

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
    return true;
}

/**
called by the base class when the application has made contact with
the MOOSDB and a channel has been opened. Place code to specify what
notifications you want to receive here.
**/
bool CSimpleApp::OnConnectToServer()
{
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
    return true;
}
