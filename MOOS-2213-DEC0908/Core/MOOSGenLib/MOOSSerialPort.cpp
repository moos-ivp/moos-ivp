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
// MOOSSerialPort.cpp: implementation of the CMOOSSerialPort class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include "windows.h"
#include "winbase.h"
#include "winnt.h"
#else
#include <pthread.h>
#endif

#define DEFAULT_COMMS_SPACE 1000    
#define MOOS_SERIAL_INBOX_MAX_SIZE 1000


#include "MOOSGenLibGlobalHelper.h"
#include "MOOSSerialPort.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




#ifdef _WIN32

DWORD WINAPI CommsLoopProc( LPVOID lpParameter)
{
    
    CMOOSSerialPort* pMe =     (CMOOSSerialPort*)lpParameter;
    
    return pMe->CommsLoop();    
}


#else

void * CommsLoopProc( void * lpParameter)
{
    
    MOOSTrace("starting comms thread in linux\n");
    CMOOSSerialPort* pMe =     (CMOOSSerialPort*)lpParameter;
    
    pMe->CommsLoop();    
    
    return NULL;
}

#endif




bool CMOOSSerialPort::StartThreads()
{
    m_bQuit = false;
    
#ifdef _WIN32
    //this is the main listen thread
    m_hCommsThread = ::CreateThread(    NULL,
        0,
        CommsLoopProc,
        this,
        CREATE_SUSPENDED,
        &m_nCommsThreadID);
    ResumeThread(m_hCommsThread);
#else
    
    MOOSTrace("In start thread\n");
    int Status = pthread_create(& m_nCommsThreadID,NULL,CommsLoopProc,this);
    
    if(Status!=0)
    {
        return false;
    }
    
    
#endif
    
    return true;
}

CMOOSSerialPort::CMOOSSerialPort()
{
    m_sPort             = DEFAULT_PORT;
    m_nBaudRate         = DEFAULT_BAUDRATE;
    m_bHandShaking     = false;
    m_bStreaming     = false;
    m_bVerbose       = false;
    m_bQuit             = false;
    m_pfnUserIsCompleteReplyCallBack = NULL;
    m_cTermCharacter = '\r';

    // ARH 14/05/2005 500kBaud CSM PCMCIA serial card setting
    m_bUseCsmExt = false;
}

CMOOSSerialPort::~CMOOSSerialPort()
{
    
}

bool CMOOSSerialPort::Configure(STRING_LIST sParams)
{
    MOOSTrace("CMOOSSerialPort::Configure() : ");
    
    STRING_LIST::iterator p;
    
    for(p=sParams.begin();p!=sParams.end();p++)
    {
        std::string sLine = *p;
        
        std::string sTok = MOOSChomp(sLine,"=");
        
        std::string sVal = sLine;
        
        
        if(MOOSStrCmp(sTok,"PORT"))
        {
            m_sPort = sVal;
            MOOSTrace("%s,",m_sPort.c_str());
        }
        else if(MOOSStrCmp(sTok,"BAUDRATE"))
        {
            m_nBaudRate = atoi(sVal.c_str());
            
            if(m_nBaudRate==0)
            {
                m_nBaudRate = DEFAULT_BAUDRATE;
            }
            MOOSTrace("%d,",m_nBaudRate);
            
        }
        else if(MOOSStrCmp(sTok,"HANDSHAKING"))
        {
            if(MOOSStrCmp(sVal,"TRUE"))
            {
                m_bHandShaking = true;
            }
            else
            {
                m_bHandShaking = false;
            }
        }
        else if(MOOSStrCmp(sTok,"VERBOSE"))
        {
            
            if(MOOSStrCmp(sVal,"TRUE"))
            {
                m_bVerbose = true;
            }
            else
            {
                m_bVerbose = false;
            }
        }
        else if(MOOSStrCmp(sTok,"STREAMING"))
        {
            
            if(MOOSStrCmp(sVal,"TRUE"))
            {
                m_bStreaming = true;
            }
            else
            {
                m_bStreaming = false;
            }
            MOOSTrace("%s,",m_bStreaming?"streaming":"standard");
            
        }

        // ARH 14/05/2005 Added to allow use of the 500kbaud CSM PCMCIA card
        else if (MOOSStrCmp(sTok, "USECSMEXT"))
        {
            if (MOOSStrCmp(sVal, "TRUE"))
            {
                m_bUseCsmExt = true;
            }
            else
            {
                m_bUseCsmExt = false;
            }
        }
    }
    
    
    bool bSuccess = Create(m_sPort.c_str(),m_nBaudRate);
    
    if(bSuccess)
    {
        Flush();
        if(m_bStreaming)
        {
            bSuccess = StartThreads();
        }
    }
    
    MOOSTrace("%s\n",bSuccess?"OK":"FAILED");
    
    return bSuccess;
    
    
    
}

void CMOOSSerialPort::Break()
{
    
}


bool CMOOSSerialPort::IsCompleteReply(char *pData, int nLen, int nRead)
{
    
    
    if(m_pfnUserIsCompleteReplyCallBack!=NULL)
    {
        return (*m_pfnUserIsCompleteReplyCallBack)(pData,nLen,nRead);
    }
    else
    {
        return (nRead>0 && pData[nRead-1]==GetTermCharacter());
        //return memchr(pData,GetTermCharacter(),nRead)!=NULL;
    }
    
}

void CMOOSSerialPort::SetIsCompleteReplyCallBack(bool (*pfn)(char *pData, int nLen, int nRead) )
{
    m_pfnUserIsCompleteReplyCallBack = pfn;
}



bool CMOOSSerialPort::CommsLoop()
{
    
    
    char    pTmp[DEFAULT_COMMS_SPACE];
    char    pAccumulator[2*DEFAULT_COMMS_SPACE];
    int        nInStore = 0;
    
    double dfTimeOut = 0.1;
    
    while(!m_bQuit)
    {
        int nRead = ReadNWithTimeOut(pTmp,DEFAULT_COMMS_SPACE,dfTimeOut);
        
        if(nRead!=-1)
        {
            
            if((nInStore+nRead)>int(sizeof(pAccumulator)))
            {
                //oops...
                MOOSTrace("Comms Loop Accumulator Overflow, reseting\n");
                nInStore = 0;
                continue;
            }
            
            //append to the accumulator
            memcpy(pAccumulator+nInStore,pTmp,nRead);
            
            //we have more in the cupboard!
            nInStore+=nRead;
            
            //lock
            m_InBoxLock.Lock();
            
            char * pTelegramEnd = NULL;
            
            do    
            {
               
               
                //A device will by default use the termination character <CR>
                //But we now allow for setting of a termination character
                //to allow for flexibility in a reply.
                pTelegramEnd = (char*)memchr(pAccumulator,GetTermCharacter(),nInStore);
                
                if(pTelegramEnd!=NULL)
                {
                    //how long is the telegram?
                    int nTelegramLength = pTelegramEnd - pAccumulator + 1;
                    
                    //make a buffer that is one larger to preserve the last character
                    //in case we are interested in it.  
                    char bBiggerBuff[2*DEFAULT_COMMS_SPACE+1];
                    
                    
                    //copy the telegram to a buffer size that is 1 larger
                    memcpy(bBiggerBuff, pAccumulator, nTelegramLength);
                    
                    //terminate it as a string
                    //*pTelegramEnd = '\0';
                    bBiggerBuff[nTelegramLength] = '\0';
                    
                    
                    //copy it
                    //std::string sTelegram((char*)pAccumulator);
                    std::string sTelegram((char*)bBiggerBuff);
                    
                    MOOSRemoveChars(sTelegram,"\r\n");
                    
                    if(IsVerbose())
                    {
                        MOOSTrace("Telegram = %s\n",sTelegram.c_str());
                    }
                    
                    //shuffle accumulator
                    memmove(pAccumulator,
                        pAccumulator+nTelegramLength,
                        sizeof(pAccumulator)-nTelegramLength);
                    
                    //we have less in our store now!
                    nInStore-=nTelegramLength;
                    
                    //make a telegram
                    CMOOSSerialTelegram Tg(sTelegram,MOOSTime());
                    
                    //stuff it
                    m_InBox.push_front(Tg);        
                }
            }while(pTelegramEnd !=NULL);
            
            
            //whatch we don't exhibit disgraceful behaviour!
            while(m_InBox.size()>MOOS_SERIAL_INBOX_MAX_SIZE)
            {
                m_InBox.pop_back();
            }
            
            //and unlock
            
            m_InBoxLock.UnLock();
            
        }
        else
        {
        }
        
    }
    return true;
}

bool CMOOSSerialPort::GetLatest(std::string &sWhat, double &dfWhen)
{
    bool bSomethingFound = false;
    
    
    m_InBoxLock.Lock();
    if(!m_InBox.empty())
    {
        sWhat = m_InBox.front().m_sTelegram;
        dfWhen = m_InBox.front().m_dfTime;
        
        m_InBox.pop_front();
        
        bSomethingFound =true;
    }
    m_InBoxLock.UnLock();
    
    return bSomethingFound;
}

bool CMOOSSerialPort::GetEarliest(std::string &sWhat, double &dfWhen)
{
    bool bSomethingFound = false;
   
   
    m_InBoxLock.Lock();
    if(!m_InBox.empty())
    {
        sWhat = m_InBox.back().m_sTelegram;
        dfWhen = m_InBox.back().m_dfTime;
       
        m_InBox.pop_back();
       
        bSomethingFound =true;
    }
    m_InBoxLock.UnLock();
   
    return bSomethingFound;
}



bool CMOOSSerialPort::IsStreaming()
{
    return m_bStreaming;
}


/**
*Sets the termination character for the serial port to watch out for
*when it constructs Telegrams for Streaming Devices.
*/

void CMOOSSerialPort::SetTermCharacter(char cTermChar)
{
    m_cTermCharacter = cTermChar;
}

/*
*@return the termination character being used by the serial port.
*/
char CMOOSSerialPort::GetTermCharacter()
{
    return m_cTermCharacter;
}




int CMOOSSerialPort::ReadNWithTimeOut(char *pData, int nLen, double dfTimeOut,double * pTime )
{
        

    int nSpace = nLen;                  //space left in buffer
    int nRead = 0;                      //total number of chars read
    bool bQuit = false;                 // exit flag on complete message
    int nGrabbed = 0;
    
    double  dfStopTime=MOOSLocalTime()+dfTimeOut;
    
    
    while (MOOSLocalTime()<dfStopTime && !bQuit)
    {

        
        //try the read
        nGrabbed = GrabN(pData+nRead,nSpace);
        
        if (nGrabbed == 0)
        {
            // wait a while...maybe it is on its way!
            MOOSPause(10);           
        }
        else if(nGrabbed<0)
        {
            MOOSTrace("Grab FAILED %s\n",MOOSHERE);
            MOOSPause(10);
        }
        else
        {
            if(nRead==0 && pTime!=NULL)
            {
                //grab the time..                        
                *pTime = MOOSTime();
            }
            
            nSpace-=nGrabbed;
            nRead+=nGrabbed;
            
            if(nSpace<=0)
                bQuit = true;            
        }
    }
    
    return nRead;
        
}



int CMOOSSerialPort::ReadNWithTimeOut2(char *pData, int nLen, double dfTimeOut, double * pTime )
{
        

    int nSpace = nLen;                  //space left in buffer
    int nRead = 0;                      //total number of chars read
    bool bQuit = false;                 //exit flag on complete message
    int nGrabbed = 0;
    
    double  dfStopTime=MOOSLocalTime()+dfTimeOut;
    
    
    while (MOOSLocalTime()<dfStopTime && !bQuit)
    {

        
        //try the read
        nGrabbed = GrabN(pData+nRead,nSpace);
        
        if (nGrabbed == 0)
        {
            // wait a while...maybe it is on its way!
            MOOSPause(10);           
        }
        else if(nGrabbed<0)
        {
            return -1; // Signal error
        }
        else
        {
            // Ensures that we only grab the time for the first byte.
            if(nRead==0 && pTime!=NULL)
            {
                //grab the time..                  
                *pTime = MOOSTime();
            }
            
            //great, so increment out buffer pointer
            //and check to see if this is a complete
            // nomad reply
            nSpace-=nGrabbed;
            nRead+=nGrabbed;
            
            if(nSpace<=0)
                bQuit = true;            
        }
    }
    
    return nRead;
        
}



bool CMOOSSerialPort::GetTelegram(std::string &sTelegram,double dfTimeOut,double *pTime)
{
    if(IsStreaming())
    {
        MOOSTrace("don't call GetTelgram on a streaming device!\n");
        return false;
    }
    
    char pData[TELEGRAM_LEN];
    double dfTimeWaited   = 0.0;              //haven't waited any tiome yet
    double dfInterval =        0.01;             //10ms
    int nRead =            0;              //total number of chars read
    
    
    while ((dfTimeWaited<dfTimeOut) && nRead<TELEGRAM_LEN)
    {
        int nGrabbed = 0;
        
        //try the read
        nGrabbed = GrabN(pData+nRead,1);

        if (nGrabbed == 0)
        {
            //OK wait a while...maybe it is on its way!
            dfTimeWaited+=dfInterval;
            
            MOOSPause((int)(dfInterval*1000.0));
        }
        else
        {
            if(nRead==0 && pTime!=NULL)
            {
                //grab the time..                        
                *pTime = MOOSTime();
            }
            
            
            nRead+=nGrabbed;
            
            //have we reached the end of the message?
            if(IsCompleteReply(pData,TELEGRAM_LEN,nRead))
            {
                pData[nRead]='\0';
                sTelegram = pData;
                MOOSRemoveChars(sTelegram,"\r\n");
            
                if(IsVerbose())
                {
                    MOOSTrace("Telegram = %s\n",sTelegram.c_str());
                }
                //MOOSTrace("Required %d retries and %d accumulates\n",nRetries,nAccumulates);
                return true;
            }            
        }
    }
    
    return false;
}


bool CMOOSSerialPort::GetTelegramOrAccumulate(std::string &sTelegram,double dfTimeOut,double *pTime)
{
    if(IsStreaming())
    {
        MOOSTrace("don't call GetTelegram on a streaming device!\n");
        return false;
    }
    
    static char telegramBuffer[TELEGRAM_LEN];
    static int nTelegramBufferRead = 0;              //total number of chars read
    
    double dfTimeWaited = 0.0;              //haven't waited any time yet
    double dfInterval = 0.01;             //10ms
    
    
    while ((dfTimeWaited<dfTimeOut) && nTelegramBufferRead<TELEGRAM_LEN)
    {
        int nGrabbed = 0;
        
        //try the read
        nGrabbed = GrabN(telegramBuffer+nTelegramBufferRead,1);
        
        if (nGrabbed == 0)
        {
            //OK wait a while...maybe it is on its way!
            dfTimeWaited+=dfInterval;
            
            MOOSPause((int)(dfInterval*1000.0));
        }
        else
        {
            if(nTelegramBufferRead==0 && pTime!=NULL)
            {
                //grab the time..                        
                *pTime = MOOSTime();
            }
            
            
            nTelegramBufferRead+=nGrabbed;
            
            //have we reached the end of the message?
            if(IsCompleteReply(telegramBuffer,TELEGRAM_LEN,nTelegramBufferRead))
            {
                telegramBuffer[nTelegramBufferRead]='\0';
                nTelegramBufferRead = 0;
                sTelegram = telegramBuffer;
                MOOSRemoveChars(sTelegram,"\r\n");
                
                if(IsVerbose())
                {
                    MOOSTrace("Telegram = %s\n",sTelegram.c_str());
                }
                //MOOSTrace("Required %d retries and %d accumulates\n",nRetries,nAccumulates);
                return true;
            }            
        }
    }
    
    return false;
}

bool CMOOSSerialPort::Close()
{

    m_bQuit = true;

    if(m_bStreaming)
    {
#ifdef _WIN32
    WaitForSingleObject(m_hCommsThread,INFINITE);
#else
    void * Result;
    pthread_join(m_nCommsThreadID,&Result);
#endif
    }
    return true;


}

std::string CMOOSSerialPort::GetPortName()
{
    return m_sPort;
}

