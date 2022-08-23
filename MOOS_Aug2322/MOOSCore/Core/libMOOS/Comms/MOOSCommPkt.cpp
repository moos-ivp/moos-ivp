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


// MOOSCommPkt.cpp: implementation of the MOOSCommPkt class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Comms/MOOSCommPkt.h"

#include <iostream>
#include <cstring>


using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSCommPkt::CMOOSCommPkt() {

    m_nStreamSpace  = sizeof(int);
    m_pStream = new unsigned char [m_nStreamSpace];
    m_pNextData = m_pStream;
    m_nByteCount = 0;
    m_nMsgLen = 0;
    m_nMsgsSerialised = 0;

}

CMOOSCommPkt::~CMOOSCommPkt()
{
    delete [] m_pStream;
}



bool CMOOSCommPkt::InflateTo(int nNewStreamSize) {
    //maybe there is nothing to do....
    if (nNewStreamSize <= m_nStreamSpace) {
        return true;
    }else{

        unsigned char * t = new unsigned char [nNewStreamSize];
        memcpy(t, m_pStream,m_nByteCount);
        delete [] m_pStream;
        m_pStream=t;
        m_nStreamSpace = nNewStreamSize;
        m_pNextData = m_pStream + m_nByteCount;

    }
    return true;
}


bool CMOOSCommPkt::OnBytesWritten(unsigned char * /*PositionWrittento*/,int nData)
{
    //std::cerr<<__PRETTY_FUNCTION__<<" "<<nData<<"\n";

    m_nByteCount += nData;
    m_pNextData += nData;

    if (m_nByteCount <= (int) sizeof(int)){
        if (m_nByteCount == sizeof(int)) {
            memcpy((void*) (&m_nMsgLen), (void*) m_pStream, sizeof(int));

            //look to swap byte order if this machine is Big End in
            if (!IsLittleEndian()) {
                m_nMsgLen = SwapByteOrder<int> (m_nMsgLen);
            }

            if(!InflateTo(m_nMsgLen))
                return false;
        }
    }

    return true;

}



int CMOOSCommPkt::GetBytesRequired() {
    if (m_nByteCount < (int) sizeof(int)) {
        return sizeof(int) - m_nByteCount;
    } else {
        return m_nMsgLen - m_nByteCount;
    }
}

int CMOOSCommPkt::GetStreamLength() {
    return m_nByteCount;
}


unsigned char * CMOOSCommPkt::Stream(){
    return m_pStream;
}

unsigned char * CMOOSCommPkt::NextWrite(){
    return m_pNextData;
}


int CMOOSCommPkt::GetNumMessagesSerialised()
{
    return m_nMsgsSerialised;
}


/** This function stuffs messages in/from a packet */
bool CMOOSCommPkt::Serialize(MOOSMSG_LIST &List,
                             bool bToStream,
                             bool bNoNULL,
                             double * pdfPktTime) {
    //note +1 is for indicator regarding compressed or not compressed
    unsigned int nHeaderSize = 2 * sizeof(int) + 1;

    if (bToStream) {

        m_nMsgLen = 0;
        m_nByteCount = 0;
        m_nMsgsSerialised = 0;

        //lets figure out how much space we need?
        unsigned int nBufferSize = nHeaderSize; //some head room
        MOOSMSG_LIST::iterator p;
        for (p = List.begin(); p != List.end(); ++p) {
            nBufferSize += p->GetSizeInBytesWhenSerialised();
        }

        InflateTo(nBufferSize);

        m_pNextData = m_pStream + nHeaderSize;
        m_nByteCount += nHeaderSize;

        for (p = List.begin(); p != List.end(); ++p)
        {

            m_nMsgsSerialised++;

            int nCopied = p->Serialize(m_pNextData, nBufferSize - m_nByteCount);

            if (nCopied == -1) {
                std::cerr << "big problem failed serialisation: "
                        << "CMOOSCommPkt::Serialize()" << "\n";  // Was: __PRETTY_FUNCTION__ which only exists in GCC

                return false;
            }

            m_pNextData += nCopied;
            m_nByteCount += nCopied;

        }

        unsigned char bCompressed = 0;

        //finally write how many bytes we have written at the start
        //look for need to swap byte order if required
        m_pNextData = m_pStream;
        int nBC = IsLittleEndian()
                                   ? m_nByteCount
                                   : SwapByteOrder<int> (m_nByteCount);

        memcpy((void*) m_pNextData, (void*) (&nBC), sizeof(m_nByteCount));
        m_pNextData += sizeof(m_nByteCount);

        //and then how many messages are included
        //look for need to swap byte order if required
        int nMessages = List.size();
        nMessages = IsLittleEndian()
                                     ? nMessages
                                     : SwapByteOrder<int> (nMessages);
        memcpy((void*) m_pNextData, (void*) (&nMessages), sizeof(nMessages));
        m_pNextData += sizeof(nMessages);

        //and is this a compressed message or not?
        *m_pNextData = bCompressed;
        m_pNextData += 1;

    } else {

        m_pNextData = m_pStream;
        m_nMsgLen = 0;
        m_nByteCount = 0;

        //first figure out the length of the message
        //look to swap byte order as required
        memcpy((void*) (&m_nMsgLen), (void*) m_pNextData, sizeof(m_nMsgLen));
        m_nMsgLen = IsLittleEndian()
                                     ? m_nMsgLen
                                     : SwapByteOrder<int> (m_nMsgLen);
        m_pNextData += sizeof(m_nMsgLen);
        m_nByteCount += sizeof(m_nMsgLen);

        int nSpaceFree = m_nMsgLen - sizeof(m_nMsgLen);

        //now figure out how many messages are packed in this packet
        //look to swap byte order as required
        int nMessages = 0;
        memcpy((void*) (&nMessages), (void*) m_pNextData, sizeof(nMessages));
        nMessages = IsLittleEndian()
                                     ? nMessages
                                     : SwapByteOrder<int> (nMessages);
        m_pNextData += sizeof(nMessages);
        nSpaceFree -= sizeof(nMessages);
        m_nByteCount += sizeof(nMessages);

        //now account for one byet of compression indication
        m_pNextData += sizeof(unsigned char);
        nSpaceFree -= sizeof(unsigned char);
        m_nByteCount += sizeof(unsigned char);

        for (int i = 0; i < nMessages; i++) {

            CMOOSMsg Msg;
            int nUsed = Msg.Serialize(m_pNextData, nSpaceFree, false);

            if (nUsed != -1) {
                //allows us to not store NULL messages
                bool bOmit = bNoNULL && (Msg.m_cMsgType == MOOS_NULL_MSG);

                if (Msg.m_cMsgType == MOOS_NULL_MSG && pdfPktTime != NULL && i
                        == 0) {
                    *pdfPktTime = Msg.GetDouble();
                }

                if (!bOmit) {
                    List.push_back(Msg);
                }

                m_pNextData += nUsed;
                m_nByteCount += nUsed;
                nSpaceFree -= nUsed;

            } else {
                //bad news...
                break;
            }
        }
    }

    //here at the last moment we can fill in our totalm length for safe keeping
    m_nMsgLen = m_nByteCount;

    return true;
}

