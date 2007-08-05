// $Header: /home/cvsroot/project-marine-shell/src/pACom/pACom.cpp,v 1.2 2005/11/12 22:27:08 osagie Exp $
// Peter Osagie, osagie@mit.edu

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CpACom.h"

using namespace std;

int main(int argc, char* argv[]) {
  char* sMissionFile = "default.moos";
  char* sMOOSName = "pACom";

  switch(argc) {
    case 3:
      sMOOSName  = argv[2];

    case 2:
      sMissionFile = argv[1];
	}

  CpACom pACom;

  pACom.Run(sMOOSName, sMissionFile);

  return 0;
}
