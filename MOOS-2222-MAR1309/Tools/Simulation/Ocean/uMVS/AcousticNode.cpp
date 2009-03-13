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

// AcousticNode.cpp: implementation of the CAcousticNode class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include "AcousticNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAcousticNode::CAcousticNode()
{
    m_OffsetPos.ReSize(3,1);
    m_OffsetPos = 0;

    m_bEnabled = true;

 


}

CAcousticNode::CAcousticNode(double dfX,double dfY,double dfZ)
{
    m_OffsetPos.ReSize(3,1);

    m_OffsetPos    <<dfX
                <<dfY
                <<dfZ;

}

CAcousticNode::~CAcousticNode()
{

}

CSimEntity * CAcousticNode::GetParent()
{
    return m_pParent;
}


void CAcousticNode::SetParent(CSimEntity *pParent)
{
    m_pParent = pParent;
}


bool CAcousticNode::Listening(AcousticChannel eChan)
{
    if(!m_bEnabled)
        return false;

    return m_RxChannels.find(eChan)!=m_RxChannels.end();
}

bool CAcousticNode::SetRxChan(AcousticChannel eChan, bool bAdd)
{
    if(bAdd)
    {
        m_RxChannels.insert(eChan);
    }
    else
    {
        m_RxChannels.erase(eChan);
    }

    return true;
}

bool CAcousticNode::SetTxChan(AcousticChannel eChan)
{
    m_TxChannel = eChan;

    return true;
}


AcousticChannel CAcousticNode::GetTxChannel()
{
    return m_TxChannel;
}

bool CAcousticNode::OnAcousticHit(CAcousticSignal &Signal, double dfTime)
{
    return true;
}

bool CAcousticNode::Iterate(double dfTimeNow)
{
    return true;
}

bool CAcousticNode::SetEnvironment(CSimEnvironment *pEnv)
{
    m_pEnvironment = pEnv;

    return pEnv!=NULL;
}

string CAcousticNode::GetFullName()
{
    return ((CSimBase*)m_pParent)->GetName()+"/"+GetName();
}
