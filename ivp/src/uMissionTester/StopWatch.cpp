/*****************************************************************************
 * AUTHOR:      Christopher Gagner
 * DATE:        August 16, 2010
 * FILE:        StopWatch.cpp
 * DESCRIPTION: A cross-platform class capable of timing operations like a
 *              stop watch. This implementation is accurate to the second.
 ****************************************************************************/

#include <stdlib.h>

#include "StopWatch.h"

/**
 * Default Constructor
 */
StopWatch::StopWatch(time_t timeout) {
    // Always reset the stopwatch
    reset();
    // Set the timeout value
    m_timeout = timeout;
} // END of Constructor

/**
 * Copy Constructor
 */
StopWatch::StopWatch(const StopWatch& orig) {
    // Copy the time values (time_t)
    m_timeout = orig.m_timeout;
    m_start_time = orig.m_start_time;
    m_end_time = orig.m_end_time;

    // Copy the status values (booleans)
    m_stopped = orig.m_stopped;
    m_started = orig.m_started;

} // END of copy Constructor

/**
 * Destructor
 */
StopWatch::~StopWatch() {

} // END of Destructor

/**
 * Starts the stopwatch and stores the start time if the stopwatch has not
 * been started. If the stopwatch has already been started, the start time
 * is not changed.
 *
 * @return always returns true
 */
bool StopWatch::start(void){

    // If the stopwatch has not been started, start it and store the
    // start time.
    if( ! m_started ){
        m_started = true;
        m_start_time = getCurrentTime();
    }// END check m_started
    
    // Set stopped to false to show that the stopwatch is running
    m_stopped = false;

    return true;
} // END of StopWatch::start(void)

/**
 * Stops the stopwatch and stores the end time if the stopwatch is NOT already
 * stopped.
 *
 * @return always returns true
 */
bool StopWatch::stop(void){
    if(! m_stopped){
        m_stopped = true;

        m_end_time = getCurrentTime();
    } // END check m_stopped
    
    return true;
} // END of StopWatch::stop(void)

/**
 * Resets the stopwatch. It does NOT reset the timeout value.
 *
 * @return always returns true
 */
bool StopWatch::reset(void){
    m_start_time = 0;
    m_end_time = 0;

    m_stopped = true;
    m_started = false;

    return true;
} // END of StopWatch::reset(void)

/**
 * Returns the time between the start-time and the end-time. If the stopwatch
 * is still running, the current time will be used as the end-time.
 *
 * @return seconds since starting
 */
time_t StopWatch::getWallTime(void){
    // If the stopwatch has not been started, return 0
    if(!m_started){
        return 0;
    } // END check if the stopwatch has started

    if( !m_stopped ){
        return getCurrentTime() - m_start_time;
    }else{
        return m_end_time - m_start_time;
    }
} // END of StopWatch::getWallTime(void)

/**
 * Checks if the wall time is greater than the current timeout value. If the
 * timeout value is not set or is set to 0, this will return false.
 *
 * @return true if the wall time is greater than the timeout value; false
 *         otherwise.
 */
bool StopWatch::timeOutCheck(void){
    time_t wall = getWallTime();
    // Check if the wall time is greater than the timeout value
    if(wall > m_timeout){
        return true;
    } else {
        return false;
    } // END check timeout
} // END of StopWatch::timeOutCheck()

/**
 * Gets the current time in seconds since the Epoch date.
 *
 * @return seconds since the Epoch date
 */
time_t StopWatch::getCurrentTime(void){
    time_t rtn;
    time(&rtn);
    return rtn;
} // END of StopWatch::getCurrentTime(void)


/*****************************************************************************
 *                           END of StopWatch.cpp
 ****************************************************************************/
