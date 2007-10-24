//#include "../MOOSLIB/MOOSLib.h"
#include "MOOSLib.h"

#include "GPSInstrument.h"
int main(int argc , char * argv[])
{
  char * sMissionFile = "Mission.moos";

  if(argc > 1) {
    sMissionFile = argv[1];
  }
  
  CGPSInstrument GPSInstrument;
  
  GPSInstrument.Run("iGPS", sMissionFile);
  
  return 0;
}
