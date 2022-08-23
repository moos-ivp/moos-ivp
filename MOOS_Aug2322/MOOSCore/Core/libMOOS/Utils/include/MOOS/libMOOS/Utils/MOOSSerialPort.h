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
// MOOSSerialPort.h: interface for the CMOOSSerialPort class.
//
//////////////////////////////////////////////////////////////////////

/*! \file MOOSSerialPort.h */

#if !defined(MOOSSERIALPORTH)
#define MOOSSERIALPORTH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
    #define DEFAULT_PORT  "COM1"
#else
    #define DEFAULT_PORT  "/dev/ttyS0"
#endif

#define DEFAULT_BAUDRATE 9600
#define TELEGRAM_LEN 1000

#include "MOOS/libMOOS/Utils/MOOSLock.h"

#include <string>
#include <list>

typedef std::list<std::string> STRING_LIST;

//! Cross Platform Serial Port Base Class
/*!
Provides cross platform functionality which is implemented in detail
by the platform dependent derivatives
*/
class CMOOSSerialPort
{
public:
    std::string GetPortName();
    virtual bool Close();
    char GetTermCharacter();
    void SetTermCharacter(char cTermChar);
    int GetBaudRate(){return m_nBaudRate;};

    virtual int Flush(){return -1;};

    bool IsStreaming();
    bool IsVerbose(){return m_bVerbose;};
    bool GetLatest(std::string & sWhat,double & dfWhen);

    bool GetEarliest(std::string &sWhat, double &dfWhen);


    class CMOOSSerialTelegram
    {
    public:
		CMOOSSerialTelegram(){};
        CMOOSSerialTelegram(const std::string & sWhat,double dfWhen):
          m_sTelegram(sWhat),m_dfTime(dfWhen){};

        std::string m_sTelegram;
        double m_dfTime;
    };
    typedef std::list<CMOOSSerialTelegram> TELEGRAM_LIST;

    TELEGRAM_LIST m_InBox;
    TELEGRAM_LIST m_OutBox;
    CMOOSLock m_InBoxLock;
    CMOOSLock m_OutBoxLock;
    CMOOSLock m_PortLock;

    bool CommsLoop();

    CMOOSSerialPort();
    virtual ~CMOOSSerialPort();


    virtual bool Configure(STRING_LIST sParams);

    //crucial functions...
    virtual bool Create(const char * pPortNum=DEFAULT_PORT, int nBaudRate=DEFAULT_BAUDRATE)=0;
    virtual int  ReadNWithTimeOut(char * pBuff, int  nBufferLen,double Timeout=0.5, double* pTime = NULL);
    virtual int  Write(const char* Str,int nLen, double* pTime=NULL)=0;

    //returns a complete telegram in sTelegram (block of terminated characters)
    bool GetTelegram(std::string &sTelegram,double dfTimeOut,double *pTime=NULL);
    
    /**
     * Similar to the GetTelegram method,
     * but uses an internal data buffer to store incomplete telegrams.
     * addition to API submitted by submitted by Arjan Vermeij*/
    bool GetTelegramOrAccumulate(std::string &sTelegram,double dfTimeOut,double *pTime=NULL);
    
    // This version of ReadNWithTimeOut returns a negative value to signal an error
    virtual int  ReadNWithTimeOut2(char * pBuff, int nBufferLen, double Timeout=0.5, double* pTime = NULL);

    void SetIsCompleteReplyCallBack(bool (*pfn)(char *pData, int nLen, int nRead) );

    virtual void Break();

protected:
    //termination character the serial port is interested in
    char m_cTermCharacter;

    /** Win32 handle to IO thread */
#ifdef _WIN32
    HANDLE m_hCommsThread;
#endif


#ifdef _WIN32
    typedef unsigned long THREAD_ID;
#else
    typedef pthread_t THREAD_ID;
#endif


    /** ID of IO thread */
    THREAD_ID        m_nCommsThreadID;


    bool StartThreads();

    bool m_bStreaming;

    bool m_bVerbose;

    virtual int GrabN(char * pBuffer,int nRequired)=0;

    bool (*m_pfnUserIsCompleteReplyCallBack)( char *pData, int nLen, int nRead);

    bool IsCompleteReply(char * pData,int nLen, int nRead);

    ///handware handshaking active flag
       bool m_bHandShaking;

    ///port name
    std::string m_sPort;

    ///baudrate
    int m_nBaudRate;

    bool m_bQuit;

    /// ARH 14/05/2005 For 500kBaud PCMCIA card
        bool m_bUseCsmExt;

};

#endif //
