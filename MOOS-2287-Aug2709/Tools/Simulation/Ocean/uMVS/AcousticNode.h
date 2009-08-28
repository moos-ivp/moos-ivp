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


// AcousticNode.h: interface for the CAcousticNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACOUSTICNODE_H__F8C17D77_CD53_4E2A_9D7D_110C9C1334B6__INCLUDED_)
#define AFX_ACOUSTICNODE_H__F8C17D77_CD53_4E2A_9D7D_110C9C1334B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <set>
using namespace std;

#include <newmat.h>
using namespace NEWMAT;

#include "SimLoggable.h"
#include "AcousticDefs.h"

#include "AcousticSignal.h"

class CSimEntity;
class CSimEnvironment;

class CAcousticNode  : public CSimLoggable
{
public:
    string GetFullName();
    bool SetEnvironment(CSimEnvironment * pEnv);
    virtual bool Iterate(double dfTimeNow);
    virtual bool OnAcousticHit(CAcousticSignal & Signal,double dfTime);
    AcousticChannel GetTxChannel();
    bool SetTxChan(AcousticChannel eChan);
    bool SetRxChan(AcousticChannel eChan, bool bAdd = true);
    bool Listening(AcousticChannel eChan);
    void SetParent(CSimEntity * pParent);
    CSimEntity * GetParent();
    void Enable(bool bEnable){m_bEnabled = bEnable;};
    CAcousticNode();
    CAcousticNode(double dfX,double dfY,double dfZ);
    virtual ~CAcousticNode();

    void   SetX(double dfVal){m_OffsetPos(1,1) = dfVal;};
    void   SetY(double dfVal){m_OffsetPos(2,1) = dfVal;};
    void   SetZ(double dfVal){m_OffsetPos(3,1) = dfVal;};
    

    CSimEntity * m_pParent;

    set<AcousticChannel>    m_RxChannels;
    AcousticChannel         m_TxChannel;


    CSimEnvironment* m_pEnvironment;
    
    bool m_bEnabled;

    double m_dfLastTxTime;

    Matrix m_OffsetPos;




};

#endif // !defined(AFX_ACOUSTICNODE_H__F8C17D77_CD53_4E2A_9D7D_110C9C1334B6__INCLUDED_)
