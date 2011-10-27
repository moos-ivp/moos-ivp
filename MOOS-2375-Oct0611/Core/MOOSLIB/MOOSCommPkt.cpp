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
#include "MOOSException.h"


#define DEFAULT_ASSUMMED_MAX_MOOS_MSG_SIZE 40000


#ifdef COMPRESSED_MOOS_PROTOCOL
	
	#define MAX_BELIEVABLE_ZLIB_COMPRESSION 256  //if we look like we are getting a compression ration of more than this then bail - there is an error.

	#define COMPRESSION_PACKET_SIZE_THRESHOLD 1024 //packets below this size won't be compressed.

	#include <zlib.h>
#endif

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
	m_dfCompression = 1.0;


}

CMOOSCommPkt::~CMOOSCommPkt()
{
    if(m_bAllocated)
    {
        delete [] m_pStream;
    }
}

double CMOOSCommPkt::GetCompression()
{
	return m_dfCompression;
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
	//note +1 is for indicator regarding compressed or not compressed
	unsigned int nHeaderSize = 2*sizeof(int)+1;
	
    if(bToStream)
    {

        m_nMsgLen=0;
        m_pNextData = m_pStream;
        m_nByteCount = 0;

        //we need to leave space at the start of the packet for total
        //length and number of include messages
		
        m_pNextData+=nHeaderSize; 
        m_nByteCount+= nHeaderSize;


        //assume to start with that mesages are reasonably sized -if they aren't we'll make an adjustment
        unsigned int nWorkingMemoryCurrentSize = DEFAULT_ASSUMMED_MAX_MOOS_MSG_SIZE;
        unsigned char * pTmpBuffer =new unsigned char[nWorkingMemoryCurrentSize] ;



        MOOSMSG_LIST::iterator p;
        int nCount = 0;
        for(p = List.begin();p!=List.end();p++,nCount++)
        {
        
            unsigned int nRequiredSize = p->GetSizeInBytesWhenSerialised();

            if(nRequiredSize>nWorkingMemoryCurrentSize)
            {
                //std::cerr<<"making more space "<<nWorkingMemoryCurrentSize<<" -> "<<nRequiredSize<<std::endl;
                nWorkingMemoryCurrentSize = nRequiredSize;
                delete [] pTmpBuffer;

                pTmpBuffer = new unsigned  char [nWorkingMemoryCurrentSize];
            }

            //MOOSTrace("Sending %s \n",p->m_sKey.c_str());
            //int nTmpSize = MAX_MOOS_MSG_SIZE;

            int nCopied = (*p).Serialize(pTmpBuffer,nWorkingMemoryCurrentSize);

            if(nCopied !=nRequiredSize )
            {
                std::cerr<<"bad news expected "<<nWorkingMemoryCurrentSize<<" but serialisation took "<<nCopied<<std::endl;
                p->Trace();
            }

            if(nCopied!=-1)
            {
                //now copy to our stream...
                CopyToStream(pTmpBuffer,nCopied);
            }
            else
            {
                delete [] pTmpBuffer;
                return false;
            }

        }

        delete [] pTmpBuffer;





		unsigned char bCompressed = 0;
#ifdef COMPRESSED_MOOS_PROTOCOL
		
		//we only compress if it is worth it
		if(m_nByteCount>COMPRESSION_PACKET_SIZE_THRESHOLD)
		{
			
			unsigned long int nDataLength = m_nByteCount-nHeaderSize;
			
			//we require .1% extra and 12 bytes
			unsigned long int nZBSize = (unsigned int) ((float)nDataLength*1.01+13);
			unsigned char * ZipBuffer = new unsigned char[nZBSize];		
			
			int nZipResult = compress(ZipBuffer,&nZBSize,m_pStream+nHeaderSize,nDataLength);
			
			
			switch (nZipResult) {
				case Z_OK:
					break;
				case Z_BUF_ERROR:
					throw CMOOSException("failed to compress outgoing data - looks like compression actually expanded data!");
					break;
				case Z_MEM_ERROR:
					throw CMOOSException("zlib memory error!");
					break;
				default:
					break;
			}
			
			//MOOSTrace("Compressed out going buffer is %d bytes and original is %d\n",nZBSize,nDataLength);
			
			memcpy(m_pStream+nHeaderSize,ZipBuffer,nZBSize);

			//maybe useful to record what compression ration we are getting
			m_dfCompression = (double)m_nByteCount/nZBSize;

			//now we have a new byte count 			
			m_nByteCount = nZBSize+nHeaderSize;
			
			delete ZipBuffer;
			
			bCompressed = 1;
			
		}
		
#endif

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
		
		//and is this a compressed message or not?
		*m_pNextData = bCompressed;
		m_pNextData+=1;


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

        //now figure out how many messages are packed in this packet
        //look to swap byte order as required
        int nMessages = 0;
        memcpy((void*)(&nMessages),(void*)m_pNextData,sizeof(nMessages));
        nMessages = IsLittleEndian() ? nMessages : SwapByteOrder<int>(nMessages);
        m_pNextData+=sizeof(nMessages);
        nSpaceFree-=sizeof(nMessages);
        m_nByteCount+=sizeof(nMessages);
		
		//now account for one byet of compression indication
		m_pNextData+=sizeof(unsigned char);
        nSpaceFree-=sizeof(unsigned char);
        m_nByteCount+=sizeof(unsigned char);
		
		
		
#ifdef COMPRESSED_MOOS_PROTOCOL		
		unsigned char bCompressed = m_pStream[nHeaderSize-1];
		//we will only have compressed if it is worth it
		
		//MOOSTrace("Message is %s compressed\n",bCompressed? "":"not");
		if(bCompressed)
		{
			
			unsigned long int nDataLength = m_nMsgLen-nHeaderSize;
			
			//we'd be surprised if we had more tha 90% compression
			int nCompressionFactor = 10;
			unsigned char * ZipBuffer = NULL;
			unsigned long int nZBSize;
			
			int nZipResult = -1;
			
			do
			{
				
				nZBSize = nCompressionFactor*nDataLength;
				
				ZipBuffer = new unsigned char[nZBSize];
				
				nZipResult = uncompress(ZipBuffer,&nZBSize,m_pStream+nHeaderSize,nDataLength);
				
				switch (nZipResult) 
				{
					case Z_MEM_ERROR:
						throw CMOOSException("ZLIB out of memory");
						break;
						
					case Z_BUF_ERROR:
					{
						//if we get here we need more space
						delete ZipBuffer;
						nCompressionFactor *=8;
						
						if(nCompressionFactor>MAX_BELIEVABLE_ZLIB_COMPRESSION)
						{
							//this is very suspcious
							throw CMOOSException("error in decompressing CMOOSPkt stream");
						}
					}
						break;
						
					case Z_DATA_ERROR:
						throw CMOOSException("ZLIB received corrupted data - this is really bad news.");
						break;
						
						
					default:
						break;
				}
			}while(nZipResult!=Z_OK);
			
			
			//MOOSTrace("received %d data bytes and uncompressed them to %d  bytes\n",nDataLength,nZBSize);
			
			//maybe useful to record what compression ration we are getting
			m_dfCompression = (double)nZBSize/nDataLength;

			
			//we have a new message length
			m_nMsgLen = nZBSize+nHeaderSize;
			
			//check we have allocated enough memory
			if(m_nMsgLen>m_nStreamSpace)
			{
				//interesting case we are out of memory - we need some more
				InflateTo(m_nMsgLen);
			}
			
			//copy the uncompressed data into our stream
			memcpy(m_pStream+nHeaderSize,ZipBuffer,nZBSize);
			
			//recalculate space free
			nSpaceFree= m_nMsgLen-nHeaderSize;
			
			//be a good citizen
			delete ZipBuffer;
		}
#endif
		

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
