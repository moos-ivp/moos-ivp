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
//   This file is part of a  MOOS Utility Component. 
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


// SimParams.cpp: implementation of the CSimParams class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include "SimParams.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimParams::CSimParams()
{
    //default of approximately 1m std...
    m_dfTOFStd = 0.00066;
    m_bImmediateAcousticLog = true;

    m_dfXYStd = 0.1;
    m_dfZStd  = 0.03;
    m_dfYawStd= MOOSDeg2Rad(0.5);
    m_dfXYVelStd = 0.02;

    m_bAddNoise = false;
    m_dfYawBias = 0;

}

CSimParams::~CSimParams()
{

}

bool CSimParams::Load(STRING_LIST &sParams)
{
    STRING_LIST::iterator p;

    for(p=sParams.begin();p!=sParams.end();p++)
    {
        string sLine,sTok,sVal;
        sLine=*p;

        if(GetTokenValPair(sLine,sTok,sVal))
        {
            if(MOOSStrCmp(sTok,"LogFile"))
            {
                m_sLogFileName = sVal;
            }
            else if(MOOSStrCmp(sTok,"AddNoise"))
            {
                m_bAddNoise = MOOSStrCmp(sVal,"TRUE");
            }
            else if(MOOSStrCmp(sTok,"InstantLogAcoustics"))
            {
                m_bImmediateAcousticLog = MOOSStrCmp(sVal,"TRUE");
            }
            else if(MOOSStrCmp(sTok,"MultiPathProbablity"))
            {
                m_dfProbMultiPath = atof(sVal.c_str());
            }
            else if(MOOSStrCmp(sTok,"TOFNoise"))
            {
                m_dfTOFStd = atof(sVal.c_str());
            }
            else if(MOOSStrCmp(sTok,"XYNOISE"))
            {
                m_dfXYStd = atof(sVal.c_str());
            }
            else if(MOOSStrCmp(sTok,"ZNOISE"))
            {
                m_dfZStd = atof(sVal.c_str());
            }
            else if(MOOSStrCmp(sTok,"YAWNOISE"))
            {
                m_dfYawStd = atof(sVal.c_str());
                m_dfYawStd = MOOSDeg2Rad(m_dfYawStd);
            }
            else if(MOOSStrCmp(sTok,"YAWBIAS"))
            {
                m_dfYawBias = atof(sVal.c_str());
            }
            else if(MOOSStrCmp(sTok,"XYVELNOISE"))
            {
                m_dfXYVelStd = atof(sVal.c_str());
            }

        
        }
    }

    return true;
}

bool CSimParams::GetTokenValPair(string sLine, string &sTok, string &sVal)
{
    if(sLine.find("=")!=string::npos)
    {
        MOOSRemoveChars(sLine," \t\r");
        sTok = MOOSChomp(sLine,"=");
        sVal = sLine;
        return true;
    }
    else
    {
        return false;
    } 
}

