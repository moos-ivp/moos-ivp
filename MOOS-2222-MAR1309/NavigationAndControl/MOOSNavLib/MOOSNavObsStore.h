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
// MOOSNavObsStore.h: interface for the CMOOSNavObsStore class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSNAVOBSSTORE_H__DD3C9562_BA3C_4B65_B5B6_181EB57AF3A8__INCLUDED_)
#define AFX_MOOSNAVOBSSTORE_H__DD3C9562_BA3C_4B65_B5B6_181EB57AF3A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MOOSObservation.h"

using namespace std;
typedef list<CMOOSObservation> OBSLIST;
typedef map<CMOOSObservation::Type,OBSLIST> OBSLISTMAP;

#include "MOOSNavBase.h"

class CMOOSNavObsStore : public CMOOSNavBase  
{
public:
    double GetNewestObsTime();
    bool GetObservationsBetween(OBSLIST& ObsList,double dfT1,double dfT2);
    bool Flush();
    bool SetSpan(double dfSpan);
    bool MarkAsUsed(OBSLIST & List);
    OBSLIST * GetListByType(CMOOSObservation::Type eType);
    bool Add(OBSLIST & ObsList);
    CMOOSNavObsStore();
    virtual ~CMOOSNavObsStore();

    OBSLISTMAP m_ObsListMap;
    double m_dfSpan;
    double m_dfNewestObsTime;
    string m_sOwnerName;
};

#endif // !defined(AFX_MOOSNAVOBSSTORE_H__DD3C9562_BA3C_4B65_B5B6_181EB57AF3A8__INCLUDED_)
