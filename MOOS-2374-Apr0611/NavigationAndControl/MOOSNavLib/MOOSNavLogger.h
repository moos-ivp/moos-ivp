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
// MOOSNavLogger.h: interface for the CMOOSNavLogger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSNAVLOGGER_H__3A39CB11_1F50_4115_99D8_589CE13FFCF9__INCLUDED_)
#define AFX_MOOSNAVLOGGER_H__3A39CB11_1F50_4115_99D8_589CE13FFCF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMOOSNavLogger  
{
public:
    bool Comment(const string & sComment,double dfTime);
    bool LogObservation(double dfTimeNow,
        CMOOSObservation & rObs,   
        int nUpdate,
        bool bNaN = false);
    bool Initialise(const string & sFileName,const string &sPath,bool bTimeStamp);
    bool LogState(double dfTimeNow ,Matrix & Xhat,Matrix & Phat);
    CMOOSNavLogger();
    virtual ~CMOOSNavLogger();

protected:
    string m_sStateFileName;
    string m_sObsFileName;

    string MakeFileName(string sStem,
                const string & sExtension,
                string sPath,
                bool bTimeStamp);

    ofstream m_StateLogFile;
    ofstream m_ObsLogFile;

    int m_nObsLogged;

};

#endif // !defined(AFX_MOOSNAVLOGGER_H__3A39CB11_1F50_4115_99D8_589CE13FFCF9__INCLUDED_)
