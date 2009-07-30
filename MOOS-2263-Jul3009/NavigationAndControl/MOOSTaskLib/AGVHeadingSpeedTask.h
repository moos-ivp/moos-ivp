/*
 *  AGVHeadingSpeedTask.h
 *  MOOS
 *
 *  Created by pnewman on 06/02/2008.
 *  Copyright 2008 P Newman. All rights reserved.
 *
 */
#include "VariableHeadingTask.h"

class CAGVHeadingSpeedTask : public CVariableHeadingTask
{
private:
    typedef CVariableHeadingTask BASE;
public:
    
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    virtual bool GetRegistrations(STRING_LIST &List);
    virtual bool SetParam(string sParam, string sVal);
    std::string m_sPoseSource;   
    bool RegularMailDelivery(double dfTimeNow);
};
