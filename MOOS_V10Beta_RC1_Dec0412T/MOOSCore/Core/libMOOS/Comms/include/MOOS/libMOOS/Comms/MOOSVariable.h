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
// MOOSVariable.h: interface for the CMOOSVariable class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MOOSVARIABLEH
#define MOOSVARIABLEH

#include "MOOSMsg.h"
#include <iomanip>
#include <ostream>
#define DEFAULT_MOOS_VAR_COMMS_TIME 0.2

class CMOOSVariable  
{
public:

    /** construction destruction*/
    CMOOSVariable();
    CMOOSVariable(std::string sName, std::string sSubscribe,std::string sPublish,double dfCommsTime = DEFAULT_MOOS_VAR_COMMS_TIME);
    virtual ~CMOOSVariable();
    
    /** get data rendered as a string*/
    std::string GetAsString(int nFieldWidth = 12) const;
    
    /** get name of MOOSClient which wrote this data */
    std::string GetWriter() const;

    /** get time since write relative to supplied tie*/
    double 		GetAge(double dfTimeNow) const;
    
    /** get name of MOOSClient which wrote this data */
    bool 		Set(const std::string & sVal,double dfTime);
    
    /** get name of MOOSClient which wrote this data */
    bool 		Set(double dfVal,double dfTime);

    /** get the name of the variable */
    std::string GetName() const;
    
    /** Get string contents if applicable*/
    std::string GetStringVal() const;
    
    /** Get Time corresponding to variable was written */
    double 		GetTime() const ;
    
    /** get numerical contents if applicable */
    double 		GetDoubleVal() const ;
    
    /** return true if type is double */
    bool 		IsDouble() const;
    
    /** get nae this variable will use if data is published into MOOS community */
    std::string GetPublishName() const;
    
    /** returns true if data has been updated (refreshed )*/
    bool 		IsFresh() const ;

    /** set or clear the IsFresh flag */
    bool 		SetFresh(bool bFresh);

    /** get the name of a MOOS comms variable this CMOOSVariable will absorb*/
    std::string GetSubscribeName() const;
    
    /** copy in data from a MOOSMsg*/
    bool 		Set(const CMOOSMsg & Msg);
    
    /** get max frequency at which this MOOSvariable could refresh */
    double 		GetCommsTime() const {return m_dfCommsTime;};
    
    
    
     


protected:
    double m_dfVal;
    std::string m_sVal;
    bool   m_bDouble;
    bool   m_bFresh; 
    double m_dfTimeWritten;
    double m_dfCommsTime; //time used when registering (how often should we receive updates?

    std::string m_sName;
    std::string m_sSrc;
    std::string m_sSubscribeName;
    std::string m_sPublishName;
};

////////////////////////////////////////////////////////////////////////
/** << operator added at bequest of vermeij@nurc.nato.int April 2009  */
////////////////////////////////////////////////////////////////////////
template <typename _CharT, typename _Traits>
static std::basic_ostream <_CharT, _Traits>&
operator<< (std::basic_ostream <_CharT, _Traits>& OutputStream, const CMOOSVariable& MOOSVar)
{
    OutputStream << "name=" << MOOSVar.GetName ();
    OutputStream << ", ";
    
    if (MOOSVar.IsDouble ()) 
    {
        OutputStream << "double=" << MOOSVar.GetDoubleVal ();
    }
    else 
    {
        OutputStream << "string=" << MOOSVar.GetStringVal ();
    }
    OutputStream << ", ";
    OutputStream << std::fixed << std::setprecision (2) << "time=" << MOOSVar.GetTime ();
    return OutputStream;
}

#endif 
