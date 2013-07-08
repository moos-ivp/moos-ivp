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
// MOOSFileReader.h: interface for the CMOOSFileReader class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
    #pragma warning(disable : 4503)
#endif

#if !defined(AFX_MOOSFILEREADER_H__B355D791_3CC0_4612_B755_020A269204F2__INCLUDED_)
#define AFX_MOOSFILEREADER_H__B355D791_3CC0_4612_B755_020A269204F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <fstream>
#include <string>
#include <map>

#ifdef _WIN32
    typedef std::map<int,std::ifstream*> THREAD2FILE_MAP;
#else
    typedef std::map<pthread_t,std::ifstream *> THREAD2FILE_MAP;
#endif
        


class CMOOSLock;

//! Base class for reading ascii files
class CMOOSFileReader  
{
public:
    CMOOSFileReader();
    virtual ~CMOOSFileReader();

    /*  true if file is open */   
    bool IsOpen();

    /* Goto a given line*/
    bool GoTo(std::string sLine);

    /* returns true if file reader is eof*/
    bool eof();

    /* reset everything */
    bool Reset();

    /** looks for a line "sName = Val" in whole file, fills in result with Val */
    bool GetValue(std::string  sName,std::string & sResult);
    /** looks for a line "sName = Val" in whole file, fills in result with Val */
    bool GetValue(std::string sName,double  & dfResult);
    /** looks for a line "sName = Val" in whole file, fills in result with Val */
    bool GetValue(std::string sName,int  & nResult);
    /** looks for a line "sName = Val" in whole file, fills in result with Val */
    bool GetValue(std::string  sName,float & fResult);
    /** looks for a line "sName = Val" in whole file, fills in result with Val */
    bool GetValue(std::string sName,bool  & bResult);
    /** looks for a line "sName = Val" in whole file, fills in result with Val */
    bool GetValue(std::string sName,unsigned int  & nResult);



    /** tell the class what file to read */
    bool SetFile(const std::string  & sFile);

    /**
     * return the name of the underlying file being parsed
     */
    std::string GetFileName(){return m_sFileName;};

    /**static helper which splits a line into token = value and by deafult removes white space*/
    static bool    GetTokenValPair(std::string  sLine, std::string &sTok, std::string & sVal,bool bPreserveWhiteSpace = false);

    /** returns a string of the next non comment line (and removs trailing comments)*/
    std::string  GetNextValidLine(bool bDoShellSubstitution = true);

	    
    bool DoVariableExpansion(std::string & sVal);
    bool BuildLocalShellVars();
    bool MakeOverloadedCopy(const std::string & sCopyName,std::map<std::string, std::string> & OverLoads);
	void EnableVerbatimQuoting(bool bEnable=true){m_bEnableVerbatimQuoting = bEnable;};


protected:
	/** iterates through filemap freeing up resources then calls filemap's clear method */
	void ClearFileMap()
	{
		for(THREAD2FILE_MAP::iterator iter = m_FileMap.begin(); iter != m_FileMap.end(); iter++)
		{
            std::ifstream * pFile = iter->second;
            if(pFile)
				delete pFile;
		}
		m_FileMap.clear();
	}
	
	
    std::ifstream * GetFile();
    CMOOSLock *m_pLock;
    static bool    IsComment(std::string & sLine);
    std::string    m_sFileName;
    std::ifstream m_File;

    std::map<std::string,std::string> m_LocalShellVariables;
    /** every thread get its own pointer to a stream*/
    THREAD2FILE_MAP m_FileMap;

    //true if quoted strings are treated as literals - quotes will be removed and comment "//"
	//characters not treated as comments
	bool m_bEnableVerbatimQuoting;

};

#endif // !defined(AFX_MOOSFILEREADER_H__B355D791_3CC0_4612_B755_020A269204F2__INCLUDED_)
