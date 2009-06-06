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
// MOOSMsg.h: interface for the CMOOSMsg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSMSG_H__B6540645_B7DA_420D_B212_96E9845BB39F__INCLUDED_)
#define AFX_MOOSMSG_H__B6540645_B7DA_420D_B212_96E9845BB39F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

//MESSAGE TYPES
#define MOOS_NOTIFY 'N'
#define MOOS_REGISTER 'R'
#define MOOS_UNREGISTER 'U'
#define MOOS_NOT_SET '~'
#define MOOS_COMMAND 'C'
#define MOOS_ANONYMOUS 'A'
#define MOOS_NULL_MSG '.'
#define MOOS_DATA 'i'
#define MOOS_POISON  'K'
#define MOOS_WELCOME 'W'
#define MOOS_SERVER_REQUEST 'Q'

//MESSAGE DATA TYPES
#define MOOS_DOUBLE 'D'
#define MOOS_STRING    'S'

//5 seconds time difference between client clock and MOOSDB clock will be allowed
#define SKEW_TOLERANCE 5

/// MOOS Comms Messaging class. This is a class encapsulating the data which the MOOS Comms API shuttles
/// between the MOOSDB and other clients 
class CMOOSMsg  
{
public:


    /**standard construction destruction*/
    CMOOSMsg();
    virtual ~CMOOSMsg();

    /** specialised construction*/
    CMOOSMsg(char cMsgType,const std::string &sKey,double dfVal,double dfTime=-1);

    /** specialised construction*/
    CMOOSMsg(char cMsgType,const std::string &sKey,const std::string & sVal,double dfTime=-1);


    /**check data type (MOOS_STRING or MOOS_DOUBLE) */
    bool IsDataType (char cDataType)const;

    /**check data type is double*/
    bool IsDouble()const{return IsDataType(MOOS_DOUBLE);}

    /**check data type is string*/
    bool IsString()const{return IsDataType(MOOS_STRING);}    

    /**return true if mesage is substantially (SKEW_TOLERANCE) older than dfTimeNow
       if pdfSkew is not NULL, the time skew is returned in *pdfSkew*/
    bool IsSkewed(double dfTimeNow, double * pdfSkew = NULL);

    /**return true if message is younger that dfAge*/
    bool IsYoungerThan(double dfAge)const;

    /**check message type MOOS_NOTIFY, REGISTER etc*/
    bool IsType (char  cType)const;

    /**return time stamp of message*/
    double GetTime()const {return m_dfTime;};

    /**return double val of message*/
    double GetDouble()const {return m_dfVal;};

    /**return string value of message*/
    std::string GetString()const {return m_sVal;};

    /**return the name of the message*/
    std::string GetKey()const {return m_sKey;};
    std::string GetName()const{return GetKey();};

    /**return the name of the process (as registered with the DB) which
    posted this notification*/
    std::string GetSource()const {return m_sSrc;};

    /**return the name of the MOOS community in which the orginator lives*/
    std::string GetCommunity()const {return m_sOriginatingCommunity;};

    /**format the message as string regardless of type*/
    std::string GetAsString(int nFieldWidth=12);

    /**print a summary of the message*/
    void Trace();

    /** set the Double value */
    void SetDouble(double dfD){m_dfVal = dfD;}

    /**what type of message is this? Notification,Command,Register etc*/
    char m_cMsgType;
    
    /**what kind of data is this? String,Double,Array?*/
    char m_cDataType;
    
    /**what is the variable name?*/
    std::string m_sKey;
    
    /**ID of message*/
    int m_nID;

    /** double precision time stamp (UNIX time)*/
    double m_dfTime;
    
    //DATA VARIABLES
    
    //a) numeric
    double m_dfVal;
    double m_dfVal2;
    
    //b) string
    std::string m_sVal;

    //who sent this message?
    std::string m_sSrc;

    //what community did it originate in?
    std::string m_sOriginatingCommunity;

    //serialise this message into/outof a character buffer
    int Serialize(unsigned char *  pBuffer,int  nLen,bool bToStream=true);

    //comparsion operator for sorting and storing
    bool operator <(const CMOOSMsg & Msg) const{ return m_dfTime<Msg.m_dfTime;};


private:
    //private things which you have no business knowing about
    unsigned char * m_pSerializeBufferStart;
    unsigned char * m_pSerializeBuffer;
    int  m_nSerializeBufferLen;
    int  m_nLength;
    int GetLength();
    void  operator << (char & cVal);
    void  operator << (double & dfVal);
    void  operator << (std::string & sVal);
    void  operator << (int & nVal);
    void  operator >> (char & cVal);
    void  operator >> (double & dfVal);
    void  operator >> (std::string & sVal);
    void  operator >> (int & nVal);
    
    bool CanSerialiseN(int N);

};

#endif // !defined(AFX_MOOSMSG_H__B6540645_B7DA_420D_B212_96E9845BB39F__INCLUDED_)
