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




//this is our main object...everything after this is configuration...
CAntler gAntler;

#ifndef _WIN32
//this is a signal handler
void CatchMeBeforeDeath(int sig) 
{
	gAntler.ShutDown();
	exit(0);
}
#endif


int main(int argc ,char *argv[])
{
	
#ifndef _WIN32
	//register a handler for shutdown
	signal(SIGINT, CatchMeBeforeDeath);
	signal(	SIGQUIT, CatchMeBeforeDeath);
	signal(	SIGTERM, CatchMeBeforeDeath);
#endif
	
      
    
    //here we look for overloading directoves which are of form --name=value
    std::vector<std::string> vArgv;
    std::map<std::string, std::string> OverLoads;
    for(int i = 0;i<argc;i++)
    {
        std::string t(argv[i]);
        if(t.find("--")==0)
        {
            //this looks like an overloading directive
            MOOSChomp(t,"--");
            std::string sVar = MOOSChomp(t,"=");
            if(t.empty())
            {
				if(MOOSStrCmp(sVar, "quiet"))
				{
					gAntler.SetVerbosity(CAntler::QUIET);
				}
				else if(MOOSStrCmp(sVar, "terse"))
				{
					gAntler.SetVerbosity(CAntler::TERSE);
				}
				else
				{
					MOOSTrace("error incomplete overloading of parameter  --%s=value (are there extraneous whitespaces?)\n",sVar.c_str());
					return -1;
				}
			}
            else
            {
            	OverLoads[sVar] = t;
            }
        }
        else
        {
            //nope this is a regular parameter so it gets passed onto what is yet to come
            vArgv.push_back(t);
        }
    }
    
      
	MOOSTrace("*************************************\n");
    MOOSTrace("*  This is Antler, head of MOOS...  *\n");
    MOOSTrace("*  P. Newman 2008                   *\n");
    MOOSTrace("*************************************\n");
	
    
    switch(vArgv.size())
    {
        case 2:
        {
            //standard principal Antler
            std::string sMissionFile = vArgv[1];
            
            if((int)vArgv.size()!=argc)
            {
                //we are overloading somehow...
                CProcessConfigReader MR;
                MR.SetFile(sMissionFile);
                
                sMissionFile+="++";
                
                //we need to take a copy of the missions file and fill in overloads
                if(!MR.MakeOverloadedCopy(sMissionFile,OverLoads))
                    return MOOSFail("error making overloaded mission file\n");
                
            }
            
                    
            return gAntler.Run(sMissionFile) ? 0 :-1;            
        }
        case 3:
        {
            //standard principal Antler but only run a subset of processes
            //arg 3 must be string quoted
            std::string sMissionFile = vArgv[1];
            
            
            if((int)vArgv.size()!=argc)
            {
                //we are overloading somehow...
                CProcessConfigReader MR;
                MR.SetFile(sMissionFile);
                
                sMissionFile+="++";
                
                //we need to take a copy of the missions file and fill in overloads
                if(!MR.MakeOverloadedCopy(sMissionFile,OverLoads))
                    return MOOSFail("error making overloaded mission file\n");
                
            }
            
            
            
            //make a set of processes we want to launch
            std::stringstream S(vArgv[2]); 
            std::set<std::string> Filter;
            //this rather fancy looking bit f stl simply iterates over a list of strings
            std::copy(istream_iterator<std::string>(S), 
                      istream_iterator<string>(),
                      std::inserter(Filter,Filter.begin()));
                       
            return gAntler.Run(sMissionFile,Filter);
        }
        case 4:
        {            
            //headless MOOS - driven my another Antler somewhere else
            std::string sDBHost = vArgv[1]; //where is DB?
            int nPort = atoi(vArgv[2].c_str()); //what port
            std::string sName = vArgv[3]; //what is our Antler name?
            return gAntler.Run(sDBHost,nPort,sName) ? 0 :-1;
        }
        default:
        {
            MOOSTrace("usage:\n pAntler missionfile.moos\nor\n pAntler missionfile.moos \"P1,P2,P3...\"\nor pAntler DBHost DBPort AntlerName\n");
			MOOSTrace("\n  --quiet to suppress console output, --terse for limited output\n");
			
            return -1;
        }
    }
  
    
}
