/*****************************************************************************
 * AUTHOR:      Christopher Gagner
 * DATE:        June 21, 2010
 * FILE:        MissionTester.h
 * DESCRIPTION: MissionTester class is use to launch pAntler for starting a 
 *              mission, then monitors the MOOSDB for the end condition. Upon
 *              detecting the end condition, the MissionTester will stop 
 *              pAntler.
 ****************************************************************************/


#ifndef _MISSIONTESTER_H
#define _MISSIONTESTER_H

#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>

#include "MOOSApp.h"
#include "MBTimer.h"
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MOOSMsg.h"

#include "Process.h"
#include "VarDataPair.h"
#include "LogicCondition.h"
#include "InfoBuffer.h"
#include "StopWatch.h"

class MissionTester : public CMOOSApp{

public:
    MissionTester(std::string server_host="localhost", long int server_port=9000);
    ~MissionTester(void);

    // Inherited from CMOOSApp
    virtual bool OnConnectToServer(void);
    virtual bool OnDisconnectFromServer(void);

    /**
     * Returns the current timeout for the utility
     */
    virtual int GetTimeout(void){return m_timeout;};
    virtual int GetStartDelay(void){return m_timeout;};

    bool RequestQuit(void){KillAntler(); return CMOOSApp::RequestQuit(); };

protected:

    // Inherited from CMOOSApp -----------------------------------------------
    virtual bool Iterate(void);
    virtual bool OnNewMail(MOOSMSG_LIST & NewMail);
    virtual bool OnCommandMsg(CMOOSMsg msg);
    virtual bool UpdateInfoBuffer(CMOOSMsg msg);

    virtual bool OnStartUp(void);
    virtual bool ConfigureComms(void);
    // END Inherited from CMOOSApp -------------------------------------------

    virtual bool ParseMissionFile(void);
    virtual bool LaunchAntler(void);
    virtual bool KillAntler(void);


    virtual bool AddStartFlag(std::string value);
    virtual bool PostStartFlags(void);

    virtual bool AddStartCondition(std::string value){
        m_start_satisfied = false;
        return AddCondition(m_start_cond, value);
    };

    virtual bool AddEndCondition(std::string value){
        return AddCondition(m_end_cond, value);
    };
    
    virtual bool CheckEndConditions(void) {
        return CheckConditions(m_end_cond);
    };

    virtual bool CheckStartConditions(void) {
        return CheckConditions(m_start_cond);
    };


private:
    int m_start_delay; // Delay in seconds if no start conditions specified
    int m_timeout; // in seconds
    std::vector<VarDataPair>  m_start_flags;
    std::vector<LogicCondition> m_start_cond;
    std::vector<LogicCondition> m_end_cond;
    InfoBuffer* m_info_buffer;

    bool m_start_satisfied;
    bool m_start_delay_specified;

    StopWatch m_end_watch;
    StopWatch m_start_watch;
    
    Process* m_process;


    virtual bool AddCondition(std::vector<LogicCondition> &conditions, std::string value);
    virtual bool CheckConditions(std::vector<LogicCondition> conditions);
};


#endif // #ifndef _MISSIONTESTER_H

/*****************************************************************************
 *                        END of MissionTester.h
 ****************************************************************************/

