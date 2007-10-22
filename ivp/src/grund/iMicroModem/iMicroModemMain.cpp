
// iMicroModem MOOS Modem driver, was developed 
// by Matt Grund, Woods Hole Oceanographic Institution
//
// This code is licensed under a Creative Commons
// Attribution Non-Commercial Share-A-Like License,
// version 2.5.
//
// For more information, see the file License.html
// or the web site:
//
//  http://creativecommons.org/licenses/by-nc-sa/2.5/
//
// Copyright(c)2004, Matt Grund, WHOI. <mgrund@whoi.edu>

#include <MOOSLIB/MOOSLib.h>
#include <string>
#include "version.h"

#include "MicroModemInstrument.h"
int main(int argc ,char * argv[])
{
	std::string sMissionFile;
	std::string sMOOSName; 


	sMOOSName.assign("iMicroModem");
	sMissionFile.assign("iMicroModem.moos");
	switch(argc)
	{
	case 3:
		sMOOSName = MOOSFormat("%s",argv[2]);
	case 2:
        sMissionFile = MOOSFormat("%s",argv[1]);
	}

    CMicroModemInstrument MicroModemInstrument;

	MOOSTrace("\nThis is %s version %d.%02d\n\n",VERSION_NAME,VERSION_MAJOR,VERSION_MINOR);

	MOOSTrace("iMicroModem running mission:\"%s\" as \"%s\"\n",sMissionFile.c_str(),sMOOSName.c_str());
    MicroModemInstrument.Run((char *)sMOOSName.c_str(),(char *)sMissionFile.c_str());

    return 0;
}
