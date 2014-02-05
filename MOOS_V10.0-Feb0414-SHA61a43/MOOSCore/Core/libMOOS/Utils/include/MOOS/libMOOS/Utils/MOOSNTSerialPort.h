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
//   http://www.gnu.org/licenses/lgpl.txt
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////

#ifndef _CMOOSNTSerialPort_H_
#define _CMOOSNTSerialPort_H_

#ifdef _WIN32
    #include <winsock2.h>
    #include "windows.h"
    #include "winbase.h"
    #include "winnt.h"
#endif

#include "MOOS/libMOOS/Utils/NTSerial.h"

//! Implements windows specialisations of MOOSSerialPort
class CMOOSNTSerialPort :  public CNTSerial
{


public:

    CMOOSNTSerialPort();

    ~CMOOSNTSerialPort() { /*Close();*/ };

    /** Send break signal */
    virtual void Break();

    /** Create an open port */
    virtual bool Create(const char * pPortNum=DEFAULT_PORT, int nBaudRate=DEFAULT_BAUDRATE);

    /** Close Port */
    bool    Close(void);

    /** Write nLen bytes out */
    int        Write(const char *pData, int nLen,double* pTime=NULL);

protected:

    /** Grab N chars NOW */
    virtual int GrabN(char * pBuffer,int nRequired);

};


#endif // _CMOOSNTSerialPort_H_
