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


// SimVehicle.cpp: implementation of the CSimVehicle class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif



#include "AcousticNode.h"
#include "AcousticTransceiver.h"
#include "AcousticResponder.h"
#include "SimVehicle.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSimVehicle::CSimVehicle()
{
    CAcousticTransceiver * pTcvr = new CAcousticTransceiver;
    pTcvr->SetParams(m_pParams);
    m_AcousticNodes.push_front(pTcvr);
    pTcvr->SetParent(this);
    pTcvr->SetName("Tcvr1");

    CAcousticResponder * pTpdr = new CAcousticResponder;
    pTpdr->SetParams(m_pParams);
    m_AcousticNodes.push_front(pTpdr);
    pTpdr->SetParent(this);
    pTpdr->SetName("Tpdr1");

}

bool CSimVehicle::ConfigureResponder(bool bEnable,AcousticChannel eRx,AcousticChannel eTx,double dfTAT)
{
    ACOUSTIC_NODE_LIST::iterator p;
    for(p = m_AcousticNodes.begin();p!=m_AcousticNodes.end();p++)
    {
        if((*p)->GetName()=="Tpdr1")
        {
            (*p)->Enable(bEnable);
            (*p)->SetRxChan(eRx);
            (*p)->SetTxChan(eTx);
            ((CAcousticResponder*)(*p))->SetTAT(dfTAT);
            return true;
        }
    }

    return false;
}


CSimVehicle::~CSimVehicle()
{

}






