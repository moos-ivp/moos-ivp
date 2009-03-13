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
// MOOSNavObsStore.cpp: implementation of the CMOOSNavObsStore class.
//
//////////////////////////////////////////////////////////////////////

#include "MOOSNavObsStore.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSNavObsStore::CMOOSNavObsStore()
{
    //lets store 20 seconds of data by default..
    m_dfSpan = 20; 
    m_dfNewestObsTime = 0;
}

CMOOSNavObsStore::~CMOOSNavObsStore()
{

}

bool CMOOSNavObsStore::Add(OBSLIST &ObsList)
{
    if(ObsList.empty())
    {
        return true;
    }

    OBSLIST::iterator p;

    for(p = ObsList.begin();p!=ObsList.end();p++)
    {
        CMOOSObservation & rObs = *p;

//        MOOSTrace("adding:");
//        rObs.Trace();
        
        CMOOSObservation::Type eType = rObs.m_eType;


        OBSLIST* pList = GetListByType(eType);

        if(pList!=NULL)
        {

            //store time of most recent addition
            if(rObs.m_dfTime>m_dfNewestObsTime)
                m_dfNewestObsTime = rObs.m_dfTime;

            //insert it in the right spot
            OBSLIST::iterator q;
            for(q =pList->begin();q!=pList->end();q++)
            {
                if(rObs.m_dfTime>=(*q).m_dfTime)
                {
                    pList->insert(q,rObs);
                    break;
                }
            }
            if(q==pList->end())
            {
                pList->push_back(rObs);
            }

            //now remove data that is too old..
            while(!pList->empty() && pList->front().m_dfTime-pList->back().m_dfTime>m_dfSpan)
            {
                pList->pop_back();
            }
        }
        else
        {
            m_ObsListMap[eType] = ObsList;
        }
    }

    return true;

}

OBSLIST * CMOOSNavObsStore::GetListByType(CMOOSObservation::Type eType)
{
    //find the relevant list
    OBSLISTMAP::iterator p = m_ObsListMap.find(eType);

    if(p!=m_ObsListMap.end())
    {
        return & p->second;
    }

    return NULL;

}

//all we do here is mark the original observation as 
//used. Note falgs like bGoodDA and bIgnore are not copiued
//back. This means that every fetch from the store comes with
//clean observations. Apart from those that have already been used 
//and which are never even passed out!
bool CMOOSNavObsStore::MarkAsUsed(OBSLIST &ToMarkList)
{
    OBSLIST::iterator r;

    bool bFound = false;

    //for all the obs we are given
    for(r = ToMarkList.begin();r!=ToMarkList.end();r++)
    {
        bFound = false;

        CMOOSObservation & rObsToFind = *r; 

        OBSLISTMAP::iterator p;

        //for all the lists of obs we are storing
        for(p = m_ObsListMap.begin();p!=m_ObsListMap.end() && !bFound;p++)
        {
            OBSLIST & rList = p->second;
            OBSLIST::iterator q;

            //for all the obs stored in that list
            for(q = rList.begin();q!=rList.end() && !bFound ;q++)
            {
                CMOOSObservation & rObsStored = *q;
                if(rObsStored.GetID() == rObsToFind.GetID())
                {
                    //found it!
                    rObsStored.m_bUsed = true;    
                    bFound = true;
                }
            }
        }
    }

    return true;
}

bool CMOOSNavObsStore::SetSpan(double dfSpan)
{
    m_dfSpan = dfSpan;
    return true;
}

bool CMOOSNavObsStore::Flush()
{

    OBSLISTMAP::iterator p;

    //for all the lists of obs we are storing
    for(p = m_ObsListMap.begin();p!=m_ObsListMap.end();p++)
    {
        OBSLIST & rList = p->second;
        rList.clear();
    }

    return true;
}

bool CMOOSNavObsStore::GetObservationsBetween(OBSLIST &ObsList, double dfT1, double dfT2)
{
    ObsList.clear();

    OBSLISTMAP::iterator p;
    for(p = m_ObsListMap.begin();p!=m_ObsListMap.end();p++)
    {
        OBSLIST & rList = p->second;
        OBSLIST::iterator q;

        bool bDone = false;
        for(q = rList.begin();q!=rList.end();q++)
        {
            //starting at youngest and getting older...
            CMOOSObservation & rObs = *q;

               if(rObs.IsType(CMOOSObservation::LBL_BEACON_2WR))
                    {
                        int lk = 0;
                    }

//            rObs.Trace();
//            MOOSTrace("Stored Obs t = %f cut off  = %f\n",rObs.m_dfTime,dfCutOffTime);
            if(rObs.m_dfTime>=dfT1 && rObs.m_dfTime<=dfT2 )
            {
                if(!rObs.m_bUsed)
                {

                    if(rObs.IsType(CMOOSObservation::LBL_BEACON_2WR))
                    {
                        int lk = 0;
                    }


                    //put on outlist with older data towards back of list
                    ObsList.push_back(rObs);
                }
            }
        }
    }

    return true;
}

double CMOOSNavObsStore::GetNewestObsTime()
{
    return m_dfNewestObsTime;
}
