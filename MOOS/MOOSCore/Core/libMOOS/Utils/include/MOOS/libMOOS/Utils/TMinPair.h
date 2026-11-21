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
//simple template class that keep track of the
//minimum keyed pair
#ifndef TMINPAIRH
#define TMINPAIRH
template <class TKey,class TData> class TMinPair
{
public:

    TMinPair() : m_bLive(false){};
    TKey Key(){return m_Key;};
    TData Data(){return m_Data;};
    bool Valid(){return m_bLive;};
    void Clear(){m_bLive = false;};


    void Update(const TKey & NewKey, const TData & NewData)
    {
        if(NewKey<m_Key || m_bLive==false)
        {
            m_bLive = true;
            m_Key = NewKey;
            m_Data = NewData;
        }
    }

private:
    TKey m_Key;
    TData m_Data;
    bool m_bLive;

};
#endif

