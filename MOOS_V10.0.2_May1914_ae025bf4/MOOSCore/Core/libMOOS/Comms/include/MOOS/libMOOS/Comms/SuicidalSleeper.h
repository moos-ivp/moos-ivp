/*
 * SuicidalSleeper.h
 *
 *  Created on: Feb 4, 2014
 *      Author: pnewman
 */

#ifndef SUICIDALSLEEPER_H_
#define SUICIDALSLEEPER_H_

#include <string>
#include "MOOS/libMOOS/Utils/MemberFuncBinding.h"
#include "MOOS/libMOOS/Utils/MOOSThread.h"


namespace MOOS {

class SuicidalSleeper {
public:
    SuicidalSleeper();

    virtual ~SuicidalSleeper();

    bool SetPassPhrase(const std::string & sPassPhrase);
    bool SetName(const std::string & app_name);
    bool SetChannel(const std::string & sAddress);
    bool SetPort(int nPort);

    static std::string  GetDefaultPassPhrase();
    static std::string  GetDefaultMulticastAddress();
    static int          GetDefaultMulticastPort();

    std::string GetPassPhrase();
    std::string GetChannel();
    int GetPort();

    template <class T>
    bool SetLastRightsCallback(T* Instance,bool (T::*memfunc)(std::string &) );

    bool Run();

public:
    //thread dispatch
    static bool _dispatch_(void*);
    bool Work();
protected:
    bool SetupAndJoinMulticast();
    bool SendToMulticast(const std::string & sString);
private:
    int socket_fd_;
    int multicast_port_;
    std::string multicast_group_IP_address_;
    std::string pass_phrase_;
    std::string name_;

    MOOS::FunctorStringRef* last_rights_callback_;
    CMOOSThread thread_;
    unsigned int count_down_seconds_;




};

template <class T>
   bool SuicidalSleeper::SetLastRightsCallback(T* Instance,bool (T::*memfunc)(std::string &))
   {
        last_rights_callback_ = MOOS::BindFunctor<T>(Instance,memfunc);
        return true;
   }

}//namepsace MOOS



#endif /* SUICIDALSLEEPER_H_ */
