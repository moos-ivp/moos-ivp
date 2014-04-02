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

struct ClientCommsStatus {
public:
    ClientCommsStatus();
    virtual ~ClientCommsStatus();


    double recent_latency_;
    double max_latency_;
    double min_latency_;
    double avg_latency_;
    std::string name_;
    std::list<std::string> subscribes_;
    std::list<std::string> publishes_;

    void Write(std::ostream & out);

    enum Quality
    {
        Excellent,
        Good,
        Fair,
        Poor,
    };
    Quality Appraise();



};

}

#endif /* CLIENTCOMMSSTATUS_H_ */
