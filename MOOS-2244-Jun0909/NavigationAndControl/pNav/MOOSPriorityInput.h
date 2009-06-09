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
///////////////////////////////////////////////////////////////////
//                                                               //
//          This file is part of MOOS Suite                      //
//            Copyright Paul Newman, September 2000                //
//                                                               // 
///////////////////////////////////////////////////////////////////

// MOOSPriorityInput.h: interface for the CMOOSPriorityInput class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif



#if !defined(AFX_MOOSPRIORITYINPUT_H__7A2A1C64_F6EC_4DDA_9493_85D35E563F8F__INCLUDED_)
#define AFX_MOOSPRIORITYINPUT_H__7A2A1C64_F6EC_4DDA_9493_85D35E563F8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
//#include <list>
#include <vector>

//using namespace std;

class CMOOSPriorityInput  
{
public:
    bool GetLastValue(double & dfTime,double &dfVal);
    bool Clear();
    bool SetInput(CMOOSMsg & InMsg,double dfTimeNow);
    bool GetOutput(CMOOSMsg & OUtMsg,double dfTimeNow);
    

    /** used to make a prioritised input from a string in a *.moos file*/
    bool Initialise(std::string sName, std::string  sSources,std::string  sStem,STRING_LIST & ToSubscribe);

    CMOOSPriorityInput();
    virtual ~CMOOSPriorityInput();


    class CPrioritySource
    {
        public:
            double GetLastInputTime();
            std::string & GetSource();
            bool Initialise(std::string & sName,double dfTimeOut);
            bool Get(double & dfval,double & dfTime);
        	bool Get(std::string  & dfval,double & dfTime);
        char GetDataType();
        	bool Set(const std::string & sVal,double dfTime);
            bool Set(double dfVal,double dfTime);
            bool HasExpired(double dfTimeNow);
            bool IsFresh(){return   m_bFresh;}; 
            CPrioritySource();
        
        protected:
            std::string m_sSource;
            double m_dfTimeOut;
            double m_dfLastInputTime;
            double m_dfVal;
            double m_dfDataTime;
        	char   m_cDataType;
        	std::string m_sVal;
            bool   m_bFresh; 



    };

    typedef std::vector<CPrioritySource> PRIORITY_SOURCE_VECTOR;

    PRIORITY_SOURCE_VECTOR m_Sources;
    int        m_nCurrentSourceNdx;
    std::string    m_sName;

protected:
    double m_dfLastValue;
    double m_dfLastTimeSet;
    bool SetActiveSource(int nSrcNdx);
};

#endif // !defined(AFX_MOOSPRIORITYINPUT_H__7A2A1C64_F6EC_4DDA_9493_85D35E563F8F__INCLUDED_)
