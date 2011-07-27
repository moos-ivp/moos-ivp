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
// MOOSNavBase.cpp: implementation of the CMOOSNavBase class.
//
//////////////////////////////////////////////////////////////////////


#include <stdarg.h>
#include "MOOSNavBase.h"
#include <typeinfo>
#include <cstring>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSNavBase::CMOOSNavBase()
{
    m_pOutputList = NULL;
}

CMOOSNavBase::~CMOOSNavBase()
{

}

void CMOOSNavBase::Trace()
{
    MOOSTrace("%s id(%d) type %s\n",m_sName.c_str(),m_nID,GetTypeName().c_str());
}

string CMOOSNavBase::GetTypeName()
{
    return typeid(*this).name();
}

string CMOOSNavBase::GetName()
{
    return m_sName;
}

int CMOOSNavBase::GetID()
{
    return m_nID;
}

bool CMOOSNavBase::AddToOutput(string  sStr)
{
    return AddToOutput("%s",sStr.c_str());
}
bool CMOOSNavBase::AddToOutput(const char *FmtStr,...)
{

    const int MAX_TRACE_STR = 1024;

    if(strlen(FmtStr)<MAX_TRACE_STR)
    {
        //double the size for format length!
        char buf[MAX_TRACE_STR*2];

        va_list arg_ptr;

        va_start( arg_ptr,FmtStr);

        vsprintf(buf,FmtStr,arg_ptr);

        va_end( arg_ptr );


        string sText = string(buf);

        if(m_pOutputList!=NULL)
        {
            CMOOSMsg Msg(MOOS_NOTIFY,"MOOS_DEBUG",sText.c_str());
            m_pOutputList->push_front(Msg);
        }

        MOOSTrace(sText+"\n");


    }
    else
    {
        return false;
    }

    return true;
}


bool CMOOSNavBase::SetOutputList(MOOSMSG_LIST *pList)
{
    m_pOutputList = pList;
    return m_pOutputList!=NULL;
}

bool CMOOSNavBase::SetName(string sName)
{
    m_sName = sName;
    return true;
}
