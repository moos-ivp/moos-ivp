///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite
//
//   A suit of Applications and Libraries for Mobile Robotics Research
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and
//   Oxford University.
//
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford
//   University 2003-2005. email: pnewman@robots.ox.ac.uk.
//
//   This file is part of a  MOOS Core Component.
//
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of the
//   License, or (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//   General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//   02111-1307, USA.
//
//////////////////////////    END_GPL    //////////////////////////////////

#ifndef _CMOOSNTSerialPort_H_
#define _CMOOSNTSerialPort_H_

#ifdef _WIN32
    #include <winsock2.h>
    #include "windows.h"
    #include "winbase.h"
    #include "winnt.h"
#endif

#include "NTSerial.h"

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
