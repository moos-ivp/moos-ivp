#ifndef __MOOSTHREADEDTIMEJOURNAL_H
#define __MOOSTHREADEDTIMEJOURNAL_H

#include <MOOSLIB/MOOSLib.h>
#include <MOOSGenLib/MOOSGenLib.h>

#include <fstream>
#include <map>
#include <list>

/* The following usage info is from the old
 * CMOOSTimeJournal class.  You now need
 * to specify a thread handle provided by
 * GetNextHandle.  And there's a straight
 * Log(handle, string) method if you don't
 * want to time anything.
 * The levels thing is temporarily disabled.
 */

//this is a utility class to do time stamping. 
//
//Tick("LABEL");
//  DO YOUR CODE
//        TICK("LABEL2")
//            MORE CODE
//        Tock("LABEL2")
//Tock("LABEL")
//Dump();
//
//this will produce a pretty file of timings
//you can call NewLevel() to print an iteration seperator
//in the dump file. This an exerpt from a dump file
/*Level:4754
Iterate              0.000 seconds
Level:4755
Iterate              0.430 seconds
ConstraintApplication 0.422 seconds
EKF                  0.012 seconds
EKF_UPD_1            0.006 seconds
EKF_OBS              0.006 seconds    
ConstraintSearch     0.003 seconds
Level:4756
Iterate              0.041 seconds
Level:4757
Iterate              0.000 seconds
*/


/* This could desparately do with some error checking! */



class CMOOSThreadedTimeJournal
{
public:
    
    typedef long H_THREADLOG;
    
    
    CMOOSThreadedTimeJournal()
    {        
        m_dfStartTime = HPMOOSTime();
        m_nextHandle = 0;
    }
    
    ~CMOOSThreadedTimeJournal()
    {
        Dump();
        m_file.close();
    }
    
    void Open(const std::string & sFile)
    {
        m_file.open(sFile.c_str());
    }
    
    void SetStartTime(double startTime)
    {
        m_dfStartTime = startTime;
    }

    H_THREADLOG GetNextHandle(std::string sName)
    {
        THREADINFO threadinfo;
        
        threadinfo.name = sName;
        threadinfo.level = 0;
        
        m_threadInfoMap[m_nextHandle] = threadinfo;
        
        return static_cast<H_THREADLOG> (m_nextHandle++);
    }
    
    bool Tick(const H_THREADLOG handle, const std::string &sTimerName)
    {       
        m_lock.Lock();
        
        bool bSuccess = false;
        THREADMAP::iterator p = m_threadInfoMap.find(handle);
        if (p != m_threadInfoMap.end()) {
            m_threadInfoMap[handle].timermap[sTimerName] = HPMOOSTime();
                        
            bSuccess = true;
        }
        
        m_lock.UnLock();
        
        return bSuccess;
        
    }
    
    bool Tock(const H_THREADLOG handle, const std::string &sTimerName)
    {
        m_lock.Lock();
        
        bool bSuccess = false;
        THREADMAP::iterator pThreadInfo = m_threadInfoMap.find(handle);
        if (pThreadInfo != m_threadInfoMap.end()) {
            THREADINFO &threadinfo = m_threadInfoMap[handle];
            
            TIMERMAP::iterator pTimer = threadinfo.timermap.find(sTimerName);
            if (pTimer != threadinfo.timermap.end()) {
                std::string msg = MOOSFormat("\t\t%-15s : %-20s %.3f seconds", threadinfo.name.c_str(), sTimerName.c_str(), HPMOOSTime()-threadinfo.timermap[sTimerName]);
                m_logList.push_back(msg);
                
                threadinfo.timermap.erase(pTimer);
                bSuccess = true;
            } else {
                MOOSTrace("No such timer %s : \"%s\"\n", threadinfo.name, sTimerName.c_str());
            }
            
        } else {
            
            
        }
        
        m_lock.UnLock();
        return bSuccess;
        
    }
    
    void Log(const H_THREADLOG handle, const std::string &str)
    {
        m_lock.Lock();
        
        THREADMAP::iterator pThreadInfo = m_threadInfoMap.find(handle);
        if (pThreadInfo != m_threadInfoMap.end()) {
            THREADINFO &threadinfo = m_threadInfoMap[handle];
            
            std::string msg = MOOSFormat("\t%.3f %-15s : %s", HPMOOSTime()-m_dfStartTime, threadinfo.name.c_str(), str.c_str());
            m_logList.push_back(msg);
        }   
        
        m_lock.UnLock();
    }
    
    void NewLevel(const H_THREADLOG handle, int nL=-1)
    {
        m_lock.Lock();
        
        THREADMAP::iterator pThreadInfo = m_threadInfoMap.find(handle);
        if (pThreadInfo != m_threadInfoMap.end()) {
            THREADINFO &threadinfo = m_threadInfoMap[handle];
            
            if (nL == -1) {
                threadinfo.level++;
            } else {
                threadinfo.level = nL;
            }
            
        }
        
        m_lock.UnLock();
    }
    
    void Dump()
    {        
        //m_File<<"Level:"<<m_nLevel<<std::endl;
        
        std::copy(m_logList.begin(),m_logList.end(),std::ostream_iterator<std::string>(m_file,"\n"));
        m_file.flush();

        m_logList.clear();
    }
    
protected:    
    
    

    typedef std::map<std::string, double> TIMERMAP;

    typedef struct {
        std::string name;
        int level;
        TIMERMAP timermap;
    } THREADINFO;
    

    typedef std::map<H_THREADLOG, THREADINFO> THREADMAP;
    
    
    THREADMAP m_threadInfoMap;
 
    CMOOSLock m_lock;
   
    double m_dfStartTime;

    // This is just a buffer of log entries which are yet to be dumped to file
    std::list<std::string> m_logList;
    
    // The log file
    std::ofstream m_file;
    
    long m_nextHandle;
    
private:
    CMOOSThreadedTimeJournal(const CMOOSThreadedTimeJournal &);  // Deliberately not implemented
    void operator=(const CMOOSThreadedTimeJournal &);  // Deliberately not implemented
    
};

#endif // __MOOSTHREADEDTIMEJOURNAL_H