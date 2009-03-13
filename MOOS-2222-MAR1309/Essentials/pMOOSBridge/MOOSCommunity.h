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
// MOOSCommunity.h: interface for the CMOOSCommunity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSCOMMUNITY_H__5F2162C1_2606_485D_8F10_610257FD8A67__INCLUDED_)
#define AFX_MOOSCOMMUNITY_H__5F2162C1_2606_485D_8F10_610257FD8A67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CMOOSCommunity  
{
public:
    typedef std::pair<std::string,std::string> SP;

    std::string GetFormattedName();
    std::string GetCommsName();
    bool DoRegistration();
    bool Initialise(const std::string & sCommunityName,
                    const std::string & sHostName,
                    long nPort,
                    const std::string & sMOOSName,
                    int nFreq);
    bool AddSource(const std::string & sStr);
    bool AddSink(const SP & sIndex,const std::string & sAlias);
    bool WantsToSink(const SP & sIndex);
    std::string GetAlias(const SP & sIndex);
    CMOOSCommClient m_CommClient;
    CMOOSCommunity();
    virtual ~CMOOSCommunity();

protected:
    std::set<std::string> m_Sources;
    std::map<SP,std::string> m_Sinks;
    
    std::string m_sCommunityName;
    int m_nSharedFreq;

};

#endif // !defined(AFX_MOOSCOMMUNITY_H__5F2162C1_2606_485D_8F10_610257FD8A67__INCLUDED_)
