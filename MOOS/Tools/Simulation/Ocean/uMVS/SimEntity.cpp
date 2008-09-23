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

// SimEntity.cpp: implementation of the CSimEntity class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif
#include "SimGlobalHelper.h"
#include "SimEnvironment.h"
#include "SimEntity.h"
#include <iostream>
#include <iomanip>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimEntity::CSimEntity()
{
    m_pEnvironment = NULL;
    m_Pos_e.ReSize(6,1);
    m_Vel_e.ReSize(6,1);
    m_dfLastLogState = -1;
    m_dfLogFrequency = 2.0;
}

CSimEntity::~CSimEntity()
{

}

bool CSimEntity::SetParams(CSimParams *pParams)
{
    m_pParams = pParams;
    
    ACOUSTIC_NODE_LIST::iterator p;

    for(p=m_AcousticNodes.begin();p!=m_AcousticNodes.end();p++)
    {
        (*p)->SetParams(m_pParams);
    }

    return true;
}



bool CSimEntity::Iterate(double dfTimeNow,double dfDT)
{
    return true;
}

bool CSimEntity::GetAcousticNodes(ACOUSTIC_NODE_LIST &List)
{
    List = m_AcousticNodes;

    return true;


}

bool CSimEntity::GetNodePosition(CAcousticNode &Node, double dfDT, Matrix &Result)
{
    //for now assume zero offset of node... put transformation in here...
    //simple stuff but boring...
    if(!Node.m_OffsetPos.IsZero())
    {
        printf("Transformation for non zero offsets not written yet\n");
    }
    
    Result = m_Pos_e+dfDT*m_Vel_e;

    return true;
}

void CSimEntity::SetEnvironment(CSimEnvironment *pEnv)
{
    m_pEnvironment  = pEnv;
}


bool CSimEntity::SolveAcoustics(double dfTime,double dfDT)
{
    
    ACOUSTIC_NODE_LIST::iterator w;
    ACOUSTIC_SIGNAL_LIST::iterator q;

//    MOOSTrace("Solving Acoustics for %s\n", GetName().c_str());


    for(w = m_AcousticNodes.begin();w!=m_AcousticNodes.end();w++)
    {
        //dereference the node
        CAcousticNode* pNode = *w;

//        MOOSTrace("\t Node %s\n", pNode->GetName().c_str());

        pNode->SetEnvironment(m_pEnvironment);

        //and where it is now...
        Matrix X0;

        GetNodePosition(*pNode,0,X0);

#ifdef SIM_ENTITY_VERBOSE                
        MOOSTraceMatrix(X0,"X0");
#endif
        
        //MOOSTrace("%d acoustic signals in environment \n",m_pEnvironment->m_AcousticSignals.size());

        for(q = m_pEnvironment->m_AcousticSignals.begin();q!=m_pEnvironment->m_AcousticSignals.end();q++)
        {
            //dereference it...
            CAcousticSignal & rSignal = *q;

            //is the node listening on this channel?
            if(pNode->Listening(rSignal.GetChannel()))
            {                            

                if(IsLocalSource(rSignal.GetSrcName()))
                {
#ifdef SIM_ENTITY_VERBOSE                
                    MOOSTrace("%s is local to %s, Tx = %f Now = %f Age = %f - continue\n",
                        rSignal.GetSrcName().c_str(),
                        pNode->GetFullName().c_str(),
                        rSignal.GetStartTime(),dfTime,rSignal.Age(dfTime));
#endif
                    continue;
                }

                //so when approximately would the signal intercept the node?
                double dfT0 = rSignal.GetExpectedIntersectionTime(X0);

#ifdef SIM_ENTITY_VERBOSE                

                MOOSTrace("T = %f : Signal[%d] [Chan=%d] from %s will intersect %s in %f s\n",
                    dfTime,
                    rSignal.m_nID,
                    rSignal.GetChannel(),
                    rSignal.GetSrcName().c_str(),
                    pNode->GetFullName().c_str(),
                    dfT0-dfTime);
#endif

                //is that close to us? ie within this epoch?
                if(dfT0>dfTime && dfT0<dfTime+dfDT)
                {
                    
                    //OK lets figure out where node would have been dfSmallDT
                    //seconds ago...(we have already moved the host entity above)
                    Matrix X1;
                    GetNodePosition(*pNode,-dfDT,X1);

        //            MOOSTraceMatrix(X1,"X1");

                    double dfT1 = rSignal.GetExpectedIntersectionTime(X1);

                    //now need to find the intersection between the line joining points
                    // (m_dfTimeNow,dfT0) and (m_dfTimeNow-dfSmallDT,dfT1) and the line
                    // with gradient one ...

                    double dfGrad = (dfT0-dfT1)/(dfTime-(dfTime-dfDT));
                    double dfC = dfT1+dfGrad*(-(dfTime-dfDT));

                    //intersection time is
                    double dfTi = dfC/(1-dfGrad);

/*                    MOOSTrace("ACoustic Hit @ %f timenow = %f\n",
                                dfTi-m_pEnvironment->GetStartTime(),
                                dfTime-m_pEnvironment->GetStartTime());

                    MOOSTrace("D = %f\n",(dfTi-rSignal.GetStartTime())*1498.0);
*/
                    pNode->OnAcousticHit(rSignal,dfTi);
                }
            }
            else
            {
#ifdef SIM_ENTITY_VERBOSE
                MOOSTrace("%s is NOT listening for Signal[%d] from %s\n",
                    pNode->GetFullName().c_str(),
                    rSignal.m_nID,
                    rSignal.GetSrcName().c_str());
#endif
            }
        }

        //this call lets the node go about its normal action such as
        //pinging when required
        pNode->Iterate(dfTime);
    }

    return true;
}

bool CSimEntity::IsLocalSource(std::string sSrc)
{
    //this is a catch for auto-excitation
    std::string sSrcVeh = MOOSChomp(sSrc,"/");
    if(GetName()==sSrcVeh)
            return true;

    return false;

}


double CSimEntity::HeadingFromYaw(double dfYaw)
{
    double dfYawDeg = dfYaw*180.0/PI;

    //heading is negative yaw
    double dfHeading = dfYawDeg>0 ? 360-dfYawDeg : -dfYawDeg;

    //subtract magnetic offset (out of simulator you add it)
    dfHeading-=m_pEnvironment->m_dfMagneticOffset;
    
    //and check again...
    dfHeading = dfHeading<0 ? 360.0+dfHeading : dfHeading;

    return dfHeading;
}


bool CSimEntity::LogState(double dfTimeNow)
{    

    //housekeeping..
    if(m_dfLogFrequency<=0)
        return false;

    if(dfTimeNow-m_dfLastLogState<1.0/m_dfLogFrequency)
        return false;

    m_dfLastLogState = dfTimeNow;

    //logging happens here..

    ostringstream os;

    os.setf(ios::fixed,ios::floatfield);
    os<<setprecision(3);

    os  <<"State,"
        
        <<"T="<<m_pEnvironment->GetElapsedTime(dfTimeNow)<<","
        
        <<"Name="<<GetName().c_str()<<","

        <<"Pose=["    <<m_Pos_e(1,1)<<","
                    <<m_Pos_e(2,1)<<","
                    <<m_Pos_e(3,1)<<","
                    <<m_Pos_e(4,1)<<","
                    <<m_Pos_e(5,1)<<","
                    <<m_Pos_e(6,1)<<"],"

        <<"Vel=["    <<m_Vel_e(1,1)<<","
                    <<m_Vel_e(2,1)<<","
                    <<m_Vel_e(3,1)<<","
                    <<m_Vel_e(4,1)<<","
                    <<m_Vel_e(5,1)<<","
                    <<m_Vel_e(6,1)<<"]"

                    <<endl<<ends;

    return Log(os);        


}

