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
//   are made available under the terms of the GNU Public License
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/gpl.txt
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
#ifndef MOOSHTTPSERVERH
#define MOOSHTTPSERVERH


#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include <map>
class XPCTcpSocket;
class CHTTPConnection;
class CMOOSCommClient;



class CMOOSDBHTTPServer
{
public:

    /**simple constructor - just give it the port of the DB to contact
    it assumes localhost.  Sets Web server port to default (9080) */
    CMOOSDBHTTPServer(long lDBPort);

    /**this constructor allows the web server port to specified in
    addition to the DB Port */
    CMOOSDBHTTPServer(long lDBPort, long lWebServerPort);
    
    virtual ~CMOOSDBHTTPServer(void);

    /**method to allow Listen thread to be launched with a MOOSThread.*/
    static bool _CB(void* pParam)
    {
        CMOOSDBHTTPServer* pMe = (CMOOSDBHTTPServer*) pParam;
        return pMe->Listen();
    }

private:

    /**start everything up*/
    void Initialise(long lDBPort, long lWebServerPort);

    /**print warm fuzzies*/
    void DoBanner();

    /**do the listening*/
    bool Listen();   

    /**web server port*/
    long m_lWebServerPort;

    /**thread to execute Listen()*/
    CMOOSThread m_ListenThread;

    /**socket on which to listen*/
    XPCTcpSocket* m_pListenSocket;

    /**collection of open connections*/
    std::list<CHTTPConnection * > m_Connections;

    /**standard interface to the DB*/
    CMOOSCommClient* m_pMOOSComms;

    /**pointer to a single  lock object shared by all connection threads*/
    CMOOSLock *m_pMOOSCommsLock;

};
#endif
