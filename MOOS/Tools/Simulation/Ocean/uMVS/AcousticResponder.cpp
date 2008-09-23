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

// AcousticResponder.cpp: implementation of the CAcousticResponder class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif


#include "SimEntity.h"
#include "AcousticResponder.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAcousticResponder::CAcousticResponder()
{
    m_dfTAT = 0.125;
    SetRxChan(ACOUSTIC_CHAN_CIF);

}

CAcousticResponder::~CAcousticResponder()
{

}


bool CAcousticResponder::OnAcousticHit(CAcousticSignal & Signal,double dfTime)
{
#ifdef VERBOSE
    MOOSTrace("CAcousticResponder::OnAcousticHit on %s Signal[%d] from %s on Channel %d\n",
        GetFullName().c_str(),
        Signal.m_nID,
        Signal.GetSrcName().c_str(),
        Signal.GetChannel());
#endif

    if(Listening(Signal.GetChannel()))
    {

        //this is a trap for self excitation..
        if(Signal.GetSrcName()==GetFullName())
            return false;


        Matrix NodePosAfterTAT;

        GetParent()->GetNodePosition(*this,
                                    GetTAT(),
                                    NodePosAfterTAT);

        CAcousticSignal ReplyPing(    NodePosAfterTAT(1,1),
                                    NodePosAfterTAT(2,1),
                                    NodePosAfterTAT(3,1),
                                    dfTime+GetTAT());

        ReplyPing.SetChannel(GetTxChannel());

        //here we set the src name to be something like "B1:TDR"
        ReplyPing.SetSrcName(GetFullName());

        m_pEnvironment->AddSignal(ReplyPing);

#ifdef VERBOSE
        MOOSTrace("%s is pinged by Signal[%d] from %s on Chan[%d] and replies with Signal[%d] Chan[%d] \n",
            GetFullName().c_str(),
            Signal.m_nID,
            Signal.GetSrcName().c_str(),
            Signal.GetChannel(),
            ReplyPing.m_nID,
            ReplyPing.GetChannel());
#endif

    }

    return true;
}
double CAcousticResponder::GetTAT()
{
    return m_dfTAT;
}

bool CAcousticResponder::SetTAT(double dfTAT)
{
    m_dfTAT = dfTAT;

    return true;
}
