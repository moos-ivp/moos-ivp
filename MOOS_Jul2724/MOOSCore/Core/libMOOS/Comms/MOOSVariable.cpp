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



// MOOSVariable.cpp: implementation of the CMOOSVariable class.
//
//////////////////////////////////////////////////////////////////////

#include "MOOS/libMOOS/Comms/MOOSVariable.h"
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

bool CMOOSVariable::Set(const CMOOSMsg &Msg)
{
    switch(Msg.m_cDataType)
    {
    case MOOS_DOUBLE:
        m_bDouble = true;
        m_dfVal = Msg.m_dfVal;
    
        break;
    case MOOS_STRING:
	case MOOS_BINARY_STRING:
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

std::string CMOOSVariable::GetSubscribeName() const
{
    return  m_sSubscribeName;
}

bool CMOOSVariable::IsFresh() const
{
    return m_bFresh;
}

std::string CMOOSVariable::GetName() const
{
    return m_sName;
}

std::string CMOOSVariable::GetPublishName() const
{
    return m_sPublishName;
}

bool CMOOSVariable::IsDouble() const
{
    return m_bDouble;
}

double CMOOSVariable::GetDoubleVal() const
{
    return m_dfVal;
}

double CMOOSVariable::GetTime() const
{
    return m_dfTimeWritten;
}

std::string CMOOSVariable::GetStringVal()  const
{
    return m_sVal;
}

std::string CMOOSVariable::GetAsString(int nFieldWidth) const
{
    ostringstream os;

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

    return sResult;
}

double CMOOSVariable::GetAge(double dfTimeNow) const
{
    return dfTimeNow-m_dfTimeWritten;
}

std::string CMOOSVariable::GetWriter() const
{
    return m_sSrc;
}


