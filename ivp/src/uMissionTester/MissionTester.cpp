/*****************************************************************************
 * AUTHOR:      Christopher Gagner
 * DATE:        June 21, 2010
 * FILE:        MissionTester.cpp
 * DESCRIPTION: MissionTester class is use to launch pAntler for starting a 
 *              mission, then monitors the MOOSDB for the end condition. Upon
 *              detecting the end condition, the MissionTester will stop 
 *              pAntler.
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <list>

#include "MissionTester.h"
#include "MBUtils.h"

#include "ProcessWin.h"
#include "ProcessPosix.h"

using namespace std;


// Extern these functions to enable the callbacks for the connect/disconnect to
// the server
extern bool MOOSAPP_OnConnect(void*);
extern bool MOOSAPP_OnDisconnect(void*);

/**
 * Default contructor
 */
MissionTester::MissionTester(std::string server_host,
        long int server_port) {

    m_sServerHost = server_host;
    m_lServerPort = server_port;

    // Set the default timeout
    m_timeout = 60*15; // 15 minutes

    // Set the default start_delay
    m_start_delay = 10; // 10 seconds

    // Disable the MOOS Comms from starting up until the mission has been
    // started.
    m_bUseMOOSComms = false;

    // Create an info buffer for incoming variables
    m_info_buffer = new InfoBuffer();

    // Create a process to run pAntler
    #ifdef _WIN32
    m_process = new ProcessWin();
    #else
    m_process = new ProcessPosix();
    #endif

    // Set the start conditions to be satisfied - If a start condition
    // gets added, this will get set to false
    m_start_satisfied = false;
    // Set a variable to indicate that a start delay has been specified
    m_start_delay_specified = false;
} // END default constructor

/**
 * Destructor
 */
MissionTester::~MissionTester(){
    delete m_info_buffer;
    delete m_process;
}// END destructor

/**
 * Called when the application connects to the server.
 */
bool MissionTester::OnConnectToServer(){

    // Register for the MOOS Variables that are in the end conditions
    vector<string> all_vars;
    vector<LogicCondition>::iterator cond_it;

    // Get all of the variables from each logic end-condition
    for(cond_it=m_end_cond.begin(); cond_it!= m_end_cond.end(); cond_it++){
        vector<string> vars = (*cond_it).getVarNames();
        all_vars = mergeVectors(all_vars, vars);
    }

    // Get all of the variables from each logic start-condition
    for(cond_it=m_start_cond.begin(); cond_it!= m_start_cond.end(); cond_it++){
        vector<string> vars = (*cond_it).getVarNames();
        all_vars = mergeVectors(all_vars, vars);
    }

    // Remove the duplicate variables
    all_vars = removeDuplicates(all_vars);
    vector<string>::iterator vars_it;
    // Register to receive updates for each variable
    for(vars_it=all_vars.begin(); vars_it!=all_vars.end(); vars_it++){
        m_Comms.Register( (*vars_it), 0);
        printf("uMissionTestser - Registered: %s\n", (*vars_it).c_str() );
    }
    return true;
} // END OnConnectToServer

/**
 * Called when the application disconnects from the server
 */
bool MissionTester::OnDisconnectFromServer(){
    
	return true;
} // END OnDisconnectFromServer

/**
 * Called during the iteration loop
 */
bool MissionTester::Iterate() {
    // Check if the start conditions need to be checked
    if (!m_start_satisfied) {
        // Check the start conditions
        if (m_start_cond.empty()) {
            // If no start conditions are specified, delay posting the start
            // flags to allow the other applications to startup
            if (m_start_watch.timeOutCheck()) {
                this->PostStartFlags();
            } // END check m_start_watch
        } else if ( !m_start_delay_specified ) {
            // If no start delay was specifed, and m_start_cond is NOT empty
            // post the start flags after the start conditions have been
            // satisfied
            if (this->CheckStartConditions()) {
                this->PostStartFlags();
            } // END check start conditions
        } else {
            // If a start_delay and start conditions are specified, post the
            // start flags as soon as the delay expires or the conditions
            // are satisfied
            if (m_start_watch.timeOutCheck() || this->CheckStartConditions()) {
                this->PostStartFlags();
            } // END check start_delay or start_conditions
        }// END check CheckStartConditions
    }// END check m_start_satisfied

    if(m_end_watch.timeOutCheck()){
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        printf("! uMissionTester has timed out! - Killing pAntler        !\n");
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        this->KillAntler();
        // Delay 2 seconds then request to quit nicely
        millipause(2000);
        this->RequestQuit();
    }

    if( this->CheckEndConditions() ){
        this->KillAntler();
        // Delay 2 seconds then request to quit nicely
        millipause(2000);
        this->RequestQuit();
    }

	return true;
} // END Iterate

/**
 * Called when the application receives new mail
 */
bool MissionTester::OnNewMail(MOOSMSG_LIST & NewMail){

    MOOSMSG_LIST::iterator it;
    // Itterate over each new mail
    for(it=NewMail.begin(); it!=NewMail.end(); it++){
        CMOOSMsg &msg = (*it);

        this->UpdateInfoBuffer(msg);

    } // END for-loop over NewMail

	return true;
} // END OnNewMail


/**
 * Called when the application needs to command a message
 */
bool MissionTester::OnCommandMsg(CMOOSMsg msg){

	return true;
} // END OnCommandMsg

/**
 * Update the info buffer
 */
bool MissionTester::UpdateInfoBuffer(CMOOSMsg msg){

    string key = msg.m_sKey;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;

    if (msg.IsDouble() ) {
        return (m_info_buffer->setValue(key, ddata));
    } else if (msg.IsString()) {
        return (m_info_buffer->setValue(key, sdata));
    }
    return (false);
} // END UpdateInfoBuffer


/**
 * Called once when the applicatoins starts up
 */
bool MissionTester::OnStartUp(){
    // Call the CMOOSApp Startup
    CMOOSApp::OnStartUp();

    // Get the configuration from the mission file
    this->ParseMissionFile();

    if(! this->LaunchAntler() ){
        printf("Failed to launch pAntler\n");
        return false;
    }
    
    if(! this->ConfigureComms() ){
        return false;
    }

    // Set the start_watch with the m_start_delay seconds
    m_start_watch.setTimeout( m_start_delay );
    m_start_watch.start();

    // Set the end_watch with the Max timeout of m_timeout
    m_end_watch.setTimeout( m_timeout );
    m_end_watch.start();
    
    return true;
} // END OnStartup

/**
 * Configure the communication with the application
 */
bool MissionTester::ConfigureComms(){

    printf("%s::ConfigureComms: \n", GetAppName().c_str() );
    printf("\tm_sServerHost: %s\n", m_sServerHost.c_str() );
    printf("\tm_lServErport: %d\n", (int) m_lServerPort);

    //cout << "**Doing things locally. " << endl;

    //register a callback for On Connect
    m_Comms.SetOnConnectCallBack(MOOSAPP_OnConnect, this);

    //and one for the disconnect callback
    m_Comms.SetOnDisconnectCallBack(MOOSAPP_OnDisconnect, this);

    //start the comms client....
    if (m_sMOOSName.empty())
        m_sMOOSName = m_sAppName;


    // Re-enable the MOOS Comms and Start receiving messages
    m_bUseMOOSComms = true;
    CMOOSApp::ConfigureComms();

    // Wait for a connection to be established
    int dT = 50;
    for(int i=0; i<100 && ! m_Comms.IsConnected() ; i++){
        millipause(dT);
    }


    // Return true if the comms are connected
    if( m_Comms.IsConnected() ){
        return true;
    } else {
        return false;
    }

} // END ConfigureComms
    

/**
 * Parse the mission file and setup the needed variables
 */
bool MissionTester::ParseMissionFile(){

    STRING_LIST sParams;
    m_MissionReader.GetConfiguration(GetAppName(), sParams);

    sParams.reverse();

    // Iterate over all of the parameters
    STRING_LIST::iterator it;
    for (it = sParams.begin(); it != sParams.end(); it++) {
        string line = *it;
        string param = biteString(line, '=');
        string value = stripQuotes(stripBlankEnds(line));
        param = stripBlankEnds(toupper(param));

        // Check each of the parameters
        if (param == "TIMEOUT") {
            char* rtn;
            int iVal = strtol(value.c_str(), &rtn, 10);
            // Verify the input string is an integer
            if ((iVal == 0) && (value == rtn)) {
                // The input value is a string. Ignore it.
                printf("Incorrect format! Ignoring TIMEOUT: %s\n",
                        value.c_str());
            } else {

                // Check if a time unit was specified
                if( (strcmp(rtn,"min") == 0) || (strcmp(rtn,"minutes") ==0 ) ){
                    iVal = iVal * 60;
                } else if( (strncmp(rtn, "hours", 6) == 0) ||
                        (strncmp(rtn, "hr", 3) == 0) ){
                    iVal = iVal * 3600;
                }

                // Verify that the timeout is positive
                if (iVal > 0) {
                    m_timeout = iVal;
                }
            } // END check iVal and rtn

        } else if (param == "START_DELAY") {
            char* rtn;
            int iVal = strtol(value.c_str(), &rtn, 10);
            // Verify the input string is an integer
            if ((iVal == 0) && (value == rtn)) {
                // The input value is a string. Ignore it.
                printf("Incorrect format! Ignoring START_DELAY: %s\n",
                        value.c_str());
            } else {

                // Check if a time unit was specified
                if( (strcmp(rtn,"min") == 0) || (strcmp(rtn,"minutes") ==0 ) ){
                    iVal = iVal * 60;
                } else if( (strncmp(rtn, "hours", 6) == 0) ||
                        (strncmp(rtn, "hr", 3) == 0) ){
                    iVal = iVal * 3600;
                }

                // Verify that the start_delay is positive
                if (iVal > 0) {
                    m_start_delay = iVal;
                    m_start_delay_specified = true;
                }
            } // END check iVal and rtn

        } else if (param == "END_CONDITION") {
            if(!this->AddEndCondition(value)){
                printf("Incorrect format! Ignoring END_CONDITION: %s\n",
                        value.c_str());
            }
        } else if (param == "START_CONDITION") {
            if(!this->AddStartCondition(value)){
                printf("Incorrect format! Ignoring START_CONDITION: %s\n",
                        value.c_str());
            }
        } else if ((param == "START_FLAGS") || (param == "START_FLAGS")) {
            if(!this->AddStartFlag(value)){
                printf("Incorrect format! Ignoring START_FLAGS: %s\n",
                        value.c_str());
            }
        }// END check parameters

    } // END for-loop over all paramaters


    // Check that START_FLAGS were specified
    if(m_start_flags.empty()){
        // No START_FLAGS were specified. Using the default condition
        this->AddStartFlag("DEPLOY=true,MOOS_MANUAL_OVERIDE=false");
    } // END check m_start_flags.empty()

    // Check that END_CONDTION were specified
    if(m_end_cond.empty()){
        // No END_CONDITION were specified. Using the default condition
        this->AddEndCondition("MISSION_COMPLETE=true");
    } // END check m_end_cond.empty()

    return true;
} // END ParseMissionFile

/**
 * Launches pAntler with the input mission file
 */
bool MissionTester::LaunchAntler() {
    m_process->setCommand( (char*) "pAntler",
             (char*) m_MissionReader.GetFileName().c_str(), (char*) NULL);
    return m_process->start();
} // END LaunchAntler

/**
 * Kills pAntler.
 */
bool MissionTester::KillAntler(){
    printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("!                         Killing pAntler                      !\n");
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    m_Comms.Close(true);
    return m_process->stop();
} // END KillAntler


/**
 * Add a start flag to be published upon connecting to the MOOSDB
 */
bool MissionTester::AddStartFlag(std::string value){

    // Split the input value by commas
    vector<string> flags = parseString(value, ',');


    vector<string>::iterator flags_it;
    for(flags_it=flags.begin();flags_it!=flags.end(); flags_it++){
        printf("FLAGS: %s", (*flags_it).c_str() );
        vector<string> vars = parseString( stripBlankEnds(*flags_it), '=' );

        // Start flags must be in pairs separated by '='
        if(vars.size() != 2){
            return false;
        }

        char* rtn;
        double dVal = strtod(vars.at(1).c_str(), &rtn);
        // Check the value to see if it is a string or a double
        if(dVal == 0 && vars.at(1) == rtn){
            // The value is a string
            printf(" - STRING\n" );
            m_start_flags.push_back(VarDataPair(vars.at(0), vars.at(1) ));
        } else{
            // The value is a double
            printf(" - DOUBLE\n" );
            m_start_flags.push_back(VarDataPair(vars.at(0), dVal ));
        } // END check variable
    } // END for-loop over flags


    return true;
} // END AddStartFlag

/**
 * Posts the start flags to the MOOSDB
 */
bool MissionTester::PostStartFlags(void) {
    m_start_satisfied = true;

    // If the start flags are not empty, print that the start flags are posted
    if( !m_start_flags.empty() ){
        printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        printf("! uMissionTester has met start condition! - Posting start flags!\n");
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    } // END check m_start_flags.empty()

    // Notify the MOOS Comms of the start variables
    vector<VarDataPair>::iterator start_it;
    for (start_it = m_start_flags.begin(); start_it != m_start_flags.end(); start_it++) {
        m_Comms.Notify((*start_it).get_var(), (*start_it).get_sdata());
    } // END forloop over all start flags
    return true;
} // END MissionTester::PostStartFlags(void)

/**
 * Adds an end condition to check
 */
bool MissionTester::AddCondition(std::vector<LogicCondition> &conditions,
        std::string value){
    LogicCondition new_condition;
    if( new_condition.setCondition(value) ){
        conditions.push_back(new_condition);
        return true;
    } else {
        return false;
    }
} // END AddEndCondition

/**
 * Checks the conditions for the logic conditions
 */
bool MissionTester::CheckConditions(std::vector<LogicCondition> conditions){
    // If the info buffer isn't initialized, return false
    if (!m_info_buffer){
        return false;
    } // END check info buffer

    // If the conditions are empty, return false
    if( conditions.empty() ){
        return false;
    } // END check conditions are empty

    unsigned int i, j, vsize, csize;

    // Phase 1: get all the variable names from all present conditions.
    vector<string> all_vars;
    csize = conditions.size();
    for (i = 0; i < csize; i++) {
        vector<string> svector = conditions[i].getVarNames();
        all_vars = mergeVectors(all_vars, svector);
    }
    all_vars = removeDuplicates(all_vars);

    // Phase 2: get values of all variables from the info_buffer and
    // propogate these values down to all the logic conditions.
    vsize = all_vars.size();
    for(i=0; i<vsize; i++) {
        string varname = all_vars[i];
        bool   ok_s, ok_d;
        string s_result = m_info_buffer->sQuery(varname, ok_s);
        double d_result = m_info_buffer->dQuery(varname, ok_d);

        for (j = 0; (j < csize) && (ok_s); j++)
            conditions[j].setVarVal(varname, s_result);
        for (j = 0; (j < csize) && (ok_d); j++)
            conditions[j].setVarVal(varname, d_result);
    }

    // Phase 3: evaluate all logic conditions. Return true only if all
    // conditions evaluate to be true.
    for (i = 0; i < csize; i++) {
        bool satisfied = conditions[i].eval();
        if (!satisfied) {
            return false;
        }// END check satisfied
    } // END for-loop over all conditions

    return true;
} // END of MissionTester::CheckConditions(vector<LogicCondition> conditions)


/*****************************************************************************
 *                        END of MissionTester.cpp
 ****************************************************************************/
