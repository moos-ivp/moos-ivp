/*****************************************************************************
 * AUTHOR:      Christopher Gagner
 * DATE:        June 23, 2010
 * FILE:        ProcessPosix.cpp
 * DESCRIPTION: TBD
 ****************************************************************************/

#include "ProcessPosix.h"

#ifndef _WIN32


/**
 * Default Constructor
 */
ProcessPosix::ProcessPosix() {
    m_pid = 0;

} // END of Constructor


/**
 * Destructor
 */
ProcessPosix::~ProcessPosix() {
    if(m_pid !=0){
        this->stop();
    }

} // END of Destructor

/**
 * Start the process
 */
bool ProcessPosix::start(){
    // Check if the launch command has been set
    if( m_args.size() < 2 ){
        return false;
    }
    // Check if the process is already started
    if(m_pid != 0){
        return false;
    }

    m_pid = fork();
    if(m_pid == 0){
        // Child process - Start the new task
        int i =0;     
        char* args[m_args.size() + 1];
		std::list<std::string>::iterator it;
        for(it = m_args.begin(); it != m_args.end(); it++){
            args[i] = (char*) (*it).c_str();
            i++;
        }
        args[m_args.size()] = (char*) 0;
        char* app = args[0];
        
        int status = execvp(app, args);
        if(status < 0){
            return false;
        }
    } else {
        // Parent process
        if (m_pid < 0) {
            // Something is wrong - return false
            printf("Failed to spawn child\n");
            return false;
        }
        // Ignore signals from the child
        signal(SIGCHLD, SIG_IGN);
        // Parent process
        return true;

    } // END check pid

    // The code should not get this far. If it does, return false
    return false;
} // END of ProcessPosix::start

bool ProcessPosix::stop(){
    if(m_pid == 0){
        return false;
    }
    if( kill(m_pid, SIGTERM) == 0){
        m_pid = 0;
        return true;
    } else{
        return false;
    }
}

bool ProcessPosix::isAlive(){
    // For posix systems, attempting to kill a process with a signal 0 will
    // return 0 iff the process is running and will not send any signals.
    if(kill(m_pid, 0) == 0){
        return true;
    } else {
        return false;
    }

}

#endif // NOT _WIN32

/*****************************************************************************
 * END of ProcessPosix.cpp
 ****************************************************************************/
