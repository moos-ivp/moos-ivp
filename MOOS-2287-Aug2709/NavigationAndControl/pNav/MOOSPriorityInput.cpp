///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman and others
//   at MIT 2001-2002 and Oxford University 2003-2005.
//   email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS Basic (Common) Application. 
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


#include <MOOSLIB/MOOSLib.h>
#include <iostream>
#include "MOOSPriorityInput.h"

#define DEFAULT_PRIORITY_TIMEOUT 3.0

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMOOSPriorityInput::CMOOSPriorityInput()
{
    m_dfLastValue = 0;
    m_dfLastTimeSet = -1;
}

CMOOSPriorityInput::~CMOOSPriorityInput()
{

}

bool CMOOSPriorityInput::Initialise(string sName,string sSources, string sStem,STRING_LIST & SubscribeTo)
{


    //what is our name?
    m_sName = sName;

    SubscribeTo.clear();

    //what named data can we be set from...
    while(!sSources.empty())
    {
        string sInput = MOOSChomp(sSources,",");
        string sWho = MOOSChomp(sInput,"@");

        //default time out
        double dfTimeOut = DEFAULT_PRIORITY_TIMEOUT;

        if(!sInput.empty())
        {
            //specified time out
            dfTimeOut = atof(sInput.c_str());
            if(dfTimeOut==0)
            {
                dfTimeOut = DEFAULT_PRIORITY_TIMEOUT;
            }
        }

        //give the stem and the header whatr is the variable name ?
        // eg if we are given stem = "X_E" and who = GPS then we are
        //expected to be updated by GPS_X_E        
        string sMOOSVariable = sWho+"_"+sStem;

        //alwaye upper case
        MOOSToUpper(sMOOSVariable);

        //we shall want to subscribe to this...
        SubscribeTo.push_front(sMOOSVariable);


        //OK so save this input source in our priority stack
        CPrioritySource PSrc;
        PSrc.Initialise(sMOOSVariable,dfTimeOut);

        //store this input..
        m_Sources.push_back(PSrc);
    }

    //a little bit of debug info to show what is going on..
    MOOSTrace("Variable \"%s\" has inputs from (decrasing order):\n",m_sName.c_str());
    
    PRIORITY_SOURCE_VECTOR::iterator p;

    for(p = m_Sources.begin(); p!=m_Sources.end();p++)
    {
        MOOSTrace("%s\n",p->GetSource().c_str());
    }
    MOOSTrace("\n");

    //current source is top most in list (higher priority)
    m_nCurrentSourceNdx =0;


    return true;
}


bool CMOOSPriorityInput::SetInput(CMOOSMsg &InMsg,double dfTimeNow)
{

    unsigned int i;
    for(i = 0; i<m_Sources.size();i++)
    {
        //does this Msg come the input source monitored by m_Sources[i]?
        if(m_Sources[i].GetSource() == InMsg.m_sKey)
        {
            if(InMsg.IsDataType(MOOS_DOUBLE))
            	m_Sources[i].Set(InMsg.m_dfVal,InMsg.m_dfTime);
            else
               m_Sources[i].Set(InMsg.GetString(),InMsg.GetTime());   
        }
    }

    return true;
}

bool CMOOSPriorityInput::GetOutput(CMOOSMsg &OutMsg,double dfTimeNow)
{

    for(unsigned int i = 0; i<m_Sources.size();i++)
    {
        if(m_Sources[i].HasExpired(dfTimeNow) ==false)
        {
            //upgrade path

            if(static_cast<int>(i)<m_nCurrentSourceNdx)
            {
                MOOSTrace("Variable %s upgraded now deriving from %s\n",
                        m_sName.c_str(),
                        m_Sources[i].GetSource().c_str());
            }

            SetActiveSource(i);


            if(m_Sources[i].IsFresh())
            {

                //rename sKey to our name
                //eg DEPTH_DEPTH -> NAV_DEPTH
                OutMsg.m_sKey = m_sName;

                //mapped names come from us.."pNav"
                OutMsg.m_sSrc = "pNav";


                //fill in a message
                OutMsg.m_cDataType = MOOS_DOUBLE;
                OutMsg.m_cMsgType = MOOS_NOTIFY;


                //fill in data...
                double dfVal,dfTime;
               std::string sVal;
                m_Sources[i].Get(dfVal,dfTime);
                m_Sources[i].Get(sVal,dfTime);

                OutMsg.m_dfVal = dfVal;
                OutMsg.m_sVal = sVal;
                OutMsg.m_cDataType = m_Sources[i].GetDataType(); 
               OutMsg.m_dfTime = dfTime;

               
                //remeber our last value
                m_dfLastValue = dfVal;

                //and the time we set it
                m_dfLastTimeSet = dfTime;

                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if(i+1<m_Sources.size())
            {
                //down-grade path
                if(static_cast<int> (i+1) > m_nCurrentSourceNdx)
                {

                    MOOSTrace("Variable %s downgraded now deriving from %s\n",
                        m_sName.c_str(),
                        m_Sources[i+1].GetSource().c_str());


                    SetActiveSource(i+1);
                }
            }
        }
        
    }


    //if we got here there is nothing to do        
    return false;

}

CMOOSPriorityInput::CPrioritySource::CPrioritySource()
{
    m_dfVal         = 0.0;
    m_dfTimeOut        = DEFAULT_PRIORITY_TIMEOUT;
    m_dfLastInputTime    = -1;
    m_bFresh = false;

}

bool CMOOSPriorityInput::CPrioritySource::HasExpired(double dfTimeNow)
{
    if(m_dfLastInputTime==-1)
    {
        m_dfLastInputTime = dfTimeNow;
    }
    return dfTimeNow-m_dfLastInputTime>m_dfTimeOut;
}

bool CMOOSPriorityInput::CPrioritySource::Set(double dfVal, double dfTime)
{
    if(dfTime>m_dfLastInputTime)
    {
        m_dfVal = dfVal;
        m_dfDataTime = dfTime;
        m_dfLastInputTime = dfTime;
        m_bFresh = true;
        m_cDataType = MOOS_DOUBLE;
    }
    return true;
}

               
               bool CMOOSPriorityInput::CPrioritySource::Set( const std::string & sVal, double dfTime)
{
	if(dfTime>m_dfLastInputTime)
    {
       m_sVal = sVal;
       m_dfDataTime = dfTime;
       m_dfLastInputTime = dfTime;
       m_bFresh = true;
       m_cDataType = MOOS_STRING;
	}
    return true;
}
               
bool CMOOSPriorityInput::CPrioritySource::Get(double &dfVal, double &dfTime)
{
    dfVal = m_dfVal;
    dfTime = m_dfDataTime;
    m_bFresh = false;

    return true;
    
}

bool CMOOSPriorityInput::CPrioritySource::Get(std::string &sVal, double &dfTime)
{
    sVal = m_sVal;
    dfTime = m_dfDataTime;
    m_bFresh = false;
    return true;
}

char CMOOSPriorityInput::CPrioritySource::GetDataType()
{
    return m_cDataType;
}

bool CMOOSPriorityInput::CPrioritySource::Initialise(string &sSourceName,double dfTimeOut)
{
    m_sSource = sSourceName;
    m_dfTimeOut = dfTimeOut;
    return true;
}

string & CMOOSPriorityInput::CPrioritySource::GetSource()
{
    return m_sSource;
}

double CMOOSPriorityInput::CPrioritySource::GetLastInputTime()
{
    return m_dfLastInputTime;
}




bool CMOOSPriorityInput::SetActiveSource(int nSrcNdx)
{

    if(nSrcNdx<0 || nSrcNdx>= static_cast<int> (m_Sources.size()))
    {
        return false;
    }

    m_nCurrentSourceNdx = nSrcNdx;

    return true;
}

bool CMOOSPriorityInput::Clear()
{
    m_Sources.clear();
    //current source is top most in list (higher priority)
    m_nCurrentSourceNdx =0;
    return true;
}

bool CMOOSPriorityInput::GetLastValue(double & dfTime, double & dfVal)
{
    if(m_dfLastTimeSet==-1)
        return false;

    dfTime = m_dfLastTimeSet;
    dfVal =  m_dfLastValue;
    return true;
}
