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
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <MOOSLIB/MOOSLib.h>
#include "MOOSDB.h"
#include <iostream>

#include "MOOSDBHTTPServer.h"

int main(int argc , char * argv[])
{
    //default command line parameters - Mission file and port number
    const char * sMissionFile = "Mission.moos";

    if(argc>1)
    {
        sMissionFile = argv[1];
    }

    //this is a main MOOS DB Object
    CMOOSDB DB;
    DB.Run(sMissionFile);

    //this is a webserver object which allows you
    //to access and prod the MOOSDB via HTTP
    #ifdef MOOSDB_HAS_WEBSERVER
        CMOOSDBHTTPServer HTTPS(DB.GetDBPort());
    #endif

    //nothing to do - all the threads in the DB object
    //do the work
    while(1)
    {
        MOOSPause(1000);
    }

    return 0;
}

