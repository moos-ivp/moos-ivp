/*
 * ClientCommsStatus.h
 *
 *  Created on: Mar 30, 2014
 *      Author: pnewman
 */

#ifndef CLIENTCOMMSSTATUS_H_
#define CLIENTCOMMSSTATUS_H_

#include <list>
#include <string>

namespace MOOS {

class ClientCommsStatus {
public:
    ClientCommsStatus();
    virtual ~ClientCommsStatus();

    enum Quality
    {
        Excellent,
        Good,
        Fair,
        Poor
    };

    double recent_latency_;
    double max_latency_;
    double min_latency_;
    double avg_latency_;
    std::string name_;
    std::list<std::string> subscribes_;
    std::list<std::string> publishes_;

    bool operator==(const ClientCommsStatus & M) const;

    void Write(std::ostream & out);

    Quality Appraise();



};

}

#endif /* CLIENTCOMMSSTATUS_H_ */
