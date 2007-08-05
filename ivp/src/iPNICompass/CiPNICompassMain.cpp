// $Header: /home/cvsroot/project-marine-shell/src/iPNICompass/CiPNICompassMain.cpp,v 1.1.1.1 2005/08/08 01:17:13 cvsadmin Exp $
// (c) 2004

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CiPNICompass.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "iPNICompass.moos";

	if (argc > 1) {
		sMissionFile = argv[1];
	}

	CiPNICompass iPNICompass;

	iPNICompass.Run("iPNICompass", sMissionFile);

	return 0;
}

