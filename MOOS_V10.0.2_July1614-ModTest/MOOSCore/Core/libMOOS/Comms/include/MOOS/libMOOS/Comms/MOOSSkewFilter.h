///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of
//   Applications and Libraries for Mobile Robotics Research
//   Copyright (C) Paul Newman
//
//   This software was written by Paul Newman at MIT 2001-2002 and
//   the University of Oxford 2003-2013
//
//   email: pnewman@robots.ox.ac.uk.
//
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txtgram is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
/*! \file MOOSSkewFilter.h */

/// This class is used in the new massively improved timing protocol added in the summer of
/// 2008 by Alastair Harrison of MRG arh@robots.ox.ac.uk


#ifndef __MOOSSkewFilter_h
#define __MOOSSkewFilter_h

#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include <deque>

namespace MOOS
{
	class CConvexEnvelope
	{
	public:
		struct tPt
		{
			tPt() : x(0), y(0) {}
			tPt(double _x, double _y) : x(_x), y(_y) {}
			double x,y;
		};

		struct tSeg
		{
			double dfPeriod;
			double dfM, dfC;     // Slope and intersect
			tPt    p1, p2;       // Anchor points through which line passes

			void DumpState() const
			{
				MOOSTrace("Period=%0.6f,dfM=%0.6f,dfC=%0.6f\n", dfPeriod, dfM, dfC);
			}
		};

		enum eDirection { envelopeAbove, envelopeBelow };

	public:

		CConvexEnvelope(eDirection aboveOrBelow);

		void Reset();
		bool AddPoint(double x, double y);

		unsigned int GetNumSegs() const { return m_segs.size(); }
        unsigned int GetNumMeas() const { return m_nMeas;       }

		void GetLineEstimate(double &m, double &c) const;

        bool GetLongestSeg(tSeg &seg) const;

        bool IsStable() const;

		/// Removes all segments from the front which have
		/// x vals less than x_min.  Will not remove the longest
		/// segment
		void CropFrontBefore(double x_min);

		void DumpState() const;

	private:
		bool DeleteLastSeg();
		bool MergeLastSeg();

		void AppendSeg(const tSeg &seg);
		bool MakeSeg(tSeg &seg, const tPt &p1, const tPt &p2) const;
		bool GetLineParams(const tPt &p1, const tPt &p2, double &M, double &C) const;

	private:
		CConvexEnvelope(const CConvexEnvelope &v); // Not implemented
		void operator=(const CConvexEnvelope &v);  // Not implemented

	private:
		std::deque<tSeg> m_segs;

		// Temporarily store first point for each pending segment.
        // It's a bit messy having to keep this around, given that
        // we could just store a list of points and make the segments
        // implicit.  But it's convenient to be able to store segments
        // explicitly, and keep all the information together in one place.
        // So this is a necessary evil.
		bool m_bHaveInitPt;
		tPt  m_InitPt;

		unsigned int     m_uiLongestSegID;
		double           m_dfLongestSegLen;
        const eDirection m_aboveOrBelow;

        unsigned int m_nMeas;
	};



	class CMOOSSkewFilter
	{
	public:
		struct tSkewInfo
		{
			tSkewInfo()
				: m(0),c(0),LB(0),UB(0),
				envLB(0),envUB(0),
				envEst(0),filtEst(0) {}
			double m, c;
			double LB, UB, envLB, envUB;
			double envEst, filtEst;
		};

	public:
		virtual ~CMOOSSkewFilter(){}
		CMOOSSkewFilter();

		virtual void Reset();

		// Returns estimated true skew value (TXtime-RXtime)
        virtual double Update(double dfRQtime, double dfTXtime, double dfRXtime, tSkewInfo *skewinfo=NULL);
		        
        unsigned int GetNumMeas() const { return m_nMeas; }

		void DumpState() const;

	private:
		CMOOSSkewFilter(const CMOOSSkewFilter &v); // Not implemented
		void  operator=(const CMOOSSkewFilter &v); // Not implemented

	private:
        // These use the envelope estimates to produce
        // an estimate of the skew at a given time.
        // They'll return false if either of the
        // convex envelopes has not stabilized.
        // There's no smoothing filter on these.
        bool   GetSkewAtServerTime(double dfTime, double &dfSkew) const;
        bool   GetSkewAtLocalTime(double dfTime, double &dfSkew) const;
        double GetLBSkewAtServerTime(double dfTime) const;
        double GetUBSkewAtServerTime(double dfTime) const;
        
        double SmoothingFilter(double dfDT, double dfOldFilterVal, double dfNewMeas, double dfGradient) const;
        void   UpdateEnvelopes(double dfTXtime, double dfSkewLB, double dfSkewUB);
        
        void   GetMidLine(double &m, double &c) const;

	private:
		CConvexEnvelope m_LowerBound;
		CConvexEnvelope m_UpperBound;
        
        // Smoothing filter state variables
        double m_dfLastVal;
		double m_dfLastTime;

        double m_dfCurrSkew;

		unsigned int m_nMeas;
	};


    // This class transforms input measurements and output values
    // transparently in such a way as to improve the numerical
    // conditioning of the skew filter
    class CMOOSConditionedSkewFilter : public CMOOSSkewFilter
    {
    public:
        
		CMOOSConditionedSkewFilter() :
		  m_dfBeginTime(0), m_dfSkewOffset(0), m_dfRXOffset(0)
		{
		}

        void Reset()
        {
            m_dfBeginTime  = 0;
            m_dfSkewOffset = 0;
            m_dfRXOffset   = 0;
            
            CMOOSSkewFilter::Reset();
        }

        // Mainly we override the 'update' function
        double Update(double dfRQtime, double dfTXtime, double dfRXtime, tSkewInfo *skewinfo)
        {
            // Transform input values
            // If this is the first call then we need to store offsets
            if (this->GetNumMeas() == 0)
            {
                m_dfBeginTime  = dfRQtime;
                m_dfSkewOffset = dfTXtime - dfRXtime;
                m_dfRXOffset   = m_dfSkewOffset - m_dfBeginTime;
            }
            
            dfRQtime = dfRQtime + m_dfRXOffset;
            dfTXtime = dfTXtime - m_dfBeginTime;
            dfRXtime = dfRXtime + m_dfRXOffset;

            // Call parent's Update function
            double dfSkew = CMOOSSkewFilter::Update(dfRQtime, dfTXtime, dfRXtime, skewinfo);

            // Transform output values
            if (skewinfo)
            {
                // Not sure whether we should transform these...
                // After all, they're just diagnostic
                // And the transformed values would suffer from the 
                // numerical precision problems we're trying to avoid

                // Do nothing
            }

            return dfSkew - m_dfSkewOffset;
        }

	private:
		CMOOSConditionedSkewFilter(const CMOOSConditionedSkewFilter &v); // Not implemented
		void             operator=(const CMOOSConditionedSkewFilter &v); // Not implemented


    private:
        double m_dfBeginTime;
        double m_dfSkewOffset;
        double m_dfRXOffset;
    };

}

#endif // __MOOSSkewFilter_h


