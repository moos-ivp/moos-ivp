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
// MOOSLinearLeastMedianFilter.cpp: implementation of the CMOOSLinearLeastMedianFilter class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif
#include <MOOSLIB/MOOSLib.h>
#include "MOOSNavLibGlobalHelper.h"
#include "MOOSLinearLeastMedianFilter.h"

#include <newmat.h>
#include <newmatap.h>
using namespace NEWMAT;

#include "math.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSLinearLeastMedianFilter::CMOOSLinearLeastMedianFilter()
{
    m_bAngular = false;
    m_dfTotalStd = 0;
}

CMOOSLinearLeastMedianFilter::~CMOOSLinearLeastMedianFilter()
{

}

CMOOSLinearLeastMedianFilter::CDataPoint::CDataPoint(double dfX,double dfY,int nID)
{
    m_dfX = dfX;
    m_dfY = dfY;
    m_bOutlier = false;
    m_nID = nID;
}
CMOOSLinearLeastMedianFilter::CDataPoint::CDataPoint()
{
    m_bOutlier = false;
    m_nID = -1;

}

bool CMOOSLinearLeastMedianFilter::AddData(double dfX, double dfY, int nID)
{
    CDataPoint NewPoint(dfX,dfY,nID);

    m_Data.push_back(NewPoint);

    return true;
}

bool CMOOSLinearLeastMedianFilter::Calculate()
{
    //start pessimistically
    m_dfMedianDistance        = 1e6;
    m_dfPercentageOutlier    = 100.0;
    m_dfChannelNoise        = 1e6;
    m_dfTotalStd            = 1e6;

    //pick two points from our set and make a line from them
    CHypothesis CurrentHypothesis;

    m_BestHypothesis.m_dfMedian = 1e60;
    m_BestHypothesis.m_pData = & m_Data;


    vector<int> nIndexes;
    unsigned int i = 0;
    for(i = 0;i<m_Data.size();i++)
        nIndexes.push_back(i);
    
    for(i = 0;i<m_nNumSamples;i++)
    {
        CurrentHypothesis.m_pData = & m_Data;

        random_shuffle(nIndexes.begin(),nIndexes.end());

        CurrentHypothesis.Calculate(m_Data[nIndexes.front()],
                                    m_Data[nIndexes.back()]);

        if(CurrentHypothesis.m_dfMedian<m_BestHypothesis.m_dfMedian)
        {
            m_BestHypothesis = CurrentHypothesis;
        }
    }


    //now we can classify each point as inliers or outliers
    m_BestHypothesis.Classify();


    //now we gather and remeber some useful statistics
    m_dfPercentageOutlier    = ((double)m_BestHypothesis.m_nOutliers)/(m_Data.size())*100.0;
    m_dfChannelNoise        = m_BestHypothesis.m_dfEigData;
    m_dfTotalStd            = m_BestHypothesis.m_dfStd;
    m_dfMedianDistance        = m_BestHypothesis.m_dfMedian;

   
    return true;

}



bool CMOOSLinearLeastMedianFilter::CHypothesis::Calculate(CDataPoint P1,CDataPoint P2)
{


    //we shall always take the first two points to define the line...
    m_P1 = P1;
    m_P2 = P2;

    //figure out that line
    double dfDistance = sqrt(pow(P1.m_dfX-P2.m_dfX,2)+pow(P1.m_dfY-P2.m_dfY,2));

    if(dfDistance==0)
        dfDistance+=1e-6;

    double dfU1 = (P2.m_dfX - P1.m_dfX)/dfDistance;

    double dfU2 = (P2.m_dfY - P1.m_dfY)/dfDistance;

    m_dfA    = -dfU2;
    m_dfB    = dfU1;
    m_dfC    = -(P1.m_dfX*m_dfA + P1.m_dfY*m_dfB);


    //get distance to all remaining points from this 
    //line
    vector<double> Distances;

    CalculateDistances(Distances);

    //now sort to get median
    sort(Distances.begin(),Distances.end());

    m_dfMedian = Distances[Distances.size()/2];

    //MOOSTrace("median = %f\n",m_dfMedian);

    return true;
}

bool CMOOSLinearLeastMedianFilter::CHypothesis::CalculateDistances( vector<double> & Distances)
{
//    MOOSTrace("Calculate Distances:\n");
    POINT_DATA::iterator q;
    for(q = m_pData->begin();q!=m_pData->end();q++)
    {
        if(q->m_nID != m_P1.m_nID &&q->m_nID != m_P2.m_nID)
        {
            q->m_dfS = fabs(q->m_dfX*m_dfA + 
                q->m_dfY*m_dfB +
                m_dfC);

            //MOOSTrace("Y = %f X = %f S =%f id = %d\n",q->m_dfY,q->m_dfX,q->m_dfS,q->m_nID);

            Distances.push_back(q->m_dfS);

        }
        else
        {
            q->m_dfS=0;
        }
    }

    return true;
}

bool CMOOSLinearLeastMedianFilter::CHypothesis::Classify()
{

    vector<double> Distances;
    CalculateDistances(Distances);    
    POINT_DATA::iterator q;

    //dat size is:
    int n = m_pData->size();
    //we need two points per line
    int p = 2;                

    //therefor threshold is
    double dfInlierThreshold = m_dfMedian*pow((1.96*1.4826*(1+ 5/(n-p))),2);


    m_nInliers  = 0;
    m_nOutliers = 0;
    m_dfXMean = 0;
    m_dfYMean = 0;
    double dfYSqd=0;
    double dfYMn=0;
    for(q = m_pData->begin();q!=m_pData->end();q++)
    {
        if(q->m_dfS>dfInlierThreshold)
        {
            q->m_bOutlier = true;
            m_nOutliers++;
            //MOOSTrace("Outlier %f @ %f ID = %d\n",q->m_dfY,q->m_dfX,q->m_nID);
        }
        else
        {
            q->m_bOutlier = false;
            m_dfXMean+=q->m_dfX;
            m_dfYMean+=q->m_dfY;
            m_nInliers++;
        }
        dfYMn +=q->m_dfY;
        dfYSqd+=q->m_dfY*q->m_dfY;
    }

    if(m_nInliers>0)
    {
        m_dfYMean/=m_nInliers;
        m_dfXMean/=m_nInliers;

        DoTotalLeastSquares();
    }

    //Figure out standard deviation of whole set
    m_dfStd = sqrt((dfYSqd/n-dfYMn*dfYMn/(n*n)));

    return true;

}

bool CMOOSLinearLeastMedianFilter::Initialise(double dfAccuracy, double dfProbOutliers)
{
    //clear data...
    m_Data.clear();

    ///////////////////////////////////////////
    //how many sample will we need to take?

    //Minimum number of points to obtain line
    int       nPointsReq = 2; 

    //from robust statistics
    double dfNumSamples = log(1-dfAccuracy)/log(1-pow((1-dfProbOutliers),nPointsReq));

    //remember
    m_nNumSamples = (int)dfNumSamples;

    return true;
}

bool CMOOSLinearLeastMedianFilter::IsInlier(int nID)
{

    POINT_DATA::iterator q;

    for(q = m_Data.begin();q!=m_Data.end();q++)
    {
        if(q->m_nID==nID)
        {
            return !q->m_bOutlier;
        }
    }
    //not found!
    return false;
}



bool CMOOSLinearLeastMedianFilter::CHypothesis::DoTotalLeastSquares()
{
    
    if(m_nInliers<=1)
    {
        return false;
    }

    Matrix            X(m_nInliers,2);
    SymmetricMatrix Cov(2);
    Matrix            EigVec(2,2);
    DiagonalMatrix    EigVal(2);

    POINT_DATA::iterator q;

    int k = 1;
    for(q = m_pData->begin();q!=m_pData->end();q++)
    {
        if(!q->m_bOutlier)
        {
            X(k,1) = q->m_dfX - m_dfXMean;
            X(k,2) = q->m_dfY - m_dfYMean;
            k++;
        }
    }

    Cov << 1.0/(m_nInliers-1)* X.t()*X;

    Jacobi(Cov,EigVal,EigVec);                    

   
    //find the eignen value close along the line
    double dfGradient = -m_dfA/m_dfB;

    Matrix Line(2,1);
    Line<<1.0<<dfGradient;
    Line/=sqrt(dfGradient*dfGradient+1);

    double dfDotProd1 = EigVec(1,1)*Line(1,1)+EigVec(2,1)*Line(2,1);
    double dfDotProd2 = EigVec(1,2)*Line(1,1)+EigVec(2,2)*Line(2,1);

    //we want to know about noise orthogonal to the line we just fitted
    int iOrthogonal = dfDotProd1<dfDotProd2?1:2;

    //MOOSTrace("e1=%f e2=%f",EigVal(1),EigVal(2));
    //MOOSMatrixTrace(EigVec,"Evec");
    
    if(EigVal(iOrthogonal)>0)
    {
        m_dfEigData = (EigVal(iOrthogonal));
    }
    else
    {
        m_dfEigData=0;
    }


    return true;
}




double CMOOSLinearLeastMedianFilter::GetStatus(double &dfInlierStd,
                                               double &dfOutlierRatio,
                                               double &dfTotalStd,
                                               double &dfMedianDistance)
{
    dfInlierStd = m_dfChannelNoise;
    dfOutlierRatio = m_dfPercentageOutlier;
    dfTotalStd = m_dfTotalStd;
    dfMedianDistance = m_dfMedianDistance;

    return true;
}

double CMOOSLinearLeastMedianFilter::GetTotalStd()
{
    return m_dfTotalStd;
}

void CMOOSLinearLeastMedianFilter::Trace()
{

    MOOSTrace("%d points %d outliers. %fx+%fy+%f = 0\n Median = %f InlierStd = %f TotalStd = %f\n",
        m_Data.size(),
        m_BestHypothesis.m_nOutliers,
        m_BestHypothesis.m_dfA,
        m_BestHypothesis.m_dfB,
        m_BestHypothesis.m_dfC,
        m_BestHypothesis.m_dfMedian,
        m_dfChannelNoise,
        m_dfTotalStd);

    POINT_DATA::iterator q;

    for(q = m_Data.begin();q!=m_Data.end();q++)
    {
        q->Trace();
    }

}

void CMOOSLinearLeastMedianFilter::CDataPoint::Trace()
{
    MOOSTrace("Point[%d] X = %f Y = %f Distance = %f Outlier = %s\n",
        m_nID,
        m_dfX,
        m_dfY,
        m_dfS,
        m_bOutlier?"Yes":"No");
}
