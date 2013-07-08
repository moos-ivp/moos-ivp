/*
 *  Zipper.cpp
 *  MOOS
 *
 *  Created by pnewman on 21/08/2009.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Zipper.h"
#include <iostream>

#ifdef ZLIB_FOUND
#define ZIP_FLUSH_SIZE 2048
#include <zlib.h>
#endif


bool _ZipThreadWorker(void * pParam)
{
	CZipper* pMe = (CZipper*) pParam;
	return pMe->DoZipLogging();
}

bool CZipper::Start(const std::string sFileBaseName)
{
	m_sFileName = sFileBaseName;
	m_Thread.Initialise(_ZipThreadWorker, this);
	return m_Thread.Start();
}

bool CZipper::Stop()
{
	return m_Thread.Stop();
}

bool CZipper::IsRunning()
{
	return m_Thread.IsThreadRunning();
}

bool CZipper::Push(const std::string & sStr)
{
	if(sStr.size()!=0)
	{
		m_Lock.Lock();
		m_ZipBuffer.push_back(sStr);
		m_Lock.UnLock();
	}
	return true;
}


bool CZipper::DoZipLogging()
{
#ifdef ZLIB_FOUND
	
	std::string sZipFile = m_sFileName+".gz";
	gzFile TheZipFile =  gzopen(sZipFile.c_str(),"wb");
	
	
	
	int nTotalWritten = 0;
	int nSinceLastFlush = 0;
	while(!m_Thread.IsQuitRequested())
	{
		MOOSPause(1000);
		
		std::list<std::string > Work;
		Work.clear();
		
		m_Lock.Lock();
		{
			Work.splice(Work.begin(),m_ZipBuffer);
		}
		m_Lock.UnLock();
		
		
		std::list<std::string >::iterator q;
		
		for(q = Work.begin();q!=Work.end();q++)
		{
			int nWritten = gzwrite(TheZipFile, q->c_str(), q->size() );
			
			if(nWritten<=0)
			{	
				int Error;
				gzerror(TheZipFile, &Error);
				switch(Error)
				{
					case Z_ERRNO:
						std::cerr<<"Z_ERRNO\n";
						break;
					case Z_STREAM_ERROR	:
						std::cerr<<"Z_STREAM_ERROR\n";
						break;
					case Z_BUF_ERROR:
						std::cerr<<"Z_BUF_ERROR\n";
						break;
					case Z_MEM_ERROR:
						std::cerr<<"Z_MEM_ERROR\n";
						break;
				}
			}
			else
			{
				nTotalWritten+=nWritten;
				nSinceLastFlush+=nWritten;
				
				if(nSinceLastFlush>ZIP_FLUSH_SIZE)
				{	
					gzflush(TheZipFile, Z_SYNC_FLUSH);
					nSinceLastFlush = 0;
				}
				
			}
		}
		Work.clear();
		
		
		
		
	}
	gzflush(TheZipFile, Z_SYNC_FLUSH);
	gzflush(TheZipFile, Z_FINISH);
	gzclose(TheZipFile);
	MOOSTrace("closed compressed  file %s \n",sZipFile.c_str());
	
#endif
	return true;
	
}

