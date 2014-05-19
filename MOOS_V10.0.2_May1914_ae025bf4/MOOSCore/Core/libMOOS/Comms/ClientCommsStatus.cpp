/*
 * ClientCommsStatus.cpp
 *
 *  Created on: Mar 30, 2014
 *      Author: pnewman
 */

#include "MOOS/libMOOS/Comms/ClientCommsStatus.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <iterator>

namespace MOOS {

ClientCommsStatus::ClientCommsStatus() {
    // TODO Auto-generated constructor stub

    recent_latency_=-1.0;
    max_latency_=std::numeric_limits<double>::min();
    min_latency_=std::numeric_limits<double>::max();
    avg_latency_=std::numeric_limits<double>::min();

}

ClientCommsStatus::~ClientCommsStatus() {
    // TODO Auto-generated destructor stub
}


/** equality operator */
bool ClientCommsStatus::operator==(const ClientCommsStatus & M) const
{
    return name_==M.name_;
}


ClientCommsStatus::Quality ClientCommsStatus::Appraise()
{
    if(recent_latency_<1)
        return Excellent;
    if(recent_latency_<10)
        return Good;
    if(recent_latency_<100)
        return Fair;
    else
        return Poor;
}


void ClientCommsStatus::Write(std::ostream & out)
{
    out<<"\n--------  "<<MOOS::TimeToDate(MOOSTime(false),false,true)<<"  --------\n";

    out<<"\nClient Name:\n    ";
    out<<name_<<"\n";

    out<<"\nLatencies:\n";
    out<<std::left<<std::setw(15);
    out<<"    recent "<<recent_latency_<<" ms\n";

    out<<std::left<<std::setw(15);
    out<<"    max "<<max_latency_<<" ms\n";

    out<<std::left<<std::setw(15);
    out<<"    min "<<min_latency_<<" ms\n";

    out<<std::left<<std::setw(15);
    out<<"    avg "<<avg_latency_<<" ms\n";

    out<<"\nSubscribes:\n    ";
    if(subscribes_.empty())
        out<<"nothing\n";
    else
        std::copy(subscribes_.begin(),subscribes_.end(),std::ostream_iterator<std::string>(out,"\n    "));

    out<<"\nPublishes:\n    ";
    if(publishes_.empty())
        out<<"nothing\n";
    else
        std::copy(publishes_.begin(),publishes_.end(),std::ostream_iterator<std::string>(out,"\n    "));

    out<<"\nSynopsis:\n    comms is ";
    switch(Appraise())
    {
        case Excellent: out<<"EXCELLENT"; break;
        case Good: out<<"GOOD"; break;
        case Fair: out<<"FAIR"; break;
        case Poor: out<<"POOR"; break;
    }

    out<<"\n\n---------------------------------\n";


}


}
