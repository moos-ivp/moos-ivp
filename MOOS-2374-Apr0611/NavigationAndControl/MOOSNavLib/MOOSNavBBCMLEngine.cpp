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
// MOOSNavBBCMLEngine.cpp: implementation of the CMOOSNavBBCMLEngine class.
//
//////////////////////////////////////////////////////////////////////
#include "MOOSNavLibGlobalHelper.h"
#include "MOOSNavLibDefs.h"
#include "MOOSNavObsStore.h"
#include "MOOSNavVehicle.h"
#include "MOOSNavBeacon.h"
#include "MOOSNavEKFEngine.h"

#include "MOOSNavBBCMLEngine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSNavBBCMLEngine::CMOOSNavBBCMLEngine()
{
    m_nTrajectoryStates = 3;
}

CMOOSNavBBCMLEngine::~CMOOSNavBBCMLEngine()
{

}

bool CMOOSNavBBCMLEngine::Initialise()
{
    m_Trajectories.clear();


    return true;
}

bool CMOOSNavBBCMLEngine::Shuffle()
{

    //we want to make a new tracked vehicle and place the old one
    //as a trajectory state. The new vehicle will have a new ID
    //all observations relating to old m_pTracked will now automatically
    //reference a trajectory state :-)
    m_pTracked->SetDroppedState(true);

    //insert it at the head of the trajectory list!
    m_Trajectories.push_front(m_pTracked);

    //Now Remake the tracked vehicle
    CMOOSNavVehicle * pNewTracked = new CMOOSNavVehicle;
    *pNewTracked = *m_pTracked;
    
    //but give it a new ID!
    pNewTracked->m_nID = GetNextID();
    
    //and copy the pointer back (this object is still ontop of the entity list)
    //m_pTracked will now point to our newly created object
    m_pTracked = pNewTracked;

    //////////////////////////////////////////
    //     make a shuffle matrix            //
    //////////////////////////////////////////
    
    Matrix FShuffle(m_Phat.Nrows(),m_Phat.Ncols());
    FShuffle = 0;

    //1) Identity matrix for vehicle and global states..
    int iRow=1;
    int iCol = 1;
    int i;
    for(i = 0;i<m_pTracked->GetStateSize();i++)
    {
        FShuffle(iRow++,iCol++) = 1.0;
    }
    
    //2) shifted identity matrix for dropped states
    iCol = m_pTracked->m_nStart; //reset column counter
    for(i = 0;i<m_pTracked->GetStateSize()*static_cast<int> (m_Trajectories.size());i++)
    {
        FShuffle(iRow++,iCol++) = 1.0;
    }
    
    //3) identity matrix for beacons/features
    for(i = 0;i<m_Beacons.front()->GetStateSize()*static_cast<int> (m_Beacons.size());i++)
    {
        FShuffle(iRow++,iRow++) = 1.0;
    }
    

    //and here is the shuffle
    m_Xhat = FShuffle*m_Xhat;
    m_Phat = FShuffle*m_Phat*FShuffle.t();

    //and now shuffle trajectory state indexes
    ENTITY_LIST::iterator p;
    for(p = m_Trajectories.begin();p!=m_Trajectories.end();p++)
    {
        CMOOSNavEntity* pTraj = *p;

        pTraj->m_nEnd+=pTraj->GetStateSize();
        pTraj->m_nStart+=pTraj->GetStateSize();
    }

    //finally drop a state of the end;
    CMOOSNavEntity* pToKill = m_Trajectories.back();
    delete pToKill;
    m_Trajectories.pop_back();


    return true;
    
}


bool CMOOSNavBBCMLEngine::DropState()
{
    
    Shuffle();    
    return true;

/*    int nTrajStart    = pTrajState->GetStart();
    int nTrajEnd    = pTrajState->GetEnd();
    int nStart        = GetStart();
    int nEnd        = GetEnd();
    int nRows        = pMap->m_Phat.Nrows();

    //LH horizontal cross block
    pMap->m_Phat.SubMatrix(nTrajStart,nTrajEnd,1,nEnd)                = pMap->m_Phat.SubMatrix(nStart,nEnd,1,nEnd);

    //covariance  block
    pMap->m_Phat.SubMatrix(nTrajStart,nTrajEnd,nTrajStart,nTrajEnd) = pMap->m_Phat.SubMatrix(nStart,nEnd,nStart,nEnd);

    //RH horizontal cross covariance  block
    pMap->m_Phat.SubMatrix(nTrajStart,nTrajEnd,nTrajEnd+1,nRows)    = pMap->m_Phat.SubMatrix(nStart,nEnd,nTrajEnd+1,nRows);

    //and now do a single big copy to make symmetric
    pMap->m_Phat.SubMatrix(1,nRows,nTrajStart,nTrajEnd)    =    (pMap->m_Phat.SubMatrix(nTrajStart,nTrajEnd,1,nRows)).t();
*/

}



bool CMOOSNavBBCMLEngine::MakeObsMatrices()
{
    //ok here we grab un associated obsevations
    OBSLIST::iterator p;
    
    for(p = m_Observations.begin();p!=m_Observations.end();p++)
    {
        CMOOSObservation  & rObs = *p;

        if(rObs.IsType(CMOOSObservation::LBL_BEACON_2WR))
        {
            if(rObs.m_pRespondingSensor==NULL)
            {
                //take a copy..
                m_UnAssociatedObs.push_front(rObs);

                //but we should ignore it  below us..
                rObs.Ignore(true);
            }
        }        
    }

    //now call the base class
    return CMOOSNavEKFEngine::MakeObsMatrices();

}
