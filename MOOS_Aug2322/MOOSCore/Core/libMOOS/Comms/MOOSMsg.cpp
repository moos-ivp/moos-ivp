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
//   http://www.gnu.org/licenses/lgpl.txt distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/


// MOOSMsg.cpp: implementation of the CMOOSMsg class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Utils/MOOSException.h"
#include "MOOS/libMOOS/Utils/MOOSPlaybackStatus.h"
#include "MOOS/libMOOS/Comms/MOOSMsg.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <cstring>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSMsg::CMOOSMsg()
    : m_cMsgType(MOOS_NULL_MSG),
      m_cDataType(MOOS_DOUBLE),
      m_nID(-1),
      m_dfTime(-1),
      m_dfVal(-1),
      m_dfVal2(-1),
      m_sSrc(""),
      m_sSrcAux("") {}

CMOOSMsg::~CMOOSMsg() {}

CMOOSMsg::CMOOSMsg(char cMsgType, const std::string &sKey, double dfVal,
                   double dfTime)
    : m_cMsgType(cMsgType),
      m_cDataType(MOOS_DOUBLE),
      m_sKey(sKey),
      m_nID(-1),
      m_dfTime((dfTime == -1) ?  MOOSTime() : dfTime),
      m_dfVal(dfVal),
      m_dfVal2(-1) {}

CMOOSMsg::CMOOSMsg(char cMsgType, const std::string &sKey,
                   const std::string &sVal, double dfTime)
    : m_cMsgType(cMsgType),
      m_cDataType(MOOS_STRING),
      m_sKey(sKey),
      m_nID(-1),
      m_dfTime((dfTime == -1) ?  MOOSTime() : dfTime ),
      m_dfVal(-1),
      m_dfVal2(-1),
      m_sVal(sVal) {}

CMOOSMsg::CMOOSMsg(char cMsgType, const std::string &sKey,
                   unsigned int nDataSize, const void *Data, double dfTime)
    : m_cMsgType(cMsgType),
      m_cDataType(MOOS_BINARY_STRING),
      m_sKey(sKey),
      m_nID(-1),
      m_dfTime((dfTime == -1) ?  MOOSTime() : dfTime),
      m_dfVal(-1),
      m_dfVal2(-1) {
  m_sVal.assign((char *)Data, nDataSize);
}

bool CMOOSMsg::operator == (const CMOOSMsg & M) const
{
    return m_cMsgType == M.m_cMsgType &&
           m_cDataType==M.m_cDataType &&
           m_sKey == M.m_sKey &&
           m_sOriginatingCommunity == M.m_sOriginatingCommunity &&
           m_sSrcAux == M.m_sSrcAux &&
           m_sSrc == M.m_sSrcAux &&
           fabs(m_dfVal - M.m_dfVal) < 2* std::numeric_limits<double>::epsilon() &&
           fabs(m_dfVal2 - M.m_dfVal2) < 2* std::numeric_limits<double>::epsilon() &&
           fabs(m_dfTime - M.m_dfTime) < 2* std::numeric_limits<double>::epsilon() &&
           m_nID == M.m_nID;
}


void CMOOSMsg::MarkAsBinary()
{
	m_cDataType=MOOS_BINARY_STRING;
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
/*
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
 */
	int nSize = sVal.size();
	*this<<nSize;
	
	if(CanSerialiseN(nSize))
    {
        memcpy((char *)m_pSerializeBuffer,sVal.data(),nSize);
        m_pSerializeBuffer+=nSize;
    }
    else
    {
        throw CMOOSException("CMOOSMsg::operator << Out Of Space");
    }
	
}

void  CMOOSMsg::operator >> (string & sVal)
{
	int nSize;
	*this>>nSize;
	
    if(CanSerialiseN(nSize))
    {
        //sVal.insert(0,(char*)m_pSerializeBuffer,nSize);

        sVal.assign((const char *)m_pSerializeBuffer,nSize);
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

unsigned int CMOOSMsg::GetSizeInBytesWhenSerialised() const
{
    unsigned int nInt = 2*sizeof(int);
    unsigned int nChar = 2*sizeof(char);
    unsigned int nString = sizeof(int)+m_sSrc.size()+
            sizeof(int)+m_sSrcAux.size()+
            sizeof(int)+m_sOriginatingCommunity.size()+
            sizeof(int)+m_sKey.size()+
            sizeof(int)+m_sVal.size();

    unsigned int nDouble = 3*sizeof(double);

    return nInt+nChar+nString+nDouble;

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

			//extra source info
			(*this)<<m_sSrcAux;

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
        catch(CMOOSException & e)
        {
            MOOSTrace("exception : CMOOSMsg::Serialize failed: %s\n ",e.m_sReason);
			MOOSTrace("perhaps accummulated messages exceeded available buffer space of %d bytes\n", m_nSerializeBufferLen);
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

			//extra source info
			(*this)>>m_sSrcAux;

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
        catch(CMOOSException & e)
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

char CMOOSMsg::GetType() const
{
	return m_cMsgType;
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
	case MOOS_BINARY_STRING:
			MOOSTrace("Data=%.3f KB of binary	data ",m_sVal.size()/1000.0);
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
//    if(IsMOOSPlayBack())
//    {
//        dfTimeNow  = m_dfTime;
//    }

    double dfSkew = fabs(dfTimeNow - m_dfTime);

    if(pdfSkew != NULL)
    {
        *pdfSkew = dfSkew;
    }

    return (dfSkew > SKEW_TOLERANCE) ? true : false;

}






string CMOOSMsg::GetAsString(int nFieldWidth/*=12*/, int nNumDP/*=5*/)
{
    ostringstream os;

    os.setf(ios::left);

    if(GetTime()!=-1)
    {        
        if(IsDataType(MOOS_DOUBLE))
        {
			os.setf(ios::fixed);

            os<<setw(nFieldWidth)<<setprecision(nNumDP)<<m_dfVal;//<<ends;
        }
		else if(IsDataType(MOOS_BINARY_STRING))
		{
			os<<"BINARY DATA ["<<m_sVal.size()/1000.0<<" kB]";//<<ends;
		}
        else 
        {
            os<<m_sVal;//.c_str()<<ends;
        }
    }
    else
    {
        os<<setw(nFieldWidth)<<"NotSet"<<ends;       
    }

    return os.str();
}

unsigned int CMOOSMsg::GetBinaryDataSize()
{
	if(!IsBinary())
		return 0;
	else
		return m_sVal.size();
}

bool CMOOSMsg::GetBinaryData(std::vector<unsigned char > &v)
{
	if(!IsBinary())
		return false;

	if(v.size()!=GetBinaryDataSize())
	{
	    v.resize(GetBinaryDataSize());
	}
	std::copy(m_sVal.begin(),m_sVal.end(),v.begin());
	return true;
}

std::vector<unsigned char >  CMOOSMsg::GetBinaryDataAsVector()
{
    std::vector<unsigned char > t;
    GetBinaryData(t);
    return t;
}


unsigned char * CMOOSMsg::GetBinaryData()
{
	if(!IsBinary())
		return NULL;
	else
		return (unsigned char*)(&m_sVal[0]);
}


bool  CMOOSMsg::IsName(const std::string & sName)
{
	return MOOSStrCmp(m_sKey, sName);
}


bool CMOOSMsg::IsDataType(char cDataType) const
{
    return m_cDataType == cDataType;
}
