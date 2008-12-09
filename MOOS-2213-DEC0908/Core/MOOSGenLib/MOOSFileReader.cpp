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
// MOOSFileReader.cpp: implementation of the CMOOSFileReader class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#ifdef _WIN32
#include <winsock2.h>
#include "windows.h"
#include "winbase.h"
#include "winnt.h"
#else
#include <pthread.h>
#endif


#include "MOOSGenLibGlobalHelper.h"
#include "MOOSFileReader.h"
#include "assert.h"
#include "MOOSLock.h"

#define MAXLINESIZE 2000
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSFileReader::CMOOSFileReader()
{
    m_pLock = new CMOOSLock();
}

CMOOSFileReader::~CMOOSFileReader()
{
    if(GetFile()->is_open())
    {
        GetFile()->close();
    }

	delete m_pLock;
	m_pLock = NULL;

	ClearFileMap();
}

bool CMOOSFileReader::SetFile(const std::string  & sFile)
{
    m_sFileName = sFile;

    THREAD2FILE_MAP::iterator p;

    for(p=m_FileMap.begin();p!=m_FileMap.end();p++)
    {
        std::ifstream * pFile = p->second;

        if(pFile!=NULL)
        {
            if(pFile->is_open())
            {
                pFile->close();
            }
            delete pFile;
            p->second = NULL;
        }
    }
    
	ClearFileMap();
    
    BuildLocalShellVars();

    return true;

}


std::string CMOOSFileReader::GetNextValidLine(bool bDoSubstitution)
{


    std::string sLine;

    if(GetFile()->eof())
    {
        return sLine;
    }

    char Tmp[MAXLINESIZE];

    //skip white space
    (*GetFile())>>std::ws;


    GetFile()->getline(Tmp,sizeof(Tmp));
    sLine = std::string(Tmp);

    while(GetFile()&& sLine.length()!=0 && IsComment(sLine))
    {
        (*GetFile())>>std::ws;
        GetFile()->getline(Tmp,sizeof(Tmp));
        sLine = std::string(Tmp);
    }

    // jckerken 8-12-2004 (MIT)
    // remove comments made in line not at beginning
    int nC = (int)sLine.find("//");

    if (nC >= 0 && nC <= (int)sLine.size()) {
        if (nC > 0)
            sLine = sLine.substr(0, nC);
        else
            sLine = "";
    } // end jckerken

    
    if(bDoSubstitution)
        DoVariableExpansion(sLine);
    
    
    
    return sLine;
}

bool CMOOSFileReader::IsComment(std::string &sLine)
{
    MOOSTrimWhiteSpace(sLine);
    if(sLine.find("//")==0)
    {
        return true;
    }

    return false;
}

bool CMOOSFileReader::GetTokenValPair(std::string sLine, std::string &sTok, std::string &sVal,bool bPreserveWhiteSpace)
{
    if(sLine.find("=")!=std::string::npos)
    {
        MOOSRemoveChars(sLine,"\r");

        if(!bPreserveWhiteSpace)
        {
            MOOSRemoveChars(sLine," \t");
        }

        sTok = MOOSChomp(sLine,"=");
        sVal = sLine;
        return true;
    }
    else
    {
        return false;
    } 
}

bool CMOOSFileReader::GetValue(std::string sName,double  & dfResult)
{
    std::string sTmp;

    if(GetValue(sName,sTmp))
    {
        dfResult = atof(sTmp.c_str());
        return true;
    }

    return false;
}


bool CMOOSFileReader::GetValue(std::string sName,int  & nResult)
{
    std::string sTmp;

    if(GetValue(sName,sTmp))
    {
        nResult = atoi(sTmp.c_str());
        return true;
    }

    return false;
}


bool CMOOSFileReader::GetValue(std::string sName,std::string & sResult)
{

    
    if(GetFile()->is_open())
    {
        Reset();
     
        GetFile()->seekg(std::ios::beg);

        std::string sLine,sVal,sTok;
        while(!GetFile()->eof())
        {
            sLine = GetNextValidLine();

            if(GetTokenValPair(sLine, sTok, sVal))
            {
                if(MOOSStrCmp(sTok,sName))
                {
                    sResult = sVal;
                    return true;
                }
            }       
        }
    }
 
    return false;
}



bool CMOOSFileReader::GetValue(std::string  sName,float & fResult)
{
    double dfT;
    if(GetValue(sName,dfT))
    {
        fResult = float(dfT);
        return true;
    }
    return false;

}
bool CMOOSFileReader::GetValue(std::string sName,bool  & bResult)
{
    std::string sT;

    if(GetValue(sName, sT))
    {
        bResult = (MOOSStrCmp(sT, "TRUE") ||
            (MOOSIsNumeric(sT) && atof(sT.c_str()) > 0));

        return true;
    }
    return false;

}
bool CMOOSFileReader::GetValue(std::string sName,unsigned int  & nResult)
{
    int nT;
    if(GetValue(sName,nT))
    {
        if(nT<0)
            nResult = 0;
        else
            nResult = (unsigned int)nT;

        return true;
    }
    return false;
}


bool CMOOSFileReader::Reset()
{
    bool bResult = true;
//    m_pLock->Lock();

    if(GetFile()->is_open())
    {
        if(GetFile()->eof())
        {
            GetFile()->clear();
            GetFile()->seekg(ios::beg);
            
        }
        else
        {
            std::ifstream * pMyFile = GetFile();
            pMyFile->seekg(ios::beg);
        }
        bResult = true;
    }
    else
    {
        bResult = false;
    }

//    m_pLock->UnLock();

    return bResult;

}

bool CMOOSFileReader::eof()
{
    return GetFile()->eof();
}

bool CMOOSFileReader::GoTo(std::string sLine)
{
    if(GetFile()->is_open())
    {
        MOOSRemoveChars(sLine," \t\r");

        std::string sTmp;
        while(!GetFile()->eof())
        {
            sTmp = GetNextValidLine();
            MOOSRemoveChars(sTmp," \t\r");

            if(MOOSStrCmp(sTmp,sLine))
            {               
                return true;
            }

        }
    }     
    else
    {
        //MOOSTrace("CMOOSFileReader::GoTo() file not open!\n");
    }

    return false;
}

std::ifstream * CMOOSFileReader::GetFile()
{
    m_pLock->Lock();
#ifdef _WIN32
    DWORD Me = GetCurrentThreadId(); 
#else
    pthread_t Me =  pthread_self();
#endif

    THREAD2FILE_MAP::iterator p = m_FileMap.find(Me);

    if(p==m_FileMap.end())
    {
        //MOOSTrace("\n::CMOOSFileReader Making mission file for thread[%d]\n",Me);
        std::ifstream * pNewStream = new ifstream;
        m_FileMap[Me] = pNewStream;
        p = m_FileMap.find(Me);
    }

    std::ifstream * pAnswer = p->second;

    if(pAnswer!=NULL)
    {
        //now open the file as required...
        if(!pAnswer->is_open() && !m_sFileName.empty())
        {
            pAnswer->open(m_sFileName.c_str());
        }
    }

    m_pLock->UnLock();

    return pAnswer;
   

}

bool CMOOSFileReader::IsOpen()
{
    if(GetFile()==NULL)
        return false;

    return GetFile()->is_open();
}



bool CMOOSFileReader::BuildLocalShellVars()
{
    
    if(GetFile()->is_open())
    {
        Reset();
        
        GetFile()->seekg(std::ios::beg);
        
        std::string sLine,sVal,sTok;
        while(!GetFile()->eof())
        {
            sLine = GetNextValidLine(false);

            MOOSChomp(sLine,"define:");

            if(!sLine.empty())
            {
                std::string sVarName,sVarVal;
                if(GetTokenValPair(sLine,sVarName,sVarVal))
                {
                    m_LocalShellVariables[sVarName] = sVarVal;    
                }
            }
        }       
        
        Reset();
        
        
        return true;
    }
    
    return false;
    
}


bool CMOOSFileReader::DoVariableExpansion(std::string & sVal)
{
    
    std::string sBuilt,sExpand;
    do
    {
        sExpand.clear();
        
        sBuilt+= MOOSChomp(sVal,"${");
        
        sExpand = MOOSChomp(sVal,"}");
        
        if(!sExpand.empty())
        {
            std::map<std::string,std::string>::iterator p;
            p = m_LocalShellVariables.find(sExpand);
            if(p == m_LocalShellVariables.end())
            {
                //maybe its a system shell var?
                char * pShellVal = getenv(sExpand.c_str());
                if(pShellVal!=NULL)
                {
                    //MOOSTrace("using system variable expansion ${%s} -> %s\n",sExpand.c_str(),pShellVal);

                    //indeed it is!
                    sExpand = std::string(pShellVal);
                    
                }
                else
                {
                    MOOSTrace("Error in mission file\n\t no shell or mission file expansion fouind for ${%s}\n",sExpand.c_str());
                }
            }
            else
            {
                //OK we have been told about this in file scope
                //MOOSTrace("using local variable expansion ${%s} ->  %s\n",sExpand.c_str(),p->second.c_str());

                sExpand=p->second;
            }
            
            sBuilt+=sExpand;
            //MOOSTrace("and sBuilt = %s\n",sBuilt.c_str());
        }
        
    }while(!sExpand.empty());
    
    
    sVal = sBuilt;
    
    
    return true;
    
    
    
    
}


