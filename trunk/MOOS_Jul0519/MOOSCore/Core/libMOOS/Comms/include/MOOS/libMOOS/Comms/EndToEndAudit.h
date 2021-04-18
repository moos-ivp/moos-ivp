/*
* EndToEndAudit.h
*
*  Created on: May 10, 2017
*      Author: pnewman
*/

#ifndef ENDTOENDAUDIT_H_
#define ENDTOENDAUDIT_H_

#include "MOOS/libMOOS/Utils/MOOSLock.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Comms/MOOSMsg.h"
#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "MOOS/libMOOS/Comms/MulticastNode.h"
#include "MOOS/libMOOS/Utils/ProcInfo.h"

namespace MOOS{
/**
 * @brief The EndToEndAudit class
 */
class EndToEndAudit{

public:

    /**
     * @brief EndToEndAudit
     */
    EndToEndAudit();

    /**
     * @brief Start
     */
    void Start();

    /**
     * @brief The MessageStatistic struct
     */
    struct MessageStatistic{
        std::string source_client;
        std::string destination_client;
        std::string message_name;
        int message_size;
        int64_t source_time;
        int64_t receive_time;
        double cpu_load;

        void ToString(std::string & out);
        void FromString(const std::string & in);

    };
    typedef std::vector<MessageStatistic> MessageStatistics;

    /**
     * @brief AddForAudit
     * @param msg
     * @param client_name
     * @param time_now
     */
    void AddForAudit(const CMOOSMsg & msg,
                     const std::string  & client_name,
                     double time_now);


    static bool ThreadDispatch(void *p){
        EndToEndAudit* pMe = (EndToEndAudit*)p;
        return pMe->TransmitWorker();
    }

    bool TransmitWorker();

private:
    CMOOSThread transmit_thread_;
    CMOOSLock audit_lock_;
    MessageStatistics message_statistics_;
    MOOS::MulticastNode multicaster_;
    MOOS::ProcInfo proc_info_;


};
};

#endif
