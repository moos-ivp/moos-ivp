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
// Moosserialport.cpp: implementation of the CMOOSLinuxSerialPort class.
//
//////////////////////////////////////////////////////////////////////
#include "MOOSGenLibGlobalHelper.h"
#include "MOOSLinuxSerialPort.h"

#include <cstring>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** constructor. */
CMOOSLinuxSerialPort::CMOOSLinuxSerialPort()
{
    m_nPortFD = -1;
}

/** Destructor.Reset the port option to what every they were before and close
port*/
CMOOSLinuxSerialPort::~CMOOSLinuxSerialPort()
{
    Close();
}

/** Create and set up the port */
bool CMOOSLinuxSerialPort::Create(const char * sPort, int nBaudRate)
{
    if (m_nPortFD >= 0)
    {
        MOOSTrace("Serial Port already open.\n");
        return false;
    }

#ifndef _WIN32
    int nLinuxBaudRate = B9600;
    switch(nBaudRate)
    {
#ifdef B500000
    case 500000:    nLinuxBaudRate = B500000; break;
#endif
    case 115200:    nLinuxBaudRate = B115200; break;
    case 38400:     nLinuxBaudRate = B38400;  break;
    case 19200:     nLinuxBaudRate = B19200;  break;
    case 9600:      nLinuxBaudRate = B9600;   break;
    case 4800:      nLinuxBaudRate = B4800;   break;
    case 2400:      nLinuxBaudRate = B2400;   break;
    case 1200:      nLinuxBaudRate = B1200;   break;
    case 600:       nLinuxBaudRate = B600;    break;
    case 300:       nLinuxBaudRate = B300;    break;
    default :
        printf("Unsupported baud rate\n");
        return false;
        break;
    }

    // open and configure the serial port
    m_nPortFD = open(sPort, O_RDWR | O_NOCTTY | O_NDELAY);

    if (m_nPortFD <0)
    {
        perror(sPort);
        exit(-1);
    }

    //save the current configuration
    tcgetattr(m_nPortFD,&m_OldPortOptions);

    //zero the buffers
    //bzero(&m_PortOptions, sizeof(m_PortOptions));
    memset(&m_PortOptions,0,sizeof(m_PortOptions));
    m_PortOptions.c_cflag = nLinuxBaudRate | CS8 | CLOCAL | CREAD;
    m_PortOptions.c_iflag = IGNPAR;
    m_PortOptions.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    m_PortOptions.c_lflag = 0;

    // inter-character timer unused
    m_PortOptions.c_cc[VTIME]    = 0;
    // blocking read until 0 chars received, i.e. don't block
    m_PortOptions.c_cc[VMIN]     = 0;

    //save the new settings
    tcflush(m_nPortFD, TCIFLUSH);
    tcsetattr(m_nPortFD,TCSANOW,&m_PortOptions);

#endif

    return  m_nPortFD!=0;
}



/** returns the port file descriptor */
int CMOOSLinuxSerialPort::GetFD()
{
    return m_nPortFD;
}


int CMOOSLinuxSerialPort::GrabN(char * pBuffer,int nRequired)
{
    if (m_nPortFD < 0)
    {
        MOOSTrace("Can't GrabN because port is not open.\n");
        return 0;
    }

    #ifndef _WIN32
        //lock the port
        m_PortLock.Lock();
        int nRead = read(m_nPortFD, pBuffer, nRequired);
        //lock the port
        m_PortLock.UnLock();
        return nRead;
    #else
        return 0;
    #endif
}




/** Write a string out of the port. The time at which it was written is written
to *pTime */
int CMOOSLinuxSerialPort::Write(const char* Str,int nLen,double* pTime)
{
    if (m_nPortFD < 0)
    {
        MOOSTrace("Cannot Write because port is not open.\n");
        return 0;
    }

    int nChars = 0;

    //grab the time..
    if(pTime!=NULL)
    {
        *pTime = MOOSTime();
    }

    //lock the port
    m_PortLock.Lock();
#ifndef _WIN32
    nChars = write(m_nPortFD, Str,nLen);
#endif
    m_PortLock.UnLock();

    return nChars;


    for(int i = 0; i<nLen;i++)
    {
        #ifndef _WIN32

        if(write(m_nPortFD, &Str[i],1)!=1)
        {
            MOOSTrace("Write Failed\n");
            nChars = -1;
            break;
        }
        else
        {

        nChars++;
        }
        #else

        #endif
    }

    m_PortLock.UnLock();
    //how many chars did we write?

    return nChars;
}

/**
 *The ability to send a Break signal (~.5sec of tying the TX pin low) in Linux
 */
void CMOOSLinuxSerialPort::Break()
{

    if (m_nPortFD < 0)
    {
        MOOSTrace("Cannot Break because port is not open\n");
        return;
    }

    //according to http://www.mkssoftware.com/docs/man3/tcsendbreak.3.asp
    //a default value of 0 should work fine, sends a break for 250-500ms,
#ifndef _WIN32
    tcsendbreak(m_nPortFD, 0);
#endif
}

/**
 *Call this method in order to free the Output Buffer of any characters
 *that may not have been sent during our last write.  We use the queue_selector
 *TCOFLUSH.
 *@see http://www.mkssoftware.com/docs/man3/tcflush.3.asp
 */
int CMOOSLinuxSerialPort::Flush()
{
    if (m_nPortFD < 0)
    {
        MOOSTrace("Cannot Flush because port is not open.\n");
        return -1;
    }

    int nRes = -1;
#ifndef _WIN32
    nRes = tcflush(m_nPortFD, TCOFLUSH);
#endif

    return nRes;
}

bool CMOOSLinuxSerialPort::Close()
{
    if (m_nPortFD < 0)
    {
        MOOSTrace("Cannot Close because port is not open.\n");
        return false;
    }

    bool bResult = true;
    CMOOSSerialPort::Close();

#ifndef _WIN32
    tcsetattr(m_nPortFD,
          TCSANOW,
          &m_OldPortOptions);

    bResult =  close(m_nPortFD)==0;
    m_nPortFD = -1;
#endif

    return bResult;
}
