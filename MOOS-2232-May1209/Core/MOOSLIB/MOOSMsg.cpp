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
// MOOSMsg.cpp: implementation of the CMOOSMsg class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <MOOSGenLib/MOOSGenLib.h>
#include "MOOSMsg.h"
#include "MOOSException.h"
#include "MOOSGlobalHelper.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstring>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMOOSMsg::CMOOSMsg()
{
    m_cMsgType=MOOS_NULL_MSG;
    m_cDataType=MOOS_DOUBLE;
    m_dfTime = -1;
    m_dfVal = -1;
    m_dfVal2 = -1;
}

CMOOSMsg::~CMOOSMsg()
{

}

CMOOSMsg::CMOOSMsg(char cMsgType,const std::string  & sKey,double dfVal,double dfTime)
{
    m_cMsgType = cMsgType;
    m_dfVal = dfVal;
    m_dfVal2 = -1;
    m_cDataType = MOOS_DOUBLE;
    m_sKey = sKey;
    m_dfTime = -1;

    if(dfTime==-1)
    {
        m_dfTime = MOOSTime();
    }
    else
    {
        m_dfTime=dfTime;
    }
}


CMOOSMsg::CMOOSMsg(char cMsgType,const std::string & sKey,const std::string &sVal,double dfTime)
{
    m_cMsgType = cMsgType;
    m_dfVal = -1;
    m_dfVal2 = -1;
    m_cDataType = MOOS_STRING;
    m_sKey = sKey;
    m_sVal = sVal;
    m_dfTime = -1;

    if(dfTime==-1)
    {
        m_dfTime = MOOSTime();
    }
    else
    {
        m_dfTime=dfTime;
    }
}

/** copies data to a buffer reversing byte order if needed */
template<class T> void CopyToBufferAsLittleEndian(T Var,unsigned char* pBuffer)
{
    int nSize = sizeof(T);
    if(!IsLittleEndian())
    {
        //watch out we need to switch to MOOS standard of sending data
        //as little endian (natural for x86 architectures)
        T ACopy = SwapByteOrder<T>(Var);

        memcpy((void*)(pBuffer),(void*)(&ACopy),nSize);
    }
    else
    {
        //x86 machines should execute this...
        memcpy((void*)(pBuffer),(void*)(&Var),nSize);
    }
}


template<class T> T CopyFromBufferAsLittleEndian(unsigned char* pBuffer)
{
    int nSize = sizeof(T);
    T Val;
    memcpy((void*)(&Val),(void*)(pBuffer),nSize);
    if(!IsLittleEndian())
    {
        //watch out we need to switch to MOOS standard of sending data
        //as little endian (natural for x86 architectures)
        Val = SwapByteOrder<T>(Val);

    }    

    return Val;
}


void  CMOOSMsg::operator << (double & dfVal)
{
    int nSize = sizeof(dfVal);

    if(CanSerialiseN(nSize))
    {
        CopyToBufferAsLittleEndian<double>(dfVal,m_pSerializeBuffer);
        m_pSerializeBuffer+=nSize;
    }
    else
    {
        throw CMOOSException("CMOOSMsg::operator << Out Of Space");
    }
}

void  CMOOSMsg::operator >> (double & dfVal)
{
    int nSize = sizeof(dfVal);

    if(CanSerialiseN(nSize))
    {
        dfVal = CopyFromBufferAsLittleEndian<double>(m_pSerializeBuffer);
        //memcpy((void*)(&dfVal),(void*)(m_pSerializeBuffer),nSize);
        m_pSerializeBuffer+=nSize;
    }
    else
    {
        throw CMOOSException("CMOOSMsg::operator >> Out Of Space");
    }
}



void  CMOOSMsg::operator << (string &   sVal)
{

    int nSize = sVal.size()+1;

    if(CanSerialiseN(nSize))
    {
        strcpy((char *)m_pSerializeBuffer,sVal.c_str());
        m_pSerializeBuffer+=nSize;
    }
    else
    {
        throw CMOOSException("CMOOSMsg::operator << Out Of Space");
    }
}

void  CMOOSMsg::operator >> (string & sVal)
{
    int nSize = strlen((char*)m_pSerializeBuffer)+1;

    if(CanSerialiseN(nSize))
    {
        sVal.insert(0,(char*)m_pSerializeBuffer,nSize-1);
        m_pSerializeBuffer+=nSize;
    }
    else
    {
        throw CMOOSException("CMOOSMsg::operator >> Out Of Space");
    }
}


void  CMOOSMsg::operator << (int & nVal)
{
    int nSize = sizeof(int);

    if(CanSerialiseN(nSize))
    {
        CopyToBufferAsLittleEndian<int>(nVal,m_pSerializeBuffer);
        //        memcpy((void*)(m_pSerializeBuffer),(void*)(&nVal),sizeof(nVal));
        m_pSerializeBuffer+=nSize;
    }
    else
    {
        throw CMOOSException("CMOOSMsg::operator << Out Of Space");
    }

}
void  CMOOSMsg::operator >> (int & nVal)
{
    int nSize = sizeof(int);

    if(CanSerialiseN(nSize))
    {
        nVal = CopyFromBufferAsLittleEndian<int>(m_pSerializeBuffer);

        //memcpy((void*)(&nVal),(void*)(m_pSerializeBuffer),sizeof(nVal));
        m_pSerializeBuffer+=nSize;
    }
    else
    {
        throw CMOOSException("CMOOSMsg::operator >> Out Of Space");
    }

}

void  CMOOSMsg::operator << (char &  cVal)
{
    int nSize = sizeof(cVal);

    if(CanSerialiseN(nSize))
    {
        memcpy((void*)(m_pSerializeBuffer),(void*)(&cVal),sizeof(cVal));
        m_pSerializeBuffer+=sizeof(cVal);
    }
    else
    {
        throw CMOOSException("CMOOSMsg::operator << Out Of Space");
    }

}

void  CMOOSMsg::operator >> (char & cVal)
{
    int nSize = sizeof(cVal);

    if(CanSerialiseN(nSize))
    {
        memcpy((void*)(&cVal),(void*)m_pSerializeBuffer,sizeof(cVal));
        m_pSerializeBuffer+=sizeof(cVal);
    }
    else
    {
        throw CMOOSException("CMOOSMsg::operator >> Out Of Space");
    }

}


int CMOOSMsg::Serialize(unsigned char *pBuffer, int nLen, bool bToStream)
{

    if(bToStream)
    {
        try
        {

            //MOOSTrace("Packing Msg with community %s\n",m_sOriginatingCommunity.c_str());

            m_pSerializeBuffer = pBuffer;
            m_pSerializeBufferStart = pBuffer;
            m_nSerializeBufferLen = nLen;

            //leave space for total byte count
            m_pSerializeBuffer +=sizeof(int);

            //what is message ID;
            (*this)<<m_nID;

            //what type of message is this?
            (*this)<<m_cMsgType;

            //what type of data is this?
            (*this)<<m_cDataType;

            //from whence does it come
            (*this)<<m_sSrc;

            //and from which community?
            (*this)<<m_sOriginatingCommunity;

            //what
            (*this)<<m_sKey;

            //what time was the notification?
            (*this)<<m_dfTime;

            //double data
            (*this)<<m_dfVal;

            //double data
            (*this)<<m_dfVal2;

            //string data
            (*this)<<m_sVal;


            //how many bytes in total have we written (this includes an int at the start)?
            m_nLength = m_pSerializeBuffer-m_pSerializeBufferStart;

            //reset destination
            m_pSerializeBuffer = m_pSerializeBufferStart;

            //write the number of bytes
            (*this)<<m_nLength;




        }
        catch(CMOOSException e)
        {
            MOOSTrace("exception : CMOOSMsg::Serialize failed: %s\n ",e.m_sReason);
            return -1;
        }
    }
    else
    {
        //this is extracting from a stream....

        try
        {
            m_pSerializeBuffer = pBuffer;
            m_pSerializeBufferStart = pBuffer;
            m_nSerializeBufferLen = nLen;


            (*this)>>m_nLength;

            //what is message ID;
            (*this)>>m_nID;

            //what type of message is this?
            (*this)>>m_cMsgType;

            //what type of data is this?
            (*this)>>m_cDataType;


            //from whence does it come
            (*this)>>m_sSrc;

            //and from which community?
            (*this)>>m_sOriginatingCommunity;

            //what
            (*this)>>m_sKey;

            //what time was the notification?
            (*this)>>m_dfTime;

            //double data
            (*this)>>m_dfVal;

            //double data
            (*this)>>m_dfVal2;


            //string data
            (*this)>>m_sVal;

        }
        catch(CMOOSException e)
        {
            MOOSTrace("exception : CMOOSMsg::Serialize failed: %s\n ",e.m_sReason);

            return -1;
        }

    }
    return m_nLength;

}

int CMOOSMsg::GetLength()
{
    return m_nLength;
}

bool CMOOSMsg::CanSerialiseN(int N)
{
    return      (m_pSerializeBuffer-m_pSerializeBufferStart)+N<=m_nSerializeBufferLen;
}

bool CMOOSMsg::IsType(char cType) const
{
    return m_cMsgType==cType;
}

void CMOOSMsg::Trace()
{
    MOOSTrace("Type=%c DataType=%c Key =%s ",m_cMsgType,m_cDataType,m_sKey.c_str());


    switch(m_cDataType)
    {
    case MOOS_DOUBLE:
        MOOSTrace("Data=%f ",m_dfVal);
        break;
    case MOOS_STRING:
        MOOSTrace("Data=%s ",m_sVal.c_str());
        break;
    }


    MOOSTrace("Source= %s Time = %10.3f\n",m_sSrc.c_str(),m_dfTime);
}

bool CMOOSMsg::IsYoungerThan(double dfAge) const
{
    return m_dfTime>=dfAge;
}

/**
*A method to check the timestamping of a MOOSMsg.
*Does so by checking the <code>TimeNow</code> passed to it, and gives the
*requesting class an idea about how out of sync this message is by comparing the
*MOOSMsg's time stamp (<code>m_dfTime</code>) to SKEW_TOLERANCE.  
*@return true if a MOOSMsg's time stamp is either SKEW_TOLERANCE seconds ahead or 
*behind the MOOSDB clock.  Will also pass you the <code>pdfSkew</code>, or amount of
*time difference between the MOOSDB and MOOSMsg timestamp if desired. 
*/
bool CMOOSMsg::IsSkewed(double dfTimeNow, double * pdfSkew)
{
    //if we are in playback mode (a global app wide flag)
    //then skew may not mean anything.. (we can stop and start at will)
    if(IsMOOSPlayBack())
    {
        dfTimeNow  = m_dfTime;
    }

    double dfSkew = fabs(dfTimeNow - m_dfTime);

    if(pdfSkew != NULL)
    {
        *pdfSkew = dfSkew;
    }

    return (dfSkew > SKEW_TOLERANCE) ? true : false;

}






string CMOOSMsg::GetAsString(int nFieldWidth/*=12*/)
{
    ostringstream os;

    os.setf(ios::left);

    if(GetTime()!=-1)
    {        
        if(IsDataType(MOOS_DOUBLE))
        {
            os<<setw(nFieldWidth)<<setprecision(12)<<m_dfVal<<ends;       
        }
        else
        {
            os<<m_sVal.c_str()<<ends;
        }
    }
    else
    {
        os<<setw(nFieldWidth)<<"NotSet"<<ends;       
    }

    return os.str();
}

bool CMOOSMsg::IsDataType(char cDataType) const
{
    return m_cDataType == cDataType;
}
