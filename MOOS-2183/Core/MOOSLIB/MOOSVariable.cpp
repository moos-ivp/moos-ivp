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
// MOOSVariable.cpp: implementation of the CMOOSVariable class.
//
//////////////////////////////////////////////////////////////////////

#include "MOOSVariable.h"
//#include <sstream>
#include <sstream>
#include <iomanip>

using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSVariable::CMOOSVariable()
{
    m_bDouble = false;//true;
    m_bFresh = false;
    m_dfCommsTime = DEFAULT_MOOS_VAR_COMMS_TIME;
    m_dfTimeWritten = -1;
    m_dfVal = 0;
  
}

CMOOSVariable::CMOOSVariable(std::string sName,std::string sSubscribe, std::string  sPublish,double dfCommsTime)
{
    m_bDouble = false;//true;
    m_bFresh = false;
    m_sName = sName;
    m_sSubscribeName =  sSubscribe;
    m_sPublishName = sPublish;
    m_dfCommsTime = dfCommsTime;
    m_dfTimeWritten = -1;
    m_dfVal = 0;
  
}

CMOOSVariable::~CMOOSVariable()
{

}
bool CMOOSVariable::Set(double dfVal,double dfTime)
{
    m_dfVal = dfVal;
    m_dfTimeWritten = dfTime;
    m_bDouble = true;
    
    SetFresh(true);

    return true;
}

bool CMOOSVariable::Set(const std::string & sVal,double dfTime)
{
    m_sVal = sVal;
    m_dfTimeWritten = dfTime;
    m_bDouble = false;
    SetFresh(true);

    return true;

}

bool CMOOSVariable::Set(CMOOSMsg &Msg)
{
    switch(Msg.m_cDataType)
    {
    case MOOS_DOUBLE:
        m_bDouble = true;
        m_dfVal = Msg.m_dfVal;
    
        break;
    case MOOS_STRING:
        m_bDouble = false;
        m_sVal = Msg.m_sVal;
        break;
    }

    m_dfTimeWritten = Msg.m_dfTime;

    m_sName = Msg.m_sKey;

    m_sSrc = Msg.m_sSrc;

    m_bFresh = true;

    return true;

}

bool CMOOSVariable::SetFresh(bool bFresh)
{
    m_bFresh = bFresh;

    return true;
}

std::string CMOOSVariable::GetSubscribeName()
{
    return  m_sSubscribeName;
}

bool CMOOSVariable::IsFresh()
{
    return m_bFresh;
}

std::string CMOOSVariable::GetName()
{
    return m_sName;
}

std::string CMOOSVariable::GetPublishName()
{
    return m_sPublishName;
}

bool CMOOSVariable::IsDouble()
{
    return m_bDouble;
}

double CMOOSVariable::GetDoubleVal()
{
    return m_dfVal;
}

double CMOOSVariable::GetTime()
{
    return m_dfTimeWritten;
}

std::string CMOOSVariable::GetStringVal()
{
    return m_sVal;
}

std::string CMOOSVariable::GetAsString(int nFieldWidth)
{
    ostringstream os;
    //ostrstream os;

    os.setf(ios::left);

    if(GetTime()!=-1)
    {        
        if(IsDouble())
        {
            os<<setw(nFieldWidth)<<m_dfVal<<ends;       
        }
        else
        {
            os<<m_sVal.c_str()<<ends;
        }

    }
    else
    {
        os<<setw(nFieldWidth)<<"NaN"<<ends;       
    }

    std::string sResult = os.str();

    //os.rdbuf()->freeze(0);

    return sResult;
}

double CMOOSVariable::GetAge(double dfTimeNow)
{
    return dfTimeNow-m_dfTimeWritten;
}

std::string CMOOSVariable::GetWriter()
{
    return m_sSrc;
}
