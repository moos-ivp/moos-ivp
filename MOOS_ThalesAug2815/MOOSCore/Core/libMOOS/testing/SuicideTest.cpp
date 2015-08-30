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
//   http://www.gnu.org/licenses/lgpl.txt distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////




/*
 * QueueTest.cpp
 *
 *  Created on: Dec 6, 2012
 *      Author: pnewman
 */
#include "MOOS/libMOOS/Utils/CommandLineParser.h"
#include "MOOS/libMOOS/Comms/SuicidalSleeper.h"
#include "MOOS/libMOOS/Utils/ThreadPrint.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"


MOOS::SuicidalSleeper Sleeper;

void PrintHelpAndExit()
{
	std::cerr<<"quick test for suicide sleeper\n\n";
	std::cerr<<"    kill with nc -u "<<Sleeper.GetChannel()<<" "<<Sleeper.GetPort()<<"\n";
	std::cerr<<"then enter passphrase which is  :\n";
	std::cerr<<"   "<<Sleeper.GetPassPhrase()<<"\n";

	exit(0);

}

class ClassToHandleQuit
{
public:
    bool on_exit(std::string &M)
    {
        M  = "I accept my fate....\n";
        return true;
    }
};

int main(int argc, char * argv[])
{
	MOOS::CommandLineParser P(argc,argv);

	if(P.GetFlag("-h","--help"))
	{
		PrintHelpAndExit();
	}

	ClassToHandleQuit Q;
	Sleeper.SetLastRightsCallback<ClassToHandleQuit>(&Q,&ClassToHandleQuit::on_exit);

	Sleeper.Run();
	while(1)
	{
	    MOOSPause(1000);
	}




}
