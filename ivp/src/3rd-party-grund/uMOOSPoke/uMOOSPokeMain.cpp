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
//   This file is part of a  MOOS Instrument. 
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
#include <MOOSLIB/MOOSLib.h>

#include "MOOSPoke.h"
int main(int argc ,char * argv[])
{
	char * sMissionFile = "MOOSPoke.moos";
	char * sMOOSName = "uMOOSPoke";

	CMOOSPoke MOOSPoke;

	//
	//  uMOOSPoke - poke a moos variable from a shell
        //
        //  Usage: 
	//
        //    1.  uMOOSPoke <variable> <new value> 
        // 
	//        Writes the string in argv[2] to variable named in argv[1]
        //
        //
        //    2.  uMOOSPoke -f <variable> <new value> 
        // 
	//        Writes the double in argv[3] to variable named in argv[2]
        //
	//  Connects to localhost:9000 unless redirected in MOOSPoke.moos
        // 

	switch(argc)
	{
	case 4:
                MOOSPoke.m_bValueIsDouble = true ;
                MOOSPoke.m_sVarName = argv[2] ;
                MOOSPoke.m_dfValue = atof(argv[3]) ;
		break;

	case 3:
                MOOSPoke.m_bValueIsDouble = false ;
                MOOSPoke.m_sVarName = argv[1] ;
                MOOSPoke.m_sValue = argv[2] ;
		break;

	case 8:
	case 7:
	case 6:
	case 5:
	case 2:
	case 1:	
		MOOSTrace("uMOOSPoke: Usage:\n\n");
		MOOSTrace("          uMOOSPoke [-f] <varname> <value>\n\n");
	}


    MOOSPoke.Run(sMOOSName,sMissionFile);

    return 0;
}
