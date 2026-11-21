/**
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
**/





#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Utils/MOOSNTSerialPort.h"
#include <iostream>
using namespace std;
#include <assert.h>


CMOOSNTSerialPort::CMOOSNTSerialPort()
{
    m_pfnUserIsCompleteReplyCallBack = NULL;
}


bool CMOOSNTSerialPort::Create(const char * pPortNum, 
                               int nBaudRate)
{

    LONG    lLastError = ERROR_SUCCESS;

    // Attempt to open the serial port (COM1)
    lLastError = CNTSerial::Open(pPortNum);
    
    if (lLastError != ERROR_SUCCESS)
    {
        MOOSTrace("Unable to open COM-port\n");
        return false;
    }
    
    EBaudrate eBaud;
    
    
    // ARH 14/05/2005
    // added check for CSM PCMCIA serial card
    if (!m_bUseCsmExt)
    {
        // Standard Serial port
        switch(nBaudRate)
        {
        case 500000:    eBaud=CNTSerial::EBaud500000;    break;
        case 256000:    eBaud=CNTSerial::EBaud256000;    break;
        case 128000:    eBaud=CNTSerial::EBaud128000;    break;
        case 115200:    eBaud=CNTSerial::EBaud115200;    break;
        case 57600:     eBaud=CNTSerial::EBaud57600;     break;
        case 38400:     eBaud=CNTSerial::EBaud38400;       break;
        case 19200:     eBaud=CNTSerial::EBaud19200;       break;
        case 9600:      eBaud=CNTSerial::EBaud9600;        break;
        case 4800:        eBaud=CNTSerial::EBaud4800;       break;
        case 2400:        eBaud=CNTSerial::EBaud2400;       break;
        case 1200:        eBaud=CNTSerial::EBaud1200;       break;
        case 600:        eBaud=CNTSerial::EBaud600;       break;
        case 300:        eBaud=CNTSerial::EBaud300;       break;
            
        default :   
            printf("unsupported baud rate\n");
            return false;
            break;
            
        }
    }
    else
    {
        // ARH 14/05/2005
        // CSM PCMCIA serial card, with non-standard clock
        switch(nBaudRate)
        {
        case 500000: eBaud=CNTSerial::EBaudCSM500000; break;
        case 38400:  eBaud=CNTSerial::EBaudCSM38400;  break;
        case 19200:  eBaud=CNTSerial::EBaudCSM19200;  break;
        case 9600:   eBaud=CNTSerial::EBaudCSM9600;   break;
            
        default :   
            printf("unsupported baud rate\n");
            return false;
            break;
        }
    }

    // Setup the serial port (9600,N81) using hardware handshaking
    lLastError = CNTSerial::Setup(  eBaud,
                                    CNTSerial::EData8,
                                    CNTSerial::EParNone,
                                    CNTSerial::EStop1);

    if (lLastError != ERROR_SUCCESS)
    {
        printf("Unable to set COM-port setting\n");
    }

    CNTSerial::EHandshake eHS = m_bHandShaking ? 
                                CNTSerial::EHandshakeHardware :
                                CNTSerial::EHandshakeOff;


    lLastError = CNTSerial::SetupHandshaking(eHS);

    if (lLastError != ERROR_SUCCESS)
    {
        printf("Unable to set COM-port handshaking\n");
        return false;
    }

    SetupReadTimeouts(CNTSerial::EReadTimeoutNonblocking);

    return true;
}


int CMOOSNTSerialPort::Write(const char *pData, int nLen,double *pTime)
{
	MOOS::DeliberatelyNotUsed(pTime);
    LONG    lLastError = ERROR_SUCCESS;

    DWORD WriteTimeOut = 2000;

    //lock the port
    m_PortLock.Lock();
        lLastError = CNTSerial::Write((void*)pData,nLen,0,0,WriteTimeOut);
    m_PortLock.UnLock();
    
    if (lLastError != ERROR_SUCCESS)
    {
        printf("Unable to send data\n");
        return -1;
    }

    return nLen;
}


int CMOOSNTSerialPort::GrabN(char * pBuffer,int nRequired)
{
    unsigned long dwRes = 0;

    m_PortLock.Lock();
        CNTSerial::NTRead(pBuffer,nRequired,&dwRes);
    m_PortLock.UnLock();
    
    return (int)dwRes;

}

bool CMOOSNTSerialPort::Close()
{
    if(CMOOSSerialPort::Close())
    {
        ClosePort();
        return true;
    }
    else
    {
        return false;
    }

}


void CMOOSNTSerialPort::Break()
{
    SetCommBreak(m_hFile);
    MOOSPause(500);
    ClearCommBreak(m_hFile);
}    

