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





#include "Antler.h"
#include <sstream>
#include <string>
#include <iterator>
#include <algorithm>

using namespace std;

int main(int argc ,char *argv[])
{
      
    MOOSTrace("*************************************\n");
    MOOSTrace("*  This is Antler, head of MOOS...  *\n");
    MOOSTrace("*  P. Newman 2008                   *\n");
    MOOSTrace("*************************************\n");
    
    switch(argc)
    {
        case 2:
        {
            //standard principal Antler
            std::string sMissionFile = argv[1];
            CAntler Antler;
            return Antler.Run(sMissionFile) ? 0 :-1;            
        }
        case 3:
        {
            //standard principal Antler but only run a subset of processes
            //arg 3 must be string quoted
            std::string sMissionFile = argv[1];
            CAntler Antler;
            
            //make a set of processes we want to launch
            std::stringstream S(argv[2]); 
            std::set<std::string> Filter;
            //this rather fancy looking bit f stl simply iterates over a list of strings
            std::copy(istream_iterator<std::string>(S), 
                      istream_iterator<string>(),
                      std::inserter(Filter,Filter.begin()));
                       
            return Antler.Run(sMissionFile,Filter);
        }
        case 4:
        {            
            //headless MOOS - driven my another Antler somewhere else
            std::string sDBHost = argv[1]; //where is DB?
            int nPort = atoi(argv[2]); //what port
            std::string sName = argv[3]; //what is our Antler name?
            CAntler Antler;
            return Antler.Run(sDBHost,nPort,sName) ? 0 :-1;
        }
        default:
        {
            MOOSTrace("usage:\n pAntler missionfile.moos\nor\n pAntler missionfile.moos \"P1,P2,P3...\"\nor pAntler DBHost DBPort AntlerName\n");
            return -1;
        }
    }
  
    
}
