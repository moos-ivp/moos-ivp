#include "MOOS/libMOOS/Comms/EndToEndAudit.h"

///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of
//   Applications and Libraries for Mobile Robotics Research
//   Copyright (C) Paul Newman
//
//   This software was written by Paul Newman at MIT 2001-2002 and
//   the University of Oxford 2003-2013
//
//   email: pnewman@robots.ox.ac.uk.
//
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////

namespace MOOS{

static const std::string kDefaultEndToEndAuditMulticastChannel = "224.1.1.8";
static const int kDefaultEndToEndAuditMulticastPort = 4000;


void EndToEndAudit::MessageStatistic::ToString(std::string & out){
    MOOSAddValToString(out,"src",source_client);
    MOOSAddValToString(out,"dest",destination_client);
    MOOSAddValToString(out,"name",message_name);
    MOOSAddValToString(out,"size",message_size);
    MOOSAddValToString(out,"tx",source_time);
    MOOSAddValToString(out,"rx",receive_time);
    MOOSAddValToString(out,"load",cpu_load);
}

void EndToEndAudit::MessageStatistic::FromString(const std::string & in){
    MOOSValFromString(source_client,in,"src");
    MOOSValFromString(destination_client,in,"dest");
    MOOSValFromString(message_name,in,"name");
    MOOSValFromString(message_size,in,"size");
    MOOSValFromString(source_time,in,"tx");
    MOOSValFromString(receive_time,in,"rx");
    MOOSValFromString(cpu_load,in,"load");
}


/**************************************************************************/
EndToEndAudit::EndToEndAudit(){


}

/**************************************************************************/
void EndToEndAudit::Start(){

    multicaster_.Configure(kDefaultEndToEndAuditMulticastChannel,
                           kDefaultEndToEndAuditMulticastPort);
    multicaster_.Run(true,false);
    transmit_thread_.Initialise(ThreadDispatch,this);
    transmit_thread_.Start();
}

/**************************************************************************/
void EndToEndAudit::AddForAudit(const CMOOSMsg & msg,
                                const std::string  & client_name,
                                double time_now){

    MessageStatistic ms;
    ms.source_client = msg.GetSource();
    ms.destination_client =client_name;
    ms.receive_time = int64_t(time_now*1e6);
    ms.source_time = int64_t(msg.GetTime()*1e6);
    ms.message_name = msg.GetKey();
    ms.message_size=msg.GetSizeInBytesWhenSerialised();
    proc_info_.GetPercentageCPULoad(ms.cpu_load);

    MOOS::ScopedLock lock(audit_lock_);
    message_statistics_.push_back(ms);

}


/**************************************************************************/
bool EndToEndAudit::TransmitWorker(){

    while(!transmit_thread_.IsQuitRequested()){
        MOOSPause(1000);
        MessageStatistics message_stats_to_send;
        {
            MOOS::ScopedLock lock(audit_lock_);
            message_stats_to_send.swap(message_statistics_);
        }

        if(message_stats_to_send.empty()){
            continue;
        }

        MessageStatistics::iterator q;
        for(q = message_stats_to_send.begin();
            q!=message_stats_to_send.end();
            ++q){
               std::string message;
               q->ToString(message);
               multicaster_.Write(message);
        }
    }
    return true;
}

}
