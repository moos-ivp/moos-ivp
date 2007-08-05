// Pings2Solution.h: interface for the CPings2Solution class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
	#pragma warning(disable : 4786)
#endif
#if !defined(AFX_PINGS2SOLUTION_H__63599480_AAD9_4096_9C22_D1B27CAF5C1E__INCLUDED_)
#define AFX_PINGS2SOLUTION_H__63599480_AAD9_4096_9C22_D1B27CAF5C1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <map>
#include <list>
#include <MOOSGenLib/InterpBuffer.h>
#include <MOOSGenLib/MOOSGenLibGlobalHelper.h>

class CPings2Solution  
{
public:

    typedef std::pair<double,double> XY_LOCATION;
    struct Solution
    {
        XY_LOCATION m_Location;
        double m_dfSSE;
        bool operator < (const Solution &s) const{ return m_dfSSE<s.m_dfSSE;};
        void Print(){MOOSTrace("[X,Y] = {%f %f} Score = %f\n",m_Location.first,m_Location.second,m_dfSSE);};
    };


public:
	void Clear();
	CPings2Solution();
	virtual ~CPings2Solution();


    bool AddX(double dfV,double dfT){m_X[dfT] = dfV;return true;}
    bool AddY(double dfV,double dfT){m_Y[dfT] = dfV;return true;}
    bool AddYaw(double dfV,double dfT){m_Yaw[dfT] = dfV;return true;}
    bool AddPing(double dfV,double dfT)
    {
        if(m_X.empty() || m_Y.empty())
            return false;

        if(m_X.MinKey()>dfT || m_Y.MinKey()>dfT)
            return false;

        m_Pings.push_back(PING(dfT,dfV));
        
        return true;
    };

    bool Solve( std::list<Solution> &);
protected:

    typedef TInterpBuffer< double,double,CTimeNumericInterpolator<double>  > STATE_VAR;

    STATE_VAR m_X,m_Y,m_Yaw;

    typedef std::pair<double,double> PING;
    std::vector<PING> m_Pings;


    bool Intersect(XY_LOCATION L1,
        double dfR1,
        XY_LOCATION L2,
        double dfR2,
        XY_LOCATION & Sol1,
        XY_LOCATION & Sol2);



};

#endif // !defined(AFX_PINGS2SOLUTION_H__63599480_AAD9_4096_9C22_D1B27CAF5C1E__INCLUDED_)
