///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of
//   Applications and Libraries for Mobile Robotics Research
//   Copyright (C) Paul Newman
//
//   This software was written by Paul Newman at MIT 2001-2002 and
//   the University of Oxford 2003-2013
//
//   email: pnewman@robots.ox.ac.uk.
//
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////

// MOOSApp.h: interface for the CMOOSApp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MOOSAPPH
#define MOOSAPPH


#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Utils/ProcessConfigReader.h"
#include "MOOS/libMOOS/Comms/SuicidalSleeper.h"
#include "MOOS/libMOOS/Utils/CommandLineParser.h"
#include "MOOS/libMOOS/Utils/ProcInfo.h"
#include "MOOS/libMOOS/Utils/Macros.h"


#include "MOOS/libMOOS/Comms/MOOSCommClient.h"
#include "MOOS/libMOOS/Comms/MOOSVariable.h"

#include "MOOS/libMOOS/App/ClientDefines.h"

#include <set>
#include <map>

#define DEFAULT_MOOS_APP_COMMS_FREQ 5
#define DEFAULT_MOOS_APP_FREQ 5
#define MOOS_MAX_APP_FREQ 100
#define MOOS_MAX_COMMS_FREQ 200

#define STATUS_PERIOD 2

typedef std::map<std::string,CMOOSVariable> MOOSVARMAP;

#ifdef ASYNCHRONOUS_CLIENT
#include "MOOS/libMOOS/Comms/MOOSAsyncCommClient.h"
namespace MOOS {
namespace Poco {
class Event;
} // namespace Poco
} // namespace MOOS
#endif



/** @brief Commonly used based class for writing MOOS Applications.
* This is a class from which  MOOS component applications may be derived
* main() will typically end with a call to MOOSAppDerivedClass::Run(). It provides
* automatic connection to the MOOSDB, provides slots for Mail Processing and application
* work, callbacks for connection/disconnection to MOOSDB, Configuration file reading and
* dynamic (runtime) variables. Definitely worth getting to know.
* @author Paul Newman
* @ingroup App
*
*/
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
    bool Run( const std::string & sName,const std::string & sMissionFile,const std::string & sSubscribeName);
    bool Run( const std::string & sName,const std::string & sMissionFile="Mission.moos");
    bool Run(const std::string &  sName,const std::string & sMissionFile, int argc,  char * argv[]);
    bool Run( const std::string &,int argc,  char * argv[]);

	/** requests the MOOSApp to quit (i.e return from Run)*/
	bool RequestQuit();

	 /**
	 * pass in a copy of any command line parameters so options can be
	 * queried later
	 */
	void SetCommandLineParameters(int argc,  char * argv[]);


protected:
    /** called when the application should iterate. Overload this function in a derived class
    and within it write all the application specific code. It will be called at approximately
    nFreq = 1/AppTick Hz*/
    virtual bool Iterate();

    /** called just after Iterate has finished - another place to overload*/
    virtual bool OnIterateComplete(){return true;};

    /** called just before Iterate is  called - another place to overload*/
    virtual bool OnIteratePrepare(){return true;};


    /** called when new mail has arrived. Overload this method in a derived class to process new mail.
    It will be called at approximately 1/CommsTick Hz. In this function you'll most likely interate over the
    collection of mail message received or call a m_Comms::PeekMail() to look for a specific named message.
    @param NewMail a list of new mail messages*/
    virtual bool OnNewMail(MOOSMSG_LIST & NewMail);

    /** called just before the main app loop is entered. Specific initialisation code can be written
    in an overloaded version of this function */
    virtual bool OnStartUp();

    /** called just before OnStartUp is called - another place to overload*/
    virtual bool OnStartUpPrepare(){return true;};

    /** called just after OnStartUp has finished - another place to overload*/
    virtual bool OnStartUpComplete(){return true;};

    /** optionally (see ::EnableCommandMessageFiltering() ) called when a command message (<MOOSNAME>_CMD) is recieved by the application.
    @param a copy of CmdMsg the message purporting to be a "command" - i.e. has the name <MOOSNAME>_CMD */
    virtual bool OnCommandMsg(CMOOSMsg Msg);

    /** make a status string - overload this in a derived class if you want to modify or what the statuts string looks like */
    virtual std::string MakeStatusString();

    /** called before OnStartUp and before communications have been established to give users option of processing command line*/
	virtual bool OnProcessCommandLine();

	/** called when command line is asking for help to be printed */
	virtual void OnPrintHelpAndExit();

	/** called when command line is asking for help to be printed */
	virtual void OnPrintExampleAndExit();

	/** called when command line is asking for help to be printed */
	virtual void OnPrintInterfaceAndExit();

	/** called when command line is asking for version to be printed */
	virtual void OnPrintVersionAndExit();

    /** print all searched for parameters */
    void PrintSearchedConfigurationFileParameters();



public:
    /** Called when the class has succesfully connected to the server. Overload this function
    and place use it to register for notification when variables of interest change */
    virtual bool OnConnectToServer();

    /** Called when the class has disconnects from  the server. Put code you want to run when this happens in a virtual version of this method*/
    virtual bool OnDisconnectFromServer();

    /** called by a separate thread if a callback
     * has been installed by calling AddMessageCallback()*/
    virtual bool OnMessage(CMOOSMsg & M);

protected:

    /** notify the MOOS community that something has changed (string)
     *
     * @param sVar Name of variable being notified /posted
     * @param sVal string contents (data payload)
     * @param dfTime time valid (if not specified this is filled in for you as MOOS::Time())
     * @return
     */
    bool Notify(const std::string &sVar, const std::string & sVal, double dfTime=-1);

    /** notify the MOOS community that something has changed (string) with an auxiliary string payload
     *
     * @param sVar Name of variable being notified /posted
     * @param sVal string contents (data payload)
     * @param sSrcAux additional string payload
     * @param dfTime time valid (if not specified this is filled in for you as MOOS::Time())
     * @return
     */
    bool Notify(const std::string &sVar, const std::string & sVal, const std::string & sSrcAux, double dfTime=-1);

    /** notify the MOOS community that something has changed (const char *)
     *
     * @param sVar Name of variable being notified /posted
     * @param sVal string contents (data payload)
     * @param dfTime dfTime time valid (if not specified this is filled in for you as MOOS::Time())
     * @return
     */
    bool Notify(const std::string &sVar, const char * sVal,double dfTime=-1);

    /** notify the MOOS community that something has changed (const char *) with an auxiliary string paylad
     *
     * @param sVar Name of variable being notified /posted
     * @param sVal string contents (data payload)
     * @param sSrcAux additional string data
     * @param dfTime  time valid (if not specified this is filled in for you as MOOS::Time())
     * @return
     */
    bool Notify(const std::string &sVar, const char * sVal,const std::string & sSrcAux, double dfTime=-1);


    /** notify the MOOS community that something has changed (double)
     *
     * @param sVar Name of variable being notified /posted
     * @param dfVal double value of data being sent
     * @param dfTime  time valid (if not specified this is filled in for you as MOOS::Time())
     * @return
     */
    bool Notify(const std::string & sVar,double dfVal, double dfTime=-1);

    /** notify the MOOS community that something has changed (double) with an auxiliary string paylad
     *
     * @param sVar Name of variable being notified /posted
     * @param dfVal double value of data being sent
     * @param sSrcAux  additional string data
     * @param dfTime  time valid (if not specified this is filled in for you as MOOS::Time())
     * @return
     */
    bool Notify(const std::string & sVar,double dfVal, const std::string & sSrcAux,double dfTime=-1);


	/** notify the MOOS community that something has changed -  binary data
     *
     * @param sVar Name of variable being notified /posted
     * @param pData pointer to a chunk of data
     * @param nDataSize size of data in number of bytes
     * @param dfTime double value of data being sent
     * @return
     */
    bool Notify(const std::string & sVar,void *  pData, unsigned int nDataSize, double dfTime=-1);

	/** notify the MOOS community that something has changed  ( binary data ) with an auxiliary string paylad
     *
     * @param sVar Name of variable being notified /posted
     * @param pData pointer to a chunk of data
     * @param nDataSize size of data
     * @param sSrcAux additional string data member
     * @param dfTime time valid
     * @return
     */
    bool Notify(const std::string & sVar,void *  pData, unsigned int nDataSize, const std::string & sSrcAux,double dfTime=-1);


    /** notify the MOOS community that something has changed -  binary data
	 *
	 * @param sVar Name of variable being notified /posted
	 * @param vData a vector of unsigned char data
	 * @param dfTime double value of data being sent
	 * @return
	 */
	bool Notify(const std::string & sVar,const std::vector<unsigned char> & vData, double dfTime=-1);

	/** notify the MOOS community that something has changed  ( binary data ) with an auxiliary string paylad
	 *
	 * @param sVar Name of variable being notified /posted
	 * @param vData a vector of unsigned char data
	 * @param sSrcAux additional string data member
	 * @param dfTime time valid
	 * @return
	 */
	bool Notify(const std::string & sVar,const std::vector<unsigned char> & vData,const std::string & sSrcAux, double dfTime=-1);

    /** Register for notification in changes of named variable
    @param sVar name of variable of interest
    @param dfInterval minimum time between notifications in seconds*/
    bool Register(const std::string & sVar,double dfInterval=0.0);

    /** Register for notification in changes of variables which match variable and source patterns
     *
     * @param sVarPattern variable name pattern eg VAR*_21
     * @param sAppPattern src name patterd eg V?R_PRODUCER
     * @param dfInterval minimum time between notifications in seconds
     * @return true on success
     */
    bool Register(const std::string & sVarPattern,const std::string & sAppPattern, double dfInterval=0.0);


    /** UnRegister for notification in changes of named variable
    @param sVar name of variable of interest*/
    bool UnRegister(const std::string & sVar);



    /**
     * Register a custom call back for a particular message. This call back
     * will be called from its own thread.
     * @param sQueueName nick name of queue
     * @param pfn  pointer to your function should be type
     * bool func(CMOOSMsg &M, void *pParam)
     * @param pYourParam a void * pointer to the thing we want passed as pParam above
     * @return true on success
     */
	bool AddActiveQueue(const std::string & sQueueName,
		bool (*pfn)(CMOOSMsg &M, void * pYourParam),
		void * pYourParam );


    /**
     * Register a custom call back for a particular message. This call back
     * will be called from its own thread.
     * @param sQueueName nick name of queue
     * @param sMsgName name of message to watch for
     * @param pfn  pointer to your function should be type
     * bool func(CMOOSMsg &M, void *pParam)
     * @param pYourParam a void * pointer to the thing we want passed as pParam above
     * @return true on success
     */
	bool AddActiveQueue(const std::string & sQueueName,
		const std::string & sMsgName,
		bool (*pfn)(CMOOSMsg &M, void * pYourParam),
		void * pYourParam );

    /**
   	 * Register a custom call back for a particular message. This call back will be called from its own thread.
   	 * @param sQueueName
   	 * @param Instance of class on which to invoke member function
   	 * @param member function of class bool func(CMOOSMsg &M)
   	 * @return true on success
   	 */
	template <class T>
	bool AddActiveQueue(const std::string & sQueueName,
		T* Instance,bool (T::*memfunc)(CMOOSMsg &)  );


	/**
	 * Add a route to an active queue (which must already exist)
	 * @param  sQueueName name of queue
	 * @param  sMsgName name of message to route
	 */
	bool AddMessageRouteToActiveQueue(const std::string & sQueueName,
                    const std::string & sMsgName);


    /**
       * Register a custom callback and create the active queue as needed.
  	 * @param sQueueName the queue name
  	 * @param sMsgName name of message to route to this queue
  	 * @param pfn  pointer to your function should be type bool func(CMOOSMsg &M, void *pParam)
  	 * @param pYourParam a void * pointer to the thing we want passed as pParam above
  	 * @return true on success
       *
       */
      bool AddMessageRouteToActiveQueue(const std::string & sQueueName,
      				const std::string & sMsgName,
      				bool (*pfn)(CMOOSMsg &M, void * pYourParam),
      				void * pYourParam );

      bool AddActiveMessageQueueCallback(const std::string & sQueueName,
          		const std::string & sMsgName,
          		bool (*pfn)(CMOOSMsg &M, void * pYourParam),
          		void * pYourParam );



      /**
		* Register a custom callback and create the active queue as needed.
		* @param sQueueName the queue name
		* @param sMsgName name of message to route to this queue
		* @param Instance of class on which to invoke member function
		* @param member function of class bool func(CMOOSMsg &M)
		* @return true on success
		*/
      template <class T>
      bool AddMessageRouteToActiveQueue(const std::string & sQueueName,
      				const std::string & sMsgName,
      				T* Instance,bool (T::*memfunc)(CMOOSMsg &) );



    /**
     * Add a callback to ::OnMessage() for a particular message. This will cause OnMessage() to be called from its own thread
     * as soon as a message named as sMsgName arrives. You do need to be careful about thread safety if you use this facilty.
     * remember OnMessage could be called simultaneously by N threads if you have N callbacks registered - don't get hurt by this.
     * @param sMsgName
     * @return true on success
     */
    bool AddMessageRouteToOnMessage(const std::string & sMsgName);


   //deprecated versions
    DEPRECATED(bool AddMessageCallback(const std::string & sMsgName));



    /** The MOOSComms node. All communications happens by way of this object.*/
#ifdef ASYNCHRONOUS_CLIENT
    MOOS::MOOSAsyncCommClient m_Comms;
#else
    CMOOSCommClient m_Comms;
#endif

    /** Set the time between calls into the DB - can be set using the CommsTick flag in the config file*/
    bool SetCommsFreq(unsigned int nFreq);

    /** Set the time  between calls of ::Iterate (which is where you'll probably do Application work)- can be set using the AppTick flag in the config file*/
    void SetAppFreq(double dfFreq,double dfMaxFreq=0.0);

    /** return the application frequency*/
    double GetAppFreq();

    /** get the comms frequency*/
    unsigned int GetCommsFreq();

    /** print out salient info at startup */
    virtual void DoBanner();

    //enumeration of ways application can iterate
    enum IterateMode
	{
		REGULAR_ITERATE_AND_MAIL=0,
		COMMS_DRIVEN_ITERATE_AND_MAIL,
		REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL
	}m_IterationMode;

	//set up the iteration mode of the app
	bool SetIterateMode(IterateMode Mode);

    /** return the boot time of the App */
    double GetAppStartTime();

    /**a very useful object that lets us retrieve configuration information from the mission file using calls like ::GetConfigurationParam() */
    CProcessConfigReader m_MissionReader;

	/** A function which Run eventually calls which itself  calls on NewMail and Iterate*/
    bool DoRunWork();
    
    /** sets the error state of the app and a comment  - this is published as a field in <PROCNAME>_STATUS */
    void SetAppError(bool bFlag, const std::string & sReason);

    /** make the whole application print not many things */
	bool SetQuiet(bool bQuiet);

	

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

    /** Set the MOOSName - ie the name this appl will use when talking MOOS*/
    void SetMOOSName(const std::string &sMOOSName);

    /** call to say if you want mail to be delivered sorted by time*/
    void SortMailByTime(bool bSort=true){m_bSortMailByTime = bSort;};
    
    /** set to true to make the App sensitive (and quit) to Iterate() returning false*/
    void SetQuitOnFailedIterate(bool bQuit){m_bQuitOnIterateFail = bQuit;};

    /**  Call this to write a debug string to the DB under the name "MOOS_DEBUG"  */
    bool MOOSDebugWrite(const std::string & sTxt);

    /** enable/disable the behind the scenes search for command messages */
    void EnableCommandMessageFiltering(bool bEnable);
    
    /** Allow ::Iterate to be called without a connection to a DB*/
    void EnableIterateWithoutComms(bool bEnable);

    /** dispatching function for ::OnCommandMsg */
    bool LookForAndHandleAppCommand(MOOSMSG_LIST & NewMail);

    /** prints default MOOSApp command line switches */
	virtual void PrintDefaultCommandLineSwitches();

    /** return the application name */
    std::string GetAppName();

    /** return the application mission file name */
    std::string GetMissionFileName();

    /** pause until all mail appears to have been sent.
     * note if you are wanting to exit and hope this function
     * completing before doing so there is a very small chance
     * you could exit while the OS is still doing the low level
     * socket work...
     */
    void WaitForEmptyOutbox();


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
    
    /** Sets the value of a previously added dynamic variable to the given CMOOSVariable.
     * @return \c true if the variable's type and value were set, \c false otherwise.   */
    bool SetMOOSVar(const CMOOSVariable& MOOSVar);
    

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

    /** start up the comms */
    virtual bool ConfigureComms();

    /** read setting from file etc */
    virtual bool Configure();

    /** confirm configuration is acceptable */
    virtual bool IsConfigOK();

    /** Port on which server application listens for new connection */
    int m_lServerPort;

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

    /** max frequency at which app can tick (if zero then anything is OK). This
     * allows apps to respond very quickly to mail but also allows users
     * to throttle their rates */
    double m_dfMaxAppTick;

    /** std::string name of mission file */
    std::string m_sMissionFile;

    /** flag specifying whether command message fitlering is enabled */
    bool m_bCommandMessageFiltering;
    
    /** flag to say whether or not App should quit after an Iterate returns false*/
	bool m_bQuitOnIterateFail;
    /** The start time of the application */
    double m_dfAppStartTime;

    /** flag to say whether or not we should be quiet*/
    bool m_bQuiet;

    /** Time at which the Run loop last ran (called Iterate)**/
    double m_dfLastRunTime;

    
    /**should mail be handed to the user sorted by increasing time*/
    bool m_bSortMailByTime;
    
    /** string that should be written to the status string if the App Error flag is true */
    std::string m_sAppError;
    
    /** flag specifying the error state of the App - set via SetAppError()*/
    bool m_bAppError;
    
    /** a tootl for parsing command lines */
    MOOS::CommandLineParser m_CommandLineParser;
    
    /** what is current CPU load?*/
    double GetCPULoad();

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
    
protected:
    /* by calling this function Iterate and OnNewMail will be
     called from the thread that is servicing the MOOS Comms client. It
     is provided to let really very specialised MOOSApps have very speedy
     response times. It is not recommended for general use*/
    bool UseMailCallBack();

    /**
     * look for  a parameter in the mission file and on the command line. If found in both command line
     * wins. It is (default) assumed "--" is prepended on command line. So foo=xx in mission file appears as
     * --foo=xx on command line
     * @param sOption name of parameter
     * @param var variable to be returned
     * @param bPrependMinusMinusForCommandLine if true then add "--" to command line
     */
    template <class T>
    bool GetParameterFromCommandLineOrConfigurationFile(std::string sOption, T & var,bool bPrependMinusMinusForCommandLine=true);

    bool GetFlagFromCommandLineOrConfigurationFile(std::string sOption,bool bPrependMinusMinusForCommandLine=true);



private:
    /* this function is used to process mail on behalf of the client just before
       the derived OnNewMail is invoked - it has no interest to the casual user*/
    void OnNewMailPrivate(MOOSMSG_LIST & NewMail);
    /* and this is a private iterate - we may need to regularly do things behind the scenes */
    void IteratePrivate();

    /**can we iterate without comms*/
    bool m_bIterateWithoutComms;

#ifdef ASYNCHRONOUS_CLIENT
    MOOS::Poco::Event * m_pMailEvent;
#endif


    /** Number of times Application has cycled */
    int m_nIterateCount;

    /** Number of  Application has had new mail */
    int m_nMailCount;

    /** last time a status message was sent */
    double m_dfLastStatusTime;

    /** called before starting the Application running. If parameters have not beedn set correctly
    it prints a help statement and returns false */
    bool CheckSetUp();

    /** controls the rate at which application runs */
    void SleepAsRequired(bool & bIterateShouldRun);
	
	/** ::Run continues forever or until this variable is false*/
	bool m_bQuitRequested;
protected:
    MOOS::ProcInfo m_ProcessMonitor;
    
    MOOS::SuicidalSleeper m_SuicidalSleeper;

};

#include "MOOS/libMOOS/App/MOOSApp.hxx"

#endif
