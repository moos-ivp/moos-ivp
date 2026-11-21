/*
 * MOOSDBLogger.h
 *
 *  Created on: Feb 1, 2014
 *      Author: pnewman
 */

#ifndef MOOSDBLOGGER_H_
#define MOOSDBLOGGER_H_
namespace MOOS
{
class MOOSDBLogger {
public:
    MOOSDBLogger();
    virtual ~MOOSDBLogger();

    bool AddEvent(const std::string & sEvent,
                const std::string & sClient,
                const std::string & sDetails);


    bool Run(const std::string & sLogFileName);

private:
    class Impl;
    Impl* Impl_;

};
}

#endif /* MOOSDBLOGGER_H_ */
