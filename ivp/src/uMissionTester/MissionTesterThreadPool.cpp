/*****************************************************************************
 * AUTHOR:      Christopher Gagner
 * DATE:        August 18, 2010, 10:33 AM
 * FILE:        MissionTesterThreadPool.cpp
 * DESCRIPTION: A thread pool of mission testers. Useful for starting
 *              multi-vehicle missions.This class takes in a vector of mission
 *              files and starts a mission tester for each specified file.
 ****************************************************************************/

#include "MissionTesterThreadPool.h"
#include "MBUtils.h"

using namespace std;

/**
 * Default Constructor
 */
MissionTesterThreadPool::MissionTesterThreadPool(string name, vector<string> mission_files) {
    m_name = name;
    m_mission_files = mission_files;
} // END of Constructor

/**
 * Destructor
 */
MissionTesterThreadPool::~MissionTesterThreadPool(void) {
    this->cleanup();
} // END of Destructor

/**
 * Starts a mission testers for each mission file specified in the
 * constructor.
 *
 * @return true if all mission testers were successfully launched; false
 *         otherwise.
 */
bool MissionTesterThreadPool::start(void){

    for(size_t i =0; i < m_mission_files.size(); i++){
        m_mission_testers.push_back( new MissionTester() );
    } // END for-loop over all mission files

    vector<MissionTester*>::iterator mt_it;
    vector<string>::iterator s_it;
    for(s_it = m_mission_files.begin(), mt_it = m_mission_testers.begin();
            s_it != m_mission_files.end() && mt_it != m_mission_testers.end();
            s_it++, mt_it++){

        m_runners.push_back( new MOOSAppRunnerThread( (*mt_it),
                m_name.c_str(), (*s_it).c_str() ));
    } // END for-loop over all mission files and testers


    // Wait for all threads to start
    vector<MOOSAppRunnerThread*>::iterator r_it;
    for(r_it = m_runners.begin(); r_it != m_runners.end(); r_it++){
        bool started = false;
        for(int i =0; i < 5 || started != true; i ++){
            started = (*r_it)->isRunning();
            // If the thread is not started sleep for 200 milliseconds
            if(!started){
                millipause(200);
            } // END check NOT started
        }
        // One of the thread failed to start - Stop the other thread
        // and return false
        if(! started ){
            this->stop();
            return false;
        }
    } // END for-loop over all runners

    return true;
} // END of MissionTesterThreadPool::start(void)

/**
 * Stops all of the running mission testers and waits for them to die. This
 * is a blocking call.
 *
 * @return true if all mission testers are stopped and cleaned up; false
 *         otherwise
 */
bool MissionTesterThreadPool::stop(void){

    vector<MissionTester*>::iterator mt_it;
    for( mt_it = m_mission_testers.begin(); mt_it != m_mission_testers.end(); mt_it++){
        (*mt_it)->RequestQuit();
    } // END for-loop over all mission files and testers

    vector<MOOSAppRunnerThread*>::iterator r_it;
    for(r_it = m_runners.begin(); r_it != m_runners.end(); r_it++){
        (*r_it)->quit();
    } // END for-loop over all runners

    this->wait();
    return cleanup();
} // END of MissionTesterThreadPool::stop(void)

/**
 * Waits for all of the running mission testers to stop running. This is a
 * blocking call.
 *
 * @ return true when all mission testers have stopped.
 */
bool MissionTesterThreadPool::wait(void){
    // Wait for all threads to stop
    vector<MOOSAppRunnerThread*>::iterator r_it;
    for(r_it = m_runners.begin(); r_it != m_runners.end(); r_it++){
        while( (*r_it)->isRunning() ){
            millipause(2000);
        }
    } // END for-loop over all runners
    return true;
} // END of MissionTesterThreadPool::wait(void)

/**
 * Cleans up all of the MissionTesters and MOOSAppRunnerThreads created
 * by itterating through each respective vector, deleting them from memory,
 * and clearing out the vector.
 *
 * @return true if the cleanup completed.
 */
bool MissionTesterThreadPool::cleanup(void){
    vector<MissionTester*>::iterator mt_it;
    for( mt_it = m_mission_testers.begin(); mt_it != m_mission_testers.end(); mt_it++){
        delete (*mt_it);
    } // END for-loop over all mission files and testers
    m_mission_testers.clear();

    // Loop over all of the runners and delete them
    vector<MOOSAppRunnerThread*>::iterator r_it;
    for(r_it = m_runners.begin(); r_it != m_runners.end(); r_it++){
        delete (*r_it);
    } // END for-loop over all runners
    m_runners.clear();
    return true;
} // END of MissionTesterThreadPool::cleanup(void)

/*****************************************************************************
 *                  END of MissionTesterThreadPool.cpp
 ****************************************************************************/
