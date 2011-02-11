/*****************************************************************************
 * AUTHOR:      Christopher Gagner
 * DATE:        August 16, 2010
 * FILE:        StopWatch.h
 * DESCRIPTION: A cross-platform class capable of timing operations like a
 *              stop watch. This implementation is accurate to the second.
 ****************************************************************************/

#ifndef _STOPWATCH_H
#define	_STOPWATCH_H

#ifndef _WIN32
#include <stdbool.h>
#endif

#include <time.h>
#include <vector>

class StopWatch {
public:
    StopWatch(time_t timeout = 0); // timeout in seconds
    StopWatch(const StopWatch& orig);
    virtual ~StopWatch(void);

    virtual bool start(void);
    virtual bool stop(void);
    virtual bool reset(void);
    
    virtual time_t getWallTime(void);

    virtual bool timeOutCheck(void);

    /**
     * Returns true if the stopwatch is running
     * @return true if the stopwatch is running
     */
    
    virtual bool isStarted(void){return m_started;};
    virtual bool isStopped(void){return m_stopped;};

    /**
     * Returns the current timeout value.
     * @return current timeout value
     */
    virtual time_t getTimeout(void){return m_timeout;};
    
    /**
     * Sets the current timeout value.
     * @param timeout New timeout value in seconds
     */
    virtual void setTimeout(time_t timeout){m_timeout = timeout;};

protected:
    bool m_stopped;
    bool m_started;

    time_t m_start_time;
    time_t m_end_time;
    time_t m_timeout;

private:
    time_t getCurrentTime(void);

};

#endif	/* _STOPWATCH_H */

/*****************************************************************************
 * END of StopWatch.h
 ****************************************************************************/
