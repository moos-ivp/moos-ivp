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
// MOOSLinearLeastMedianFilter.h: interface for the CMOOSLinearLeastMedianFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSLINEARLEASTMEDIANFILTER_H__D9A2036A_AD07_4B5E_AF17_C769C0ABB546__INCLUDED_)
#define AFX_MOOSLINEARLEASTMEDIANFILTER_H__D9A2036A_AD07_4B5E_AF17_C769C0ABB546__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;
class CMOOSLinearLeastMedianFilter  
{
protected:
    class CDataPoint
    {
    public:
        void Trace();
        double m_dfX;
        double m_dfY;
        bool   m_bOutlier;
        int       m_nID;
        double m_dfS;
        CDataPoint();
        CDataPoint(double dfX,double dfY,int nID);
    };
    typedef vector<CDataPoint> POINT_DATA;

    class CHypothesis
    {
    public:
        bool Classify();
        bool Calculate(CDataPoint P1,CDataPoint P2);
        bool CalculateDistances(vector<double> & Distances);
        bool DoTotalLeastSquares();

        //line coefficients
        double m_dfA;
        double m_dfB;
        double m_dfC;
        double m_dfMedian;

        CDataPoint m_P1;
        CDataPoint m_P2;

        int m_nOutliers;
        int m_nInliers;
        double m_dfXMean;
        double m_dfYMean;
        double m_dfEigData;
        double m_dfStd;


        POINT_DATA * m_pData;
    };


public:
    void Trace();
    double GetTotalStd();
    double GetStatus(double & dfInlierStd,
                    double & dfOutlierRatio,
                    double & dfTotalStd,
                    double & dfMedian);
    bool IsInlier(int nID);
    bool Initialise(double dfAccuracy,double dfProbOutliers);
    bool AddData(double dfX,double dfY,int nID);
    CMOOSLinearLeastMedianFilter();
    virtual ~CMOOSLinearLeastMedianFilter();
    bool Calculate();

protected:

private:
    POINT_DATA m_Data;

    unsigned int m_nNumSamples;

    double m_dfPercentageOutlier;
    double m_dfChannelNoise;
    double m_dfTotalStd;
    double m_dfMedianDistance;
    bool   m_bAngular;
    CHypothesis m_BestHypothesis;


};

#endif // !defined(AFX_MOOSLINEARLEASTMEDIANFILTER_H__D9A2036A_AD07_4B5E_AF17_C769C0ABB546__INCLUDED_)
