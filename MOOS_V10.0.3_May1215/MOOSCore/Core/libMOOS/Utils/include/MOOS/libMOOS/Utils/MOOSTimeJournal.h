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
//   http://www.gnu.org/licenses/lgpl.txt  This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////

/*! \file MOOSTimeJournal.h */
#ifndef MOOSTimeJournalH
#define MOOSTimeJournalH
#include <fstream>
#include <map>
#include <string>

//this is a utility class to do time stamping. 
//
//Tick("LABEL");
//  DO YOUR CODE
//        TICK("LABEL2")
//            MORE CODE
//        Tock("LABLE2")
//Tock("LABEL")
//Dump();
//
//this will produce a pretty file of timings
//you can call NewLevel() to print an iteration seperator
//in the dump file. This an exerpt from a dump file
/*Level:4754
    Iterate              0.000 seconds
Level:4755
    Iterate              0.430 seconds
    ConstraintApplication 0.422 seconds
    EKF                  0.012 seconds
    EKF_UPD_1            0.006 seconds
    EKF_OBS              0.006 seconds    
    ConstraintSearch     0.003 seconds
Level:4756
    Iterate              0.041 seconds
Level:4757
    Iterate              0.000 seconds
*/

class CMOOSTimeJournal
{
public:

    CMOOSTimeJournal()
    {        
        m_nLevel = 0;
        m_nStack = 0;
    }

    ~CMOOSTimeJournal()
    {
        m_File.close();
    }

    void Open(const std::string & sFile)
    {
        m_File.open(sFile.c_str());
        m_nLevel = 0;
    }

    void Tick(const std::string & S)
    {
        m_T[S] = HPMOOSTime();
        m_nStack++;
    }

    void Tock(const std::string & S)
    {
        if(m_T.find(S)==m_T.end())
        {
            MOOSTrace("No such timer \"%s\"\n",S.c_str());
            return;
        }
        else
        {            
            std::string space(4*(m_nStack+1),' ');
            std::string R= MOOSFormat("%s%-40s %.3f seconds",space.c_str(),S.c_str(),HPMOOSTime()-m_T[S]);
            m_L.push_back(R);

            if(m_nStack>0)
                m_nStack--;

        }


    }

    void NewLevel(int nL=-1)
    {
        m_nLevel = nL>-1 ? nL:m_nLevel+1;
    }

    void Dump()
    {        
        m_File<<"Level:"<<m_nLevel<<std::endl;
        std::copy(m_L.rbegin(),m_L.rend(),std::ostream_iterator<std::string>(m_File,"\n"));
        m_File.flush();
        m_L.clear();
        m_T.clear();
    }

protected:    
    std::map<std::string, double> m_T;
    std::list<std::string> m_L;
    std::ofstream m_File;
    int m_nLevel;
    int m_nStack;

};


#endif
