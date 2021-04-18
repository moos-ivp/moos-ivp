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
//   http://www.gnu.org/licenses/lgpl.txt
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
#ifndef INTERP_BUFFER_H
#define INTERP_BUFFER_H

#include <map>
#include <iterator>
#include <assert.h>
#include <math.h>
#include <algorithm>
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"

//! a buffer to store data and get interp values by index with time
template< class Key, class Data, class InterpFunc, class Compare=std::less<Key> > 
class TInterpBuffer : public std::map<Key,Data,Compare>
{
    typedef std::map<Key,Data,Compare> BASE_TYPE;
    
    InterpFunc m_interpFunc;
public:
    void SetInterpFunc(const InterpFunc &interp) {m_interpFunc = interp;}
    InterpFunc &GetInterpFunc() {return m_interpFunc; }

    // const version
    Data operator()(const Key &interp_time) const
    {
        assert(this->size() > 0 );
        
        if (this->size()==1) return this->begin()->second;

        typename BASE_TYPE::const_iterator hi;
        
        hi = BASE_TYPE::lower_bound(interp_time);
        if (hi != this->begin()  && hi != this->end())
        {
          typename BASE_TYPE::const_iterator low = hi; --low;
            return m_interpFunc(*low,*hi,interp_time);
        }
        else
        {
            if (hi == this->begin()) 
            {
                typename BASE_TYPE::const_iterator hi2 = hi; ++hi2;
                return m_interpFunc(*hi,*hi2,interp_time);
            }
            else if (hi == this->end()) 
            {
                typename BASE_TYPE::const_iterator low  = hi;  --low;
                typename BASE_TYPE::const_iterator low2 = low; --low2;
                
                return m_interpFunc(*low2,*low,interp_time);
            }
        }
        assert("should not get here"==0);
        return Data();
    }


    void MakeSpanTime(double dfSpan)
    {
        if (this->size() > 0)
        {
            EraseOld(MaxKey()-dfSpan);
        }
    }


    void EraseOld(double dfTime)
    {
        if(this->size()>0)
        {
            typename BASE_TYPE::iterator oldest = this->lower_bound(dfTime);
            erase(this->begin(),oldest);
        }
    }

    Key MaxKey() const
    {
        // Don't call this unless it's going to
        // give a valid output!
        assert(this->size() > 0);
        
        Key maxkey = this->rbegin()->first;
        return maxkey;
    }

    Key MinKey() const
    {
        // Don't call this unless it's going to
        // give a valid output!
        assert(this->size() > 0);

        Key minkey = this->begin()->first;        
        return minkey;        
    };


    bool MaxData(Data & D) const
    {
        if(this->size())
        {
            D  = this->rbegin()->second;
            return true;
        }
        return false;
    }

    
    bool MinData(Data & D) const
    {
        if(this->size())
        {
            D  = this->begin()->second;
            return true;
        }
        return false;
    }

};

//! Interpolator to use with interpbuffer
template <class T>
class CTimeNumericInterpolator 
{
    public:
    //return object with time closest to requested time
    typedef std::pair<double,T> TIME_DOUBLE_NUM_PAIR;

    T operator()(const TIME_DOUBLE_NUM_PAIR &loPair, const TIME_DOUBLE_NUM_PAIR &hiPair, double dfInterpTime) const 
    {   

        double dfLoTime = loPair.first;
        double dfHiTime = hiPair.first;

        double dfMidTime;  
        
        dfMidTime = dfInterpTime;    
        
        double dt = (dfHiTime - dfLoTime);    
        
        double alpha = 0.0;    
        
        if ( dt != 0.0 ) 
        {      
            alpha = (dfMidTime - dfLoTime) / dt; 
        }    
        
        if(alpha>1.0 || alpha<0)
        {
            double dfExtrapTime = 0.0;
			if (alpha > 0)
			{
				dfExtrapTime = dfInterpTime - dfHiTime;
			}
			else
			{
				dfExtrapTime = dfLoTime - dfInterpTime;
			}

			if(dfExtrapTime>0.5)
            {
                MOOSTrace("Warning more than 0.5 seconds data extrapolation (%f)\n",dfExtrapTime);
            }
        }

        const T & Lo = loPair.second;
        const T & Hi = hiPair.second;

        T MidVal = alpha*Hi + (1-alpha)*Lo;


        return MidVal;
    }

};

//! TimeInterpolator for use with InterpBuffer
template <class T>
class CTimeGenericInterpolator
{
    public:
    
    typedef std::pair<double,T> TIME_DOUBLE_VAL_PAIR;

    T operator()(const TIME_DOUBLE_VAL_PAIR &loPair, const TIME_DOUBLE_VAL_PAIR &hiPair, double dfInterpTime) const
    {   

        double dfLoTime = loPair.first;
        double dfHiTime = hiPair.first;

        double dfMidTime;  
        
        dfMidTime = dfInterpTime;    
        
        double dt = (dfHiTime - dfLoTime);    
        
        double alpha = 0.0;    
        
        if ( dt != 0.0 ) 
        {      
            alpha = (dfMidTime - dfLoTime) / dt; 
        }    
        
        if(alpha>1.0 || alpha<0)
        {
            double dfExtrapTime = 0.0;
			if (alpha > 0)
			{
				dfExtrapTime = dfInterpTime - dfHiTime;
			}
			else
			{
				dfExtrapTime = dfLoTime - dfInterpTime;
			}

			if(dfExtrapTime>0.5)
            {
                MOOSTrace("Warning more than 0.5 seconds data extrapolation (%f)\n",dfExtrapTime);
            }
        }

        const T & Lo = loPair.second;
        const T & Hi = hiPair.second;
        T Mid;


        double dfValLo,dfValHi;
        int i = 0;
        while(Lo.GetInterpValue(i,dfValLo) && Hi.GetInterpValue(i,dfValHi))
        {
            double dfMidVal = alpha*dfValHi + (1-alpha)*dfValLo;
            Mid.SetInterpValue(i++,dfMidVal);
        }


        return Mid;
    }
};

//!Interpfunction for use with InterpBuffer
/** Simply returns closest element to index */
template <class T>
class ClosestInterpFunc
{
    typedef std::pair<double, T > val_type;
    public:
    //return object with time closest to requested time
    T operator()(const val_type &loPair, const val_type &hiPair, double interp_time) const
    {   
        return fabs(interp_time-hiPair.first)>fabs(interp_time-loPair.first) ? loPair.second : hiPair.second;
    }
};



#endif
