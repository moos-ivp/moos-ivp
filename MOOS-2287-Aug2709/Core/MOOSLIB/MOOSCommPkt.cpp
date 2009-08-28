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
// MOOSCommPkt.cpp: implementation of the MOOSCommPkt class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
    #pragma warning(disable : 4503)
#endif


#include <MOOSGenLib/MOOSGenLib.h>
#include "MOOSCommPkt.h"
#include "MOOSMsg.h"
#include "MOOSGlobalHelper.h"
#include <assert.h>
#include <cstring>
#include <iostream>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSCommPkt::CMOOSCommPkt()
{
    m_pStream = DefaultStream;
    m_nStreamSpace = MOOS_PKT_DEFAULT_SPACE;
    m_pNextData = m_pStream;
    m_bAllocated = false;
    m_nByteCount = 0;
    m_nMsgLen = 0;


}

CMOOSCommPkt::~CMOOSCommPkt()
{
    if(m_bAllocated)
    {
        delete [] m_pStream;
    }
}


int CMOOSCommPkt::GetBytesRequired()
{
    if(m_nByteCount==0)
    {
        return sizeof(int);
    }
    else
    {
        return m_nMsgLen-m_nByteCount;
    }
}

bool CMOOSCommPkt::Fill(unsigned char *InData, int nData)
{
    
    if(    m_nByteCount ==0)
    {
        //here we figure out how many bytes we are expecting
        assert(nData==sizeof(int));

        memcpy((void*)(&m_nMsgLen),(void*)InData,sizeof(int));

        //look to swap byte order if this machine is Big End in
        if(!IsLittleEndian())
        {
            m_nMsgLen = SwapByteOrder<int>(m_nMsgLen);
        }

    }

    if(m_nByteCount+nData>=m_nStreamSpace)
    {
        InflateTo(2*(m_nStreamSpace+nData));
    }
    memcpy(m_pNextData,InData,nData);
    m_pNextData+=nData;
    m_nByteCount+=nData;

    return true;
}


int CMOOSCommPkt::GetStreamLength()
{
    return m_nMsgLen;
}


/** This function stuffs messages in/from a packet */
bool CMOOSCommPkt::Serialize(MOOSMSG_LIST &List, bool bToStream, bool bNoNULL, double * pdfPktTime)
{
    if(bToStream)
    {

        m_nMsgLen=0;
        m_pNextData = m_pStream;
        m_nByteCount = 0;

        //we need to leave space at the start of the packet for total
        //length and number of include messages
        m_pNextData+=2*sizeof(int);
        m_nByteCount+= 2* sizeof(int);


        #define PKT_TMP_BUFFER_SIZE 40000
        unsigned char TmpBuffer[PKT_TMP_BUFFER_SIZE];
        unsigned char * pTmpBuffer = TmpBuffer;


        MOOSMSG_LIST::iterator p;
        int nCount = 0;
        for(p = List.begin();p!=List.end();p++,nCount++)
        {
        
            //MOOSTrace("Sending %s \n",p->m_sKey.c_str());
            int nTmpSize = PKT_TMP_BUFFER_SIZE;

            int nCopied = (*p).Serialize(pTmpBuffer,nTmpSize);

            if(nCopied!=-1)
            {
                //now copy to our stream...
                CopyToStream(pTmpBuffer,nCopied);
            }
            else
            {
                return false;
            }

        }

        //finally write how many bytes we have written at the start
        //look for need to swap byte order if required
        m_pNextData = m_pStream;
        int nBC = IsLittleEndian() ? m_nByteCount : SwapByteOrder<int>(m_nByteCount);
        memcpy((void*)m_pNextData,(void*)(&nBC),sizeof(m_nByteCount));
        m_pNextData+=sizeof(m_nByteCount);


        //and then how many messages are included
        //look for need to swap byte order if required
        int nMessages = List.size();
        nMessages = IsLittleEndian() ? nMessages : SwapByteOrder<int>(nMessages);
        memcpy((void*)m_pNextData,(void*)(&nMessages),sizeof(nMessages));
        m_pNextData+=sizeof(nMessages);


    }
    else
    {
        
        m_pNextData = m_pStream;
        m_nMsgLen = 0;
        m_nByteCount = 0;

        //first figure out the length of the message
        //look to swap byte order as required
        memcpy((void*)(&m_nMsgLen),(void*)m_pNextData,sizeof(m_nMsgLen));
        m_nMsgLen = IsLittleEndian() ? m_nMsgLen : SwapByteOrder<int>(m_nMsgLen);
        m_pNextData+=sizeof(m_nMsgLen);
        m_nByteCount+=sizeof(m_nMsgLen);

        int nSpaceFree=m_nMsgLen - sizeof(m_nMsgLen);

        //no figure out how many messages are packed in this packet
        //look to swap byte order as required
        int nMessages = 0;
        memcpy((void*)(&nMessages),(void*)m_pNextData,sizeof(nMessages));
        nMessages = IsLittleEndian() ? nMessages : SwapByteOrder<int>(nMessages);
        m_pNextData+=sizeof(nMessages);
        nSpaceFree-=sizeof(nMessages);
        m_nByteCount+=sizeof(nMessages);

        for(int i = 0; i<nMessages;i++)
        {
                    
            CMOOSMsg Msg;
            int nUsed = Msg.Serialize(m_pNextData,nSpaceFree,false);

            if(nUsed!=-1) 
            {
                //allows us to not store NULL messages
                bool bOmit = bNoNULL && (Msg.m_cMsgType==MOOS_NULL_MSG);

                if(Msg.m_cMsgType==MOOS_NULL_MSG && pdfPktTime!=NULL && i == 0 )
                {
                    *pdfPktTime     = Msg.GetDouble();                    
                }

                if(!bOmit)
                {
                    List.push_front(Msg);
                }


                m_pNextData+=nUsed;
                m_nByteCount+=nUsed;
                nSpaceFree-=nUsed;

            }
            else
            {
                //bad news...
                break;
            }        
        }
    }

    //here at the last moment we can fill in our totalm length for safe keeping
    m_nMsgLen = m_nByteCount;

    return true;
}

bool CMOOSCommPkt::CopyToStream(unsigned char *pData, int nBytes)
{
    //well do we have enough space to do this?
    if(m_nByteCount+nBytes>=m_nStreamSpace)
    {
        //no..better inflate ourselves...
        InflateTo(2*(m_nStreamSpace+nBytes));
    }

    //by this point we are guaranteed enough space..
    //unless (new has failed which is really the end of the world and we will be dead :-(  )

    //copy temporary buffer to our main buffer
    memcpy(m_pNextData,pData,nBytes);

    //increment hot spot
    m_pNextData+=nBytes;

    //increment byte count
    m_nByteCount+=nBytes;

    return true;
}

bool CMOOSCommPkt::InflateTo(int nNewStreamSize)
{
    //make more space then..
    m_nStreamSpace = nNewStreamSize;

    unsigned char * pNew = new unsigned char[m_nStreamSpace];

    if(pNew==NULL)
    {
        //absolute disaster..nothing we can do ...whole process must exit
        MOOSTrace("memory allocation failed in CMOOSCommPkt::CopyToStream");
        return false;
    }

    //copy what we have already assembled into new space
    memcpy(pNew,m_pStream,m_nByteCount);

    
    if(m_bAllocated)
    {
        delete [] m_pStream;
    }
    else
    {
        m_bAllocated = true;
    }

    //switch m_pStream to point to the new space;
    m_pStream = pNew;

    //and set the next data point to be m_nMsgLen bytes from start
    //so we will start writin after are existing stuff
    m_pNextData=m_pStream+m_nByteCount;

    return true;
}
