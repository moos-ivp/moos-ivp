/*****************************************************************************
 * AUTHOR:      Christopher Gagner
 * DATE:        July 29, 2010
 * FILE:        LogChecker.h
 * DESCRIPTION: Checks an alog for specified conditions and reports if those
 *              conditions have been satisfied.
 ****************************************************************************/

#ifndef _LOGCHECKER_H
#define	_LOGCHECKER_H

#include <vector>
#include <string>



#include "ALogEntry.h"
#include "InfoBuffer.h"
#include "LogicCondition.h"
#include "LogUtils.h"




const std::string m_timestamp = "ALOG_TIMESTAMP";

class LogChecker {
    
public:
    LogChecker();
    LogChecker(const LogChecker& orig);
    virtual ~LogChecker();

    virtual bool parseInputFile(std::string input_file);
    virtual bool check(std::string alogfile, std::string output_file = "");
    virtual bool addFlag(std::string value, std::vector<LogicCondition> &flags);
    virtual bool updateInfoBuffer(ALogEntry entry);
    virtual bool checkFlags(std::vector<LogicCondition> flags);

    // Inlined functions
    virtual void setVerbose(bool verbose){m_verbose = verbose;};
    virtual bool getVerbose(void){return m_verbose;};

    virtual bool addStartFlag(std::string value) {
        return addFlag(value, m_start_flags);
    };

    virtual bool addEndFlag(std::string value) {
        return addFlag(value, m_end_flags);
    };

    virtual bool addFailFlag(std::string value) {
        return addFlag(value, m_fail_flags);
    };

    virtual bool addPassFlag(std::string value) {
        return addFlag(value, m_pass_flags);
    };

    virtual bool checkStartFlags(){return checkFlags(m_start_flags);};
    virtual bool checkEndFlags(){return checkFlags(m_end_flags);};
    virtual bool checkFailFlags(){return checkFlags(m_fail_flags);};
    virtual bool checkPassFlags(){return checkFlags(m_pass_flags);};

    
protected:

    bool m_verbose;
    bool m_overwrite_output;

    InfoBuffer* m_info_buffer;
    FILE* m_output_file;

    std::vector<LogicCondition> m_start_flags;
    std::vector<LogicCondition> m_end_flags;
    std::vector<LogicCondition> m_pass_flags;
    std::vector<LogicCondition> m_fail_flags;

    std::vector<std::string> m_vars;

};

#endif	/* _LOGCHECKER_H */

/*****************************************************************************
 *                         END of LogChecker.h
 ****************************************************************************/
