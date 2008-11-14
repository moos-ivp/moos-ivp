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
//   This file is part of a  MOOS Core Component. 
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

#include "MOOSSkewFilter.h"
#include <cmath>

namespace MOOS
{

	void CMOOSSkewFilter::DumpState() const
	{
        MOOSTrace("LowerBound:\n");
        m_LowerBound.DumpState();
        MOOSTrace("UpperBound:\n");
        m_UpperBound.DumpState();
	}


    CMOOSSkewFilter::CMOOSSkewFilter()
        : m_LowerBound(CConvexEnvelope::envelopeAbove),
        m_UpperBound(CConvexEnvelope::envelopeBelow)
    {
        Reset();
    }


    double CMOOSSkewFilter::GetLBSkewAtServerTime(double dfTime) const
    {
        // Result from this may not be good if envelope is
        // not yet stable
        
        double m=0.0,c=0.0;
        m_LowerBound.GetLineEstimate(m, c);
        
        return m*dfTime + c;
    }
    


    double CMOOSSkewFilter::GetUBSkewAtServerTime(double dfTime) const
    {
        // Result from this may not be good if envelope is
        // not yet stable
        double m=0.0,c=0.0;
        m_UpperBound.GetLineEstimate(m, c);
        
        return m*dfTime + c;
    }
        


    bool CMOOSSkewFilter::GetSkewAtServerTime(double dfTime, double &dfSkew) const
    {
        double m=0.0, c=0.0;
        GetMidLine(m, c);

        dfSkew = m*dfTime + c;
        return true;
    }


    // This tells us the skew at an un-corrected local time
    bool CMOOSSkewFilter::GetSkewAtLocalTime(double dfTime, double &dfSkew) const
    {

        double m=0.0, c=0.0;
        GetMidLine(m, c);
        
        if (m >= 1.0) return false;

        dfSkew = m*dfTime + c;

        // We need to adjust for the fact that the local
        // clock runs at a different rate to the server's clock
        dfSkew = dfSkew / (1.0-m);

        return true;
    }


    void CMOOSSkewFilter::GetMidLine(double &m, double &c) const
    {
        // This finds the mean line between the two current
        // envelope estimates.  Not a good idea to call it
        // before the envelopes have stabilized

        double m1=0.0,c1=0.0;
        m_LowerBound.GetLineEstimate(m1, c1);

        double m2=0.0,c2=0.0;
        m_UpperBound.GetLineEstimate(m2, c2);

        m = (m1+m2)/2.0;
        c = (c1+c2)/2.0;
    }



	void CMOOSSkewFilter::Reset()
	{
		m_dfLastVal    = 0.0;
		m_dfLastTime   = 0.0;
		m_nMeas        = 0;
		m_LowerBound.Reset();
        m_UpperBound.Reset();
	}
	

	double CMOOSSkewFilter::SmoothingFilter(double dfDT, double dfOldFilterVal, double dfNewMeas, double dfGradient) const
	{
        // Yes, this is a long winded way of writing things,
        // but it's an easier form for applying nonlinear
        // gubbins like maximum skew rates at a later date
        const double alpha = 0.001;
        double pred        = dfOldFilterVal + dfGradient * dfDT;
        double innov       = dfNewMeas - pred;
        return pred + alpha * innov;
	}


    void CMOOSSkewFilter::UpdateEnvelopes(double dfTXtime, double dfSkewLB, double dfSkewUB)
    {
        // If these fail it's probably because we've
        // got a duplicated time stamp (ie same as previous
        // measurement)
        // But could be because something's gone more wrong
        // with the convex envelope.
        // Until we get better error output we'll just
        // have to assume the worst!
        if (!m_LowerBound.AddPoint(dfTXtime, dfSkewLB))
        {
            m_LowerBound.Reset();
        }

		if (!m_UpperBound.AddPoint(dfTXtime, dfSkewUB))
		{
			m_UpperBound.Reset();
		}
    }



	double CMOOSSkewFilter::Update(double dfRQtime, double dfTXtime, double dfRXtime, tSkewInfo *skewinfo)
	{
		double dfSkewLB = dfTXtime - dfRXtime;
        double dfSkewUB = dfTXtime - dfRQtime;

        // Push the new measurements into the convex envelope filters
        UpdateEnvelopes(dfTXtime, dfSkewLB, dfSkewUB);

		// Get current best estimates of drift rate and offset
        // We won't use envelope vals if envelopes are not stable
        double dfEnvSkewLB = GetLBSkewAtServerTime(dfTXtime);
        double dfEnvSkewUB = GetUBSkewAtServerTime(dfTXtime);
        
        // Get the best estimate we can of
        // skew and drift rate
        double dfSkew = (dfSkewLB + dfSkewUB) / 2.0;

		if (m_nMeas < 10) dfSkew = dfSkewLB;  // UB often in error early on 

        double dfM    = 0.0;
        double dfC    = dfSkew;
        if (m_LowerBound.IsStable())
        {
            dfSkew = dfEnvSkewLB;
            m_LowerBound.GetLineEstimate(dfM, dfC);

            if (m_UpperBound.IsStable())
            {
                if (dfEnvSkewUB >= dfEnvSkewLB)
                {
                    dfSkew = (dfEnvSkewLB + dfEnvSkewUB) / 2.0;
                    GetMidLine(dfM, dfC);
                }
            }
        } 
        else if (m_UpperBound.IsStable())
        {
            dfSkew = dfEnvSkewUB;
            m_UpperBound.GetLineEstimate(dfM, dfC);
        }

        
        // Push those values through a smoothing filter
		double dfFiltOut = dfSkew;
		if (m_nMeas > 0)
		{
			double dt = dfTXtime - m_dfLastTime;
			dfFiltOut = SmoothingFilter(dt, m_dfLastVal, dfSkew, dfM);
		}

		// They're highly unlikely ever to get larger than about 25
        // segments, but just in case...
		double dfMaxPeriod_s   = 1.0 * 60.0*60.0; // 1 hour
        if (m_LowerBound.GetNumSegs() > 500)
        {
            m_LowerBound.CropFrontBefore(dfTXtime - dfMaxPeriod_s);
		}
        if (m_UpperBound.GetNumSegs() > 500)
        {
            m_UpperBound.CropFrontBefore(dfTXtime - dfMaxPeriod_s);
		}

        if (skewinfo)
        {
            // TODO: Output some useful stuff!
            skewinfo->m = dfM;
			skewinfo->c = dfC;
			skewinfo->LB = dfSkewLB;
			skewinfo->UB = dfSkewUB;
			skewinfo->envLB = dfEnvSkewLB;
			skewinfo->envUB = dfEnvSkewUB;
			skewinfo->envEst = dfSkew;
			skewinfo->filtEst = dfFiltOut;
        }


		m_dfLastVal  = dfFiltOut;
		m_dfLastTime = dfTXtime;

		m_nMeas++;

		return dfFiltOut;
	}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////



	CConvexEnvelope::CConvexEnvelope(eDirection aboveOrBelow)
        : m_aboveOrBelow(aboveOrBelow)
	{
		Reset();
	}


    void CConvexEnvelope::DumpState() const
    {
		MOOSTrace("%d Segments\n", GetNumSegs());
		for (unsigned int i=0; i<m_segs.size(); i++)
		{
			m_segs[i].DumpState();
		}
	}


    bool CConvexEnvelope::IsStable() const
    {
        // This is fairly arbitrary but should
        // be more than enough
        return m_nMeas > 50;
    }


	void CConvexEnvelope::Reset()
	{
		m_segs.clear();
		m_bHaveInitPt     =  false;
		m_InitPt          =  tPt(0,0);
		m_uiLongestSegID  =  0;
		m_dfLongestSegLen =  0;
        m_nMeas = 0;
	}


    void CConvexEnvelope::GetLineEstimate(double &m, double &c) const
    {
        tSeg seg;
        if (GetLongestSeg(seg))
        {
            m = seg.dfM;
            c = seg.dfC;
        }
        else
        {
            m = 0.0;
            c = m_InitPt.y;
        }
    }


	bool CConvexEnvelope::GetLongestSeg(tSeg &seg) const
	{
		if (GetNumSegs() < 1) return false;
		if (m_uiLongestSegID >= GetNumSegs()) return false;
		seg = m_segs[m_uiLongestSegID];
		return true;
	}



	bool CConvexEnvelope::AddPoint(double x, double y)
	{
		tPt pt(x, y);

		if (GetNumSegs() == 0 && !m_bHaveInitPt)
		{
			// Store this initial point
			m_InitPt = pt;
			m_bHaveInitPt = true;
			return true;
		}

		// Add a new segment
		if (!m_bHaveInitPt)
			return false;

		tSeg seg;
		if (!MakeSeg(seg, m_InitPt, pt)) return false;
		AppendSeg(seg);		


		// Now try to merge segments from the back
		bool bMerged = true;
		while (GetNumSegs()>1 && bMerged)
		{
			bMerged = MergeLastSeg();
		}	

		// Store latest point for use next time
		// a point is added
		m_InitPt = pt;
		m_nMeas++;
		return true;

	}

	
	void CConvexEnvelope::AppendSeg(const tSeg & seg)
	{
		m_segs.push_back(seg);

		tSeg longestSeg;
		if (GetLongestSeg(longestSeg))
		{
			if (longestSeg.dfPeriod < seg.dfPeriod)
			{
				m_uiLongestSegID  = GetNumSegs()-1;
				m_dfLongestSegLen = seg.dfPeriod;
			}
		}
		else
		{
			m_uiLongestSegID  = 0;
			m_dfLongestSegLen = seg.dfPeriod;
		}
	}


	void CConvexEnvelope::CropFrontBefore(double x_min)
	{
		while (GetNumSegs() > 0 && m_uiLongestSegID > 0)
		{
			if (m_segs[0].p2.x < x_min)
			{
				m_segs.pop_front();
				m_uiLongestSegID -= 1;
			}
		}
	}


	bool CConvexEnvelope::MakeSeg(tSeg &seg, const tPt &p1, const tPt &p2) const
	{
		double M=0, C=0;

		if (!GetLineParams(p1, p2, M, C)) return false;
		
		tSeg s;
		s.p1 = p1;
		s.p2 = p2;
		s.dfM = M;
		s.dfC = C;
		s.dfPeriod = fabs(p2.x - p1.x);

		seg = s;
		return true;
	}

	bool CConvexEnvelope::GetLineParams(const tPt &p1, const tPt &p2, double &M, double &C) const
	{
		if (p1.x == p2.x) return false;

		M = (p2.y-p1.y) / (p2.x - p1.x);
		C = p2.y - M*p2.x;

		return true;
	}


	bool CConvexEnvelope::MergeLastSeg()
	{
		// Try to merge the latest segment with the one before

		if (GetNumSegs() < 2) return false;

		// Get merge candidates
		tSeg seg1 = *(m_segs.rbegin()+1);
		tSeg seg2 = *(m_segs.rbegin());


        if (m_aboveOrBelow == envelopeAbove)
        {
            if (seg2.dfM < seg1.dfM) return false;
        }
        else
        {
            if (seg2.dfM > seg1.dfM) return false;
        }

        // OK, merge is possible
		if (m_uiLongestSegID >= GetNumSegs()-1)
		{
			// Ensure we check again for max segment length
			m_dfLongestSegLen = 0;
		}

		tSeg mergedSeg;
		if (!MakeSeg(mergedSeg, seg1.p1, seg2.p2))
		{
			return false;
		}

		mergedSeg.dfPeriod = seg1.dfPeriod + seg2.dfPeriod;

		// Remove the last two segments and replace them with
        // the new merged segment.
        m_segs.pop_back();
		m_segs.pop_back();
		m_segs.push_back(mergedSeg);

		if (mergedSeg.dfPeriod > m_dfLongestSegLen)
		{
			m_uiLongestSegID = GetNumSegs()-1;
			m_dfLongestSegLen = mergedSeg.dfPeriod;
		}

		return true;

	}

}


