// $Header: /raid/cvs-server/REPOSITORY/software/MOOS/apps/logger/zLoggerNG/CzLoggerNGMain.cpp,v 1.1 2006/04/10 04:43:15 anrp Exp $
// (c) 2005

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CzLoggerNG.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "zLoggerNG.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CzLoggerNG zLoggerNG;
	
	try {
	zLoggerNG.Run("zLoggerNG", sMissionFile);
	} catch(...) {
	}

	fprintf(stderr, "zLoggerNG: man down\n");

	return 0;
}

