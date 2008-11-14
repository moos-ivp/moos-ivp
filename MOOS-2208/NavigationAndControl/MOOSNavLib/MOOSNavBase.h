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
// MOOSNavBase.h: interface for the CMOOSNavBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSNAVBASE_H__67A316C1_8503_42CA_A683_9668DF0BC8AB__INCLUDED_)
#define AFX_MOOSNAVBASE_H__67A316C1_8503_42CA_A683_9668DF0BC8AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//other project utitlities
#include <MOOSLIB/MOOSLib.h>

//standard template library help
#include <string>
#include <map>
using namespace std;


// tools for managing indexes..
enum StateNdx
{
    iiX =0,
    iiY,
    iiZ,
    iiH,
    iiXdot,
    iiYdot,
    iiZdot,
    iiHdot,
};


#define I_X(M,n)        (M(n+iiX,1))
#define I_Y(M,n)        (M(n+iiY,1))
#define I_Z(M,n)        (M(n+iiZ,1))
#define I_H(M,n)        (M(n+iiH,1))
#define I_Xdot(M,n)        (M(n+iiXdot,1))
#define I_Ydot(M,n)        (M(n+iiYdot,1))
#define I_Zdot(M,n)        (M(n+iiZdot,1))
#define I_Hdot(M,n)        (M(n+iiHdot,1))

#define SQR(a) (a*a)

#define FULL_STATES 8

#define POSE_ONLY_STATES 4
#define POSE_AND_RATE_STATES FULL_STATES

/** The base class for all MOOSNavLib objects
conatains naming, timestamps and a pointer to
the broker to get any object */
class CMOOSNavBase
{
public:
    bool SetName(string sName);
    bool SetOutputList(MOOSMSG_LIST * pList);
    int GetID();
    string GetName();
    virtual string GetTypeName();
    virtual void Trace();
    CMOOSNavBase();
    virtual ~CMOOSNavBase();

    int m_nID;
    string m_sName;


protected:
    bool AddToOutput(string sStr);
    bool AddToOutput(const char *FmtStr,...);
    MOOSMSG_LIST * m_pOutputList;
};

typedef map<int,CMOOSNavBase *> NAVBASE_MAP;

#endif // !defined(AFX_MOOSNAVBASE_H__67A316C1_8503_42CA_A683_9668DF0BC8AB__INCLUDED_)
