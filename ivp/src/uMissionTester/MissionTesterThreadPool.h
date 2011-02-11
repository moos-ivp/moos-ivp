/*****************************************************************************
 * AUTHOR:      Christopher Gagner
 * DATE:        August 18, 2010, 10:33 AM
 * FILE:        MissionTesterThreadPool.h
 * DESCRIPTION: A thread pool of mission testers. Useful for starting
 *              multi-vehicle missions.This class takes in a vector of mission
 *              files and starts a mission tester for each specified file.
 ****************************************************************************/

#ifndef _MISSIONTESTERTHREADPOOL_H
#define	_MISSIONTESTERTHREADPOOL_H

#include <vector>
#include <string>
#include "MOOSAppRunnerThread.h"
#include "MissionTester.h"

class MissionTesterThreadPool {
public:
    MissionTesterThreadPool(std::string name, std::vector<std::string> mission_files);
    virtual ~MissionTesterThreadPool(void);

    virtual bool start(void);
    virtual bool stop(void);
    virtual bool wait(void);

protected:
    std::vector<MOOSAppRunnerThread*> m_runners;
    std::vector<MissionTester*> m_mission_testers;
    std::vector<std::string> m_mission_files;

    std::string m_name;

    virtual bool cleanup(void);

private:

};

#endif	/* _MISSIONTESTERTHREADPOOL_H */

/*****************************************************************************
 * END of MissionTesterThreadPool.h
 ****************************************************************************/
