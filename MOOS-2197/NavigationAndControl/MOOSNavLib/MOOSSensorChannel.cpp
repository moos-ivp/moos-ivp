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
// MOOSSensorChannel.cpp: implementation of the CMOOSSensorChannel class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include "MOOSSensorChannel.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSSensorChannel::CMOOSSensorChannel()
{
    //how deep should the history be
    m_nHistoryDepth = 4;
    

    //by defualt streams are active at birth
    m_bActive = true;

    //this must be set for each stream in the mission file
    //if it is to have an effect
    m_dfNoiseLimit = 10;

    m_dfMean = 0;

    m_dfStd = 0;

    m_dfHistoryTimeSpan = 20;

    m_bBuilt = false;

}

CMOOSSensorChannel::~CMOOSSensorChannel()
{

}


bool CMOOSSensorChannel::SetHistoryDepth(int nDepth)
{
    m_nHistoryDepth = nDepth;
    return true;
}

bool CMOOSSensorChannel::Add(CMOOSObservation &rObs)
{
    
    //have we already stored this observation?
    OBSLIST::iterator w;
    for(w = m_History.begin();w!=m_History.end();w++)
    {
        if(w->m_nID==rObs.m_nID)
        {
            //yes!
            //we do nothing (this isn't an error)
            return true;
        }
    }
    
    //no -> store it now
    m_History.push_front(rObs);
    
    //now sort list with biggest time at the front
    //default behaviour of sort is in ascending order..
    m_History.sort(greater< CMOOSObservation >());
    
    
    if(m_History.size()==m_nHistoryDepth)
    {
        AddToOutput("Working history (%d deep) for %s : BUILT",
            m_nHistoryDepth,
            m_sName.c_str());
        m_bBuilt = true;
    }
    
    
    //now keep history a sensible length
    double dfSpan = m_History.front().m_dfTime-m_History.back().m_dfTime;
    while(m_History.size() >m_nHistoryDepth || dfSpan > m_dfHistoryTimeSpan)
    {        
        m_History.pop_back();
        dfSpan = m_History.front().m_dfTime-m_History.back().m_dfTime;
    }
    
    return true;
}

bool CMOOSSensorChannel::Agrees(CMOOSObservation &rObs)
{
    //if small history can't make a call
    if(m_History.size()<m_nHistoryDepth)
    {
        return false;
    }
    
    //set up the median filter
    m_MedianFilter.Initialise(0.9,0.8);


    bool bTest = false;
    if(bTest)
    {
        //test data 
        double dfTest[] = {7,2,4,4,5,6,7};

        int i = 0;
        for(i =0;i<sizeof(dfTest)/sizeof(double);i++)
        {
            m_MedianFilter.AddData(i,dfTest[i],i);
        }

        //calculte outliers
        m_MedianFilter.Calculate();

        //figure out how well the channel is doing (is it clean data)
        AutoDiagnose();

        for(i =0;i<sizeof(dfTest)/sizeof(double);i++)
        {
            if(!m_MedianFilter.IsInlier(i))
            {
                MOOSTrace("Rejected fake obs %d\n",i);
            }
        }
        return true;        
    }
    else
    {
        //add data
        OBSLIST::iterator p;

        for(p = m_History.begin();p!=m_History.end();p++)
        {
            m_MedianFilter.AddData(p->m_dfTime,p->m_dfData,p->m_nID);
        }

    
        //calculte outliers
        m_MedianFilter.Calculate();

        //figure out how well the channel is doing (is it clean data)
        AutoDiagnose();

        if(m_bActive)
        {
            //previous auto-diagnose could have caused us to turn off..
            return   m_MedianFilter.IsInlier(rObs.m_nID);
        }
        else
        {
            //it did...
            return false;
        }
    }
}


bool CMOOSSensorChannel::SetNoiseLimit(double dfLimit)
{
    m_dfNoiseLimit = dfLimit;
    return true;
}

bool CMOOSSensorChannel::AutoDiagnose()
{


   //figure out trajectory statistics...
    if(m_History.size()<m_nHistoryDepth)
        return true;

    //need to fit a least medians line to data....
    double dfChannelNoise;
    double dfOutlierRatio;
    double dfTotalStd;
    double dfMedianDistance;
    m_MedianFilter.GetStatus(    dfChannelNoise,
                                dfOutlierRatio,
                                dfTotalStd,
                                dfMedianDistance);

    //MOOSTrace("Channel Noise = %f, OutlierRatio = %f%%\n",dfChannelNoise,dfOutlierRatio);

    //robust stats is only good to about 50 % in ideal circumstances we'll back off a little
//    if((dfOutlierRatio>50 ||dfChannelNoise>=m_dfNoiseLimit) && dfTotalStd>m_dfNoiseLimit )
    if((dfOutlierRatio>50 ||dfMedianDistance>=m_dfNoiseLimit))
    {
        if(m_bActive==true)
        {
            
            OBSLIST::iterator w;
            for(w = m_History.begin();w!=m_History.end();w++)
            {
                MOOSTrace("Data = %f\n",w->m_dfData);
            }

            
            string sOutput = MOOSFormat("%s is suspect %.1f percent are outliers (of %d), MedianDev = %.4f (limit %.4f): turning stream off!\n",
                m_sName.c_str(),
                dfOutlierRatio,
                m_nHistoryDepth,
                dfMedianDistance,
                m_dfNoiseLimit);

            AddToOutput(sOutput);
        }

        m_bActive=false;
    }
    else
    {
        if(m_bActive==false)
        {
            //tell the world we are re-enabling stream...
            string sOutput = MOOSFormat("Sensor Channel %s better behaved now : turning stream on!\n",
                m_sName.c_str());

            AddToOutput(sOutput);
        }
        m_bActive = true;
    }

    return true;
}

bool CMOOSSensorChannel::IsOnline()
{
    return m_History.size()>=m_nHistoryDepth;
}

double CMOOSSensorChannel::GetPercentFull()
{
    return 100.0*m_History.size()/m_nHistoryDepth;
}

void CMOOSSensorChannel::Trace()
{
    MOOSTrace("\n*** CMOOSSensorChannel::Trace() ***\n");
    m_MedianFilter.Trace();
}

bool CMOOSSensorChannel::IsBuilt()
{
    return m_bBuilt;
}
