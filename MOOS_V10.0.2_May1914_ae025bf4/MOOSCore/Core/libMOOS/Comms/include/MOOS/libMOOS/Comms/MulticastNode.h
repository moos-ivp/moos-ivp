/*
 * FullDuplexUDPChannel.h
 *
 *  Created on: Mar 1, 2014
 *      Author: pnewman
 */

#ifndef FULLDUPLEXUDPCHANNEL_H_
#define FULLDUPLEXUDPCHANNEL_H_

#include <vector>
#include "MOOS/libMOOS/Utils/SafeList.h"
#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "MOOS/libMOOS/Utils/IPV4Address.h"

namespace MOOS {

class MulticastNode {
public:
    MulticastNode();
    virtual ~MulticastNode();
    bool Configure(const std::string & address, int port,int hops=1);
    bool SetUnreadLimit(unsigned int limit);
    bool Read(std::vector<unsigned char > & data,int  timeout_ms);
    bool Write(std::vector<unsigned char > & data);
    bool Read(std::string &data,int  timeout_ms);
    bool Write(const std::string & data);

    bool Run(bool run_write,bool run_read);


public:
    //but meant for internal use...
    bool ReadLoop();
    bool WriteLoop();


protected:
    CMOOSThread read_thread_;
    CMOOSThread write_thread_;
    MOOS::IPV4Address ipv4_address_;
    MOOS::SafeList<std::vector<unsigned char > > outbox_;
    MOOS::SafeList<std::vector<unsigned char > > inbox_;
    unsigned int unread_limit_;
    int hops_;

};

}

#endif /* FULLDUPLEXUDPCHANNEL_H_ */
