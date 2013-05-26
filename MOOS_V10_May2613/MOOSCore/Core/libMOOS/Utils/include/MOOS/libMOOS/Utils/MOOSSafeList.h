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
//   http://www.gnu.org/licenses/lgpl.txtgram is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
/*
 * MOOSSafeList.h
 *
 *  Created on: Aug 29, 2011
 *      Author: pnewman
 */

#ifndef MOOSSAFELIST_H_
#define MOOSSAFELIST_H_



#include <list>
#include "MOOS/libMOOS/Thirdparty/PocoBits/ScopedLock.h"
#include "MOOS/libMOOS/Thirdparty/PocoBits/Event.h"
#include "MOOS/libMOOS/Thirdparty/PocoBits/Mutex.h"

namespace MOOS
{
/**
 * templated class which makes thread safe, waitable list.
 */
template<class T>
class SafeList
{
public:

    void Push(const T & Element)
    {
        Poco::FastMutex::ScopedLock Lock(_mutex);
        _List.push_back(Element);
        _PushEvent.set();

    }

    bool Pull(T & Element)
    {
        Poco::FastMutex::ScopedLock Lock(_mutex);
        if (!_List.empty())
        {
            Element = _List.front();
            _List.pop_front();

            return true;
        } else
            return false;

    }


    bool PeekLatest(T & Element)
    {
        Poco::FastMutex::ScopedLock Lock(_mutex);
        if (_List.empty())
            return false;

        Element = _List.back();
        return true;
    }



    bool PeekNext(T & Element)
    {
        Poco::FastMutex::ScopedLock Lock(_mutex);
        if (_List.empty())
            return false;

        Element = _List.front();
        return true;
    }

    template <typename  Predicate>
      bool FindIf( Predicate P,T & Element)
      {
	Poco::FastMutex::ScopedLock Lock(_mutex);
	std::list<T>::iterator q = _List.find_if(_List.begin(),_List.end(),P);

        if(q==_List.end())
	  {
	    return false;
	  }
        else
	  {
	    Element=*q;
	    return true
	      }

      }



    bool WaitForPush(long milliseconds = -1)
    {
        if (milliseconds < 0)
        {
            _PushEvent.wait();
            return true;
        }
        else
        {
            return _PushEvent.tryWait(milliseconds);
        }
    }

    unsigned int Size()
    {
        Poco::FastMutex::ScopedLock Lock(_mutex);
        return _List.size();
    }

    void Clear()
    {
        Poco::FastMutex::ScopedLock Lock(_mutex);
        _List.clear();
    }

private:
    Poco::FastMutex _mutex;
    std::list<T> _List;
    Poco::Event _PushEvent;
};
}




#endif /* MOOSSAFELIST_H_ */
