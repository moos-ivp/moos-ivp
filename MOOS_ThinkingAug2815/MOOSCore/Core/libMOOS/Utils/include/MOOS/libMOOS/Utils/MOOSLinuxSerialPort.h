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
// MOOSSerialPort.h: interface for the CMOOSLinuxSerialPort class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#if !defined(AFX_MOOSLINUXSERIALPORT_H__7C4C08B7_D9E9_4226_A227_81B9621BC09A__INCLUDED_)
#define AFX_MOOSLINUXSERIALPORT_H__7C4C08B7_D9E9_4226_A227_81B9621BC09A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#ifndef _WIN32
    #include <fcntl.h>
    #include <sys/signal.h>
    #include <sys/types.h>
    #include <termios.h>
        #include <unistd.h>

        #include <stdio.h>
#endif

#include "MOOS/libMOOS/Utils/MOOSSerialPort.h"

//! Implements linux aspects of CMOOSSerialPort
class CMOOSLinuxSerialPort :public CMOOSSerialPort
{
public:
    virtual bool Close();

    CMOOSLinuxSerialPort();

    virtual ~CMOOSLinuxSerialPort();

    /** Create and set up the port */
    virtual bool Create(const char * pPortNum=DEFAULT_PORT, int nBaudRate=DEFAULT_BAUDRATE);

    /** Write a string out of port */
    int Write(const char* Str,int nLen, double* pTime=NULL);

    /** send break signal */
    virtual void Break();

    /*Flush the Port */
    virtual int Flush();

    /** returns the file descriptor */
    int GetFD();
protected:
    /** FileDescriptor of Port */
    int m_nPortFD;

    /** Just grab N characters NOW */
    virtual int GrabN(char * pBuffer,int nRequired);

    #ifndef _WIN32
      struct termios m_OldPortOptions;
      struct termios m_PortOptions;
    #endif


};

#endif // !defined(AFX_MOOSSERIALPORT_H__7C4C08B7_D9E9_4226_A227_81B9621BC09A__INCLUDED_)
