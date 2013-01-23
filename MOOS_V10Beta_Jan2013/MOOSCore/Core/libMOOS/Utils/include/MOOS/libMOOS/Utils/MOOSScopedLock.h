/*
 * MOOSScopedLock.h
 *
 *  Created on: Aug 29, 2011
 *      Author: pnewman
 */

#ifndef MOOSSCOPEDLOCK_H_
#define MOOSSCOPEDLOCK_H_

#include "MOOS/libMOOS/Utils/MOOSLock.h"

namespace MOOS
{
//pretty useful thing - idea borrowed from POCO
//lock is opened in constructor unlocked in destruttor
class ScopedLock
{
public:
    ScopedLock(CMOOSLock & Lock):_Lock(Lock)
    {
        _Lock.Lock();
    }
    ~ScopedLock()
    {
        _Lock.UnLock();
    }
private:
    CMOOSLock & _Lock;


};

};

#endif /* MOOSSCOPEDLOCK_H_ */
