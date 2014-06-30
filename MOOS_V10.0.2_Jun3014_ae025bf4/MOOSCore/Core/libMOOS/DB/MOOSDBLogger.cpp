/*
 * MOOSDBLogger.cpp
 *
 *  Created on: Feb 1, 2014
 *      Author: pnewman
 */

#include<string>
#include <fstream>
#include <iostream>
#include <iomanip>


#include "MOOS/libMOOS/DB/MOOSDBLogger.h"
#include "MOOS/libMOOS/Utils/MOOSThread.h"

#include "MOOS/libMOOS/Utils/SafeList.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"


namespace MOOS
{
struct DBLogEvent
{
    DBLogEvent(){};
    DBLogEvent(const std::string & event_name,
               const std::string & event_client,
               const std::string & event_details,
               const std::string & event_time):
                   event_name_(event_name),
                   event_client_(event_client),
                   event_details_(event_details),
                   event_time_(event_time)
    {
        MOOSToUpper(event_name_);
    }

    void Write(std::ofstream & output)
    {
        //std::cerr
        output<<std::left<<std::setw(20);
        output<<event_time_<<" ";
        output<<std::left<<std::setw(20);
        output<<event_name_<<" ";
        output<<std::left<<std::setw(15);
        output<<event_client_<<" ";
        output<<std::left;
        output<<event_details_<<" ";
        output<<std::endl;
    }

    std::string event_name_;
    std::string event_client_;
    std::string event_details_;
    std::string event_time_;
};

class MOOSDBLogger::Impl
{
public:
    Impl(){};
    ~Impl()
    {
        thread_.Stop();
    };

    std::string TimeStamp()
    {
        time_t aclock=0;
        time( &aclock );
        struct tm *Now = localtime( &aclock );
        return MOOSFormat("%.2d:%.2d:%.2d %d/%d/%d ",
               Now->tm_hour,
               Now->tm_min,
               Now->tm_sec,
               Now->tm_mday,
               Now->tm_mon+1,
               Now->tm_year+1900);


    }
    bool AddEvent(const std::string & sEvent,
             const std::string & sClient,
             const std::string & sDetails)
    {
        if(thread_.IsThreadRunning())
        {
            return event_list_.Push(DBLogEvent(sEvent,sClient,sDetails,TimeStamp()));
        }
        else
            return false;
    }

    bool Run(const std::string & sOutputFile)
    {
        output_stream_name_=sOutputFile;
        thread_.Initialise(dispatch_,this);
        return thread_.Start();
    }

    static bool dispatch_(void * pParam)
    {
        MOOSDBLogger::Impl* pMe = (MOOSDBLogger::Impl*)pParam;
        return pMe->Work();
    }

    bool Work()
    {
        try
        {
            std::ofstream output_stream;

            output_stream.open( output_stream_name_.c_str(), std::ios::app | std::ios::out );

            if(!output_stream)
                throw std::runtime_error("failed to open log file "+output_stream_name_);

            while(!thread_.IsQuitRequested())
            {
                DBLogEvent e;
                if(!event_list_.IsEmpty() || event_list_.WaitForPush(500) )
                {
                    event_list_.Pull(e);
                    e.Write(output_stream);
                }
            }
        }
        catch(std::exception &e)
        {
            std::cerr<<e.what();
            return false;
        }
        return true;
    }

    CMOOSThread thread_;
    MOOS::SafeList<DBLogEvent> event_list_;
    std::string  output_stream_name_;

};

MOOSDBLogger::MOOSDBLogger(): Impl_(new MOOSDBLogger::Impl)
{
    // TODO Auto-generated constructor stub

}

MOOSDBLogger::~MOOSDBLogger()
{
    delete Impl_;
    // TODO Auto-generated destructor stub
}

bool MOOSDBLogger::Run(const std::string & sLogFileName)
{
    return Impl_->Run(sLogFileName);
}

bool MOOSDBLogger::AddEvent(const std::string & sEvent,
            const std::string & sClient,
            const std::string & sDetails)
{
    return Impl_->AddEvent(sEvent,sClient,sDetails);
}


}
