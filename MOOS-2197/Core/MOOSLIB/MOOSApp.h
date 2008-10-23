///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite
//
//   A suit of Applications and Libraries for Mobile Robotics Research
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and
//   Oxford University.
//
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford
//   University 2003-2005. email: pnewman@robots.ox.ac.uk.
//
//   This file is part of a  MOOS Core Component.
//
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of the
//   License, or (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//   General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//   02111-1307, USA.
//
//////////////////////////    END_GPL    //////////////////////////////////
// MOOSApp.h: interface for the CMOOSApp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MOOSAPPH
#define MOOSAPPH

#include <MOOSGenLib/MOOSGenLib.h>
#include "MOOSCommClient.h"


#define DEFAULT_MOOS_APP_COMMS_FREQ 5
#define DEFAULT_MOOS_APP_FREQ 5
#define MOOS_MAX_APP_FREQ 50
#define MOOS_MAX_COMMS_FREQ 200
#define STATUS_PERIOD 2

#include "MOOSVariable.h"
#include <set>
#include <map>

typedef std::map<std::string,CMOOSVariable> MOOSVARMAP;

/** This is a class from which all MOOS component applications can be derived
main() will typically end with a call to MOOSAppDerivedClass::Run(). It provides
automatic connection to the MOOSDB, provides slots for Mail Processing and application
work, callbacks for connection/disconnection to MOOSDB, Configuration file reading and
dynamic (runtime) variables. Definately worth getting to know. */
class CMOOSApp
{
public:

    CMOOSApp();
    virtual ~CMOOSApp();


protected:


    /////////////////////////////////////////////////////////////////////////////////////////////
    //                       THE MOST IMPORTANT AND USEFUL MEMBERS
    /////////////////////////////////////////////////////////////////////////////////////////////
public:
    /**called to start the application
    @param sName The name of this application (used to read configuration from a mission file and if sSubscribeName is NULL, to register with the MOOSDB)
    @param the name of the mission file
    @param the subscribe name of the application. If NULL then sName
    */
    bool Run( const char * sName,const char * sMissionFile,const char * sSubscribeName);
    bool Run( const char * sName,const char * sMissionFile);

    /** Called when the class has succesully connected to the server. Overload this function
    and place use it to register for notification when variables of interest change */
    virtual bool OnConnectToServer();

    /** Called when the class has disconnects from  the server. Put code you want to run when this happens in a virtual version of this method*/
    virtual bool OnDisconnectFromServer();

protected:
    /** called when the application should iterate. Overload this function in a derived class
    and within it write all the application specific code. It will be called at approximately
    nFreq = 1/AppTick Hz*/
    virtual bool Iterate();

    /** called when new mail has arrived. Overload this method in a derived class to process new mail.
    It will be called at approximately 1/CommsTick Hz. In this function you'll most likely interate over the
    collection of mail message received or call a m_Comms::PeekMail() to look for a specific named message.
    @param NewMail a list of new mail messages*/
    virtual bool OnNewMail(MOOSMSG_LIST & NewMail);

    /** optionally (see ::EnableCommandMessageFiltering() ) called when a command message (<MOOSNAME>_CMD) is recieved by the application.
    @param a copy of CmdMsg the message purporting to be a "command" - i.e. has the name <MOOSNAME>_CMD */
    virtual bool OnCommandMsg(CMOOSMsg Msg);

    /** make a status string - overload this in a derived class if you want to modify or what the statuts string looks like */
    virtual std::string MakeStatusString();

    /** The MOOSComms node. All communications happens by way of this object. You'll often do things like  m_Comms.Notify("VARIABLE_X","STRING_DATA",dfTime) top send data */
    CMOOSCommClient m_Comms;

    /** Set the time between calls into the DB - can be set using the CommsTick flag in the config file*/
    bool SetCommsFreq(unsigned int nFreq);

    /** Set the time  between calls of ::Iterate (which is where you'll probably do Application work)- can be set using the AppTick flag in the config file*/
    void SetAppFreq(double dfFreq);

    /** return the boot time of the App */
    double GetAppStartTime();

    /**a very useful object that lets us retrieve configuration information from the mission file using calls like ::GetConfigurationParam() */
    CProcessConfigReader m_MissionReader;

	/** A function which Run eventually calls which itself  calls on NewMail and Iterate*/
    bool DoRunWork();
    
    /** sets the error state of the app and a comment  - this is published as a field in <PROCNAME>_STATUS */
    void SetAppError(bool bFlag, const std::string & sReason);
    

    /////////////////////////////////////////////////////////////////////////////////////////////
    //                       UTITLITY  METHODS
    /////////////////////////////////////////////////////////////////////////////////////////////


    /** Called to set the MOOS server info used rarely usually this info will be picked up by the MOOSApp
    automatically when it ::Run is called specifying the configuration file (which contains the DB's coordinates)
    @param sServerHost name of the machine hosting the MOOSDB application
    @param lPort port nuymber that MOOSDB listens on*/
    void SetServer(const char * sServerHost="LOCALHOST",long lPort=9000);


    /** By default MOOSDB comms are on - but you may want to use the structuire of MOOSApp as a standalone
    application - if so call this function with a false parameter*/
    bool UseMOOSComms(bool bUse);

    /** call to say if you want mail to be delivered sorted by time*/
    void SortMailByTime(bool bSort=true){m_bSortMailByTime = bSort;};

    /**  Call this to write a debug string to the DB under the name "MOOS_DEBUG"  */
    bool MOOSDebugWrite(const std::string & sTxt);

    /** enable/disable the behind the scenes search for command messages */
    void EnableCommandMessageFiltering(bool bEnable);
    
    /** Allow ::Iterate to be called without a connection to a DB*/
    void EnableIterateWithoutComms(bool bEnable);

    /** dispatching function for ::OnCommandMsg */
    bool LookForAndHandleAppCommand(MOOSMSG_LIST & NewMail);


    /** return the application name */
    std::string GetAppName();

    /** return the application mission file name */
    std::string GetMissionFileName();


    /////////////////////////////////////////////////////////////////////////////////////////////
    //  DYNAMIC VARIABLES  - AN OPTIONAL GARNISH
    /////////////////////////////////////////////////////////////////////////////////////////////

    /** Add a dynamic (run time) variable
        @param sName name of the variable
        @param sSubscribeName if you call RegisterMOOSVariables() the variable will be updated with mail
        called <sSubscribeName> if and when you call UpdateMOOSVariables()
        @param sPublishName  if you call PublishFreshMOOSVariables() (and you've written to the dynamic varible since the last call) the variable will be published under this name.
        @param CommsTime - if sSubscribeName is not empty this is the minimum time between updates which you are interested in knowing about, so if CommsTime=0.1 then the maximum update rate you will see on the variable from the DB is 10HZ. */
    bool AddMOOSVariable(std::string sName,std::string sSubscribeName,std::string sPublishName,double dfCommsTime);

    /** return a pointer to a named variable */
    CMOOSVariable * GetMOOSVar(std::string sName);

    /** Register with the DB to be mailed about any changes to any dynamic variables which were created with non-empty sSubscribeName fields */
    bool RegisterMOOSVariables();


    /** Pass mail (usually collected in OnNewMail) to the set of dynamic variables. If they are interested (mail name matches their subscribe name) they will update themselves automatically */
    bool UpdateMOOSVariables(MOOSMSG_LIST & NewMail);

    /** Set  value in a dynamic variable if the variable is of type double (type is set on first write )*/
    bool SetMOOSVar(const std::string & sName,const std::string & sVal,double dfTime);

    /** Set  value in a dynamic variable if the variable is of type string (type is set on first write ) */
    bool SetMOOSVar(const std::string & sVarName,double dfVal,double dfTime);


    /** Send any variables (under their sPublishName see AddMOOSVariable)  which been written too since the last call of PublishFreshMOOSVariables()*/
    bool PublishFreshMOOSVariables();


    /** a map of dynamic/run time moos variables that may be set by comms - avoid messy long
    if else if statements */
    MOOSVARMAP m_MOOSVars;


    /** Returns true if Simulate = true is found in the mission/configuration file (a global flag) - the mission file is not re-read on each call */
    bool IsSimulateMode();

    /** flag saying whether MOOS is running with a simulator
    can be set by registering for SIMULATION_MODE variable*/
    bool m_bSimMode;


    /** called just before the main app loop is entered. Specific initialisation code can be written
    in an overloaded version of this function */
    virtual bool OnStartUp();

    /** start up the comms */
    virtual bool ConfigureComms();

    /** Port on which server application listens for new connection */
    long m_lServerPort;

    /** name of machine on which MOOS Server resides */
    std::string m_sServerHost;

    /** std::string version of m_lServerPort*/
    std::string m_sServerPort;

    /** true if the server has been set */
    bool m_bServerSet;

    /** true if we want to use MOOS comms */
    bool m_bUseMOOSComms;

    /** name of this application */
    std::string m_sAppName;

    /** subscribe name of application usually by default this will be m_sAppName*/
    std::string m_sMOOSName;
    
    /** frequency at which server will be contacted */
    int m_nCommsFreq;

    /** frequency at which this application will iterate */
    double m_dfFreq;

    /** std::string name of mission file */
    std::string m_sMissionFile;

    /** flag specifying whether command message fitlering is enabled */
    bool m_bCommandMessageFiltering;

    /** The start time of the application */
    double m_dfAppStartTime;

    /** Time at which the Run loop last ran (called Iterate)**/
    double m_dfLastRunTime;

    
    /**should mail be handed to the user sorted by increasing time*/
    bool m_bSortMailByTime;
    
    /** string that should be written to the status string if the App Error flag is true */
    std::string m_sAppError;
    
    /** flag specifying the error state of the App - set via SetAppError()*/
    bool m_bAppError;
    
    
    /** Time since last iterate was called*/
    double GetTimeSinceIterate();

    /** Return time at which the Run loop last ran (called Iterate) - this is a local time - you need
     to add GetMOOSSKew to produce a unified system time**/
    double GetLastIterateTime();

    /** return number of times iterate has been called*/
    int GetIterateCount();

    /** returns true if we can iterate without comms*/
    bool CanIterateWithoutComms();
    
    
    
    
    /** returns the string which constitutes a command string for this application.
    if CommandFiltering is enabled (see EnableCommandMessageFiltering() ) the
    application will filter incoming mail and Call OnCommandMsg() (which can be overiden)
    if a message with this command string as a name is received. Command strings look
    like APPNAME_CMD */
    std::string GetCommandKey();


    bool m_bDebug;

    bool   IsDebug(){return m_bDebug;};


public:
    /**these two functions are used to handle private MOOSApp work that
    need to occur on behalf of derived classes at the same time as the
    overloaded OnConnectToServer and OnDisconnectFromServer methods are
    called. They are public to allow their invokation from a call back. They
    are not interesting to the casual user*/
    void OnDisconnectToServerPrivate();
    void OnConnectToServerPrivate();
    bool OnMailCallBack();
    
    /* by calling this function Iterate and OnNewMail will be
     called from the thread that is servicing the MOOS Comms client. It
     is provided to let really very specialised MOOSApps have very speedy
     response times. It is not recommended for general use*/
    bool UseMailCallBack();

private:
    /* this function is used to process mail on behalf of the client just before
       the derived OnNewMail is invoked - it has no interest to the casual user*/
    void OnNewMailPrivate(MOOSMSG_LIST & NewMail);
    /* and this is a private iterate - we may need to regularly do things behind the scenes */
    void IteratePrivate();

    /**can we iterate without comms*/
    bool m_bIterateWithoutComms;


private:

    /** Number of times Application has cycled */
    int m_nIterateCount;

    /** Number of  Application has had new mail */
    int m_nMailCount;

    /** last time a status message was sent */
    double m_dfLastStatusTime;

    /** called before starting the Application running. If parameters have not beedn set correctly
    it prints a help statement and returns false */
    bool CheckSetUp();
    
    
};

#endif
